// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)


#pragma once

#include "TextField.h"
#include "CortegeContainer.h"
#include "../../common/cortege.h"

class article_parse_error : public std::runtime_error {
	mutable std::string message;
public:
	int source_line_no;
	explicit article_parse_error(std::string what_arg, int _source_line_no);
	const char* what() const noexcept override;
};

class CDictionary;

class CTempArticle : public TCortegeContainer
{
	std::string					m_ArticleStr;
	const TCortege& GetRossCortege(size_t i) const;
	std::string		ConstructFldName(BYTE FieldNo, BYTE LeafId, BYTE BracketLeafId);
	bool		PutCortegeOnTheRigthPosition(const TCortege& C);
	bool		ArticleToText();
	void		AddCortegeToVector(CTextField& F);
	void		MarkUp();
	void		BuildCortegeList();
	void		CheckCortegeVector();

	CDictionary* m_pRoss;
	bool  m_ReadOnly;


public:
	std::string				m_EntryStr;
	BYTE					m_MeanNum;
	uint16_t					m_UnitNo;
	std::vector<CTextField>		m_Fields;
	
	CTempArticle(CDictionary* pRossDoc=nullptr);
	
	size_t				GetCortegesSize () const;
	const TCortege&	GetCortege (size_t i)  const;
	bool		IsPartOf(const CTempArticle *Article, bool UseWildCards) const;
	int			IntersectByFields(const CTempArticle *Article) const;
	bool		AddArticle(const CTempArticle *Article);
	void		ReadFromUtf8String(const char * s);
	void		WriteToDictionary();
	bool		IsModified() const;
	void		ReadFromDictionary(uint16_t UnitNo, bool VisualOrder, bool ReadOnly);
	std::string GetArticleStrUtf8(bool check=false);
	void        SetUnitNo(uint16_t UnitNo);
};

