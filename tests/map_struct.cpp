#include <map>
#include <iostream>

using namespace std;

struct AStruct
{
public:
    AStruct():value(0){cout<<"init:"<<value<<endl;}
    AStruct(int a):value(a){cout<<"value:"<<value<<endl;}
private:
    int value;
};

int main()
{
    map<string, AStruct> vmap;
    vmap["Hello"] = AStruct(5);
}
