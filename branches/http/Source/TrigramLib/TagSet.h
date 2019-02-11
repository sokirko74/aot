#ifndef TagSet_h
#define TagSet_h

#include "../common/util_classes.h"
class CAgramtab;


struct CTag 
{
	string			m_Name;
	vector<string>	m_Lemmas;
	BYTE			m_Pos;
	QWORD			m_Grammems;
	

	CTag();
	bool operator < (const CTag& T) const;
	bool IsEmptyTag() const;
};



class CTagSet
{
public:
	map<string, CTag>	m_Tags;
	CTagSet(void);
	~CTagSet(void);
	// build the default (part of speech) tag set
	bool			BuildDefaultTags(const CAgramtab* pAgramtab);
	// read the tag set from file
	bool			ReadTagSet(string FileName, const CAgramtab* pAgramtab);
	//bool			ReadMyStemTagSet(string FileName, const CAgramtab* pAgramtab);

	CTag			GetBestSuitedTag (const CTag& A) const;
	vector<CTag>	DecipherTagStr (string TagStr, const CAgramtab* pAgramtab) const;
	CTag			GetTagFromAncode(const CAgramtab* pAgramtab, const char* Ancode, QWORD CommonGrammems, const string& Lemma) const;
};


#endif
