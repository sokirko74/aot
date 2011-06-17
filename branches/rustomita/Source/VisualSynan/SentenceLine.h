// SentenceLine.h: interface for the CSentenceLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENTENCELINE_H__B0ECF574_23B8_11D3_8C48_00105A68ADF3__INCLUDED_)
#define AFX_SENTENCELINE_H__B0ECF574_23B8_11D3_8C48_00105A68ADF3__INCLUDED_

#include "stdafx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef vector<int> CIntVector;

struct SFigure 
{
	SFigure(int iNum, int iPartNum)
	{
		m_iNum = iNum;
		m_iPartNum = iPartNum;
	};

	SFigure() {};

	int m_iNum;
	int m_iPartNum;
};

struct SFigureInClause : public SFigure
{
	SFigureInClause() {};

	int m_iClauseNum;
};

typedef vector<SFigure> CFigureVector;
typedef vector<SFigureInClause> CFigureInClauseVector;


class CSentenceLine  
{
public:
	CSentenceLine();
	virtual ~CSentenceLine();
	void Reset()
	{
		m_vectorWordsNum.clear();
		m_vectorGrArc.clear();
		m_vectorRelArc.clear();
		m_vectorClauseArc.clear();
	};

	//номера слов на этой строчке
	CIntVector m_vectorWordsNum;

	//номера и номера частей всяких дуг и арок на этой строчке
	//(часть - кусок фигуры)
	CFigureInClauseVector	m_vectorGrArc;
	CFigureInClauseVector	m_vectorRelArc;
	CFigureVector			m_vectorClauseArc;

	CRect					m_rectLine;
	BOOL					m_bShowGroups;
};



enum ETypeFigure { ArcFig, Line };



class CPartOfArc
{
	
public:

	CPartOfArc() { m_bHasDescription = FALSE; };
	BOOL	m_bHasDescription;
	int		m_iLine;
	BOOL	m_bWholeArc;
	long    m_Color;		
	CPoint	m_pointStart;
	CPoint	m_pointEnd;	
	CRect	m_RectForArc;

};

class CPartOfGrArc : public CPartOfArc
{
public:

	CPartOfGrArc()
	{ Reset(); };

	void Reset()
	{
		m_bHasDescription = FALSE;
		m_iLine = -1;
		m_Color = 0;
		m_bWholeArc = FALSE;
	};

	//CPoint	m_Point1;
	//CPoint	m_Point2;	
	ETypeFigure		type;
	CPoint	m_pointTextStart;	
};

class CPartOfRelArc : public CPartOfArc
{
public:

	CPartOfRelArc()
	{ Reset(); };

	void Reset()
	{ 
		m_bHasDescription;
		m_RectForArc.SetRectEmpty();
		m_pointStart = CPoint(0,0);
		m_pointEnd = CPoint(0,0);
		m_rectForDescr.SetRectEmpty(); 
		m_bWholeArc = FALSE;
	};
	
	CRect	m_rectForDescr;	
	
};


#endif // !defined(AFX_SENTENCELINE_H__B0ECF574_23B8_11D3_8C48_00105A68ADF3__INCLUDED_)
