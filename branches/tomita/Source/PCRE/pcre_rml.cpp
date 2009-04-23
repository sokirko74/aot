
#include "pcre_rml.h"
//#include "internal.h"

#ifdef WIN32
	
	bool pcrecpp::Arg::parse_null          (const char* str, int n, void* dest)
	{
		return str == 0;
	}
#endif

/* Bit definitions for entries in the pcre_ctypes table. */

#define ctype_space   0x01
#define ctype_letter  0x02
#define ctype_digit   0x04
#define ctype_xdigit  0x08
#define ctype_word    0x10   /* alphameric or '_' */
#define ctype_meta    0x80   /* regexp meta char or zero (end pattern) */

/* Offsets for the bitmap tables in pcre_cbits. Each table contains a set
of bits for a class map. Some classes are built by combining these tables. */

#define cbit_space     0      /* [:space:] or \s */
#define cbit_xdigit   32      /* [:xdigit:] */
#define cbit_digit    64      /* [:digit:] or \d */
#define cbit_upper    96      /* [:upper:] */
#define cbit_lower   128      /* [:lower:] */
#define cbit_word    160      /* [:word:] or \w */
#define cbit_graph   192      /* [:graph:] */
#define cbit_print   224      /* [:print:] */
#define cbit_punct   256      /* [:punct:] */
#define cbit_cntrl   288      /* [:cntrl:] */
#define cbit_length  320      /* Length of the cbits table */

/* Offsets of the various tables from the base tables pointer, and
total length. */

#define lcc_offset      0
#define fcc_offset    256
#define cbits_offset  512
#define ctypes_offset (cbits_offset + cbit_length)
#define tables_length (ctypes_offset + 256)



void RmlPcreMakeTables(vector<BYTE>& table, MorphLanguageEnum Langua)
{
	table.resize(tables_length);
	int start  = 0;
	/* First comes the lower casing table */
	for (size_t i = 0; i < 256; i++) 
		if (is_upper_alpha(i, Langua))
			table[i+start] = ReverseChar(i, Langua);

	start = 256;
	/* Next the case-flipping table */
	for (size_t i = 0; i < 256; i++) 
		table[i+start] = ReverseChar(i, Langua);

	start += 256;
	for (size_t i=0; i < cbit_length; i++)
		table[i+start] = 0;
	
	for (size_t i = 0; i < 256; i++)
	{
		if (isdigit(i))
		{
			table[start+cbit_digit  + i/8] |= 1 << (i&7);
			table[start+cbit_word   + i/8] |= 1 << (i&7);
		}

		if (is_upper_alpha(i, Langua))
		{
			table[start+cbit_upper  + i/8] |= 1 << (i&7);
			table[start+cbit_word   + i/8] |= 1 << (i&7);
		}

		if (is_lower_alpha(i, Langua))
		{
			table[start+cbit_lower  + i/8] |= 1 << (i&7);
			table[start+cbit_word   + i/8] |= 1 << (i&7);
		}
		if (i == '_')   table[start+cbit_word   + i/8] |= 1 << (i&7);
		if (isspace(i)) table[start+cbit_space  + i/8] |= 1 << (i&7);
		if (isxdigit(i))table[start+cbit_xdigit + i/8] |= 1 << (i&7);
		if (is_alpha(i, Langua) ||ispunct(i)) table[start+cbit_graph  + i/8] |= 1 << (i&7);
		if (is_alpha(i, Langua) ||isprint(i)) table[start+cbit_print  + i/8] |= 1 << (i&7);
		if (ispunct(i)) table[start+cbit_punct  + i/8] |= 1 << (i&7);
		if (iscntrl(i)) table[start+cbit_cntrl  + i/8] |= 1 << (i&7);
	}

	start += cbit_length;

/* Finally, the character type table. In this, we exclude VT from the white
space chars, because Perl doesn't recognize it as such for \s and for comments
within regexes. */

	for (size_t i = 0; i < 256; i++)
	{
		int x = 0;
		if (i != 0x0b && isspace(i)) x += ctype_space;
		if (isalpha(i)) x += ctype_letter;
		if (isdigit(i)) x += ctype_digit;
		if (isxdigit(i)) x += ctype_xdigit;
		if (isalnum(i) || i == '_') x += ctype_word;

		/* Note: strchr includes the terminating zero in the characters it considers.
		In this instance, that is ok because we want binary zero to be flagged as a
		meta-character, which in this sense is any character that terminates a run
		of data characters. */

		if (strchr("*+?{^.$|()[", i) != 0) 
			x += ctype_meta; 

		table[start+i] = x; 
	}

}

