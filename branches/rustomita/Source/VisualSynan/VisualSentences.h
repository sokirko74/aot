#ifndef _VisualSentences
	#define _VisualSentences


#define COM_NO_WINDOWS_H

#include "SentenceLine.h"


typedef vector<CSentenceLine> CSentLineVector;
typedef vector<CPartOfArc*>  CPartOfArcVector;
typedef vector<int>	CIntVector;
typedef pair<int, int>		CIntPair;

#import "../../bin/Graphan.tlb"

#define RADIUS_ANGLE 8
#define PLACE_HOLDER "###"
#define SPACE_BETWEEN_SENTENCE 20
#define ON_DISPLAY -1

#define DELETE_ARR_IF_NOT_NULL(p) if(p) delete[] (p)




class CVisualSentence;

const char g_strAgramTabRegPath[] = "Software\\Dialing\\Lemmatizer\\Russian\\Rgramtab";

class CVisualSentences
{
public:
	CVisualSentences();
	~CVisualSentences();
	CString m_WorkTimeStr;

	void Reset();

	int SentCount();
	void BuildRels(CString& report);
	BOOL CalculateCoordinates(CDC* pDC,int iWidth,BOOL bShowGroups);
	BOOL GetHomonymsArray(CPoint& point,CPtrArray** pHomonymsArray, int* iActiveSentence,int* iActiveWord);
	BOOL SetActiveHomonym(int iActiveSentence,int iActiveWord, int iActiveHomonim);
	BOOL GetActiveHomDescr(int m_iActiveSentenceTT,int m_iActiveWordTT,CString& strLemma,CString& strGramChar);
	CSize GetSentencesSize();	
	BOOL PrintSentences(CDC* pDC, CRect& rectForDrawing, int iOffset);
	int PrintSentencesOnPrn(CDC* pDC, int iSent, int& iLine, int iBottom, int& iOffset);
	void SetPlmFile(CString strPlmFile)
	{
		m_strPlmFile = strPlmFile;
	}	
	BOOL FillSentencesArray(ISentencesCollectionPtr& piSentCollection);

protected:
	
	CPtrArray m_arrSentences;
	CString m_strPlmFile;	
	BOOL m_bLocked;
};

struct CVisualGroup;
struct CVisualPeriod;
class CVisualClause;

class CVisualGroups 
{
public:
	void  CalculateGroupsCoordinates(CDC* pDC,int iWidth,int& iLine,int iBottom);
	void SetWordsArray(CPtrArray* pArr)
		{ m_pWordsArr = pArr; }


protected:
	BOOL GetRightLegPointForGroupArc(int iGroupNum, CPoint* pPoint, int iWidth);
	BOOL  GetLeftLegPointForGroupArc(int iGroupNum, CPoint* pPoint, int iWidth);
	vector<CVisualGroup*> m_arrActiveGroups;
	CPtrArray* m_pWordsArr;
	int m_iMaxGroupLevel;
	int m_iSpaceBetweenLinesG;
};



class CVisualSentence  : public CVisualGroups
{

public:
	CVisualSentence();
	~CVisualSentence();
	int CalculateSpaceBetweenLines(CDC* pDC, BOOL bShowGroups);
	int GetSpaceBetweenLines()
	{ return m_iSpaceBetweenLinesG; };
	BOOL PrintSentences(CDC* pDC);
	BOOL FillActiveGroupsArray(BOOL bUseGroupID);
	BOOL Init(ISentencePtr& piSentence);
	BOOL PointInSentRect(CPoint& point) { return m_SentRect.PtInRect(point); };
	BOOL GetHomonymsArray(CPoint& point,CPtrArray** pHomonymsArray, int* iActiveWord);
	BOOL GetActiveHomDescr(int iActiveWord,CString& strLemma,CString& strGramChar);
	BOOL SetActiveHomonym(int iActiveWord, int iActiveHomonim);
	int GetMaxGroupLevel()
		{ return m_iMaxGroupLevel; }
	int CalculateCoordinates(CDC* pDC,int iY,int iWidth,BOOL bShowGroups);
	void FillLinesVector(BOOL bShowGroups);
	void FillClauseLine(CFigureVector& vectorFigure, CIntPair& LineTopAndBot);	
	void PrintAllLines(CDC* pDC);
	void PrintLine(CDC* pDC, int i, int& iOffset);
	int	 InRect(CRect& rectForDrawing);
	BOOL PrintAllLinesInRect(int iStartLine, CDC* pDC, CRect& rectForDrawing, int iOffset);
	BOOL LineInrRect(CRect& rectForDrawing, CSentenceLine& line);
	BOOL PrintAllLinesTillBottom(int& iStartLine,CDC*  pDC,int iBottom,int& iOffset) ;
	int GetLineBottom(int i);
	int GetLinesCount()
	{ return m_vectorSentLines.size();}

