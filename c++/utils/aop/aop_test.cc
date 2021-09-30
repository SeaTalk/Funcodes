
#include <type_traits>
#include <functional>
#include <chrono>
#include <iostream>

#define HAS_MEMBER(member) \
template<typename T, typename ...Args> struct has_member_##member {	    \
 private: \
    template<typename U> \
    static auto check(int) -> decltype(std::declval<U>().member(std::declval<Args>()...), std::true_type()); \
    template<typename U>  static std::false_type check(...);    \
 public: \
    enum {value = std::is_same<decltype(check<T>(0)), std::true_type>::value}; \
};\


HAS_MEMBER(Foo)
HAS_MEMBER(Before)
HAS_MEMBER(After)


class NonCopyable {
    public:
        NonCopyable(const NonCopyable& n) = delete; // deleted
        NonCopyable& operator=(const NonCopyable& n) = delete; // deleted
        NonCopyable() = default; // available
};

template<typename Func, typename ...Args>
struct Aspect : NonCopyable {
    Aspect(Func &&f) : m_func(std::forward<Func>(f)) { }

    template<typename T>
    typename std::enable_if<has_member_Before<T, Args...>::value && has_member_After<T, Args...>::value>::type
    Invoke(Args&&... args, T&& aspect) {
        aspect.Before(std::forward<Args>(args)...);
        m_func(std::forward<Args>(args)...);
        aspect.After(std::forward<Args>(args)...);
    }

    template<typename T>
    typename std::enable_if<has_member_Before<T, Args...>::value && !has_member_After<T, Args...>::value>::type
    Invoke(Args&&... args, T&& aspect) {
        aspect.Before(std::forward<Args>(args)...);
        m_func(std::forward<Args>(args)...);
    }

    template<typename T>
    typename std::enable_if<!has_member_Before<T, Args...>::value && has_member_After<T, Args...>::value>::type
    Invoke(Args&&... args, T&& aspect) {
        m_func(std::forward<Args>(args)...);
        aspect.After(std::forward<Args>(args)...);
    }

    template<typename Head, typename... Tail>
    void Invoke(Args&&... args, Head&& headAspect, Tail&&... tailAspect) {
        headAspect.Before(std::forward<Args>(args)...);
        Invoke(std::forward<Args>(args)..., std::forward<Tail>(tailAspect)...);
        headAspect.After(std::forward<Args>(args)...);
    }

 private:
    Func m_func; //被织入的函数
};

template<typename... AP, typename... Args, typename Func>
void Invoke(Func&& f, Args&&... args) {
    Aspect<Func, Args...> asp(std::forward<Func>(f));
    asp.Invoke(std::forward<Args>(args)..., AP()...);
}


struct TimeElapsedAspect {
    void Before(int i, int j) {
        m_lastTime = std::chrono::high_resolution_clock::now();
    }
    void After(int i, int j) {
        std::cout << "time elapsed:" << std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - m_lastTime
        ).count() << std::endl;
    }

 private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;
};

struct LoggingAspect {
    void Before(int i, int j) {
        std::cout << "entering" << std::endl;
    }
    void After(int i, int j) {
        std::cout << "leaving" << std::endl;
    }
};

void foo(int a, int b) {
    std::cout << "real HT func: " << (a + b) << std::endl;
}

int main() {
    Invoke<LoggingAspect, TimeElapsedAspect>(&foo, 1, 2); //织入方法
std::cout <<"-----------------------"<< std::endl;
    Invoke<TimeElapsedAspect, LoggingAspect>(&foo, 1, 3);
    return 0;
}