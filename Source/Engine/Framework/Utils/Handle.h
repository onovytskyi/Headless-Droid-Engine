#pragma once

namespace hd
{
    namespace util
    {
        template<typename Tag, typename Type, Type DefaultValue>
        class Handle
        {
        public:
            static Handle Invalid();

            Handle();
            explicit Handle(Type value);

            explicit operator Type() const;

            bool operator==(Handle other);
            bool operator!=(Handle other);

        private:
            Type m_Value;
        };




    }
}

#include "Engine/Framework/Utils/Handle.hpp"