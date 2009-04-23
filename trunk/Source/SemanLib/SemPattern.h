#ifndef SemPattern_H
#define SemPattern_H

class CSemanRossHolder;
class CRossHolder;
#include "RossHolder.h"

class CSemPattern {
public:	 
  // ����, ���� �������� �� ��������
  long					m_SourceNo; 	 
  // �����������, ��� ����� �������, ������ �� ���� VAL
  CValency				m_PatternValency;
  // GFi ��� ����� ��������
  vector<TCortege10>	m_GramCorteges;
  // LEXi ��� ����� ��������
  StringVector		m_LexFets;
  // MANLEXi ��� ����� ��������
  StringVector		m_ObligLexFets;
  // DETERMi ��� ����� ��������
  vector<TCortege10>	m_ArticleCorteges;
  // �����������, � ������� ������� ������ ���� ������ (��. ����� D_POSITION)
  string				m_Direction;
  // SFi ��� ����� ��������
  vector< vector<string> >		m_ActantSemFets;
  // �������, �� �������� ���� ����� ��� �������������� ���������
  const CRossHolder*			m_pRossDoc;
  const CRossHolder*			GetRossHolder() const;


  CSemPattern ();
  void		InitSemPattern(const CRossHolder* pRossDoc, WORD UnitNo, BYTE LeafId, BYTE BracketLeafId);
  long		GetSynRel(long CortegeNo) const;
  long		GetSynFet(long CortegeNo) const;
  // �� ��, ��� � ���������� ��� �������, �� �������� ������
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

