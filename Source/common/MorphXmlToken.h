#ifndef __MorphXMLtoken_h
#define __MorphXMLtoken_h

#include "../common/utilit.h"

const string XmlPunctType = "pun";
const string XmlWordType = "w";
//! CXmlMorphAnnot holds one morphological interpretation, which is read from xml under CConcIndexator::m_IndexType == MorphXML_Index
struct  CXmlMorphAnnot
{
	//! lemma
	string				m_Lemma;
	//! other morphological features
	string				m_GrammemsStr;
    //! reference to xml node
    void*               m_XmlRef;
	//! return the structure as a set of token properties
	void	GetAsSetOfProperties(vector<string>& Result)  const;
    //! return string annotation for words from Russian corpus
    string  BuildRusCorpAnnot() const;
};

//! CXmlToken holds a word and all ist  morphological interpretations (used under CConcIndexator::m_IndexType == MorphXML_Index)
struct CXmlToken 
{
	//! type of token (a word, a punctuation mark)
	string						m_Type;
	//! the string itself
	string						m_WordStr;
	//! original  string itself 
	string						m_OrigWordStr;
	//! all morphological annotations
	vector<CXmlMorphAnnot>		m_Annots;
	//! true if the token is last in the sentence
	bool						m_bLastInSentence;
    bool                        IsImportantPunct() const;

	CXmlToken();
};

extern string POS_Dialing2MyStem(string pos);
extern string Grammem_Dialing2MyStem(string pos);
extern string POS_MyStem2Dialing(string pos);
extern string Grammem_MyStem2Dialing(string pos);
extern bool	  IsMyStemSpecificGrammem(string g);
extern bool IsImportantRusCorpFeature(const string& s);

#endif

