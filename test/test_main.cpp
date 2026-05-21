#include <map>
#include <memory>
#include <vector>

#include <serializen/json_deserialize.hpp>
#include <serializen/json_serialize.hpp>
#include <serializen/annotations.hpp>

struct data
{
    int a = 0;
    std::string b;
    std::vector<data> c;
    std::shared_ptr<int> d;

    bool operator==(const data& rhs) const
    {
        bool ok = a == rhs.a && b == rhs.b && c == rhs.c;
        if (d == nullptr && rhs.d == nullptr)
        {
            ok &= true;
        }
        else if (d == nullptr || rhs.d == nullptr && d.get() != rhs.d.get())
        {
            ok &= false;
        }
        else
        {
            ok &= *d == *rhs.d;
        }

        return ok;
    }
};

int main()
{
    data d;
    d.a = 1;
    d.b = "hello";
    d.c = {data{.a = 5, .b = "ffff", .c = {}, .d = std::make_shared<int>(1111)}};
    d.d = nullptr;
    std::stringstream buffer;
    auto result = serializen::json::serialize(d);
    printf("%s\n", buffer.str().c_str());

    data u;
    serializen::json::deserialize(result, u);

    return !(d == u);
}
