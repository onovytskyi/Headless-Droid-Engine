#pragma once

namespace hd
{
    namespace mem
    {
        class AllocationScope;
        class Buffer;
        class VirtualBuffer;
    }

    namespace str
    {
        class String;
    }

    namespace file
    {
        void ReadWholeFile(mem::AllocationScope& scratch, str::String const& filePath, mem::Buffer& output);
        void ReadWholeFile(mem::AllocationScope& scratch, str::String const& filePath, mem::VirtualBuffer& output);
        void WriteWholeFile(str::String const& filePath, mem::Buffer const& data);
        void WriteWholeFile(str::String const& filePath, mem::VirtualBuffer const& data);
        void WriteWholeFile(str::String const& filePath, std::byte const* data, size_t size);

        void CreateDirectories(str::String const& dirPath);
        void GetDirectory(mem::AllocationScope& scratch, str::String const& filePath, str::String& output);

        bool FileExist(str::String const& filePath);
        bool DestinationOlder(str::String const& sourceFilePath, str::String const& destinationFilePath);

        void Merge(mem::AllocationScope& scratch, str::String const& left, str::String const& right, str::String& output);
        void GetFileBasename(mem::AllocationScope& scratch, str::String const& filePath, str::String& output);
        void ReplaceFilename(mem::AllocationScope& scratch, str::String const& filePath, str::String const& newName, str::String& output);
        void ReplaceExtension(mem::AllocationScope& scratch, str::String const& filePath, str::String const& newExtension, str::String& output);

        void ConvertToShaderPath(mem::AllocationScope& scratch, str::String const& path, str::String& output);
        void ConvertToMediaPath(mem::AllocationScope& scratch, str::String const& path, str::String& output);
        void ConvertToCookedPath(mem::AllocationScope& scratch, str::String const& path, str::String& output);
        void ConvertToCookedPathPrefixed(mem::AllocationScope& scratch, str::String const& path, str::String const& prefix, str::String& output);
    }
}