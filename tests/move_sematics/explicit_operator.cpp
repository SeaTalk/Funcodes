
class ConvertTo {};
class Convertable {
 public:
    explicit operator ConvertTo() const {   // Convertable 转换到ConvertTo类型的类型转换符
        return ConvertTo();
    }
};

void Func(ConvertTo ct) {}
void test() {
    Convertable c;
    ConvertTo ct(c);
    ConvertTo ct2 = c;  // 拷贝构造， 编译失败
    ConvertTo ct3 = static_cast<ConvertTo>(c);
    Func(c);    // 拷贝构造，编译失败
}