// Special object that stands-in for no argument
pcrecpp::Arg no_arg((void*)NULL);

// Maximum number of args we can set
static const int kMaxArgs = 16;
static const int kVecSize = (1 + kMaxArgs) * 3;  // results + PCRE workspace


// If a regular expression has no error, its error_ field points here
static const string empty_string;

// If the user doesn't ask for any options, we just use this one
static pcrecpp::RE_Options default_options;

void RML_RE::Init(const char* pat, const RE_Options* options) { Init(pat, options, NULL); }


void RML_RE::Init(const char* pat, const pcrecpp::RE_Options* options, const unsigned char *tableptr) {

  pattern_ = pat;
  if (options == NULL) {
    options_ = default_options;
  } else {
    options_ = *options;
  }
  error_ = &empty_string;
  re_full_ = NULL;
  re_partial_ = NULL;

  re_partial_ = Compile(UNANCHORED, tableptr);
  if (re_partial_ != NULL) {
    // Check for complicated patterns.  The following change is
    // conservative in that it may treat some "simple" patterns
    // as "complex" (e.g., if the vertical bar is in a character
    // class or is escaped).  But it seems good enough.
    if (strchr(pat, '|') == NULL) {
      // Simple pattern: we can use position-based checks to perform
      // fully anchored matches
      re_full_ = re_partial_;
    } else {
      // We need a special pattern for anchored matches
      re_full_ = Compile(ANCHOR_BOTH, tableptr);
    }
  }
}

RML_RE::~RML_RE() {
  if (re_full_ != NULL && re_full_ != re_partial_) (*pcre_free)(re_full_);
  if (re_partial_ != NULL)                         (*pcre_free)(re_partial_);
  if (error_ != &empty_string)                     delete error_;
}

pcre* RML_RE::Compile(Anchor anchor, const unsigned char *tableptr) {
  // First, convert RE_Options into pcre options
  int pcre_options = 0;
  pcre_options = options_.all_options();

  // Special treatment for anchoring.  This is needed because at
  // runtime pcre only provides an option for anchoring at the
  // beginning of a string (unless you use offset).
  //
  // There are three types of anchoring we want:
  //    UNANCHORED      Compile the original pattern, and use
  //                    a pcre unanchored match.
  //    ANCHOR_START    Compile the original pattern, and use
  //                    a pcre anchored match.
  //    ANCHOR_BOTH     Tack a "\z" to the end of the original pattern
  //                    and use a pcre anchored match.

  const char* compile_error;
  int eoffset;
  pcre* re;
  if (anchor != ANCHOR_BOTH) {
    re = pcre_compile(pattern_.c_str(), pcre_options,
                      &compile_error, &eoffset, tableptr);
  } else {
    // Tack a '\z' at the end of RE.  Parenthesize it first so that
    // the '\z' applies to all top-level alternatives in the regexp.
    string wrapped = "(?:";  // A non-counting grouping operator
    wrapped += pattern_;
    wrapped += ")\\z";
    re = pcre_compile(wrapped.c_str(), pcre_options,
                      &compile_error, &eoffset, tableptr);
  }
  if (re == NULL) {
    if (error_ == &empty_string) error_ = new string(compile_error);
  }
  return re;
}

