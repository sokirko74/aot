#include "morph_dict/common/util_classes.h"
#include "gra_descr.h"


const std::string DesArray[NumberOfGraphematicalDescriptors] = {
    	     "dummy",
			 "RLE", 
			 "LLE","DEL","PUN",//the first position :  //0-4
		     "DC", "DSC","EA","_UNK_", // 5-8
		     //the second position :
		     "SPC","HYP","EOLN","EOP", //9-12
		     "PLP","AA", "aa", "Aa", // 13-16
			 "dummy", "OPN","CLS", // 17-19
			 "EMSYM", "BUL", "INDENT", // 20-22
             "FIXED", "#QUOTED", "PAR_SYM", // 23-25

             // macrosyntax
             "CS?","CS","QUA","CS_PRNT","HDNG","CS_AUX","DOC", // 26-32

             "SENT_END", //33
             "ROMAN", //34
		     // pair descriptors : 
			 // if d is the first descriptor, then it should be always even, and d+1 should  be the corresponding second descriptor
		     "EXPR1","EXPR2", // 35-36
		     "FAM1","FAM2", // 37-38
             "RE1",  "RE2", // 39-40
			 "FILE1", "FILE2", // 41-42
			 "ABB1", "ABB2", // 43-44
             "KEY1", "KEY2", // 45-46
			 "GDC1", "GDC2", // 47-48  // German divided compounds

			 // only pair descriptors!!!

};

static std::unordered_map<std::string, Descriptors> init_map_str_to_id() {
	std::unordered_map<std::string, Descriptors> result;
	for (size_t i = 0; i < NumberOfGraphematicalDescriptors; ++i) {
		result[DesArray[i]] = (Descriptors)i;
	}
	return result;
}

const std::unordered_map<std::string, Descriptors> DesStr_To_Id = init_map_str_to_id();



bool IsFirstMemberOfPairDesciptor(Descriptors d) 
{
	return		(d%2 == 0) 
			&&	(d >= (int)OEXPR1) 
			&&  (d < NumberOfGraphematicalDescriptors);
};

bool IsSecondMemberOfPairDesciptor(Descriptors d) 
{
	return		(d%2 != 0) 
			&&	(d >= (int)OEXPR2)
			&&  (d < NumberOfGraphematicalDescriptors);
};

Descriptors GetSecondMemberByTheFirst(Descriptors d) 
{
	assert   (IsFirstMemberOfPairDesciptor(d));
	return (Descriptors)(d+1);
};

Descriptors GetFirstMemberByTheSecond(Descriptors d) 
{
	assert   (IsSecondMemberOfPairDesciptor(d));
	return (Descriptors)(d-1);
};

const std::string& GetDescriptorStr(Descriptors d)
{
	return DesArray[d];
};



 
