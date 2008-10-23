#if CGICDEBUG
#define CGICDEBUGSTART \
	{ \
		FILE *dout; \
		dout = fopen("/home/boutell/public_html/debug", "a"); \
	
#define CGICDEBUGEND \
		fclose(dout); \
	}
#else /* CGICDEBUG */
#define CGICDEBUGSTART
#define CGICDEBUGEND
#endif /* CGICDEBUG */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#ifndef NO_UNISTD
#include <unistd.h>
#endif /* NO_UNISTD */
#include "cgic.h"
#include "utilit.h"



#ifdef WIN32
	#include <io.h>
	#include <fcntl.h>
#endif 

#define cgiStrEq(a, b) (!strcmp((a), (b)))

char *cgiServerSoftware;
char *cgiServerName;
char *cgiGatewayInterface;
char *cgiServerProtocol;
char *cgiServerPort;
char *cgiRequestMethod;
char *cgiPathInfo;
char *cgiPathTranslated;
char *cgiScriptName;
char *cgiQueryString;
char *cgiRemoteHost;
char *cgiRemoteAddr;
char *cgiAuthType;
char *cgiRemoteUser;
char *cgiRemoteIdent;
char *cgiContentType;
int cgiContentLength;
char *cgiAccept;
char *cgiUserAgent;
char *cgiReferrer;

FILE *cgiIn;
FILE *cgiOut;

/* One form entry, consisting of an attribute-value pair. */

typedef struct cgiFormEntryStruct {
        char *attr;
        char *value;
        struct cgiFormEntryStruct *next;
} cgiFormEntry;

/* The first form entry. */
static cgiFormEntry *cgiFormEntryFirst;

/* True if CGI environment was restored from a file. */
static int cgiRestored = 0;

static void cgiGetenv(char **s, char *var);

typedef enum {
	cgiParseSuccess,
	cgiParseMemory,
	cgiParseIO
} cgiParseResultType;

static cgiParseResultType cgiParseGetFormInput();
static cgiParseResultType cgiParsePostFormInput();
static cgiParseResultType cgiParseFormInput(char *data, int length);
static void cgiSetupConstants();
static void cgiFreeResources();
static int cgiStrEqNc(char *s1, char *s2);

