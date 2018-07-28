class Base
{
public:
    int pub_mem;
    void pub_func();
protected:
    int prot_mem;
    void prot_func();
private:
    int priv_mem;
    void priv_func();
};

struct Pub_Derv : public Base
{
    int f() {return prot_mem;}
    int g() {return priv_mem;}
};

/**
 * private 继承的派生类，其继承自Base的所有成员都将私有的
 */
struct Priv_Derv : private Base
{
    int f() {return prot_mem;}
    int g() {return priv_mem;}
}

/**
 * protect继承的派生类，其继承自Base的所有#公有#成员在新定义的类中都是受保护的
*/
struct Prot_Derv : protected Base
{
    int f() {return prot_mem;}
    int g() {return priv_mem;}
}
