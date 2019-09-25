#include "charactor.h"
#include <sstream>
#include <cstdio>
#include <iostream>
//#include <>

namespace charactor_match
{

	bool CharactorMatcher::bigEndian = isBigEndian();

	bool CharactorMatcher::preProcessASCII(const string &str, vector<charactor> &chars)
	{
		if (valid)
		{
			string::size_type len = str.length();
			for ( int i=0; i<len; ++i)
			{
				chars.push_back(charactor(1, str[i]));
			}
			return true;
		}
		return false;
	}

	/**
	 * A Charactor encoded by GBK take 2 bytes at most.
	 * When using a single byte , the Charactor is same as ASCII, and the value of this byte is less than 0x80
	 *
	 */
	bool CharactorMatcher::preProcessGBK(const string &str, vector<charactor> &chars)
	{
		if(valid)
		{
			std::stringstream ss;
			ss << str;
			int tmp=0, value=0, n=0;
			char *cs = (char *)&value;
			try
			{
				if(bigEndian)
				{

					while ((tmp=ss.peek()) != EOF)
					{
						if(tmp < 0x80)
						{
							ss.get(cs,1);
							n=1;
						}
						else
						{
							ss.get(cs,2);
							n=2;
						}
						chars.push_back(charactor(n,value));
						value = 0;
					}
				}
				else
				{
					tmp = ss.peek();
					bool is_first = true;
					while(tmp!=EOF)
					{
						if(tmp < 0x80)
						{
							if(is_first)
							{
								ss.read(cs,1);
								is_first = false;
							}
							else
							{
								chars.push_back(charactor(1,cs[0]));
								//ss.read(cs,1);
								is_first = true;
							}
							tmp = ss.peek();
						}
						else
						{
							if(is_first)
							{
								std::cout << "wrong GBK encoding string" <<std::endl;
								return false;
							}
							else
							{
								is_first=true;
								cs[1] = cs[0];
								ss.read(cs,1);
								chars.push_back(charactor(2,value));
							}
							tmp = ss.peek();
						}
					}
					if(tmp == EOF && is_first==false)
					{
						chars.push_back(charactor(1,cs[0]));
					}
				}

				return true;
			}
			catch(...)
			{
				return false;
			}

		}
		return false;
	}


	CharactorMatcher::CharactorMatcher(const string search_str,const string match_str, charactor_match::encodingType type)
	{
			valid = (this->search_str = search_str) != "";
			valid = valid && (this->match_str = match_str)!="";

			switch(type)
			{
				case charactor_match::ASCII:
					valid = valid
					&& preProcessASCII(match_str, match_chars)
					&& preProcessASCII(search_str, search_chars);
					break;
				case charactor_match::GBK:
					valid = valid
					&& preProcessGBK(match_str, match_chars)
					&& preProcessGBK(search_str, search_chars);
					break;
				case charactor_match::UTF8:
					break;
				default:
					valid =false;
					break;
			}
	}

	void getNextVal(vector<charactor> &t, size_t *nextVal)
	{
		size_t len= t.size();
		nextVal[0] = -1;
		size_t j = 0;
		size_t k = -1;
		while(j < len - 1)
		{
			if (k==-1 || t[j] == t[k])
			{
				++k;
				++j;
				nextVal[j] = k;
			}
			else
			{
				k = nextVal[k];
			}
		}
	}

	bool CharactorMatcher::KMPSearch(size_t &pos)
	{
		int i = 0, j = 0;
		int sLen = search_chars.size(), mLen = match_chars.size();
		size_t *next = new size_t[mLen];
		getNextVal(match_chars, next);

		while( i < sLen && j < mLen)
		{
			if (j==-1)
			{
				++i;
				++j;
			}
			else if(sLen - i >= mLen - j)
			{
				if ( search_chars[i] == match_chars[j])
				{
					++i;
					++j;

				}
				else
				{
					j = next[j];
				}
			}
			else
				break;
		}
		delete next;

		if(j>0 && j==mLen)
		{
			pos = i - j;
			return true;
		}
		else
		{
			pos =  -1;
			return false;
		}



	}
};
