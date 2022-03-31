#include "Config/Bootstrap.h"

#include "Framework/File/Utils.h"

#include "Debug/Assert.h"
#include "Foundation/String/StringConverter.h"
#include "Framework/Memory/FrameworkMemoryInterface.h"
#include "Framework/Memory/VirtualBuffer.h"

//#TODO replace filesystem code with manual implementation
#include <filesystem>

namespace hd
{
    namespace file
    {
        void ReadWholeFile(std::pmr::u8string const& filePath, PlainDataArray<std::byte>& output)
        {
            std::pmr::wstring wideFilePath{ &Scratch() };
            ToWide(filePath, wideFilePath);

            std::ifstream file{ wideFilePath.c_str(), std::ios::binary | std::ios::ate};

            hdEnsure(file.good(), u8"Failed to open file %", filePath.c_str());

            std::streamsize size{ file.tellg() };
            file.seekg(0, std::ios::beg);

            output.Resize(size);
            file.read(reinterpret_cast<char*>(output.Data()), output.Size());
            file.close();
        }

        void ReadWholeFile(std::pmr::u8string const& filePath, VirtualBuffer& output)
        {
            std::pmr::wstring wideFilePath{ &Scratch() };
            ToWide(filePath, wideFilePath);

            std::ifstream file{ wideFilePath.c_str(), std::ios::binary | std::ios::ate};

            hdEnsure(file.good(), u8"Failed to open file %", filePath.c_str());

            std::streamsize size{ file.tellg() };
            file.seekg(0, std::ios::beg);

            output.Resize(size);
            file.read(output.GetDataAs<char*>(), output.GetSize());
            file.close();
        }

        void WriteWholeFile(std::pmr::u8string const& filePath, PlainDataArray<std::byte> const& data)
        {
            WriteWholeFile(filePath, data.Data(), data.Size());
        }

        void WriteWholeFile(std::pmr::u8string const& filePath, VirtualBuffer const& data)
        {
            WriteWholeFile(filePath, data.GetData(), data.GetSize());
        }

        void WriteWholeFile(std::pmr::u8string const& filePath, std::byte const* data, size_t size)
        {
            ScopedScratchMemory scopedScratch{};

            std::pmr::wstring wideFilePath{ &Scratch() };
            ToWide(filePath, wideFilePath);

            std::filesystem::create_directories(std::filesystem::path(wideFilePath).parent_path());

            std::ofstream file{ wideFilePath.c_str(), std::ios::binary | std::ios::trunc};

            hdEnsure(file.good(), u8"Failed to create file %", filePath.c_str());

            file.write(reinterpret_cast<char const*>(data), size);
            file.close();
        }

        void CreateDirectories(std::pmr::u8string const& dirPath)
        {
            ScopedScratchMemory scopedScratch{};

            std::pmr::wstring wideDirPath{ &Scratch() };
            ToWide(dirPath, wideDirPath);

            std::filesystem::create_directories(std::filesystem::path(wideDirPath));
        }

        void GetDirectory(std::pmr::u8string const& filePath, std::pmr::u8string& output)
        {
            ScopedScratchMemory scopedScratch{};

            std::pmr::wstring wideFilePath{ &Scratch() };
            ToWide(filePath, wideFilePath);

            std::filesystem::path directory = std::filesystem::path(wideFilePath).parent_path();

            ToNarrow(directory.native(), output);
        }

        bool FileExist(std::pmr::u8string const& filePath)
        {
            ScopedScratchMemory scopedScratch{};

            std::pmr::wstring wideFilePath{ &Scratch() };
            ToWide(filePath, wideFilePath);

            return std::filesystem::exists(wideFilePath);
        }

