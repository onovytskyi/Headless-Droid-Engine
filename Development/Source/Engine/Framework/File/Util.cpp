#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/File/Util.h"

#include "Engine/Framework/Memory/AllocationScope.h"
#include "Engine/Framework/Memory/Buffer.h"
#include "Engine/Framework/Memory/FrameworkMemoryInterface.h"
#include "Engine/Framework/String/String.h"

//#TODO replace filesystem code with manual implementation
#include <filesystem>
#include "Engine/Foundation/String/StringConverter.h"

namespace hd
{
    namespace file
    {
        void ReadWholeFile(mem::AllocationScope& scratch, str::String const& filePath, mem::Buffer& output)
        {
            std::ifstream file{ filePath.AsWide(scratch), std::ios::binary | std::ios::ate };

            hdEnsure(file.good(), u8"Failed to open file %", filePath.CStr());

            std::streamsize size{ file.tellg() };
            file.seekg(0, std::ios::beg);

            output.Resize(size);
            file.read(output.GetDataAs<char*>(), output.GetSize());
            file.close();
        }

        void WriteWholeFile(str::String const& filePath, mem::Buffer const& data)
        {
            mem::AllocationScope scratchScope{ mem::GetScratchAllocator() };

            std::filesystem::create_directories(std::filesystem::path(filePath.AsWide(scratchScope)).parent_path());

            std::ofstream file{ filePath.AsWide(scratchScope), std::ios::binary | std::ios::trunc };

            hdEnsure(file.good(), u8"Failed to create file %", filePath.CStr());

            file.write(data.GetDataAs<const char*>(), data.GetSize());
            file.close();
        }

        void CreateDirectories(str::String const& dirPath)
        {
            mem::AllocationScope scratchScope{ mem::GetScratchAllocator() };

            std::filesystem::create_directories(std::filesystem::path(dirPath.AsWide(scratchScope)));
        }

        void GetDirectory(mem::AllocationScope& scratch, str::String const& filePath, str::String& output)
        {
            std::filesystem::path directory = std::filesystem::path(filePath.AsWide(scratch)).parent_path();

            size_t narrowSize = str::SizeAsNarrow(directory.c_str());
            char8_t* directoryString = reinterpret_cast<char8_t*>(scratch.AllocateMemory(narrowSize, alignof(char8_t)));
            str::ToNarrow(directory.c_str(), directoryString, narrowSize);

            output.Assign(directoryString);
        }

        bool FileExist(str::String const& filePath)
        {
            mem::AllocationScope scratchScope{ mem::GetScratchAllocator() };

            return std::filesystem::exists(filePath.AsWide(scratchScope));
        }

        bool DestinationOlder(str::String const& sourceFilePath, str::String const& destinationFilePath)
        {
            mem::AllocationScope scratchScope{ mem::GetScratchAllocator() };

            hdEnsure(FileExist(sourceFilePath), u8"Failed to open file %", sourceFilePath.CStr());
            if (!FileExist(destinationFilePath))
            {
                return true;
            }

            auto sourceModificationTime = std::filesystem::last_write_time(sourceFilePath.AsWide(scratchScope));
            auto destinationModificationTime = std::filesystem::last_write_time(destinationFilePath.AsWide(scratchScope));

            return sourceModificationTime > destinationModificationTime;
        }

        void Merge(mem::AllocationScope& scratch, str::String const& left, str::String const& right, str::String& output)
        {
            std::filesystem::path pathToFile(left.AsWide(scratch));
            pathToFile.append(right.AsWide(scratch));
            if (!pathToFile.is_absolute())
            {
                pathToFile = std::filesystem::weakly_canonical(std::filesystem::absolute(pathToFile));
            }

            size_t narrowSize = str::SizeAsNarrow(pathToFile.c_str());
            char8_t* pathString = reinterpret_cast<char8_t*>(scratch.AllocateMemory(narrowSize, alignof(char8_t)));
            str::ToNarrow(pathToFile.c_str(), pathString, narrowSize);

            output.Assign(pathString);
        }

        void GetFileBasename(mem::AllocationScope& scratch, str::String const& filePath, str::String& output)
        {
            std::filesystem::path pathToFile(filePath.AsWide(scratch));
            std::filesystem::path fileBasename = pathToFile.stem();

            size_t narrowSize = str::SizeAsNarrow(fileBasename.stem().c_str());
            char8_t* basename = reinterpret_cast<char8_t*>(scratch.AllocateMemory(narrowSize, alignof(char8_t)));
            str::ToNarrow(fileBasename.c_str(), basename, narrowSize);

            output.Assign(basename);
        }

        void ReplaceFilename(mem::AllocationScope& scratch, str::String const& filePath, str::String const& newName, str::String& output)
        {
            std::filesystem::path pathToFile(filePath.AsWide(scratch));

            std::filesystem::path parentPath = pathToFile.parent_path();
            parentPath.append(newName.AsWide(scratch));

            size_t narrowSize = str::SizeAsNarrow(parentPath.c_str());
            char8_t* filename = reinterpret_cast<char8_t*>(scratch.AllocateMemory(narrowSize, alignof(char8_t)));
            str::ToNarrow(parentPath.c_str(), filename, narrowSize);

            output.Assign(filename);
        }

        void ReplaceExtension(mem::AllocationScope& scratch, str::String const& filePath, str::String const& newExtension, str::String& output)
        {
            std::filesystem::path pathToFile(filePath.AsWide(scratch));
            pathToFile.replace_extension(newExtension.AsWide(scratch));

            size_t narrowSize = str::SizeAsNarrow(pathToFile.c_str());
            char8_t* filename = reinterpret_cast<char8_t*>(scratch.AllocateMemory(narrowSize, alignof(char8_t)));
            str::ToNarrow(pathToFile.c_str(), filename, narrowSize);

            output.Assign(filename);
        }

        void ConvertToShaderPath(mem::AllocationScope& scratch, str::String const& path, str::String& output)
        {
            str::String cookedPath{ scratch, cfg::ShadersPath() };
            return Merge(scratch, cookedPath, path, output);
        }

        void ConvertToCookedPath(mem::AllocationScope& scratch, str::String const& path, str::String& output)
        {
            str::String cookedPath{ scratch, cfg::CookedFilePath() };
            return Merge(scratch, cookedPath, path, output);
        }

        void ConvertToCookedPathPrefixed(mem::AllocationScope& scratch, str::String const& path, str::String const& prefix, str::String& output)
        {
            str::String prefixedFileName{ scratch, prefix.CStr() };
            str::String fileBaseName{ scratch };
            GetFileBasename(scratch, path, fileBaseName);

            prefixedFileName = prefixedFileName + u8"_" + fileBaseName + u8".bin";
            str::String prefixedPath{ scratch };
            ReplaceFilename(scratch, path, prefixedFileName, prefixedPath);

            return ConvertToCookedPath(scratch, prefixedPath, output);
        }
    }
}