int main(int argc, char *argv[]) {
	//_sleep(20000);
	int result;
	char *cgiContentLengthString;
	cgiSetupConstants();
	cgiGetenv(&cgiServerSoftware, "SERVER_SOFTWARE");
	cgiGetenv(&cgiServerName, "SERVER_NAME");
	cgiGetenv(&cgiGatewayInterface, "GATEWAY_INTERFACE");
	cgiGetenv(&cgiServerProtocol, "SERVER_PROTOCOL");
	cgiGetenv(&cgiServerPort, "SERVER_PORT");
	cgiGetenv(&cgiRequestMethod, "REQUEST_METHOD");
	cgiGetenv(&cgiPathInfo, "PATH_INFO");
	cgiGetenv(&cgiPathTranslated, "PATH_TRANSLATED");
	cgiGetenv(&cgiScriptName, "SCRIPT_NAME");
	cgiGetenv(&cgiQueryString, "QUERY_STRING");
	cgiGetenv(&cgiRemoteHost, "REMOTE_HOST");
	cgiGetenv(&cgiRemoteAddr, "REMOTE_ADDR");
	cgiGetenv(&cgiAuthType, "AUTH_TYPE");
	cgiGetenv(&cgiRemoteUser, "REMOTE_USER");
	cgiGetenv(&cgiRemoteIdent, "REMOTE_IDENT");
	cgiGetenv(&cgiContentType, "CONTENT_TYPE");
	cgiGetenv(&cgiContentLengthString, "CONTENT_LENGTH");
	cgiContentLength = atoi(cgiContentLengthString);	
	cgiGetenv(&cgiAccept, "HTTP_ACCEPT");
	cgiGetenv(&cgiUserAgent, "HTTP_USER_AGENT");
	cgiGetenv(&cgiReferrer, "HTTP_REFERER");
#ifdef CGICDEBUG
	CGICDEBUGSTART
	fprintf(dout, "%d\n", cgiContentLength);
	fprintf(dout, "%s\n", cgiRequestMethod);
	fprintf(dout, "%s\n", cgiContentType);
	CGICDEBUGEND	
#endif /* CGICDEBUG */
#ifdef WIN32
	/* 1.07: Must set stdin and stdout to binary mode */
	_setmode( _fileno( stdin ), _O_BINARY );
	_setmode( _fileno( stdout ), _O_BINARY );
#endif /* WIN32 */
	cgiFormEntryFirst = 0;
	cgiIn = stdin;
	cgiOut = stdout;
	cgiRestored = 0;


	/* These five lines keep compilers from
		producing warnings that argc and argv
		are unused. They have no actual function. */
	if (argc) {
		if (argv[0]) {
			cgiRestored = 0;
		}
	}	


	if (cgiStrEqNc(cgiRequestMethod, "post")) {
#ifdef CGICDEBUG
		CGICDEBUGSTART
		fprintf(dout, "POST recognized\n");
		CGICDEBUGEND
#endif /* CGICDEBUG */
		if (cgiStrEqNc(cgiContentType, "application/x-www-form-urlencoded")) {	
#ifdef CGICDEBUG
			CGICDEBUGSTART
			fprintf(dout, "Calling PostFormInput\n");
			CGICDEBUGEND	
#endif /* CGICDEBUG */
			if (cgiParsePostFormInput() != cgiParseSuccess) {
#ifdef CGICDEBUG
				CGICDEBUGSTART
				fprintf(dout, "PostFormInput failed\n");
				CGICDEBUGEND	
#endif /* CGICDEBUG */
				cgiFreeResources();
				return -1;
			}	
#ifdef CGICDEBUG
			CGICDEBUGSTART
			fprintf(dout, "PostFormInput succeeded\n");
			CGICDEBUGEND	
#endif /* CGICDEBUG */
		}
	} else if (cgiStrEqNc(cgiRequestMethod, "get")) {	
		/* The spec says this should be taken care of by
			the server, but... it isn't */
		cgiContentLength = strlen(cgiQueryString);
		if (cgiParseGetFormInput() != cgiParseSuccess) {
#ifdef CGICDEBUG
			CGICDEBUGSTART
			fprintf(dout, "GetFormInput failed\n");
			CGICDEBUGEND	
#endif /* CGICDEBUG */
			cgiFreeResources();
			return -1;
		} else {	
#ifdef CGICDEBUG
			CGICDEBUGSTART
			fprintf(dout, "GetFormInput succeeded\n");
			CGICDEBUGEND	
#endif /* CGICDEBUG */
		}
	}
	result = cgiMain();
	cgiFreeResources();
	return result;
}

static void cgiGetenv(char **s, char *var){
	*s = getenv(var);
	if (!(*s)) {
		*s = "";
	}
}


static cgiParseResultType cgiParsePostFormInput() {
	char *input;
	cgiParseResultType result;
	if (!cgiContentLength) {
		return cgiParseSuccess;
	}
	input = (char *) malloc(cgiContentLength);
	if (!input) {
		return cgiParseMemory;	
	}
	if (fread(input, 1, cgiContentLength, cgiIn) != cgiContentLength) {
		return cgiParseIO;
	}
	
//	FILE * fp = fopen ("c:/query.txt","w");
//	fprintf (fp, "%s",  input);
//	fclose(fp);


	result = cgiParseFormInput(input, cgiContentLength);
	free(input);
	return result;
}

static cgiParseResultType cgiParseGetFormInput() {
	return cgiParseFormInput(cgiQueryString, cgiContentLength);
}

typedef enum {
	cgiEscapeRest,
	cgiEscapeFirst,
	cgiEscapeSecond
} cgiEscapeState;

typedef enum {
	cgiUnescapeSuccess,
	cgiUnescapeMemory
} cgiUnescapeResultType;

static cgiUnescapeResultType cgiUnescapeChars(char **sp, char *cp, int len);

