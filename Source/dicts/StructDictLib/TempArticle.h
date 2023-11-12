// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)


#pragma once

#include "TextField.h"
#include "CortegeContainer.h"
#include "../../common/cortege.h"


class CTempArticle : public TCortegeContainer
{
	std::string					m_ArticleStr;
	const TCortege& GetRossCortege(size_t i) const;
	std::string		ConstructFldName(BYTE FieldNo, BYTE LeafId, BYTE BracketLeafId);
	bool		PutCortegeOnTheRigthPosition(const TCortege& C);
	bool		ArticleToText();
	
	CDictionary* m_pRoss;
	bool  m_ReadOnly;


public:
	char					m_EntryStr[EntryStrSize];
	BYTE					m_MeanNum;
	uint16_t					m_UnitNo;
	std::vector<CTextField>		m_Fields;
	
	std::string				m_LastError;
	int						m_ErrorLine;

	CTempArticle(CDictionary* pRossDoc=nullptr);
	
	size_t				GetCortegesSize () const;
	const TCortege&	GetCortege (size_t i)  const;
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
	void		ReadFromDictionary(uint16_t UnitNo, bool VisualOrder, bool ReadOnly);
	const std::string& GetArticleStr();
	std::string GetArticleStrUtf8(bool check=false);
	
};