	CRect m_SentRect;

protected:

	BOOL ReadWords(ISentencePtr& piSentence);
	BOOL ReadClauses(ISentencePtr& piSentence);
	vector<CVisualClause*> m_arrClauses;
	CPtrArray m_arrWords;
	int m_iMaxClauseGroupLevel;
	int m_iMaxClausesLevel;
	int m_iMaxClauseRelsLevel;
	CSentLineVector m_vectorSentLines;
};

struct CVisualPeriod
{
	int m_iSpaceBetweenLines;
	int m_iFirstWord;
	int m_iLastWord;
	int m_iLevel;
	CString m_strDescription;
	CPartOfArcVector m_vectorParts;

	CVisualPeriod()
	{
		m_iFirstWord = -1;
		m_iLastWord = -1;
		m_iLevel = -1;
		m_strDescription.Empty();
	}

	~CVisualPeriod()
	{
		for(int i = 0 ; i < m_vectorParts.size() ; i++ )
			delete m_vectorParts[i];
		
	}


	void ResetParts()
	{
		for(int i = 0 ; i < m_vectorParts.size() ; i++ )
			delete m_vectorParts[i];
		m_vectorParts.clear();
	}


	BOOL operator<(const CVisualPeriod& period) const
	{  
			if( ( period.m_iFirstWord <= m_iFirstWord ) &&
				( period.m_iLastWord >= m_iLastWord ) )
				return TRUE;

			if( period.m_iFirstWord >= m_iLastWord )
				return TRUE;

			return FALSE;
	}

	BOOL CreateSmallFont(LOGFONT& p_lfFont,CFont* pFont);
	void SetSpaceBetweenLines(int iVal)
	{ m_iSpaceBetweenLines = iVal; }

	
}; 





struct CVisualGroup : public CVisualPeriod  
{
	CPoint m_pointLeft;
	CPoint m_pointRight;	
	BOOL   m_bClause;


	CVisualGroup();
	BOOL Init(IGroupPtr& piGroup);
	BOOL CalculateCoordinates(CDC* pDC, CPoint& pointLeftLeg, CPoint& poitRightLeg, int top, int iWidth, BOOL bOnDifferentLines, COLORREF Color,int iLine);
	void PrintGroupPart(CDC* pDC, int i, int iOffset = 0);
};


struct SUnit
{
	SUnit()
	{
		m_iClauseNum = -1;
		m_iWordNum = -1;
		m_iHommonyNum = -1;
		m_iClauseTypeNum = -1;
	}

	EUnitType m_Type;
	int m_iClauseNum;
	int m_iWordNum;
	int m_iHommonyNum;
	int m_iClauseTypeNum;
	CString m_strOborotsNum;
	CString m_strGrammems;
}; 


struct SHomNumAndGrammems
{
	CString m_strGrammems;
	int m_iHomNum;
};



class CVisualSynVariant 
{
public:
	vector<SUnit> m_vectorUnits;
 	vector<CVisualGroup> m_Groups;
	vector<int>		   m_Subjects;
	int		   m_iPredk;		
	int		   m_iClauseTypeNum;
	int		   m_Weight;


	CVisualSynVariant() { Reset(); };
	
	void Reset()
	{
		
		m_vectorUnits.clear();
		m_Subjects.clear();
		m_iPredk = -1;		
	};	

	void InitClauseVariant(IClausePtr& piClause, int ClauseVariantNo);

	bool operator==(const vector<SUnit> vectorUnits) const
	{
		if( m_vectorUnits.size() != vectorUnits.size() )
			return false;

		for(int i = 0 ; i < m_vectorUnits.size() ; i++ )
		{
			if(		(vectorUnits[i].m_Type != m_vectorUnits[i].m_Type)
				||	(vectorUnits[i].m_iClauseNum != m_vectorUnits[i].m_iClauseNum)
				||	(vectorUnits[i].m_iWordNum != m_vectorUnits[i].m_iWordNum)
				||	(vectorUnits[i].m_iHommonyNum != m_vectorUnits[i].m_iHommonyNum)
				||	(vectorUnits[i].m_iClauseTypeNum != m_vectorUnits[i].m_iClauseTypeNum)
			   )
				return false;


		}

		return true;
	}


};



class CClauseType 
{
public:
	CClauseType() 
	{
		m_iWordNum = 0;
		m_bHomChoosen = FALSE;
	};