/***** Matching interfaces *****/

bool RML_RE::FullMatch(const StringPiece& text,
                   const Arg& ptr1,
                   const Arg& ptr2,
                   const Arg& ptr3,
                   const Arg& ptr4,
                   const Arg& ptr5,
                   const Arg& ptr6,
                   const Arg& ptr7,
                   const Arg& ptr8,
                   const Arg& ptr9,
                   const Arg& ptr10,
                   const Arg& ptr11,
                   const Arg& ptr12,
                   const Arg& ptr13,
                   const Arg& ptr14,
                   const Arg& ptr15,
                   const Arg& ptr16) const {
  const Arg* args[kMaxArgs];
  int n = 0;
  if (&ptr1  == &no_arg) goto done; args[n++] = &ptr1;
  if (&ptr2  == &no_arg) goto done; args[n++] = &ptr2;
  if (&ptr3  == &no_arg) goto done; args[n++] = &ptr3;
  if (&ptr4  == &no_arg) goto done; args[n++] = &ptr4;
  if (&ptr5  == &no_arg) goto done; args[n++] = &ptr5;
  if (&ptr6  == &no_arg) goto done; args[n++] = &ptr6;
  if (&ptr7  == &no_arg) goto done; args[n++] = &ptr7;
  if (&ptr8  == &no_arg) goto done; args[n++] = &ptr8;
  if (&ptr9  == &no_arg) goto done; args[n++] = &ptr9;
  if (&ptr10 == &no_arg) goto done; args[n++] = &ptr10;
  if (&ptr11 == &no_arg) goto done; args[n++] = &ptr11;
  if (&ptr12 == &no_arg) goto done; args[n++] = &ptr12;
  if (&ptr13 == &no_arg) goto done; args[n++] = &ptr13;
  if (&ptr14 == &no_arg) goto done; args[n++] = &ptr14;
  if (&ptr15 == &no_arg) goto done; args[n++] = &ptr15;
  if (&ptr16 == &no_arg) goto done; args[n++] = &ptr16;
 done:

  int consumed;
  int vec[kVecSize];
  return DoMatchImpl(text, ANCHOR_BOTH, &consumed, args, n, vec, kVecSize);
}

bool RML_RE::PartialMatch(const StringPiece& text,
                      const Arg& ptr1,
                      const Arg& ptr2,
                      const Arg& ptr3,
                      const Arg& ptr4,
                      const Arg& ptr5,
                      const Arg& ptr6,
                      const Arg& ptr7,
                      const Arg& ptr8,
                      const Arg& ptr9,
                      const Arg& ptr10,
                      const Arg& ptr11,
                      const Arg& ptr12,
                      const Arg& ptr13,
                      const Arg& ptr14,
                      const Arg& ptr15,
                      const Arg& ptr16) const {
  const Arg* args[kMaxArgs];
  int n = 0;
  if (&ptr1  == &no_arg) goto done; args[n++] = &ptr1;
  if (&ptr2  == &no_arg) goto done; args[n++] = &ptr2;
  if (&ptr3  == &no_arg) goto done; args[n++] = &ptr3;
  if (&ptr4  == &no_arg) goto done; args[n++] = &ptr4;
  if (&ptr5  == &no_arg) goto done; args[n++] = &ptr5;
  if (&ptr6  == &no_arg) goto done; args[n++] = &ptr6;
  if (&ptr7  == &no_arg) goto done; args[n++] = &ptr7;
  if (&ptr8  == &no_arg) goto done; args[n++] = &ptr8;
  if (&ptr9  == &no_arg) goto done; args[n++] = &ptr9;
  if (&ptr10 == &no_arg) goto done; args[n++] = &ptr10;
  if (&ptr11 == &no_arg) goto done; args[n++] = &ptr11;
  if (&ptr12 == &no_arg) goto done; args[n++] = &ptr12;
  if (&ptr13 == &no_arg) goto done; args[n++] = &ptr13;
  if (&ptr14 == &no_arg) goto done; args[n++] = &ptr14;
  if (&ptr15 == &no_arg) goto done; args[n++] = &ptr15;
  if (&ptr16 == &no_arg) goto done; args[n++] = &ptr16;
 done:

  int consumed;
  int vec[kVecSize];
  return DoMatchImpl(text, UNANCHORED, &consumed, args, n, vec, kVecSize);
}

