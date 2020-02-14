#include "utilit.h"
#include "gra_descr.h"
extern bool IsEndTextPeriodDescriptor (Descriptors d);
extern bool IsFirstMemberOfPairDesciptor(Descriptors d);
extern bool IsSecondMemberOfPairDesciptor(Descriptors d);
extern Descriptors GetSecondMemberByTheFirst(Descriptors d);
extern Descriptors GetFirstMemberByTheSecond(Descriptors d);
extern QWORD parse_gra_descriptors ( const char * s, string& unparsed);
extern const char* GetDescriptorStr(int DescriptorNo);
