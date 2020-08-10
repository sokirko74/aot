#pragma once
#include "../contrib/nlohmann/json.hpp"
#include "utilit.h"

inline void ConvertToUtfRecursive(nlohmann::json& r, MorphLanguageEnum langua) {
	if (r.is_array()) {
		for (auto& a : r) {
			ConvertToUtfRecursive(a, langua);
		}
	}
	else if (r.is_object()) {
		for (auto& a : r.items()) {
			ConvertToUtfRecursive(a.value(), langua);
		}

	}
	else if (r.is_string()) {
		r = convert_to_utf8(r, langua);
	}
}