bool RML_RE::Consume(StringPiece* input,
                 const Arg& ptr1,
                 const Arg& ptr2,
                 const Arg& ptr3,
                 const Arg& ptr4,
                 const Arg& ptr5,
                 const Arg& ptr6,
                 const Arg& ptr7,
                 const Arg& ptr8,
                 const Arg& ptr9,
                 const Arg& ptr10,
                 const Arg& ptr11,
                 const Arg& ptr12,
                 const Arg& ptr13,
                 const Arg& ptr14,
                 const Arg& ptr15,
                 const Arg& ptr16) const {
  const Arg* args[kMaxArgs];
  int n = 0;
  if (&ptr1  == &no_arg) goto done; args[n++] = &ptr1;
  if (&ptr2  == &no_arg) goto done; args[n++] = &ptr2;
  if (&ptr3  == &no_arg) goto done; args[n++] = &ptr3;
  if (&ptr4  == &no_arg) goto done; args[n++] = &ptr4;
  if (&ptr5  == &no_arg) goto done; args[n++] = &ptr5;
  if (&ptr6  == &no_arg) goto done; args[n++] = &ptr6;
  if (&ptr7  == &no_arg) goto done; args[n++] = &ptr7;
  if (&ptr8  == &no_arg) goto done; args[n++] = &ptr8;
  if (&ptr9  == &no_arg) goto done; args[n++] = &ptr9;
  if (&ptr10 == &no_arg) goto done; args[n++] = &ptr10;
  if (&ptr11 == &no_arg) goto done; args[n++] = &ptr11;
  if (&ptr12 == &no_arg) goto done; args[n++] = &ptr12;
  if (&ptr13 == &no_arg) goto done; args[n++] = &ptr13;
  if (&ptr14 == &no_arg) goto done; args[n++] = &ptr14;
  if (&ptr15 == &no_arg) goto done; args[n++] = &ptr15;
  if (&ptr16 == &no_arg) goto done; args[n++] = &ptr16;
 done:

  int consumed;
  int vec[kVecSize];
  if (DoMatchImpl(*input, ANCHOR_START, &consumed,
                  args, n, vec, kVecSize)) {
    input->remove_prefix(consumed);
    return true;
  } else {
    return false;
  }
}

bool RML_RE::FindAndConsume(StringPiece* input,
                        const Arg& ptr1,
                        const Arg& ptr2,
                        const Arg& ptr3,
                        const Arg& ptr4,
                        const Arg& ptr5,
                        const Arg& ptr6,
                        const Arg& ptr7,
                        const Arg& ptr8,
                        const Arg& ptr9,
                        const Arg& ptr10,
                        const Arg& ptr11,
                        const Arg& ptr12,
                        const Arg& ptr13,
                        const Arg& ptr14,
                        const Arg& ptr15,
                        const Arg& ptr16) const {
  const Arg* args[kMaxArgs];
  int n = 0;
  if (&ptr1  == &no_arg) goto done; args[n++] = &ptr1;
  if (&ptr2  == &no_arg) goto done; args[n++] = &ptr2;
  if (&ptr3  == &no_arg) goto done; args[n++] = &ptr3;
  if (&ptr4  == &no_arg) goto done; args[n++] = &ptr4;
  if (&ptr5  == &no_arg) goto done; args[n++] = &ptr5;
  if (&ptr6  == &no_arg) goto done; args[n++] = &ptr6;
  if (&ptr7  == &no_arg) goto done; args[n++] = &ptr7;
  if (&ptr8  == &no_arg) goto done; args[n++] = &ptr8;
  if (&ptr9  == &no_arg) goto done; args[n++] = &ptr9;
  if (&ptr10 == &no_arg) goto done; args[n++] = &ptr10;
  if (&ptr11 == &no_arg) goto done; args[n++] = &ptr11;
  if (&ptr12 == &no_arg) goto done; args[n++] = &ptr12;
  if (&ptr13 == &no_arg) goto done; args[n++] = &ptr13;
  if (&ptr14 == &no_arg) goto done; args[n++] = &ptr14;
  if (&ptr15 == &no_arg) goto done; args[n++] = &ptr15;
  if (&ptr16 == &no_arg) goto done; args[n++] = &ptr16;
 done:

  int consumed;
  int vec[kVecSize];
  if (DoMatchImpl(*input, UNANCHORED, &consumed,
                  args, n, vec, kVecSize)) {
    input->remove_prefix(consumed);
    return true;
  } else {
    return false;
  }
}