        bool DestinationOlder(std::pmr::u8string const& sourceFilePath, std::pmr::u8string const& destinationFilePath)
        {
            hdEnsure(FileExist(sourceFilePath), u8"Failed to open file %", sourceFilePath.c_str());
            if (!FileExist(destinationFilePath))
            {
                return true;
            }

            ScopedScratchMemory scopedScratch{};

            std::pmr::wstring wideSourceFilePath{ &Scratch() };
            ToWide(sourceFilePath, wideSourceFilePath);

            std::pmr::wstring wideDestFilePath{ &Scratch() };
            ToWide(destinationFilePath, wideDestFilePath);

            auto sourceModificationTime = std::filesystem::last_write_time(wideSourceFilePath);
            auto destinationModificationTime = std::filesystem::last_write_time(wideDestFilePath);

            return sourceModificationTime > destinationModificationTime;
        }

        void Merge(std::pmr::u8string const& left, std::pmr::u8string const& right, std::pmr::u8string& output)
        {
            std::pmr::wstring wideLeft{ &Scratch() };
            ToWide(left, wideLeft);

            std::pmr::wstring wideRight{ &Scratch() };
            ToWide(right, wideRight);

            std::filesystem::path pathToFile(wideLeft);
            pathToFile.append(wideRight);
            if (!pathToFile.is_absolute())
            {
                pathToFile = std::filesystem::weakly_canonical(std::filesystem::absolute(pathToFile));
            }

            ToNarrow(pathToFile.native(), output);
        }

        void GetFileBasename(std::pmr::u8string const& filePath, std::pmr::u8string& output)
        {
            std::pmr::wstring wideFilePath{ &Scratch() };
            ToWide(filePath, wideFilePath);

            std::filesystem::path pathToFile(wideFilePath);
            std::filesystem::path fileBasename = pathToFile.stem();

            ToNarrow(fileBasename.native(), output);
        }

        void ReplaceFilename(std::pmr::u8string const& filePath, std::pmr::u8string const& newName, std::pmr::u8string& output)
        {
            std::pmr::wstring wideFilePath{ &Scratch() };
            ToWide(filePath, wideFilePath);

            std::pmr::wstring wideNewName{ &Scratch() };
            ToWide(newName, wideNewName);

            std::filesystem::path pathToFile(wideFilePath);

            std::filesystem::path parentPath = pathToFile.parent_path();
            parentPath.append(wideNewName);

            ToNarrow(parentPath.native(), output);
        }

        void ReplaceExtension(std::pmr::u8string const& filePath, std::pmr::u8string const& newExtension, std::pmr::u8string& output)
        {
            std::pmr::wstring wideFilePath{ &Scratch() };
            ToWide(filePath, wideFilePath);

            std::pmr::wstring wideNewExtension{ &Scratch() };
            ToWide(newExtension, wideNewExtension);

            std::filesystem::path pathToFile(wideFilePath);
            pathToFile.replace_extension(wideNewExtension);

            ToNarrow(pathToFile.native(), output);
        }

        void ConvertToShaderPath(std::pmr::u8string const& path, std::pmr::u8string& output)
        {
            std::pmr::u8string cookedPath{ cfg::ShadersPath(), &Scratch() };
            return Merge(cookedPath, path, output);
        }

        void ConvertToMediaPath(std::pmr::u8string const& path, std::pmr::u8string& output)
        {
            std::pmr::u8string cookedPath{ cfg::MediaPath(), &Scratch() };
            return Merge(cookedPath, path, output);
        }

        void ConvertToCookedPath(std::pmr::u8string const& path, std::pmr::u8string& output)
        {
            std::pmr::u8string cookedPath{ cfg::CookedFilePath(), &Scratch() };
            return Merge(cookedPath, path, output);
        }

        void ConvertToCookedPathPrefixed(std::pmr::u8string const& path, std::pmr::u8string const& prefix, std::pmr::u8string& output)
        {
            std::pmr::u8string fileBaseName{ &Scratch() };
            GetFileBasename(path, fileBaseName);

            std::pmr::u8string prefixedFileName = prefix + u8"_" + fileBaseName + u8".bin";
            std::pmr::u8string prefixedPath{ &Scratch() };
            ReplaceFilename(path, prefixedFileName, prefixedPath);

            return ConvertToCookedPath(prefixedPath, output);
        }
    }
}