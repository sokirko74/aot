
#include <stdint.h>
#include <string.h>
#include <string>
#include "iconv.h"
#include <sstream>

typedef int conv_t;  // to compile iconv
typedef uint32_t ucs4_t;
const int RET_ILSEQ = 0;
const int RET_ILUNI = -1;
const int RET_TOOFEW = -2;
const int RET_TOOSMALL = -3;
#define RET_TOOFEW(n)       (-2-2*(n))
typedef int (*multibyte_to_widechar)(conv_t, ucs4_t*, const unsigned char*, size_t);
typedef int (*widechar_to_multibyte)(conv_t, unsigned char*, ucs4_t, size_t);
#include "lib/cp1251.h"
#include "lib/cp1252.h"
#include "lib/utf8.h"


static std::string convert_to_utf8(const std::string& single_byte_string, multibyte_to_widechar converter_to_wide) {
	std::string utf8_string(4 * single_byte_string.length(), ' ');
	unsigned char* buffer = (unsigned char*)&utf8_string[0];
	size_t buffer_offset = 0;
	for (size_t i = 0; i < single_byte_string.length(); ++i) {
		ucs4_t unicode_char;
		auto input = (const unsigned char*)(single_byte_string.c_str() + i);
		int result = converter_to_wide(
			0, 
			&unicode_char, 
			input,
			0);
		if (RET_ILSEQ == result) {
			std::string message = "cannot convert from single byte to wide char, ord=" +
				std::to_string((unsigned char)*input);
			throw convert_exception(message.c_str());
		}
		result = utf8_wctomb(0, buffer + buffer_offset, unicode_char, utf8_string.length() - buffer_offset);
		if (RET_ILUNI == result) {
			throw convert_exception("bad conversion: from wide char to utf8");
		}
		if (RET_TOOSMALL == result) {
			throw convert_exception("bad conversion: from wide char to utf8");
		}
		buffer_offset += result;
	}
	utf8_string.resize(buffer_offset);
	return utf8_string;
}

std::string convert_cp1251_to_utf8(const std::string& s) {
	return convert_to_utf8(s, cp1251_mbtowc);
}

std::string convert_cp1252_to_utf8(const std::string& s){
	return convert_to_utf8(s, cp1252_mbtowc);
}

static std::string convert_from_utf8(const char* utfstring, widechar_to_multibyte converter_from_wide) {
	if (utfstring == nullptr) {
		return std::string();
	}
	size_t utfstring_len = strlen(utfstring);
	size_t utfstring_offset = 0;
	std::string single_byte_string(utfstring_len, ' ');
	unsigned char* output_buffer = (unsigned char*)&single_byte_string[0];
	size_t output_buffer_offset = 0;
	while(utfstring_offset < utfstring_len) {
		ucs4_t unicode_char;
		int count = utf8_mbtowc(0, 
							    &unicode_char, 
								(const unsigned char*)(utfstring + utfstring_offset),
								single_byte_string.length() - output_buffer_offset);
		if (RET_ILUNI == count) {
			throw convert_exception("bad conversion: from utf8 to wide");
		}
		if (RET_TOOSMALL == count) {
			throw convert_exception("bad conversion: not enough space");
		}
		utfstring_offset += count;

		int result = converter_from_wide(0, 
					output_buffer + output_buffer_offset, 
					unicode_char, 
					single_byte_string.length() - output_buffer_offset);
		if (RET_ILSEQ == result) {
			throw convert_exception("cannot convert from wide to single_byte");
		}
		output_buffer_offset += 1;
	}
	single_byte_string.resize(output_buffer_offset);
	return single_byte_string;
}

std::string convert_utf8_to_cp1251(const char* s) {
	return convert_from_utf8(s, cp1251_wctomb);
}

std::string convert_utf8_to_cp1252(const char* s) {
	return convert_from_utf8(s, cp1252_wctomb);
}
