#ifndef __MorphXMLtoken_h
#define __MorphXMLtoken_h

#include "../common/utilit.h"

const std::string XmlPunctType = "pun";
const std::string XmlWordType = "w";
//! CXmlMorphAnnot holds one morphological interpretation, which is read from xml under CConcIndexator::m_IndexType == MorphXML_Index
struct  CXmlMorphAnnot
{
	//! lemma
	std::string				m_Lemma;
	//! other morphological features
	std::string				m_GrammemsStr;
    //! reference to xml node
    void*               m_XmlRef;
	//! return the structure as a std::set of token properties
	void	GetAsSetOfProperties(std::vector<std::string>& Result)  const;
    //! return std::string annotation for words from Russian corpus
    std::string  BuildRusCorpAnnot() const;
};

//! CXmlToken holds a word and all ist  morphological interpretations (used under CConcIndexator::m_IndexType == MorphXML_Index)
struct CXmlToken 
{
	//! type of token (a word, a punctuation mark)
	std::string						m_Type;
	//! the std::string itself
	std::string						m_WordStr;
	//! original  std::string itself 
	std::string						m_OrigWordStr;
	//! all morphological annotations
	std::vector<CXmlMorphAnnot>		m_Annots;
	//! true if the token is last in the sentence
	bool						m_bLastInSentence;
    bool                        IsImportantPunct() const;

	CXmlToken();
};

extern std::string POS_Dialing2MyStem(std::string pos);
extern std::string Grammem_Dialing2MyStem(std::string pos);
extern std::string POS_MyStem2Dialing(std::string pos);
extern std::string Grammem_MyStem2Dialing(std::string pos);
extern bool	  IsMyStemSpecificGrammem(std::string g);
extern bool IsImportantRusCorpFeature(const std::string& s);

#endif

