
#ifndef __iconv_h
#define __iconv_h

#include <iconv.h>
#include "utilit.h"

class tIConv {
    iconv_t ConversionHandle;

  public:
    tIConv(const string &to, const string &from);
    ~tIConv();
    string convert(const char *data, size_t size) const;
    string convert(const string &str);
    void reset();
};

extern string WinToUtf8(const string& s);
extern string WinToUtf8(const char *data, size_t size);
extern string Utf8ToWin(const string& s);
extern string Utf8ToWin(const char *data, size_t size);





#endif

