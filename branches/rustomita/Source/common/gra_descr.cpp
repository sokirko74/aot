#include "util_classes.h"
#include "gra_descr.h"

const int ODesLen    = 9;                   /* the length of unit's descriptor */

const char DesArray[NumberOfGraphematicalDescriptors][ODesLen] = 
{
		     
    	     "BEG","RLE", "LLE","DEL","PUN",//the first position :  //0-4
		     "DC", "DSC","EA","_UNK_", // 5-8
		     //the second position :
		     "SPC","HYP","EOLN","EOP", //9-12
		     "PLP","AA", "aa", "Aa", // 13-16
			 "NAM?", "OPN","CLS", // 17-19
			 "EMSYM", "BUL", "INDENT", // 20-22
             "dummy", "#QUOTED", "PAR_SYM", // 23-25

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

bool GetDescriptorStr(int DescriptorNo, string& Result)
{
	if (DescriptorNo >= NumberOfGraphematicalDescriptors)
		return false;

	Result = DesArray[DescriptorNo];
	return true;
};

const char* GetDescriptorStr(int DescriptorNo)
{
	assert (DescriptorNo < NumberOfGraphematicalDescriptors);
	return DesArray[DescriptorNo];
};



/*
 all descriptors which end a text period like a sentence or a paragraph
*/
bool IsEndTextPeriodDescriptor (Descriptors d)
{
	return     (d == CS_Undef)
			|| (d == CS_Simple)
			|| (d == CS_Parent)
			|| (d == CS_Quasi)
			|| (d == CS_Heading)
			|| (d == CS_Explan)
			|| (d == OSentEnd);
};



QWORD parse_gra_descriptors ( const char * s, string& unparsed)
{
    unparsed = "";
    StringTokenizer tok (s, "\t\r\n ");
    QWORD Res = 0;
    while (tok())
    {
        const char * d =  tok.val();
        int i = 0;
	    for (; i< NumberOfGraphematicalDescriptors; i++)
		    if (!strcmp(d,DesArray[i]) )
            {
                Res |= _QM(i);
                break;
            }

        if (i == NumberOfGraphematicalDescriptors)
        {
            if (!unparsed.empty())
                unparsed += " ";
            unparsed += d;
        }
    }

	return Res;
}
 
