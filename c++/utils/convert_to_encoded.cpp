#include <iconv.h>                                                                                                                         
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

bool isGBKFirstPart(unsigned char ch)
{
    return (ch >= 129 && ch <= 254) ;   
}
    bool urlEncode(const std::string& srcStr, std::string& dstStr)
    {
        static char hex[] = "0123456789ABCDEF";

        dstStr.clear();
        for (size_t index = 0; index < srcStr.size(); ++index) {
            unsigned char cc = srcStr[index];
            if (isalnum(cc)) {
                dstStr += cc;
            } else if (isspace(cc)) {
                dstStr += "%20";
            } else {
                unsigned char c = static_cast<unsigned char>(srcStr[index]);
                dstStr += '%';
                dstStr += hex[c >> 4];
                dstStr += hex[c % 16];
                if(isGBKFirstPart(cc) && index < srcStr.size() - 1) {
                    ++index;
                    unsigned char c_next = static_cast<unsigned char>(srcStr[index]);
                    dstStr += '%';
                    dstStr += hex[c_next >> 4];
                    dstStr += hex[c_next % 16];
                }
            }
        }
        return true;
    }
    bool convertCore(const std::string& inCodeType,
                     const std::string& outCodeType,
                     const std::string& input,
                     std::string& output)
    {
        iconv_t cd = iconv_open(outCodeType.c_str(), inCodeType.c_str());
        if (cd == 0 || cd == (iconv_t)(-1)) {
            printf("iconv_open fails\n");
            return false;
        }

        char *convBuff = NULL;

        bool hasError = false;
        size_t convBuffLen = 1024;
        convBuffLen = convBuffLen < input .size() * 2 ? input .size() * 2 : convBuffLen;
        convBuff = new char[convBuffLen];
        if (convBuff == NULL) {
            std::string info = (
                                   "memalloc error, convBuffLen: "
                                   + std::to_string(convBuffLen)
                                   + "\n");
            printf(info.c_str());
            hasError = true;
            return false;
        }
        convBuff[0] = '\0';

        {
            char *out = convBuff;
            size_t inLen = input.size() + 1;
            size_t outLen = inLen * 2;

            size_t rt;
            //iconv_t rt;
            //rt = iconv(cd, &in, &inLen, &out, &outLen);
#ifdef __CYGWIN__
            const char *p = (char*)input.c_str();
#else
            char *p = (char*)input.c_str();
#endif
            rt = iconv(cd, &p, &inLen, &out, &outLen);

            if ((size_t) - 1 == rt) {
#if defined(DEBUG)
                printf("iconv false\n");
#endif // DEBUG
                hasError = true;
            } else {
                output = convBuff;
            }
        }

        iconv_close(cd);

        delete []convBuff;
        convBuff = NULL;

        if (hasError == true) {
            return false;
        }

        return true;
    }
inline bool utf8_to_gbk(const std::string& in, std::string& out)
{
    return convertCore("utf-8", "GBK", in, out);
}

    bool encodeSub(const std::string &srcStr, int &len, int &index, int sub_len, std::stringstream &ss)
    {
        bool ret = true;
        if ( index + sub_len < len )
        {   
            std::string tmp,tmp2;
            ret = utf8_to_gbk(srcStr.substr(index, sub_len + 1), tmp);
            if (!ret)
            {   
                return ret;
            }
            ret = urlEncode(tmp,tmp2);
            if (!ret)
            {   
                return ret;
            }
            ss << tmp2;
			index += sub_len;
        }
        else
        {   
            ret = false;
        }
        return ret;
    }

    bool convertToEncode(const std::string &srcStr, std::string &dstStr)
    {
        bool ret = true;
        std::stringstream ss("");
        try
        {   
            int len = srcStr.length();
            if (len == 0)
            {   
                ret = false;
            }
            else
            {   
                const char *p = srcStr.c_str();
                for (int i = 0; i < len; ++i)
                {   
                    if (p[i] > 0 && p[i] < 0x80)
                    {   
                        if(p[i] == ' ')
                        {  
                           ss << "%20";
                        }
                        else
                        {   
                            ss << p[i];
                        }
                    } 
                    else if (p[i] >= (char)0xC0 && p[i] < (char)0xE0)
                    {   
                        ret = encodeSub(srcStr, len, i, 1, ss);
                        if (!ret)
                        {   
                            break;
                        }
                    }
                    else if (p[i] >= (char)0xE0 && p[i] < (char)0xF0)
                    {   
                        ret = encodeSub(srcStr, len, i, 2, ss);
                        if (!ret)
                        {   
                            break; 
                        }          
                    }
                    else
                    {   
                        ret = false;
                        break;
                    }
                }
            }
            
        }
        catch(std::exception &e)
        {   
            ret = false;
        }
        if (!ret)
        {   
            dstStr = "";
        }
        else
        {   
            ss >> dstStr;
        }
        return ret;
    }
int main()
{
std::string cmd ( "cmd: key=干衣机&enc_url_gbk=yes&client=1516345463041&only_rcount=yes&filt_type=article_type,l2m1;not_style,l2m2;app_limit;not_sub_position,l10m10&abc=0");
std::string dcmd;
bool ret = convertToEncode(cmd,dcmd);
std::cout<<"return value:"<<ret<<std::endl;
std::cout<<dcmd<<std::endl;
}
