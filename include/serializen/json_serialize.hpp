#pragma once

#include <algorithm>
#include <format>
#include <meta>
#include <sstream>

#include "annotations.hpp"
#include "concepts.hpp"

namespace serializen::json
{
    namespace bits
    {
        // clang-format off
        template <concepts::AppendableCollection T> void serialize_collection_items(const T& obj, std::stringstream& buffer);
        template <concepts::AppendableCollection T> void serialize_collection_inline(const T& obj, std::stringstream& buffer);
        template <concepts::AppendableCollection T> void serialize_collection_named(const T& obj, std::stringstream& buffer, const std::string& name);

        template <concepts::MapLike T> void serialize_map_inline(const T& obj, std::stringstream& buffer);
        template <concepts::MapLike T> void serialize_map_named(const T& obj, std::stringstream& buffer, const std::string& name);

        template <concepts::Tuple T> void serialize_tuple_inline(const T& obj, std::stringstream& buffer);
        template <concepts::Tuple T> void serialize_tuple_named(const T& obj, std::stringstream& buffer, const std::string& name);

        template <concepts::Class T> void serialize_class_inline(const T& obj, std::stringstream& buffer);
        template <concepts::Class T> void serialize_class_named(const T& obj, std::stringstream& buffer, const std::string& name);

        template <typename T> void serialize_object_inline(const T& obj, std::stringstream& buffer);
        template <typename T> void serialize_object_named(const T& obj, std::stringstream& buffer, const std::string& name);
        // clang-format on

        template <concepts::AppendableCollection T>
        void serialize_collection_inline(const T& obj, std::stringstream& buffer)
        {
            buffer << "[";
            serialize_collection_items(obj, buffer);
            buffer << "]";
        }

        template <concepts::AppendableCollection T>
        void serialize_collection_named(const T& obj, std::stringstream& buffer, const std::string& name)
        {
            buffer << "\"" << name << "\":";
            serialize_collection_inline(obj, buffer);
        }

        template <concepts::AppendableCollection T>
        void serialize_collection_items(const T& obj, std::stringstream& buffer)
        {
            bool first = true;
            for (const auto& item : obj)
            {
                if (!first)
                {
                    buffer << ",";
                }
                serialize_object_inline(item, buffer);
                if (first)
                {
                    first = false;
                }
            }
        }

        template <concepts::MapLike T>
        void serialize_map_inline(const T& obj, std::stringstream& buffer)
        {
            buffer << "{";
            bool first = true;
            for (const auto& [k, v] : obj)
            {
                if (!first)
                {
                    buffer << ",";
                }
                buffer << std::format("\"{}\":", k);
                serialize_object_inline(v, buffer);
                if (first)
                {
                    first = false;
                }
            }
            buffer << "}";
        }

        template <concepts::MapLike T>
        void serialize_map_named(const T& obj, std::stringstream& buffer, const std::string& name)
        {
            buffer << "\"" << name << "\":";
            serialize_map_inline(obj, buffer);
        }

        template <concepts::Tuple T>
        void serialize_tuple_inline(const T& obj, std::stringstream& buffer)
        {
            buffer << "[";
            bool first = true;
            template for (const auto& item : obj)
            {
                if (!first)
                {
                    buffer << ",";
                }
                serialize_object_inline(item, buffer);
                if (first)
                {
                    first = false;
                }
            }
            buffer << "]";
        }

        template <concepts::Tuple T>
        void serialize_tuple_named(const T& obj, std::stringstream& buffer, const std::string& name)
        {
            buffer << "\"" << name << "\":";
            serialize_tuple_inline(obj, buffer);
        }