static cgiParseResultType cgiParseFormInput(char *data, int length) {
	/* Scan for pairs, unescaping and storing them as they are found. */
	int pos = 0;
	cgiFormEntry *n;
	cgiFormEntry *l = 0;
	while (pos != length) {
		int foundEq = 0;
		int foundAmp = 0;
		int start = pos;
		int len = 0;
		char *attr;
		char *value;
		while (pos != length) {
			if (data[pos] == '=') {
				foundEq = 1;
				pos++;
				break;
			}
			pos++;
			len++;
		}
		if (!foundEq) {
			break;
		}
		if (cgiUnescapeChars(&attr, data+start, len)
			!= cgiUnescapeSuccess) {
			return cgiParseMemory;
		}	
		start = pos;
		len = 0;
		while (pos != length) {
			if (data[pos] == '&') {
				foundAmp = 1;
				pos++;
				break;
			}
			pos++;
			len++;
		}
		/* The last pair probably won't be followed by a &, but
			that's fine, so check for that after accepting it */
		if (cgiUnescapeChars(&value, data+start, len)
			!= cgiUnescapeSuccess) {
			return cgiParseMemory;
		}	
		/* OK, we have a new pair, add it to the list. */
		n = (cgiFormEntry *) malloc(sizeof(cgiFormEntry));	
		if (!n) {
			return cgiParseMemory;
		}
		n->attr = attr;
		n->value = value;
		n->next = 0;
		if (!l) {
			cgiFormEntryFirst = n;
		} else {
			l->next = n;
		}
		l = n;
		if (!foundAmp) {
			break;
		}			
	}
	return cgiParseSuccess;
}

static int cgiHexValue[256];

cgiUnescapeResultType cgiUnescapeChars(char **sp, char *cp, int len) {
	char *s;
	cgiEscapeState escapeState = cgiEscapeRest;
	int escapedValue = 0;
	int srcPos = 0;
	int dstPos = 0;
	s = (char *) malloc(len + 1);
	if (!s) {
		return cgiUnescapeMemory;
	}
	while (srcPos < len) {
		int ch = cp[srcPos];
		switch (escapeState) {
			case cgiEscapeRest:
			if (ch == '%') {
				escapeState = cgiEscapeFirst;
			} else if (ch == '+') {
				s[dstPos++] = ' ';
			} else {
				s[dstPos++] = ch;	
			}
			break;
			case cgiEscapeFirst:
			escapedValue = cgiHexValue[ch] << 4;	
			escapeState = cgiEscapeSecond;
			break;
			case cgiEscapeSecond:
			escapedValue += cgiHexValue[ch];
			s[dstPos++] = escapedValue;
			escapeState = cgiEscapeRest;
			break;
		}
		srcPos++;
	}
	s[dstPos] = '\0';
	*sp = s;
	return cgiUnescapeSuccess;
}		
	
static void cgiSetupConstants() {
	int i;
	for (i=0; (i < 256); i++) {
		cgiHexValue[i] = 0;
	}
	cgiHexValue['0'] = 0;	
	cgiHexValue['1'] = 1;	
	cgiHexValue['2'] = 2;	
	cgiHexValue['3'] = 3;	
	cgiHexValue['4'] = 4;	
	cgiHexValue['5'] = 5;	
	cgiHexValue['6'] = 6;	
	cgiHexValue['7'] = 7;	
	cgiHexValue['8'] = 8;	
	cgiHexValue['9'] = 9;
	cgiHexValue['A'] = 10;
	cgiHexValue['B'] = 11;
	cgiHexValue['C'] = 12;
	cgiHexValue['D'] = 13;
	cgiHexValue['E'] = 14;
	cgiHexValue['F'] = 15;
	cgiHexValue['a'] = 10;
	cgiHexValue['b'] = 11;
	cgiHexValue['c'] = 12;
	cgiHexValue['d'] = 13;
	cgiHexValue['e'] = 14;
	cgiHexValue['f'] = 15;
}

static void cgiFreeResources() {
	cgiFormEntry *c = cgiFormEntryFirst;
	cgiFormEntry *n;
	while (c) {
		n = c->next;
		free(c->attr);
		free(c->value);
		free(c);
		c = n;
	}
	/* If the cgi environment was restored from a saved environment,
		then these are in allocated space and must also be freed */
	if (cgiRestored) {
		free(cgiServerSoftware);
		free(cgiServerName);
		free(cgiGatewayInterface);
		free(cgiServerProtocol);
		free(cgiServerPort);
		free(cgiRequestMethod);
		free(cgiPathInfo);
		free(cgiPathTranslated);
		free(cgiScriptName);
		free(cgiQueryString);
		free(cgiRemoteHost);
		free(cgiRemoteAddr);
		free(cgiAuthType);
		free(cgiRemoteUser);
		free(cgiRemoteIdent);
		free(cgiContentType);
		free(cgiAccept);
		free(cgiUserAgent);
		free(cgiReferrer);
	}
}

