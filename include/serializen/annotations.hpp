#pragma once

namespace serializen
{
    struct no_serialize_tag
    {
    };

    constexpr auto no_serialize = no_serialize_tag{};
}
