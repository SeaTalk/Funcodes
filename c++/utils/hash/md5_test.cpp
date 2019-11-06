#include <iostream>
#include <fstream>
#include <string>
#include "md5.h"

using namespace std;

int main() {
	fstream f("md5.cpp");
	if (f) {
		f.seekg(0, f.end);
		auto len = f.tellg();
		f.seekg(0, f.beg);
		char *buff = new char[len];
		f.read(buff, len);
		f.close();
		MD5 md5(string(buff, len));
		
		cout << "md5:" << md5.md5() << endl;
		delete []buff;
	}
	return 0;
}
