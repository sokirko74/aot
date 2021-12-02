// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef MorphAutomat_h
#define MorphAutomat_h

#include "../common/util_classes.h"


const size_t MaxAlphabetSize = 54;
const size_t ChildrenCacheSize = 1000;

struct CMorphAutomNode 
{
	//  the highest bit of CMorphAutomNode::m_Data contains final/not final flag;
	//  the rest is an index to CMorphAutomat::m_Relations (an index 
	//   to the place where the first child is)
	uint32_t							m_Data;

	uint32_t	GetChildrenStart()  const
	{
		return m_Data&(0x80000000-1); 
	};
	bool	IsFinal()  const
	{
		return (m_Data&0x80000000) > 0; 
	};
	void	SetChildrenStart(uint32_t v) 
	{
		m_Data = (0x80000000&m_Data)|v;  
	};
	void	SetFinal(bool v) 
	{
		if (v)
			m_Data |= 0x80000000;  
		else
			m_Data &= 0x80000000-1;  
	};

};

struct CMorphAutomRelation
{
	//  the highest byte of CMorphAutomRelation::m_Data contains relational char;
	//  the rest is an index to CMorphAutomat::m_Nodes 

	uint32_t							m_Data;

	uint32_t	GetChildNo()  const
	{
		return m_Data & 0xffffff; 
	};
	BYTE	GetRelationalChar()  const
	{
		return m_Data>>24; 
	};

	void	SetChildNo(uint32_t v) 
	{
		m_Data = (0xff000000&m_Data)|v;  
	};

	void	SetRelationalChar(BYTE v) 
	{
		m_Data = (0xffffff&m_Data)| (v<<24);  
	};

};

struct CAutomAnnotationInner
{
	// these members are read from the automat
	uint16_t	m_ModelNo;
	uint16_t	m_ItemNo;
	uint16_t	m_PrefixNo;

	// these members are calculated later
	int		m_LemmaInfoNo;
	int		m_nWeight;

	uint32_t GetParadigmId() const 
	{
		return (m_PrefixNo<<23) | m_LemmaInfoNo;  
	}
	void SplitParadigmId(uint32_t value) 
	{
		m_PrefixNo = value>>23;
		m_LemmaInfoNo = value&0x7fffff;
	}
};
const size_t MinimalPredictionSuffix  = 3;
const BYTE MorphAnnotChar = '+';
class CABCEncoder 
{
public:

	MorphLanguageEnum			m_Language;
	const BYTE					m_AnnotChar;
	int							m_AlphabetSize;
	int							m_Alphabet2Code[256];					
	int							m_Code2Alphabet[MaxAlphabetSize];
	

	int							m_AlphabetSizeWithoutAnnotator;
	int							m_Alphabet2CodeWithoutAnnotator[256];					
	int							m_Code2AlphabetWithoutAnnotator[MaxAlphabetSize];

	void	CheckABCWithAnnotator(const std::string& WordForm) const;
	bool	CheckABCWithoutAnnotator(const std::string& WordForm) const;
	std::string	EncodeIntToAlphabet(uint32_t v) const;
	uint32_t	DecodeFromAlphabet(const std::string& v) const;
	std::string	GetCriticalNounLetterPack() const;
	CABCEncoder(MorphLanguageEnum Language, BYTE AnnotChar);


};

class CMorphAutomat : public CABCEncoder
{
	
protected:
	
	CMorphAutomNode*				m_pNodes;
	size_t							m_NodesCount;

	CMorphAutomRelation*			m_pRelations;
	size_t							m_RelationsCount;

	std::vector<int>						m_ChildrenCache;
	

	
	void	DumpAllStringsRecursive(FILE* fp, int NodeNo, std::string CurrPath) const;
	void	BuildChildrenCache();
	void	GetAllMorphInterpsRecursive (int NodeNo, std::string& curr_path, std::vector<CAutomAnnotationInner>& Infos) const;
	int		FindStringAndPassAnnotChar (const std::string& Text, size_t TextPos) const;
	void	Clear();

public:
	
	CMorphAutomat(MorphLanguageEnum Language, BYTE AnnotChar);
	~CMorphAutomat();
	void	Load(std::string GrammarFileName);
	bool	Save(std::string GrammarFileName) const;
	bool	DumpAllStrings(std::string FileName) const;
	void	GetInnerMorphInfos (const std::string& Text, size_t TextPos, std::vector<CAutomAnnotationInner>& Infos) const;
	const CMorphAutomRelation*  GetChildren(size_t NodeNo) const;
	int		NextNode(int NodeNo, BYTE Child) const;
	size_t	GetChildrenCount(size_t NodeNo)  const;
	const CMorphAutomNode& GetNode(int NodeNo) const { return m_pNodes[NodeNo];} ;
	uint32_t	EncodeMorphAutomatInfo (size_t ModelNo, size_t ItemNo, size_t PrefixNo) const;
	void	DecodeMorphAutomatInfo (uint32_t Info, size_t& ModelNo, size_t& ItemNo, size_t& PrefixNo) const;
	std::string	GetFirstResult (const std::string& Text) const;
	
};



#endif