static cgiFormResultType cgiFormEntryString(
	cgiFormEntry *e, char *result, int max, int newlines);

static cgiFormEntry *cgiFormEntryFindFirst(char *name);
static cgiFormEntry *cgiFormEntryFindNext();

cgiFormResultType cgiFormString(
        char *name, char *result, int max) {
	cgiFormEntry *e;
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		strcpy(result, "");
		return cgiFormNotFound;
	}
	return cgiFormEntryString(e, result, max, 1);
}

cgiFormResultType cgiFormStringNoNewlines(
        char *name, char *result, int max) {
	cgiFormEntry *e;
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		strcpy(result, "");
		return cgiFormNotFound;
	}
	return cgiFormEntryString(e, result, max, 0);
}

cgiFormResultType cgiFormStringMultiple(
        char *name, char ***result) {
	char **stringArray;
	cgiFormEntry *e;
	int i;
	int total = 0;
	/* Make two passes. One would be more efficient, but this
		function is not commonly used. The select menu and
		radio box functions are faster. */
	e = cgiFormEntryFindFirst(name);
	if (e != 0) {
		do {
			total++;
		} while ((e = cgiFormEntryFindNext()) != 0); 
	}
	stringArray = (char **) malloc(sizeof(char *) * (total + 1));
	if (!stringArray) {
		*result = 0;
		return cgiFormMemory;
	}
	/* initialize all entries to null; the last will stay that way */
	for (i=0; (i <= total); i++) {
		stringArray[i] = 0;
	}
	/* Now go get the entries */
	e = cgiFormEntryFindFirst(name);
#ifdef CGICDEBUG
	CGICDEBUGSTART
	fprintf(dout, "StringMultiple Beginning\n");
	CGICDEBUGEND
#endif /* CGICDEBUG */
	if (e) {
		i = 0;
		do {
			int max = strlen(e->value) + 1;
			stringArray[i] = (char *) malloc(max);
			if (stringArray[i] == 0) {
				/* Memory problems */
				cgiStringArrayFree(stringArray);
				*result = 0;
				return cgiFormMemory;
			}	
			strcpy(stringArray[i], e->value);
			cgiFormEntryString(e, stringArray[i], max, 1);
			i++;
		} while ((e = cgiFormEntryFindNext()) != 0); 
		*result = stringArray;
#ifdef CGICDEBUG
		CGICDEBUGSTART
		fprintf(dout, "StringMultiple Succeeding\n");
		CGICDEBUGEND
#endif /* CGICDEBUG */
		return cgiFormSuccess;
	} else {
		*result = stringArray;
#ifdef CGICDEBUG
		CGICDEBUGSTART
		fprintf(dout, "StringMultiple found nothing\n");
		CGICDEBUGEND
#endif /* CGICDEBUG */
		return cgiFormNotFound;
	}	
}

cgiFormResultType cgiFormStringSpaceNeeded(
        char *name, int *result) {
	cgiFormEntry *e;
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		*result = 1;
		return cgiFormNotFound; 
	}
	*result = strlen(e->value) + 1;
	return cgiFormSuccess;
}

