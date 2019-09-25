#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

/*********************************
 *** get_next用来产生偏移量数组 **
 ********************************/
void get_next(string t, int *next)
{
	int len = t.length();
	next[0] = -1;
	int k = -1;
	int j = 0;
	while (j < len - 1)
	{
		// p[k]表示前缀，p[j]表示后缀
		if (k == -1 || t[j] == t[k])
		{
			++k;
			++j;
			next[j] = k;
		}
		else
		{
			k = next[k]; // 找到之前一次更小的匹配
		}
	}
}

/***************************************
 * Optimized get_next function
 ***************************************/
void get_nextval(string t, int *nextval)
{
	int len = t.length();
	nextval[0] = -1;
	int j = 0;
	int k = -1;
	while (j < len - 1)
	{
		if (k == -1 || t[j] == t[k])
		{
			++k;
			++j;
			if (t[j] != t[k])
			{
				nextval[j] = k;
			}
			else
			{
				nextval[j] = nextval[k];
			}
		}
		else
		{
			k = nextval[k];
		}
	}
}

/*********************************
 * kmp_search用来从s中找出第一次
 * 匹配t成功的index(从0开始)
 ********************************/
int kmp_search(string s, string t)
{
	int i = 0;
	int j = 0;
	int slen = s.length();
	int tlen = t.length();
	int *next = new int[tlen];
	//get_next(t, next);
	get_nextval(t, next);
	while (i < slen && j < tlen)
	{
		// 如果j==-1
		if (j == -1)
		{
			i++;
			j++;
		}
		else if (slen - i >= tlen - j)// 注意这里与原文代码不同
		{
			if (s[i] == t[j])
			{ // 当前字符匹配成功时
				i++;
				j++;
			}
			else
			{
				j = next[j];
			}
		}
		else
			break;
	}
	if (j > 0 && j == tlen) // j>0表示不是空串
		return i - j;
	else
		return -1; // 未匹配成功
}

/*
   int bm_search(string s, string t)
   {
   int slen = s.length();
   int tlen = t.length();
// 1. 首先建立字符串t中每个字符出现的位置的哈希表
unordered_map<char, vector<int> > ct;
unordered_map<char, vector<int> >::const_iterator iter = nullptr;
for(int i = 0; i < tlen; ++i)
{
iter = ct.find(t[i]);
if(iter == ct.end())
{
ct.insert(t[i], nullptr);
iter = ct.find(t[i]);
}
vector<int> v = iter->second;
if(v == nullptr)
{
vector<int> t;
v = t;
ct.insert(t[i],v);
}
v.push_back(i);
}

// 
}
 */

int main()
{
	cout << kmp_search("", "") << endl;
	cout << kmp_search("bbc abcdab abcdabdcabde", "abcdabd") << endl;
	return 0;
}
