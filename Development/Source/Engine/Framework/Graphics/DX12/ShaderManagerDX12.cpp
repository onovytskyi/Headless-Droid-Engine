#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/ShaderManagerDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Debug/Log.h"
#include "Engine/Framework/File/Utils.h"
#include "Engine/Framework/Memory/AllocationScope.h"
#include "Engine/Framework/Memory/FrameworkMemoryInterface.h"

namespace hd
{
    namespace gfx
    {
        ShaderManager::ShaderManager(mem::AllocationScope& allocationScope)
            : m_AllocationScope{ allocationScope }
            , m_FirstShaderHolder{}
        {

        }

        ShaderManager::~ShaderManager()
        {

        }

        hd::mem::Buffer& ShaderManager::GetShader(char8_t const* shaderName, char8_t const* entryPoint, char8_t const* profile, ShaderFlags flags)
        {
            mem::AllocationScope scratchScope{ mem::GetScratchAllocator() };
            str::String shaderNameString{ scratchScope, shaderName };
            str::String entryPointString{ scratchScope, entryPoint };
            str::String profileString{ scratchScope, profile };

            str::String cookedFilePath{ scratchScope };
            file::ConvertToCookedPathPrefixed(scratchScope, shaderNameString, entryPointString, cookedFilePath);

#if defined(HD_ENABLE_RESOURCE_COOKING)
            str::String shaderFilePath{ scratchScope };
            file::ConvertToShaderPath(scratchScope, shaderNameString, shaderFilePath);
#endif

            ShaderHolder* holder = m_FirstShaderHolder;
            while (holder)
            {
                if (holder->ShaderKey == cookedFilePath)
                {
                    break;
                }

                holder = holder->Next;
            }

            if (holder == nullptr)
            {
                holder = m_AllocationScope.AllocateObject<ShaderHolder>(m_AllocationScope);
                holder->ShaderKey.Assign(cookedFilePath.CStr());

#if defined(HD_ENABLE_RESOURCE_COOKING)
                if (flags.IsSet(ShaderFlagsBits::IgnoreCache) || file::DestinationOlder(shaderFilePath, cookedFilePath))
                {
                    CookShader(scratchScope, shaderFilePath, entryPointString, profileString, cookedFilePath, flags);
                }
#endif
                file::ReadWholeFile(scratchScope, cookedFilePath, holder->ShaderMicrocode);

                hdLogInfo(u8"Shader %:% loaded.", shaderNameString.CStr(), entryPointString.CStr());

                holder->Next = m_FirstShaderHolder;
                m_FirstShaderHolder = holder;
            }

            return holder->ShaderMicrocode;
        }

#if defined(HD_ENABLE_RESOURCE_COOKING)
        void ShaderManager::CookShader(mem::AllocationScope& scratch, str::String const& shaderFilePath, str::String const& entryPoint, str::String const& profile, 
            str::String const& cookedFilePath, ShaderFlags flags)
        {
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

            str::String pdbFilePath{ scratch };
            str::String pdbExtension{ scratch, u8".pdb" };
            file::ReplaceExtension(scratch, cookedFilePath, pdbExtension, pdbFilePath);

            HRESULT result;
            uint32_t codePage = CP_UTF8;
            ComPtr<IDxcBlobEncoding> sourceBlob;
            result = s_DXCRuntime.Utils->LoadFile(shaderFilePath.AsWide(scratch), &codePage, &sourceBlob);
            hdEnsure(result, u8"Cannot create shader blob from file %. Check if file exist.");

            DxcBuffer sourceBuffer{};
            sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
            sourceBuffer.Size = sourceBlob->GetBufferSize();
            BOOL knownEncoding{};
            sourceBlob->GetEncoding(&knownEncoding, &sourceBuffer.Encoding);

            std::vector<LPCWSTR> arguments;

            // Entrypoint
            arguments.push_back(L"-E");
            arguments.push_back(entryPoint.AsWide(scratch));

            // Shader model
            arguments.push_back(L"-T");
            arguments.push_back(profile.AsWide(scratch));

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
                arguments.push_back(DXC_ARG_DEBUG);
                arguments.push_back(L"-Fd");
                arguments.push_back(pdbFilePath.AsWide(scratch));
            }

            ComPtr<IDxcResult> operationResult;
            result = s_DXCRuntime.Compiler->Compile(&sourceBuffer, arguments.data(), uint32_t(arguments.size()), s_DXCRuntime.IncludeHandler, IID_PPV_ARGS(&operationResult));
            if (FAILED(result) || !operationResult)
            {
                hdAssert(false, u8"Shader % % % failed to compile. Operation result is not available.", shaderFilePath.CStr(), entryPoint.CStr(), profile.CStr());
            }

            ComPtr<IDxcBlobUtf16> dummyName;

            ComPtr<IDxcBlobUtf8> errorBlob;
            operationResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errorBlob), dummyName.ReleaseAndGetAddressOf());
            if (errorBlob && errorBlob->GetStringLength() > 0)
            {
                hdAssert(false, u8"Shader % % % failed to compile.\n Error: %.", shaderFilePath.CStr(), entryPoint.CStr(), profile.CStr(), 
                    reinterpret_cast<char8_t const*>(errorBlob->GetStringPointer()));
            }

            hdEnsure(operationResult->GetStatus(&result));

            ComPtr<IDxcBlob> shaderBlob;
            operationResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), dummyName.ReleaseAndGetAddressOf());
            if (shaderBlob && shaderBlob->GetBufferSize() > 0)
            {
                mem::Buffer buffer{ scratch };
                buffer.Assign(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

                file::WriteWholeFile(cookedFilePath, buffer);
            }
            else
            {
                hdAssert(false, u8"Shader % % % doesn't have shader microcode artifact.\n Error: %.", shaderFilePath.CStr(), entryPoint.CStr(), profile.CStr(), 
                    reinterpret_cast<char8_t const*>(errorBlob->GetStringPointer()));
            }

            if (flags.IsSet(ShaderFlagsBits::GenerateSymbols))
            {
                ComPtr<IDxcBlob> pdbBlob;
                operationResult->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdbBlob), dummyName.ReleaseAndGetAddressOf());
                if (pdbBlob && pdbBlob->GetBufferSize() > 0)
                {
                    size_t narrowSize = str::SizeAsNarrow(dummyName->GetStringPointer());
                    char8_t* pdbFileName = reinterpret_cast<char8_t*>(scratch.AllocateMemory(narrowSize, alignof(char8_t)));
                    str::ToNarrow(dummyName->GetStringPointer(), pdbFileName, narrowSize);

                    str::String pdbFileNameString{ scratch, pdbFileName };

                    mem::Buffer buffer{ scratch };
                    buffer.Assign(pdbBlob->GetBufferPointer(), pdbBlob->GetBufferSize());

                    file::WriteWholeFile(pdbFileNameString, buffer);
                }
            }

            hdLogInfo(u8"Shader % cooked to % % debug symbols.", shaderFilePath.CStr(), cookedFilePath.CStr(), flags.IsSet(ShaderFlagsBits::GenerateSymbols) ? u8"with" : u8"without");
        }
#endif

        ShaderManager::ShaderHolder::ShaderHolder(mem::AllocationScope& allocationScope)
            : ShaderKey{ allocationScope }
            , ShaderMicrocode{ allocationScope }
            , Next{}
        {

        }
    }
}

#endif