static cgiFormResultType cgiFormEntryString(
	cgiFormEntry *e, char *result, int max, int newlines) {
	char *dp, *sp;
	int truncated = 0;
	int len = 0;
	int avail = max-1;
	int crCount = 0;
	int lfCount = 0;	
	dp = result;
	sp = e->value;	
	while (1) {
		int ch;
		/* 1.07: don't check for available space now.
			We check for it immediately before adding
			an actual character. 1.06 handled the
			trailing null of the source string improperly,
			resulting in a cgiFormTruncated error. */
		ch = *sp;
		/* Fix the CR/LF, LF, CR nightmare: watch for
			consecutive bursts of CRs and LFs in whatever
			pattern, then actually output the larger number 
			of LFs. Consistently sane, yet it still allows
			consecutive blank lines when the user
			actually intends them. */
		if ((ch == 13) || (ch == 10)) {
			if (ch == 13) {
				crCount++;
			} else {
				lfCount++;
			}	
		} else {
			if (crCount || lfCount) {
				int lfsAdd = crCount;
				if (lfCount > crCount) {
					lfsAdd = lfCount;
				}
				/* Stomp all newlines if desired */
				if (!newlines) {
					lfsAdd = 0;
				}
				while (lfsAdd) {
					if (len >= avail) {
						truncated = 1;
						break;
					}
					*dp = 10;
					dp++;
					lfsAdd--;
					len++;		
				}
				crCount = 0;
				lfCount = 0;
			}
			if (ch == '\0') {
				/* The end of the source string */
				break;				
			}	
			/* 1.06: check available space before adding
				the character, because a previously added
				LF may have brought us to the limit */
			if (len >= avail) {
				truncated = 1;
				break;
			}
			*dp = ch;
			dp++;
			len++;
		}
		sp++;	
	}	
	*dp = '\0';
	if (truncated) {
		return cgiFormTruncated;
	} else if (!len) {
		return cgiFormEmpty;
	} else {
		return cgiFormSuccess;
	}
}

static int cgiFirstNonspaceChar(char *s);

cgiFormResultType cgiFormInteger(
        char *name, int *result, int defaultV) {
	cgiFormEntry *e;
	int ch;
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		*result = defaultV;
		return cgiFormNotFound; 
	}	
	if (!strlen(e->value)) {
		*result = defaultV;
		return cgiFormEmpty;
	}
	ch = cgiFirstNonspaceChar(e->value);
	if (!(isdigit(ch)) && (ch != '-') && (ch != '+')) {
		*result = defaultV;
		return cgiFormBadType;
	} else {
		*result = atoi(e->value);
		return cgiFormSuccess;
	}
}

cgiFormResultType cgiFormIntegerBounded(
        char *name, int *result, int min, int max, int defaultV) {
	cgiFormResultType error = cgiFormInteger(name, result, defaultV);
	if (error != cgiFormSuccess) {
		return error;
	}
	if (*result < min) {
		*result = min;
		return cgiFormConstrained;
	} 
	if (*result > max) {
		*result = max;
		return cgiFormConstrained;
	} 
	return cgiFormSuccess;
}

cgiFormResultType cgiFormDouble(
        char *name, double *result, double defaultV) {
	cgiFormEntry *e;
	int ch;
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		*result = defaultV;
		return cgiFormNotFound; 
	}	
	if (!strlen(e->value)) {
		*result = defaultV;
		return cgiFormEmpty;
	} 
	ch = cgiFirstNonspaceChar(e->value);
	if (!(isdigit(ch)) && (ch != '.') && (ch != '-') && (ch != '+')) {
		*result = defaultV;
		return cgiFormBadType;
	} else {
		*result = atof(e->value);
		return cgiFormSuccess;
	}
}

cgiFormResultType cgiFormDoubleBounded(
        char *name, double *result, double min, double max, double defaultV) {
	cgiFormResultType error = cgiFormDouble(name, result, defaultV);
	if (error != cgiFormSuccess) {
		return error;
	}
	if (*result < min) {
		*result = min;
		return cgiFormConstrained;
	} 
	if (*result > max) {
		*result = max;
		return cgiFormConstrained;
	} 
	return cgiFormSuccess;
}

cgiFormResultType cgiFormSelectSingle(
	char *name, char **choicesText, int choicesTotal, 
	int *result, int defaultV) 
{
	cgiFormEntry *e;
	int i;
	e = cgiFormEntryFindFirst(name);
#ifdef CGICDEBUG
	CGICDEBUGSTART
	fprintf(dout, "%d\n", (int) e);
	CGICDEBUGEND
#endif /* CGICDEBUG */
	if (!e) {
		*result = defaultV;
		return cgiFormNotFound;
	}
	for (i=0; (i < choicesTotal); i++) {
#ifdef CGICDEBUG
		CGICDEBUGSTART
		fprintf(dout, "%s %s\n", choicesText[i], e->value);
		CGICDEBUGEND
#endif /* CGICDEBUG */
		if (cgiStrEq(choicesText[i], e->value)) {
#ifdef CGICDEBUG
			CGICDEBUGSTART
			fprintf(dout, "MATCH\n");
			CGICDEBUGEND
#endif /* CGICDEBUG */
			*result = i;
			return cgiFormSuccess;
		}
	}
	*result = defaultV;
	return cgiFormNoSuchChoice;
}

