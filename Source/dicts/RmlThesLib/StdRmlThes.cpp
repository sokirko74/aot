// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)


#pragma warning (disable : 4786)

#include "StdRmlThes.h"

std::string& trim_quotes(std::string& s) {
	if (startswith(s, "\"")) {
		s.erase(0, 1);
	}
	if (endswith(s, "\"")) {
		s.erase(s.length() - 1, 1);
	}
	return s;
}
