#pragma once

#include <meta>
#include <nlohmann/json.hpp>
#include <print>
#include <serializen/concepts.hpp>
#include <string_view>

namespace serializen::json
{
    template <typename T>
    void deserialize_within_context(T& obj_root, const nlohmann::json& json_root)
    {
        using namespace concepts;
        using clean_type = std::remove_cvref_t<T>;

        if constexpr (CPtr<clean_type>)
        {
            obj_root = nullptr;
            return;
        }

        if constexpr (Scalar<clean_type> || String<clean_type> || Bool<clean_type>)
        {
            obj_root = json_root.get<clean_type>();
        }
        else if constexpr (MapLike<T>)
        {
            for (const auto& [key, value] : json_root.items())
            {
                using map_value = decltype(obj_root)::mapped_type;
                map_value v = {};
                deserialize_within_context(v, value);

                obj_root[key] = std::move(v);
            }
        }
        else if constexpr (Tuple<T>)
        {
            size_t index = 0;
            template for (constexpr auto elem : std::meta::template_arguments_of(^^T))
            {
                auto& ref = std::get<index>(obj_root);
                deserialize_within_context(ref, json_root[index]);
            }
        }
        else if constexpr (AppendableCollection<clean_type>)
        {
            for (const auto& json_elem : json_root)
            {
                typename std::remove_cvref_t<decltype(obj_root)>::value_type collection_elem = {};
                deserialize_within_context(collection_elem, json_elem);
                obj_root.push_back(std::move(collection_elem));
            }
        }
        else if constexpr (FixedSizeArray<clean_type>)
        {
            for (size_t i = 0; i < std::size(obj_root); ++i)
            {
                auto& ref = obj_root[i];
                deserialize_within_context(ref, json_root[i]);
            }
        }
        else if constexpr (SmartPtr<clean_type>)
        {
            using elem_t = std::remove_cvref_t<decltype(obj_root)>::element_type;
            if (json_root.empty())
            {
                obj_root = nullptr;
            }
            else
            {
                obj_root = clean_type(new elem_t());
                deserialize_within_context(*obj_root, json_root);
            }
        }
        else if constexpr (Class<clean_type>)
        {
            constexpr auto context = std::meta::access_context::current();
            constexpr static auto members =
                std::define_static_array(std::meta::nonstatic_data_members_of(^^T, context));
            template for (constexpr auto member : members)
            {
                constexpr auto name = std::meta::identifier_of(member);
                if (!json_root.contains(name))
                {
                    std::print("Skipping non present JSON field '{}'", name);
                    continue;
                }
                // clang-format off
                deserialize_within_context(obj_root.[:member:], json_root[name]);
                // clang-format on
            }
        }
    }

    template <typename T>
    void deserialize(std::string_view json_text, T& obj_root)
    {
        const auto& json = nlohmann::json::parse(json_text);

        constexpr auto context = std::meta::access_context::current();
        constexpr static auto members = std::define_static_array(std::meta::nonstatic_data_members_of(^^T, context));
        template for (constexpr auto member : members)
        {
            constexpr auto name = std::meta::identifier_of(member);
            if (!json.contains(name))
            {
                std::print("Skipping non present JSON field '{}'", name);
                continue;
            }
            // clang-format off
            deserialize_within_context(obj_root.[:member:], json[name]);
            // clang-format on
        }
    }
}