cgiFormResultType cgiFormSelectMultiple(
	char *name, char **choicesText, int choicesTotal, 
	int *result, int *invalid) 
{
	cgiFormEntry *e;
	int i;
	int hits = 0;
	int invalidE = 0;
	for (i=0; (i < choicesTotal); i++) {
		result[i] = 0;
	}
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		*invalid = invalidE;
		return cgiFormNotFound;
	}
	do {
		int hit = 0;
		for (i=0; (i < choicesTotal); i++) {
			if (cgiStrEq(choicesText[i], e->value)) {
				result[i] = 1;
				hits++;
				hit = 1;
				break;
			}
		}
		if (!(hit)) {
			invalidE++;
		}
	} while ((e = cgiFormEntryFindNext()) != 0);

	*invalid = invalidE;

	if (hits) {
		return cgiFormSuccess;
	} else {
		return cgiFormNotFound;
	}
}

cgiFormResultType cgiFormCheckboxSingle(
	char *name)
{
	cgiFormEntry *e;
	e = cgiFormEntryFindFirst(name);
	if (!e) {
		return cgiFormNotFound;
	}
	return cgiFormSuccess;
}

extern cgiFormResultType cgiFormCheckboxMultiple(
	char *name, char **valuesText, int valuesTotal, 
	int *result, int *invalid)
{
	/* Implementation is identical to cgiFormSelectMultiple. */
	return cgiFormSelectMultiple(name, valuesText, 
		valuesTotal, result, invalid);
}

cgiFormResultType cgiFormRadio(
	char *name, 
	char **valuesText, int valuesTotal, int *result, int defaultV)
{
	/* Implementation is identical to cgiFormSelectSingle. */
	return cgiFormSelectSingle(name, valuesText, valuesTotal, 
		result, defaultV);
}

void cgiHeaderLocation(char *redirectUrl) {
	fprintf(cgiOut, "Location: %s%c%c", redirectUrl, 10, 10); 
}

void cgiHeaderStatus(int status, char *statusMessage) {
	fprintf(cgiOut, "Status: %d %s%c%c", status, statusMessage, 
	10, 10);
}

void cgiHeaderContentType(char *mimeType) {
	fprintf(cgiOut, "Content-type: %s%c%c", mimeType, 10, 10);
}

static int cgiWriteString(FILE *out, char *s);

static int cgiWriteInt(FILE *out, int i);

cgiEnvironmentResultType cgiWriteEnvironment(char *filename) {
	FILE *out;
	cgiFormEntry *e;
	/* Be sure to open in binary mode */
	out = fopen(filename, "wb");
	if (!out) {
		/* Can't create file */
		return cgiEnvironmentIO;
	}
	if (!cgiWriteString(out, cgiServerSoftware)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiServerName)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiGatewayInterface)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiServerProtocol)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiServerPort)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiRequestMethod)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiPathInfo)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiPathTranslated)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiScriptName)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiQueryString)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiRemoteHost)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiRemoteAddr)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiAuthType)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiRemoteUser)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiRemoteIdent)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiContentType)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiAccept)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiUserAgent)) {
		goto error;
	}
	if (!cgiWriteString(out, cgiReferrer)) {
		goto error;
	}
	if (!cgiWriteInt(out, cgiContentLength)) {
		goto error;
	}
	e = cgiFormEntryFirst;
	while (e) {
		if (!cgiWriteString(out, e->attr)) {
			goto error;
		}
		if (!cgiWriteString(out, e->value)) {
			goto error;
		}
		e = e->next;
	}
	fclose(out);
	return cgiEnvironmentSuccess;
error:
	fclose(out);
	/* If this function is not defined in your system,
		you must substitute the appropriate 
		file-deletion function. */
	unlink(filename);
	return cgiEnvironmentIO;
}

