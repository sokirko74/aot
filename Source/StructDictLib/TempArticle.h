// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)


#ifndef _TEMPARTICLE_H_
#define _TEMPARTICLE_H_

#include "Ross.h"       // main symbols
#include "TextField.h"       // main symbols


class CTempArticle 
{
	std::string					m_ArticleStr;
	const TCortege10& GetRossCortege(size_t i) const;
	std::string		ConstructFldName(BYTE FieldNo, BYTE LeafId, BYTE BracketLeafId);
	bool		PutCortegeOnTheRigthPosition(const TCortege10& C);
	bool		ArticleToText();

public:
	char					m_EntryStr[EntryStrSize];
	BYTE					m_MeanNum;
	WORD					m_UnitNo;
	bool						m_ReadOnly;
	std::vector<CTextField>		m_Fields;
	CDictionary*				m_pRoss;
	
	std::vector<TCortege10>		m_Corteges;
	std::string				m_LastError;
	int						m_ErrorLine;
	CTempArticle();
	


	size_t				GetCortegesSize () const;
	const TCortege10&	GetCortege (size_t i)  const;
	bool		IsPartOf(const CTempArticle *Article, bool UseWildCards) const;
	int			IntersectByFields(const CTempArticle *Article) const;
	bool		AddArticle(const CTempArticle *Article);
	bool		AddCortegeToVector (CTextField& F);
	bool		CheckCortegeVector ();
	bool		ReadFromUtf8String(const char * s);
	bool		MarkUp();
	bool		BuildCortegeList();
	bool		WriteToDictionary();
	bool		IsModified() const;
	void		ReadFromDictionary(WORD UnitNo, bool VisualOrder, bool ReadOnly);
	const std::string& GetArticleStr();
	std::string GetArticleStrUtf8(bool check=false);
	
};

#endif