bool RML_RE::Replace(const StringPiece& rewrite,
                 string *str) const {
  int vec[kVecSize];
  int matches = TryMatch(*str, 0, UNANCHORED, vec, kVecSize);
  if (matches == 0)
    return false;

  string s;
  if (!Rewrite(&s, rewrite, *str, vec, matches))
    return false;

  assert(vec[0] >= 0);
  assert(vec[1] >= 0);
  str->replace(vec[0], vec[1] - vec[0], s);
  return true;
}

int RML_RE::GlobalReplace(const StringPiece& rewrite,
                      string *str) const {
  int count = 0;
  int vec[kVecSize];
  string out;
  int start = 0;
  int lastend = -1;

  for (; start <= static_cast<int>(str->length()); count++) {
    int matches = TryMatch(*str, start, UNANCHORED, vec, kVecSize);
    if (matches <= 0)
      break;
    int matchstart = vec[0], matchend = vec[1];
    assert(matchstart >= start);
    assert(matchend >= matchstart);
    if (matchstart == matchend && matchstart == lastend) {
      // advance one character if we matched an empty string at the same
      // place as the last match occurred
      if (start < static_cast<int>(str->length()))
        out.push_back((*str)[start]);
      start++;
    } else {
      out.append(*str, start, matchstart - start);
      Rewrite(&out, rewrite, *str, vec, matches);
      start = matchend;
      lastend = matchend;
      count++;
    }
  }

  if (count == 0)
    return 0;

  if (start < static_cast<int>(str->length()))
    out.append(*str, start, str->length() - start);
  swap(out, *str);
  return count;
}

bool RML_RE::Extract(const StringPiece& rewrite,
                 const StringPiece& text,
                 string *out) const {
  int vec[kVecSize];
  int matches = TryMatch(text, 0, UNANCHORED, vec, kVecSize);
  if (matches == 0)
    return false;
  out->erase();
  return Rewrite(out, rewrite, text, vec, matches);
}

/***** Actual matching and rewriting code *****/

int RML_RE::TryMatch(const StringPiece& text,
                 int startpos,
                 Anchor anchor,
                 int *vec,
                 int vecsize) const {
  pcre* re = (anchor == ANCHOR_BOTH) ? re_full_ : re_partial_;
  if (re == NULL) {
    //fprintf(stderr, "Matching against invalid re: %s\n", error_->c_str());
    return 0;
  }

  pcre_extra extra = { 0 };
  if (options_.match_limit() > 0) {
    extra.flags = PCRE_EXTRA_MATCH_LIMIT;
    extra.match_limit = options_.match_limit();
  }
  int rc = pcre_exec(re,              // The regular expression object
                     &extra,
                     text.data(),
                     text.size(),
                     startpos,
                     (anchor == UNANCHORED) ? 0 : PCRE_ANCHORED,
                     vec,
                     vecsize);

  // Handle errors
  if (rc == PCRE_ERROR_NOMATCH) {
    return 0;
  } else if (rc < 0) {
    //fprintf(stderr, "Unexpected return code: %d when matching '%s'\n",
    //        re, pattern_.c_str());
    return 0;
  } else if (rc == 0) {
    // pcre_exec() returns 0 as a special case when the number of
    // capturing subpatterns exceeds the size of the vector.
    // When this happens, there is a match and the output vector
    // is filled, but we miss out on the positions of the extra subpatterns.
    rc = vecsize / 2;
  }

  if ((anchor == ANCHOR_BOTH) && (re_full_ == re_partial_)) {
    // We need an extra check to make sure that the match extended
    // to the end of the input string
    assert(vec[0] == 0);                 // PCRE_ANCHORED forces starting match
    if (vec[1] != text.size()) return 0; // Did not get ending match
  }

  return rc;
}

