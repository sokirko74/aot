#include "morph_dict/common/utilit.h"
#include "gra_descr.h"
extern bool IsFirstMemberOfPairDesciptor(Descriptors d);
extern bool IsSecondMemberOfPairDesciptor(Descriptors d);
extern Descriptors GetSecondMemberByTheFirst(Descriptors d);
extern Descriptors GetFirstMemberByTheSecond(Descriptors d);
extern uint64_t parse_gra_descriptors ( const char * s, std::string& unparsed);
extern const char* GetDescriptorStr(int DescriptorNo);