        template <concepts::Class T>
        void serialize_class_inline(const T& obj, std::stringstream& buffer)
        {
            buffer << "{";
            bool first = true;

            constexpr auto ctx = std::meta::access_context::current();
            constexpr static auto members = std::define_static_array(std::meta::nonstatic_data_members_of(^^T, ctx));
            template for (constexpr auto member : members)
            {
                bool skip = false;
                constexpr static auto annotations = std::define_static_array(std::meta::annotations_of(member));
                template for (constexpr auto annotation : annotations)
                {
                    constexpr auto annotation_type = std::meta::remove_cv(std::meta::type_of(annotation));
                    if constexpr (std::meta::is_same_type(annotation_type, ^^no_serialize_tag))
                    {
                        skip = true;
                    }
                }
                if (skip)
                {
                    continue;
                }

                if (!first)
                {
                    buffer << ",";
                }

                constexpr auto name = std::meta::identifier_of(member);
                buffer << std::format("\"{}\":", name);
                // clang-format off
                const auto& member_data = obj.[:member:];
                // clang-format on
                serialize_object_inline(member_data, buffer);

                if (first)
                {
                    first = false;
                }
            }

            buffer << "}";
        }

        template <concepts::Class T>
        void serialize_class_named(const T& obj, std::stringstream& buffer, const std::string& name)
        {
            buffer << "\"" << name << "\":";
            serialize_class_inline(obj, buffer);
        }

        template <typename T>
        void serialize_object_inline(const T& obj, std::stringstream& buffer)
        {
            using clean_type = std::remove_reference_t<T>;
            using namespace concepts;

            if constexpr (CPtr<clean_type>)
            {
                static_assert(false, "Cannot serialize C-pointer types");
            }

            if constexpr (Scalar<clean_type>)
            {
                buffer << obj;
            }
            else if constexpr (String<clean_type>)
            {
                buffer << std::format("\"{}\"", obj);
            }
            else if constexpr (MapLike<clean_type>)
            {
                serialize_map_inline(obj, buffer);
            }
            else if constexpr (Tuple<clean_type>)
            {
                serialize_tuple_inline(obj, buffer);
            }
            else if constexpr (AppendableCollection<clean_type> || FixedSizeArray<clean_type>)
            {
                serialize_collection_inline(obj, buffer);
            }
            else if constexpr (SmartPtr<clean_type>)
            {
                if (obj.get() != nullptr)
                {
                    serialize_object_inline(*obj, buffer);
                }
                else
                {
                    buffer << "{}";
                }
            }
            else if constexpr (Class<clean_type>)
            {
                serialize_class_inline(obj, buffer);
            }
            else
            {
                static_assert(false, "Unsupported type");
            }
        }

        template <typename T>
        void serialize_object_named(const T& obj, std::stringstream& buffer, const std::string& name)
        {
            using clean_type = std::remove_reference_t<T>;
            using namespace concepts;

            if constexpr (CPtr<clean_type>)
            {
                static_assert(false, "Cannot serialize C-pointer types");
            }

            if constexpr (Scalar<clean_type>)
            {
                buffer << std::format(R"("{}":{})", name, obj);
            }
            else if constexpr (String<clean_type>)
            {
                buffer << std::format(R"("{}":"{}")", name, obj);
            }
            else if constexpr (MapLike<clean_type>)
            {
                serialize_map_named(obj, buffer, name);
            }
            else if constexpr (Tuple<clean_type>)
            {
                serialize_tuple_named(obj, buffer, name);
            }
            else if constexpr (AppendableCollection<clean_type> || FixedSizeArray<clean_type>)
            {
                serialize_collection_named(obj, buffer, name);
            }
            else if constexpr (SmartPtr<clean_type>)
            {
                if (obj.get() != nullptr)
                {
                    serialize_object_named(*obj, buffer, name);
                }
                else
                {
                    buffer << "{}";
                }
            }
            else if constexpr (Class<clean_type>)
            {
                serialize_class_named(obj, buffer, name);
            }
        }
    }

    template <typename T>
    std::string serialize(const T& obj)
    {
        using clean_type = std::remove_cvref_t<T>;
        using namespace concepts;

        std::stringstream buffer;
        if constexpr (AppendableCollection<clean_type> || MapLike<clean_type> || Class<clean_type>)
        {
            bits::serialize_object_inline(obj, buffer);
        }
        else
        {
            buffer << "{\"unnamed\":";
            serialize_object_inline(obj, buffer);
            buffer << "}";
        }
        return buffer.str();
    }
}