bool RML_RE::DoMatchImpl(const StringPiece& text,
                     Anchor anchor,
                     int* consumed,
                     const Arg* const* args,
                     int n,
                     int* vec,
                     int vecsize) const {
  assert((1 + n) * 3 <= vecsize);  // results + PCRE workspace
  int matches = TryMatch(text, 0, anchor, vec, vecsize);
  assert(matches >= 0);  // TryMatch never returns negatives
  if (matches == 0)
    return false;

  *consumed = vec[1];

  if (args == NULL) {
    // We are not interested in results
    return true;
  }

  // If we got here, we must have matched the whole pattern.
  // We do not need (can not do) any more checks on the value of 'matches' here
  // -- see the comment for TryMatch.
  for (int i = 0; i < n; i++) {
    const int start = vec[2*(i+1)];
    const int limit = vec[2*(i+1)+1];
    if (!args[i]->Parse(text.data() + start, limit-start)) {
      // TODO: Should we indicate what the error was?
      return false;
    }
  }

  return true;
}

bool RML_RE::DoMatch(const StringPiece& text,
                 Anchor anchor,
                 int* consumed,
                 const Arg* const args[],
                 int n) const {
  assert(n >= 0);
  size_t const vecsize = (1 + n) * 3;  // results + PCRE workspace
                                       // (as for kVecSize)
  int space[21];   // use stack allocation for small vecsize (common case)
  int* vec = vecsize <= 21 ? space : new int[vecsize];
  bool retval = DoMatchImpl(text, anchor, consumed, args, n, vec, vecsize);
  if (vec != space) delete [] vec;
  return retval;
}

bool RML_RE::Rewrite(string *out, const StringPiece &rewrite,
                 const StringPiece &text, int *vec, int veclen) const {
  for (const char *s = rewrite.data(), *end = s + rewrite.size();
       s < end; s++) {
    int c = *s;
    if (c == '\\') {
      c = *++s;
      if (isdigit(c)) {
        int n = (c - '0');
        if (n >= veclen) {
          //fprintf(stderr, requested group %d in regexp %.*s\n",
          //        n, rewrite.size(), rewrite.data());
          return false;
        }
        int start = vec[2 * n];
        if (start >= 0)
          out->append(text.data() + start, vec[2 * n + 1] - start);
      } else if (c == '\\') {
        out->push_back('\\');
      } else {
        //fprintf(stderr, "invalid rewrite pattern: %.*s\n",
        //        rewrite.size(), rewrite.data());
        return false;
      }
    } else {
      out->push_back(c);
    }
  }
  return true;
}

// Return the number of capturing subpatterns, or -1 if the
// regexp wasn't valid on construction.
int RML_RE::NumberOfCapturingGroups() {
  if (re_partial_ == NULL) return -1;

  int result;
  int pcre_retval = pcre_fullinfo(re_partial_,  // The regular expression object
                                  NULL,         // We did not study the pattern
                                  PCRE_INFO_CAPTURECOUNT,
                                  &result);
  assert(pcre_retval == 0);
  return result;
}


