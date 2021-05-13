namespace hd
{
    namespace util
    {
        template<typename Tag, typename Type, Type DefaultValue>
        inline Handle<Tag, Type, DefaultValue> Handle<Tag, Type, DefaultValue>::Invalid()
        {
            return Handle();
        }

        template<typename Tag, typename Type, Type DefaultValue>
        inline Handle<Tag, Type, DefaultValue>::Handle()
            : m_Value(DefaultValue)
        {

        }

        template<typename Tag, typename Type, Type DefaultValue>
        inline Handle<Tag, Type, DefaultValue>::Handle(Type value)
            : m_Value(value)
        {

        }

        template<typename Tag, typename Type, Type DefaultValue>
        inline Handle<Tag, Type, DefaultValue>::operator Type() const
        {
            return m_Value;
        }

        template<typename Tag, typename Type, Type DefaultValue>
        inline bool Handle<Tag, Type, DefaultValue>::operator==(Handle<Tag, Type, DefaultValue> other)
        {
            return m_Value == other.m_Value;
        }

        template<typename Tag, typename Type, Type DefaultValue>
        inline bool Handle<Tag, Type, DefaultValue>::operator!=(Handle<Tag, Type, DefaultValue> other)
        {
            return m_Value != other.m_Value;
        }

    }
}
