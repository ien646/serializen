Sample project to test C++26 reflection and annotations for automatic serialization/deserialization.

Only a JSON serializer and deserializer (on top of nlohmann) is implemented.
There's probably a hundred edge cases not covered and ambiguous behavior relating to specialization selection. YMMV.

There's only one annotation that instructs serialization to skip it:
```
struct data
{
    int a = 0;
    std::string b;
    [[=serializen::no_serialize{}]]
    std::vector<double> c;
};
```