	~CClauseType () {};
	BOOL Init(IClauseRootPtr piClauseType);
	BOOL HasHom(int iHom)
	{
		for(int i = 0 ; i <  m_arrHomNum.GetSize() ; i++ )
			if(m_arrHomNum.GetAt(i) == (unsigned int)iHom)
				return TRUE;

		return FALSE;
	}


	CUIntArray m_arrHomNum;
	int m_iWordNum;
	BOOL m_bHomChoosen;
	CString m_strName;
};

class CVisualClause  : public CVisualGroup, public CVisualGroups
{

friend class CVisualSentence;

public:
	CVisualClause();
	~CVisualClause();
	BOOL InitVisualClause(IClausePtr& piClause);
	BOOL CalculateClauseGroupCoordinates(CDC* pDC,int iWidth,BOOL bShowGroups, int& iLine);
	BOOL FillActiveGroupsArray(BOOL bUseGroupID, map<int, int>& mapClauseNumTypeNum, int iClauseNum);
	
	int GetMaxGroupLevel()
		{ return m_iMaxGroupLevel; }
	int GetMaxRelsLevel()
		{ return m_iMaxRelsLevel; }

	void SetMaxRelsLevel(int  iMaxRelsLevel )
	{ m_iMaxRelsLevel = iMaxRelsLevel;}

	void SetMaxGropLevel(int iMaxGroupLevel )
	{ m_iMaxGroupLevel = iMaxGroupLevel;}

	void  CalculateRelationsCoordinates(CDC* pDC,int iWidth, int& iLine);
	void SetBestHomonyms();
	void SetSpaceBetweenLines(int iSpaceBetweenLines)
	{
		m_iSpaceBetweenLinesG = iSpaceBetweenLines;
		m_iSpaceBetweenLines = iSpaceBetweenLines;
	};

protected:

	void SetActiveType(int iType);
	void  AssignGroupsLevel();
	CString m_strConjsName;
	int m_iArrActiveRelsCount;
	vector<CVisualSynVariant> m_SynVars;
	CPtrArray m_arrTypes;
	CUIntArray m_arrPresentSetOfHomonims;
	CUIntArray m_arrPresentSetOfGroups;
	int m_iMaxRelsLevel;
	int m_iAllRelsCount;
	int m_iActiveType;
	BOOL m_bBestVariant;
	CString m_strTypeName;	
	int		m_Weight;
};

class CSynHomonym : public CObject
{
public:

	CSynHomonym()
	{
		m_bSubj = FALSE;
		m_bPredk = FALSE;
	}
	~CSynHomonym()
	{
	}
	BOOL m_bPredk;
	BOOL m_bSubj;
	CString m_strLemma;
	CString m_strCommonGrammems;
	CString m_strPOS;
	CString m_strSomeDescr;
	CString m_strOborotsNum;
	vector<int> m_OborotsNum;
	int m_iPradigmID;

protected:

};


class CVisualWord : public CObject
{
public:
	CVisualWord();
	~CVisualWord();

	BOOL Init(IWordPtr& piWord, ISentencePtr& piSentence);
	int GetWordLen(CDC* pDC);
	int GetWordHight(CDC* pDC);
	int PrintWord(CDC* pDC,int iX, int iY);
	BOOL PointInWordRect(CPoint& point);
	void GetHomonymsArray(CPtrArray** pHomonymsArray);
	BOOL SetActiveHomonym(int iActiveHomonim);
	BOOL GetActiveHomDescr(CString& strLemma,CString& strGramChar);
	int  GetHomonymsCount()
	{ return m_arrHomonyms.GetSize(); };
	BOOL PrintWord(CDC* pDC, int iOffset = 0);
	int  CalculateCoordinates(CDC* pDC,int iX, int iY);
	void ResetSubjAndPred();

//protected:

	CString m_strWord;
	CString m_strSomeDescr;

	CRect m_WordRect;
	int m_iActiveHomonym;
	CWordArray m_arrActiveVariants;
	CPtrArray m_arrHomonyms;
	BOOL m_bInTermin;
	BOOL m_bArtificialCreated;
	CString m_strActiveGrammems;
	int m_iClauseNo;
	int m_ReferenceWordNo;
	vector<int>	m_MainVerbs;

protected:
	BOOL m_bBold;
};




CPartOfGrArc* NewGroupArcPart(long color, int line, CPoint start, CPoint end,ETypeFigure type, CRect rectForArc, BOOL has_descr, BOOL whole_arc);

//this one is for qsort(...)
int  AssignPeriodLevel(vector<CVisualGroup*>& pPeriodArr);
bool PeriodsCompare_sort(const CVisualPeriod* pPeriod1, const CVisualPeriod* pPeriod2 );

#endif //_VisualSentences