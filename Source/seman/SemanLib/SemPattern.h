#ifndef SemPattern_H
#define SemPattern_H

class CSemanRossHolder;
class CRossHolder;
#include "RossHolder.h"

class CSemPattern {
public:	 
  // узел, чьим актантом он является
  long					m_SourceNo; 	 
  // валентность, для этого актанта, взятая из поля VAL
  CValency				m_PatternValency;
  // GFi для этого актанита
  std::vector<TCortege>	m_GramCorteges;
  // LEXi для этого актанита
  StringVector		m_LexFets;
  // MANLEXi для этого актанита
  StringVector		m_ObligLexFets;
  // DETERMi для этого актанита
  std::vector<TCortege>	m_ArticleCorteges;
  // направление, в котором следует искать этот актант (см. домен D_POSITION)
  std::string				m_Direction;
  // SFi для этого актанита
  std::vector< std::vector<std::string> >		m_ActantSemFets;
  // словарь, из которого было взято это грамматическое выражение
  const CRossHolder*			m_pRossDoc;
  const CRossHolder*			GetRossHolder() const;


  CSemPattern ();
  void		InitSemPattern(const CRossHolder* pRossDoc, uint16_t UnitNo, BYTE LeafId, BYTE BracketLeafId);
  dom_item_id_t		GetSynRel(long CortegeNo) const;
  dom_item_id_t		GetSynFet(long CortegeNo) const;
  // то же, что и предыдущие две функции, но выдаются строки
  std::string	GetSynFetStr(long CortegeNo) const;
  std::string	GetSynRelStr(long CortegeNo) const;
  bool      HasSemFet (const std::string& SemFet) const;
  void		SetEmpty();
  bool		IsEmpty() const;
  void		InsertReverseSynOToTheBegining();
  void		TracePattern () const;
  bool		LoadSemFromDict();
  bool		LoadGramFromDict();
  

};


#endif

