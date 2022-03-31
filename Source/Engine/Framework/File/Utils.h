#pragma once

#include "Engine/Framework/Memory/PlainDataArray.h"

namespace hd
{
    namespace mem
    {
        class VirtualBuffer;
    }

    namespace file
    {
        void ReadWholeFile(std::pmr::u8string const& filePath, PlainDataArray<std::byte>& output);
        void ReadWholeFile(std::pmr::u8string const& filePath, mem::VirtualBuffer& output);
        void WriteWholeFile(std::pmr::u8string const& filePath, PlainDataArray<std::byte> const& data);
        void WriteWholeFile(std::pmr::u8string const& filePath, mem::VirtualBuffer const& data);
        void WriteWholeFile(std::pmr::u8string const& filePath, std::byte const* data, size_t size);

        void CreateDirectories(std::pmr::u8string const& dirPath);
        void GetDirectory(std::pmr::u8string const& filePath, std::pmr::u8string& output);

        bool FileExist(std::pmr::u8string const& filePath);
        bool DestinationOlder(std::pmr::u8string const& sourceFilePath, std::pmr::u8string const& destinationFilePath);

        void Merge(std::pmr::u8string const& left, std::pmr::u8string const& right, std::pmr::u8string& output);
        void GetFileBasename(std::pmr::u8string const& filePath, std::pmr::u8string& output);
        void ReplaceFilename(std::pmr::u8string const& filePath, std::pmr::u8string const& newName, std::pmr::u8string& output);
        void ReplaceExtension(std::pmr::u8string const& filePath, std::pmr::u8string const& newExtension, std::pmr::u8string& output);

        void ConvertToShaderPath(std::pmr::u8string const& path, std::pmr::u8string& output);
        void ConvertToMediaPath(std::pmr::u8string const& path, std::pmr::u8string& output);
        void ConvertToCookedPath(std::pmr::u8string const& path, std::pmr::u8string& output);
        void ConvertToCookedPathPrefixed(std::pmr::u8string const& path, std::pmr::u8string const& prefix, std::pmr::u8string& output);
    }
}