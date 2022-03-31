#include "Config/Bootstrap.h"

#include "Framework/Graphics/DX12/ShaderManagerDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Debug/Assert.h"
#include "Debug/Log.h"
#include "Foundation/Memory/Utils.h"
#include "Framework/File/Utils.h"
#include "Framework/Memory/FrameworkMemoryInterface.h"

namespace hd
{
	ShaderManager::ShaderManager()
		: m_LocalAllocator{ MB(500) }
		  , m_Shaders{ &m_LocalAllocator }
	{

	}

	ShaderManager::~ShaderManager()
	{

	}

	PlainDataArray<std::byte> const& ShaderManager::GetShader(char8_t const* shaderName, char8_t const* entryPoint, char8_t const* profile, ShaderFlags flags)
	{
		ScopedScratchMemory scopedScratch{};

		std::pmr::u8string shaderNameString{ shaderName, &Scratch() };
		std::pmr::u8string entryPointString{ entryPoint, &Scratch() };
		std::pmr::u8string profileString{ profile, &Scratch() };

		std::pmr::u8string cookedFilePath{ &Scratch() };
		file::ConvertToCookedPathPrefixed(shaderNameString, entryPointString, cookedFilePath);

#if defined(HD_ENABLE_RESOURCE_COOKING)
		std::pmr::u8string shaderFilePath{ &Scratch() };
		file::ConvertToShaderPath(shaderNameString, shaderFilePath);
#endif
		auto shaderItem = m_Shaders.find(cookedFilePath);
		if (shaderItem != m_Shaders.end())
		{
			return shaderItem->second;
		}

#if defined(HD_ENABLE_RESOURCE_COOKING)
		if (flags.IsSet(ShaderFlagsBits::IgnoreCache) || file::DestinationOlder(shaderFilePath, cookedFilePath))
		{
			CookShader(shaderFilePath, entryPointString, profileString, cookedFilePath, flags);
		}
#endif
		PlainDataArray<std::byte>& shaderMicrocode = m_Shaders[cookedFilePath];
		file::ReadWholeFile(cookedFilePath, shaderMicrocode);

		hdLogInfo(u8"Shader %:% loaded.", shaderNameString.c_str(), entryPointString.c_str());

		return shaderMicrocode;
	}

	void ShaderManager::ResetShaderCache()
	{
		m_Shaders.clear();
		m_LocalAllocator.Reset();
	}

#if defined(HD_ENABLE_RESOURCE_COOKING)
	void ShaderManager::CookShader(std::pmr::u8string const& shaderFilePath, std::pmr::u8string const& entryPoint, std::pmr::u8string const& profile, 
	                               std::pmr::u8string const& cookedFilePath, ShaderFlags flags)
	{
		ScopedScratchMemory scopedScratch{};

		struct DXCRuntime
		{
			bool Initialized;
			IDxcUtils* Utils;
			IDxcCompiler3* Compiler;
			IDxcIncludeHandler* IncludeHandler;
		};

		static DXCRuntime s_DXCRuntime{};

		if (!s_DXCRuntime.Initialized)
		{
			hdEnsure(::DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&s_DXCRuntime.Utils)));
			hdEnsure(::DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&s_DXCRuntime.Compiler)));
			hdEnsure(s_DXCRuntime.Utils->CreateDefaultIncludeHandler(&s_DXCRuntime.IncludeHandler));
			s_DXCRuntime.Initialized = true;
		}

		std::pmr::u8string pdbFilePath{ &Scratch() };
		std::pmr::u8string pdbExtension{ u8".pdb", &Scratch() };
		file::ReplaceExtension(cookedFilePath, pdbExtension, pdbFilePath);

		std::pmr::wstring wideShaderFilePath{ &Scratch() };
		ToWide(shaderFilePath, wideShaderFilePath);

		HRESULT result;
		uint32_t codePage = CP_UTF8;
		ComPtr<IDxcBlobEncoding> sourceBlob;
		result = s_DXCRuntime.Utils->LoadFile(wideShaderFilePath.c_str(), &codePage, &sourceBlob);
		hdEnsure(result, u8"Cannot create shader blob from file %. Check if file exist.");

		DxcBuffer sourceBuffer{};
		sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
		sourceBuffer.Size = sourceBlob->GetBufferSize();
		BOOL knownEncoding{};
		sourceBlob->GetEncoding(&knownEncoding, &sourceBuffer.Encoding);

		std::vector<LPCWSTR> arguments;

		std::pmr::wstring wideEntryPoint{ &Scratch() };
		ToWide(entryPoint, wideEntryPoint);

		// Entrypoint
		arguments.push_back(L"-E");
		arguments.push_back(wideEntryPoint.c_str());

		std::pmr::wstring wideProfile{ &Scratch() };
		ToWide(profile, wideProfile);

		// Shader model
		arguments.push_back(L"-T");
		arguments.push_back(wideProfile.c_str());

		// Optimizations
