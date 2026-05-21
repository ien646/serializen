#pragma once

namespace serializen::json
{
    namespace concepts
    {
        template <typename T>
        concept AppendableCollection = requires(T t, typename T::value_type v)
        {
            typename T::value_type;
            { *t.begin()++ };
            { t.push_back(v) };
        };

        template <typename T>
        concept FixedSizeArray = std::is_array_v<std::remove_cvref_t<T>> || requires(T t, size_t idx)
        {
            !t.resize();
            !t.push_back();
            t[idx] || t.at(idx);
            t.size();
        };

        template <typename T>
        concept MapLike = requires(T t)
        {
            typename T::key_type;
            typename T::mapped_type;
            { t.size() } -> std::convertible_to<std::size_t>;
            *t.begin();
            { t.begin()->first } -> std::convertible_to<typename T::key_type>;
            { t.begin()->second } -> std::convertible_to<typename T::mapped_type>;
        };

        template <typename T>
        concept Tuple = requires(T t)
        {
            std::tuple_size_v<T>;
            std::get<0>(t);
        };

        template <typename T>
        concept CPtr = std::is_pointer_v<std::remove_cvref_t<T>>;

        template <typename T>
        concept SmartPtr = requires(T t)
        {
            typename T::element_type;
            { *t } -> std::convertible_to<typename T::element_type>;
            t.get();
        };

        template <typename T>
        concept Class = std::is_class_v<std::remove_cvref_t<T>>;

        template <typename T>
        concept String = std::is_same_v<std::remove_cvref_t<T>, std::string>;

        template <typename T>
        concept Bool = std::is_same_v<std::remove_cvref_t<T>, bool>;

        template <typename T>
        concept Scalar = std::is_scalar_v<std::remove_cvref_t<T>>;
    }
}
