// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)


#ifndef _TEMPARTICLE_H_
#define _TEMPARTICLE_H_

#include "Ross.h"       // main symbols
#include "TextField.h"       // main symbols


class CTempArticle 
{
	string					m_ArticleStr;
public:
	char					m_EntryStr[EntryStrSize];
	BYTE					m_MeanNum;
	WORD					m_UnitNo;
	bool						m_ReadOnly;
	vector<CTextField>		m_Fields;
	CDictionary*				m_pRoss;
	
	vector<TCortege10>		m_Corteges;
	string				m_LastError;
	int						m_ErrorLine;
	CTempArticle();
	


	const TCortege10&	GetRossCortege (size_t i) const;
	size_t				GetCortegesSize () const;
	const TCortege10&	GetCortege (size_t i)  const;
	string		ConstructFldName (BYTE FieldNo, BYTE LeafId, BYTE BracketLeafId);
	bool		IsPartOf(const CTempArticle *Article, bool UseWildCards) const;
	int			IntersectByFields(const CTempArticle *Article) const;
	bool		AddArticle(const CTempArticle *Article);
	bool		PutCortegeOnTheRigthPosition (const TCortege10& C);
	bool		ArticleToText ();
	bool		AddCortegeToVector (CTextField& F);
	bool		CheckCortegeVector ();
	bool		SetArticleStr(const char * s);
	bool		MarkUp();
	bool		BuildCortegeList();
	bool		WriteToDictionary();
	bool		IsModified() const;
	void		ReadFromDictionary(WORD UnitNo, bool VisualOrder, bool ReadOnly);
	const string& GetArticleStr();
	
};

#endif
