// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

// ============          Consent.CPP    ==========================
//==================================================================
//  Этот модуль - часть  макросинтаксического анализа. 
//  Сюда включены функции анализа, которые создают 
//  макросинтаксические единицы (ConSent),  проводят инициализацию,

//  записывают в выходной файл. 
//==================================================================

#include "StdGraph.h"
#include "GraphmatFile.h"

#include <assert.h>
#include <stdlib.h>

#include "Consent.h"


const CGraLine& CConSent :: GetUnit(size_t i) const 
{
	return m_GraFile->GetUnits()[i];
};
 

bool  CConSent :: IsBullet () const
{
	return IsGround() && m_GraFile->HasDescr(m_HardGraphStartNo,OBullet);
};

bool  CConSent :: IsParag  () const
{
	return IsGround() && m_GraFile->HasDescr(m_HardGraphStartNo,OPar);
};
bool  CConSent :: IsSoft()   const
{
	return m_HardGraphEndNo == 0;
};


bool  CConSent :: IsFirstWord(const char* s)	 const 
{
		return m_GraFile->StrSuperCompare(m_HardGraphStartNo,s);
};




//==  Условие,  что начало МЕ оформлено стандартным способом 
//===  для российских нормативных актов
// Примеры стандартного оформления:
//  Глава 1.
//  Подраздел X-A.
inline bool CConSent :: SetUsualLawDocumentDivision ()
{
  
  if (IsFirstWord("глава")) 
	  m_UsualLawDocumentDivision = Chapter;
  else
	  if (IsFirstWord("статья"))
		  m_UsualLawDocumentDivision = Article;
	  else
	   if  (IsFirstWord("раздел"))
		   m_UsualLawDocumentDivision = Section;
	   else
		   if (IsFirstWord("подраздел"))
			   m_UsualLawDocumentDivision = SubSection;
		   else
			   if (IsFirstWord("часть"))
				   m_UsualLawDocumentDivision = Part;
			   else
			     m_UsualLawDocumentDivision = None;

  if ( (m_UsualLawDocumentDivision == None) && (!GetUnit(m_HardGraphStartNo).IsParagraphChar()))
	   return false;

  size_t nh = m_HardGraphStartNo;
  size_t nt = m_HardGraphEndNo+1;
  size_t k = nh+1;
  if (k >= nt) return false;
  if (!GetUnit(k).IsSpace()) return false;
  k++;
  if (k >= nt) return false;
  return m_GraFile->IsBulletWord(k);
}


bool CConSent :: InitBasicInformation ()
{

    m_NumberOfLines = 0;
    m_CountFullStops =0;
	
    m_HardGraphStartNo = m_GraFile->PSoft(m_StartNo, m_GraFile->GetUnits().size());
	assert (m_HardGraphStartNo < m_GraFile->GetUnits().size());
    m_HardGraphEndNo  = m_GraFile->BSoft(m_EndNo);      
	assert (m_HardGraphEndNo > 0 );

	if (IsSoft()) 
      m_GraLineNoToMark  = m_EndNo;
    else
      m_GraLineNoToMark = m_GraFile->HasDescr (m_HardGraphEndNo,OPun) ? m_HardGraphEndNo : m_GraFile->BSpace(m_EndNo);
	

	m_bFirstUpper = m_GraFile->HasDescr (m_HardGraphEndNo,OUp); 

     
	for (size_t i= m_StartNo; i<= m_EndNo; i++)
		{
			if  ( GetUnit(i).IsEOLN () )  
				m_NumberOfLines  += GetUnit(i).GetScreenLength();
            if (m_GraFile->IsSentenceEndMark(i))
              m_CountFullStops ++;
            
		};

   
   SetUsualLawDocumentDivision ();
	   
   return true;
}



string  CConSent :: GetString() const
{
	string Result;
	int LastLength  = 0;
	bool bHasHard = false; 
	Result = "<sent>";
	for (int  i= m_StartNo; i <= m_EndNo; i++)
	{
		string s (m_GraFile->GetUnits()[i].GetToken(), m_GraFile->GetUnits()[i].GetTokenLength());
		Result += ConvertASCIIToHtmlSymbols(s);
		if (m_GraFile->HasDescr(i, OSentEnd))
		{
			Result += "</sent>";
			LastLength = Result.length();
			Result += "<sent>";
		};
	};

	if (bHasHard)
		Result += "</sent>";
	else
		Result.erase(LastLength);

	return Result;

};
