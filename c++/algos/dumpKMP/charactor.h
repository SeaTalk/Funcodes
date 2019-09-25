#ifndef _CHARACTOR_MATCH_H_
#define _CHARACTOR_MATCH_H

#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

namespace charactor_match
{
	struct charactor
	{
		uint32_t length;	//how much bytes this charactor taken
		uint32_t value;
		charactor(uint32_t len, uint32_t v)
		{
			length=len;
			value = v;
		}

		bool operator == (const charactor c) const
		{
			return this->length == c.length && this->value == c.value;
		}
	};

	enum encodingType{ASCII, GBK, UTF8};

	static bool isBigEndian()
	{
		uint16_t v = 0x0102;
		char *c = (char *)&v;
		if(2==c[0]) return false;
		else return true;
	}

	class CharactorMatcher
	{
		public:

			//CharactorMatcher(const char search_str[], const char match_str[], charactor_match::encodingType type);
			CharactorMatcher(const string search_str, const string match_str, charactor_match::encodingType type = charactor_match::ASCII);

			/**************************************************
			 * return value represents whether match or not
			 * pos indicate the first matched pos in search_str
			 **************************************************/
			bool KMPSearch(size_t &pos);
			~CharactorMatcher()
			{
				if(NULL != next)
				{
					delete next;
				}
			}

		private:
			//bool pre_process(const char str[]);
			bool preProcessASCII(const string &str, vector<charactor> &chars);
			bool preProcessGBK(const string &str, vector<charactor> &chars);

			string search_str;
			string match_str;
			vector<charactor> match_chars;
			vector<charactor> search_chars;
			int *next = NULL;
			bool valid ;
			static bool bigEndian ;
	};
}

#endif
