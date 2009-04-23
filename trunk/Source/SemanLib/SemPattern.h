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
  vector<TCortege10>	m_GramCorteges;
  // LEXi для этого актанита
  StringVector		m_LexFets;
  // MANLEXi для этого актанита
  StringVector		m_ObligLexFets;
  // DETERMi для этого актанита
  vector<TCortege10>	m_ArticleCorteges;
  // направление, в котором следует искать этот актант (см. домен D_POSITION)
  string				m_Direction;
  // SFi для этого актанита
  vector< vector<string> >		m_ActantSemFets;
  // словарь, из которого было взято это грамматическое выражение
  const CRossHolder*			m_pRossDoc;
  const CRossHolder*			GetRossHolder() const;


  CSemPattern ();
  void		InitSemPattern(const CRossHolder* pRossDoc, WORD UnitNo, BYTE LeafId, BYTE BracketLeafId);
  long		GetSynRel(long CortegeNo) const;
  long		GetSynFet(long CortegeNo) const;
  // то же, что и предыдущие две функции, но выдаются строки
  string	GetSynFetStr(long CortegeNo) const;
  string	GetSynRelStr(long CortegeNo) const;
  bool      HasSemFet (const string& SemFet) const;
  void		SetEmpty();
  bool		IsEmpty() const;
  void		InsertReverseSynOToTheBegining();
  void		TracePattern () const;
  bool		LoadSemFromDict();
  bool		LoadGramFromDict();
  

};


#endif

