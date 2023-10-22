#pragma once

//  a header for windows MFC Unicode projects only (VisualSynan and demo.exe)

#include "morph_dict/common/utilit.h"
#include <afxwin.h>
#include <comutil.h>



inline std::string WstrToUtf8Str(CStringW wide_string) {
	std::wstring w(wide_string);
	return utf16_to_utf8(w);
}
