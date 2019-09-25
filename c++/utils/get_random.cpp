/**
 * C和C++标准库的随机数都是伪随机数算法，而且一般来说随机度不够也不均匀。
 * 最好采用平台上对应的加密级别的随机数算法，随机度高且均匀
 * 以下是一个跨平台的例子。
 *
 */

#include <fcntl.h>
#include <unistd.h>
#include <iostream>
using namespace std;

//作者：Pluto Hades
//链接：https://www.zhihu.com/question/20397465/answer/28102335
//来源：知乎
//著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。

int GetRandom() {
    int rnum = 0;
#if defined _MSC_VER
#if defined _WIN32_WCE
    CeGenRandom(sizeof(int), (PBYTE)&rnum);
#else
    HCRYPTPROV hProvider = 0;
    const DWORD dwLength = sizeof(int);
    BYTE pbBuffer[dwLength] = {};
    DWORD result =::CryptAcquireContext(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
    assert(result);
    result = ::CryptGenRandom(hProvider, dwLength, pbBuffer);
    rnum = *(int*)pbBuffer;
    assert(result);
    ::CryptReleaseContext(hProvider, 0);
#endif
#elif defined __GNUC__
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd != -1) {
        (void) read(fd, (void *) &rnum, sizeof(int));
        (void) close(fd);
    }
#endif
    return rnum;
}

int main()
{
    for(int i=0; i<10; ++i)
    {
        cout << " " << GetRandom() ;
    }
    cout << endl;
}
