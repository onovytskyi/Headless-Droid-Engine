#include "Engine/Config/Bootstrap.h"

#include "Engine/Foundation/Memory/Utils.h"

namespace hd
{
    namespace mem
    {
        size_t AlignBelow(size_t value, size_t alignment)
        {
            if (alignment == 0 || value == 0)
                return value;

            size_t result = ((value - 1) / alignment) * alignment;
            return result;
        }

        void* AlignBelow(void* pointer, size_t alignment)
        {
            return reinterpret_cast<void*>(AlignBelow(reinterpret_cast<size_t>(pointer), alignment));
        }

        size_t AlignAbove(size_t value, size_t alignment)
        {
            if (alignment == 0 || value == 0)
                return value;

            size_t result = (((value - 1) / alignment) + 1) * alignment;
            return result;
        }

        void* AlignAbove(void* pointer, size_t alignment)
        {
            return reinterpret_cast<void*>(AlignAbove(reinterpret_cast<size_t>(pointer), alignment));
        }
    }
}