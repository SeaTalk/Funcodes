#include <string>
#include <vector>

struct Collector {
    template<typename T>
    static std::vector<std::string> Collect(T name) {
        return { name };
    }
    template<typename T, typename...Args>
    static std::vector<std::string> Collect(T name, Args&&...args) {
        std::vector<std::string> &&collects = Collect<Args...>(std::forward<Args>(args)...);
        collects.emplace_back(name);
        return collects;
    }
};

#define PARENS ()
#define ToStringImpl(arg) #arg
#define ToString(arg, ...) ToStringImpl(arg)
#define EXPAND(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) __VA_ARGS__
// #define EXPAND2(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
// #define EXPAND3(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
// #define EXPAND4(...) __VA_ARGS__
#define FOR_EACH(macro, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER(macro, a1, ...)                         \
  macro(a1) __VA_OPT__(,)                                              \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (macro, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER
#define EXPAND_STRINGS(...) FOR_EACH(ToString, __VA_ARGS__)
#define FeatureCollector(...) Collector::Collect(EXPAND_STRINGS(__VA_ARGS__))

int main() {
    // LOG("hello");
    FeatureCollector(abc, d, s, d, f, s, c, e, 12, adsf, 93, 012,
                     4, 4, ,2, 2, ad, f, s, d,f );
    return 0;
}