static int cgiWriteString(FILE *out, char *s) {
	int len = strlen(s);
	cgiWriteInt(out, len);
	if (fwrite(s, 1, len, out) != len) {
		return 0;
	}
	return 1;
}

static int cgiWriteInt(FILE *out, int i) {
	if (!fwrite(&i, sizeof(int), 1, out)) {
		return 0;
	}
	return 1;
}

static int cgiReadString(FILE *out, char **s);

static int cgiReadInt(FILE *out, int *i);

cgiEnvironmentResultType cgiReadEnvironment(char *filename) {
	FILE *in;
	cgiFormEntry *e, *p;
	/* Free any existing data first */
	cgiFreeResources();
	/* Be sure to open in binary mode */
	in = fopen(filename, "rb");
	if (!in) {
		/* Can't access file */
		return cgiEnvironmentIO;
	}
	if (!cgiReadString(in, &cgiServerSoftware)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiServerName)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiGatewayInterface)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiServerProtocol)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiServerPort)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiRequestMethod)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiPathInfo)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiPathTranslated)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiScriptName)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiQueryString)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiRemoteHost)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiRemoteAddr)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiAuthType)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiRemoteUser)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiRemoteIdent)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiContentType)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiAccept)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiUserAgent)) {
		goto error;
	}
	if (!cgiReadString(in, &cgiReferrer)) {
		goto error;
	}
	if (!cgiReadInt(in, &cgiContentLength)) {
		goto error;
	}
	p = 0;
	while (1) {
		e = (cgiFormEntry *) malloc(sizeof(cgiFormEntry));
		if (!e) {
			cgiFreeResources();
			fclose(in);
			return cgiEnvironmentMemory;
		}
		if (!cgiReadString(in, &e->attr)) {
			/* This means we've reached the end of the list. */
			free(e);
			break;
		}
		if (!cgiReadString(in, &e->value)) {
			free(e);
			goto error;
		}
		e->next = 0;
		if (p) {
			p->next = e;
		} else {
			cgiFormEntryFirst = e;
		}	
		p = e;
	}
	fclose(in);
	cgiRestored = 1;
	return cgiEnvironmentSuccess;
error:
	cgiFreeResources();
	fclose(in);
	return cgiEnvironmentIO;
}

static int cgiReadString(FILE *in, char **s) {
	int len;
	cgiReadInt(in, &len);
	*s = (char *) malloc(len + 1);
	if (!(*s)) {
		return 0;
	}	
	if (fread(*s, 1, len, in) != len) {
		return 0;
	}
	(*s)[len] = '\0';
	return 1;
}

static int cgiReadInt(FILE *out, int *i) {
	if (!fread(i, sizeof(int), 1, out)) {
		return 0;
	}
	return 1;
}

static int cgiStrEqNc(char *s1, char *s2) {
	while(1) {
		if (!(*s1)) {
			if (!(*s2)) {
				return 1;
			} else {
				return 0;
			}
		} else if (!(*s2)) {
			return 0;
		}
		if (isalpha(*s1)) {
			if (tolower(*s1) != tolower(*s2)) {
				return 0;
			}
		} else if ((*s1) != (*s2)) {
			return 0;
		}
		s1++;
		s2++;
	}
}

static char *cgiFindTarget = 0;
static cgiFormEntry *cgiFindPos = 0;

static cgiFormEntry *cgiFormEntryFindFirst(char *name) {
	cgiFindTarget = name;
	cgiFindPos = cgiFormEntryFirst;
	return cgiFormEntryFindNext();
}

static cgiFormEntry *cgiFormEntryFindNext() {
	while (cgiFindPos) {
		cgiFormEntry *c = cgiFindPos;
		cgiFindPos = c->next;
		if (!strcmp(c -> attr, cgiFindTarget)) {
			return c;
		}
	}
	return 0;
}

static int cgiFirstNonspaceChar(char *s) {
	int len = strspn(s, " \n\r\t");
	return s[len];
}

void cgiStringArrayFree(char **stringArray) {
	char *p;
	p = *stringArray;
	while (p) {
		free(p);
		stringArray++;
		p = *stringArray;
	}
}	