#if defined(HD_ENABLE_GFX_DEBUG)
		arguments.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);
#else
            arguments.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif

		// Strip everything
		arguments.push_back(L"-Qstrip_debug");
		arguments.push_back(L"-Qstrip_reflect");
		arguments.push_back(L"-Qstrip_rootsignature");

		// Generate symbols
		if (flags.IsSet(ShaderFlagsBits::GenerateSymbols))
		{
			std::pmr::wstring widePdbFilePath{ &Scratch() };
			ToWide(pdbFilePath, widePdbFilePath);

			arguments.push_back(DXC_ARG_DEBUG);
			arguments.push_back(L"-Fd");
			arguments.push_back(widePdbFilePath.c_str());
		}

		ComPtr<IDxcResult> operationResult;
		result = s_DXCRuntime.Compiler->Compile(&sourceBuffer, arguments.data(), uint32_t(arguments.size()), s_DXCRuntime.IncludeHandler, IID_PPV_ARGS(&operationResult));
		if (FAILED(result) || !operationResult)
		{
			hdAssert(false, u8"Shader % % % failed to compile. Operation result is not available.", shaderFilePath.c_str(), entryPoint.c_str(), profile.c_str());
		}

		ComPtr<IDxcBlobUtf16> dummyName;

		ComPtr<IDxcBlobUtf8> errorBlob;
		operationResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errorBlob), dummyName.ReleaseAndGetAddressOf());
		if (errorBlob && errorBlob->GetStringLength() > 0)
		{
			hdAssert(false, u8"Shader % % % failed to compile.\n Error: %.", shaderFilePath.c_str(), entryPoint.c_str(), profile.c_str(), 
			         reinterpret_cast<char8_t const*>(errorBlob->GetStringPointer()));
		}

		hdEnsure(operationResult->GetStatus(&result));

		ComPtr<IDxcBlob> shaderBlob;
		operationResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), dummyName.ReleaseAndGetAddressOf());
		if (shaderBlob && shaderBlob->GetBufferSize() > 0)
		{
			PlainDataArray<std::byte> buffer{ &Scratch() };
			buffer.Assign(reinterpret_cast<std::byte*>(shaderBlob->GetBufferPointer()), shaderBlob->GetBufferSize());

			file::WriteWholeFile(cookedFilePath, buffer);
		}
		else
		{
			hdAssert(false, u8"Shader % % % doesn't have shader microcode artifact.\n Error: %.", shaderFilePath.c_str(), entryPoint.c_str(), profile.c_str(), 
			         reinterpret_cast<char8_t const*>(errorBlob->GetStringPointer()));
		}

		if (flags.IsSet(ShaderFlagsBits::GenerateSymbols))
		{
			ComPtr<IDxcBlob> pdbBlob;
			operationResult->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdbBlob), dummyName.ReleaseAndGetAddressOf());
			if (pdbBlob && pdbBlob->GetBufferSize() > 0)
			{
				std::pmr::wstring widePdbFileName{ dummyName->GetStringPointer(), &Scratch() };
				std::pmr::u8string pdbFileName{ &Scratch() };

				ToNarrow(widePdbFileName, pdbFileName);

				PlainDataArray<std::byte> buffer{ &Scratch() };
				buffer.Assign(reinterpret_cast<std::byte*>(pdbBlob->GetBufferPointer()), pdbBlob->GetBufferSize());

				file::WriteWholeFile(pdbFileName, buffer);
			}
		}

		hdLogInfo(u8"Shader % cooked to % % debug symbols.", shaderFilePath.c_str(), cookedFilePath.c_str(), flags.IsSet(ShaderFlagsBits::GenerateSymbols) ? u8"with" : u8"without");
	}
#endif
}

#endif
