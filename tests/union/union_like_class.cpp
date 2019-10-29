#include <cstring>
using namespace std;
struct Student {
    using Gender = bool;
    Student(bool g, int a) : male(g), age(a) {}
    Gender male;
    int age;
};

class Singer {
public:
    enum Type {STUDENT, NATIVE, FOREIGNER};
    Singer(bool g, int a) : s(g, a) {
        t = STUDENT;
    }
    Singer(int i) : id(i) { t = NATIVE; }
    Singer(const char* n, int s) {
        int size = (s > 9) ? 9 : s;
        memcpy(name, n, size);
        name[size] = '\0';
        t = FOREIGNER;
    }
    ~Singer() {}
private:
    Type t;
    union {     // 匿名的非受限联合体
        Student s;
        int id;
        char name[10];
    };
};
int main() {
    Singer(true, 13);
    Singer(31217);
    Singer("J Michael", 9);
    return 0;
}