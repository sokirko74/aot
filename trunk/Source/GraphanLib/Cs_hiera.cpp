// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

// ============          CS_HEIRA.CPP    ==========================
//==================================================================
//  Этот модуль - часть  макросинтаксического анализа. 
//  Сюда включены функции распознания макросинтаксических единиц (ME). 
//  Сюда же помещены процедуры, которые вызывают процедуры,
//  которые устанавливают макросинтаксические отношения.
//==================================================================


#include "StdGraph.h"
#include "GraphmatFile.h"
#include "Consent.h"




bool  ParagraphTagBeforeGraph (const CGraphmatFile& C, size_t i, size_t HB) 
{
	if (i == 0) return false;

	for (; i >0; i--)
	{
		if (C.GetUnits()[i].IsParagraphTag()) 
			return  true;

		if (!C.GetUnits()[i].IsSoft())
			break;
	};
	return false;
};


RubiconEnum RubiconText (const CGraphmatFile& G, size_t i) 
{
	if (G.m_bEmptyLineIsSentenceDelim)
		if (G.EmptyLineBeforeGraph(i, G.GetUnits().size())) 
			return rELine;

	if ( ParagraphTagBeforeGraph(G, i,G.GetUnits().size()) ) 
		return rPar;

	if (G.HasDescr(i,OPar)) 
		return rPar;

	return rUndef;
}






// Примеры работы процедуры:
//   Space, Eoln, Space, Eoln, Graph => (0,4)
//   Space, Eoln, Space, Eoln, Graph, OPar => (0,4), (5,5)
//   Space, Eoln  => (0,0)
//   OPar, OPar => (0,0) (1,1)

void  RecognizeSimpleCS (const CGraphmatFile& G,vector<CConSent> &CSL, size_t LB, size_t HB, RubiconEnum LB_Rubicon, RubiconEnum HB_Rubicon )
{
  RubiconEnum TypeRubicon = LB_Rubicon;

  size_t i = G.PSoft(LB, HB);

  while (i < HB) 
	{
		CConSent CCS(&G);
		CCS.m_StartNo = i;
		CCS.m_UpperRubiconType = TypeRubicon; 
		for (i++; i<HB; i++)
		{
		  TypeRubicon = RubiconText(G, i);
		  if  ( TypeRubicon > 0 ) break;
		}
		if (i < HB)
          CCS.m_LowerRubiconType = TypeRubicon; 
		else
          CCS.m_LowerRubiconType = HB_Rubicon; 
		CCS.m_EndNo = i;
		CSL.push_back(CCS);
		i = G.PSoft(i, HB);
  };
}





// Процедура приближения границ ME. Пусть даны два соседних ME A и Б,
// причем А идет раньше Б. На вход процедуры подаем Б.StartNo и A.EndNo,
// На выходе процедура добивается того, чтобы Б.StartNo == A.EndNo-1.
inline void AdjustRubiconsInSpaceCompact  (const CGraphmatFile& C, size_t &StartNo,size_t &EndNo)
{
	EndNo = C.PassSpace(EndNo,StartNo);

	if (EndNo ==  StartNo) 
		EndNo = StartNo - 1;
	 else
		{	 
			 StartNo = C.BSpace(StartNo, EndNo);
			 if (EndNo ==  StartNo) 
	        	StartNo = EndNo+1;
			  else
				EndNo = StartNo - 1;
		} 
}


// Эта процедура распознает (находит границы) ME (макросинтаксические единицы)
// Процедура работает в два этапа. Первый этап находит все ME 
// c точностью до пробельных символов. Принципы разбиения на границы 
// следующие:
// 1. Начало и конец кукушки являются границами, причем внутри кукушек  
// не  должны проходить границы  ME кроме типа rMaxLimit.
// 2. Те участки текста, которые не вошли  в кукушки  
// разделяются по пустым строкам, красным строкам и пунктам перечисления.
// 3. Длина ME не должна превышает  MaxUpLen.
///   Второй этап слегка изменяет 
// положение границ, не выходя за пределы пробельного отрезка, разделяющего ME,
// таким образом, чтобы система ME стала разбиением (всякий символ входного 
// текста принадлежит одному и только одной ME).

void RecognizeCS (const CGraphmatFile& G, vector<CConSent>& CSL, size_t LB, size_t HB)
{
  // =====   Первый этап  =====
  RecognizeSimpleCS (G, CSL, LB, HB, rBeg,rEnd);

  // Если файл состоит только из пробелов
  if (CSL.empty())
	  CSL.push_back(CConSent(&G));
   
  // =====   Второй этап  =====
  CSL[0].m_StartNo = LB;
  CSL[0].m_UpperRubiconType = rBeg;

  long i=0;
  for (; i < CSL.size() - 1; i++)
    AdjustRubiconsInSpaceCompact (G, CSL[i+1].m_StartNo, CSL[i].m_EndNo);
 
  CSL[i].m_EndNo = HB-1;
  CSL[i].m_LowerRubiconType = rEnd;
}






// Инициализация самого корневого узла иерархии
CConSent InitDoc (const CGraphmatFile* GraFile)
{
 CConSent C (GraFile);
 C.m_Type = CS_Doc;
 C.m_bConnected = true;
 C.m_StartNo = UnGroundNo;
 C.m_EndNo = UnGroundNo;
 C.m_HardGraphEndNo = 0;
 C.m_GraLineNoToMark = 0;
 C.m_NumberOfLines = 0;
 return C;
}



// устанавливает связь примыкания
void LinkParat (CSList& List,  long SentNo, long  m_ParatTail)
{
  List[SentNo].m_ParatNo = m_ParatTail;
  if (List[SentNo].m_Type == CS_Undef)  List[SentNo].m_Type = CS_Simple;
  List[SentNo].m_HostNo = -1;
  List[SentNo].m_bConnected = true;
}




/*
	подчиняет поле  эквивалентности с номером FieldNo узлу m_HostNo
*/
void SubdueSimilarField (CSList& List, int FieldNo, long m_HostNo)
{
 for (long i=0; i < List.size(); i++)
  if  (List[i].m_SimilarFieldNo == FieldNo)
   if  (!List[i].m_bConnected)
   {
    List[i].m_bConnected = true;
	List[i].m_HostNo = m_HostNo;
   };
}





// ============================================================
// ========     Типы макросинтаксических единиц (МЕ)     ======
// ============================================================

// =========  Процедура определения глубины перечислителя пункта перечисления
// Примеры работы:
// BulletDepth ("1.2.") = 2
// BulletDepth ("2.2") = 1 // т.к. последней точки нет
// BulletDepth ("2.") = 1
// BulletDept ("2)3.",2) = 0
// BulletDepth ("2 .",1) = 0
// BulletDepth ("АБРАКАДАБРА",0) = 0
//#undef GetUnits()

void SetDepthOfBullet (CConSent& C)
{
  const CGraphmatFile* GraFile = C.m_GraFile;
  size_t nt = C.m_HardGraphStartNo;
  C.m_BulletDepth = 0;
  while(nt <=  C.m_HardGraphEndNo)
  {
     if (    !GraFile->HasDescr(nt,ODigits) 
		  && !C.GetUnit(nt).IsString("З")
		 ) break;
	 nt ++;
	 if (nt >  C.m_HardGraphEndNo) break;
	 if (!GraFile->IsOneFullStop (nt)) break;
	 nt++;
	 C.m_BulletDepth++;
  };

 if  (nt <= C.m_HardGraphEndNo)
 {
	 //5)
	 //5.)
	 if (    C.GetUnit(nt).IsChar((unsigned char)')') 
		  || (     C.GetUnit(nt).IsSpace() 
		        && nt < C.m_HardGraphEndNo
				&& C.GetUnit(nt+1).IsChar((unsigned char)')')
			 )
		)
		 C.m_BulletLastDelim = CConSent :: Bracket;
      else
		  // 5.
		  // 6.7.
		  if (GraFile->IsOneFullStop (nt-1))
			  C.m_BulletLastDelim = CConSent :: FullStop;
		  else
			  C.m_BulletLastDelim = CConSent :: Other;
			   
 }
 else
   C.m_BulletLastDelim = CConSent :: Other;

}

// =========  Условие, что две МЕ  начинаются с буквы одинакового регистра
bool HaveEquCase (const CConSent& CS1, const CConSent& CS2)
{
   int ch1 = CS1.FirstChar();
   int ch2 = CS2.FirstChar();
   if (is_russian_lower(ch1) && is_russian_lower(ch2)) return true;
   if (is_russian_upper(ch1) && is_russian_upper(ch2)) return true;
   if (is_german_lower(ch1) && is_german_lower(ch2)) return true;
   if (is_german_upper(ch1) && is_german_upper(ch2)) return true;
   return false;

}


// =========  Условие, что две МЕ   являются однородными пунктами перечисления
bool SimilarBullets (const CConSent& CS1, const CConSent& CS2)
 {
   assert  (CS1.IsBullet() && CS2.IsBullet());
   int ch1 = CS1.FirstChar();
   int ch2 = CS2.FirstChar();
   if  ( (ch1 ==  '-') &&  (ch2 ==  '-')) return true;
   if ((CS1.m_BulletDepth !=  CS2.m_BulletDepth) ) return false;
   if (CS1.m_BulletLastDelim !=  CS2.m_BulletLastDelim) return false;
   if (ch2+1 == ch1)  return true;
   
   if (isdigit(ch1) && isdigit(ch2)) return true;
   if (is_lower_roman_digit(ch1) && is_lower_roman_digit(ch2)) return true;
   if (is_upper_roman_digit(ch1) && is_upper_roman_digit(ch2)) return true;
   assert (CS2.m_StartNo < CS1.m_StartNo);

   if (   CS2.FirstUnit().IsString ("1") 
	   && CS1.FirstUnit().IsString ("II")
	  )	
	  return true;

   if (   CS2.FirstUnit().IsString ("2") 
	   && CS1.FirstUnit().IsString ("3")
	  )	
	  return true;

   if (   CS2.FirstUnit().IsString ("3") 
	   && CS1.FirstUnit().IsString ("4")
	  )	
	  return true;

   return  HaveEquCase(CS1,CS2);
 }

// =========  Обобщенное условие, что две МЕ  являются однородными 
inline bool CanBeBrothers (CConSent& CS1, CConSent& CS2)
{

	if (   (CS1.m_UsualLawDocumentDivision !=  CConSent :: None) 
		|| (CS2.m_UsualLawDocumentDivision !=  CConSent :: None))
	return CS1.m_UsualLawDocumentDivision == CS2.m_UsualLawDocumentDivision;

	if ( ( (CS1.m_Type == CS_Heading) && (CS2.m_Type != CS_Heading))  ||
		( (CS2.m_Type == CS_Heading) && (CS1.m_Type != CS_Heading))  
	   )
	return false;


	if (CS1.IsBullet() != CS2.IsBullet()) 
	   return false;
	else
		if (CS1.IsBullet() && CS2.IsBullet()) 
			{
				if (!SimilarBullets   (CS1,CS2))
					return false;
			}

	return true;
 }

// =========  Условие, что МЕ может примыкать
inline bool CanBeParataxed (const CConSent& CS)
{
 return  
		CS.IsBullet() == false     &&
		CS.m_Type   !=  CS_Heading   && 
		CS.m_UsualLawDocumentDivision == CConSent :: None;
}

// =========  Условие, что МЕ может иметь братьев после себя
inline bool CanHaveBrotherAfterItself (const CSList& List, long SentNo)
{
 if(       List[SentNo].IsBullet() 
		|| List[SentNo].m_Type == CS_Heading
		|| List[SentNo].m_UsualLawDocumentDivision != CConSent :: None
	    //  или же его отцом является вводящее предложение
		|| (		(List[SentNo].m_HostNo != -1) 
				&& (List[List[SentNo].m_HostNo].m_Type == CS_Parent)
			)
	)
 {

   /*
	если заканчивается на точку, а среди предыдущих пяти братьев был хотя бы один, которые заканчивался 
	на ";", тогда этот брат последний, и он не может иметь дальше братьев.
   */
   if (List[SentNo].LastChar() == '.')
	   for (long j=SentNo -1; j >=0 && ((SentNo - j) < 5); j--)
		 if ( List[j].m_HostNo == List[SentNo].m_HostNo)
			 if (List[j].LastChar() == ';')
				 return false;

   return true;
 };
 return false;
}

// =========  Условие, что МЕ может управлять другими МЕ
inline bool CanBeHost (const CConSent& CS)
{ 
  return CS.m_bConnected &&
	     (CS.m_Type == CS_Heading ||
	      CS.m_Type == CS_Parent ||
	      CS.m_Type == CS_Doc ); 
}

/*
	удаление МЕ, уничтожение ссылок на него внутри других МЕ
*/
void DelCS(CSList& List, long SentNo)
{
	for (long i=0; i < List.size(); i++)
	{
     if (List[i].m_HostNo == SentNo)
	 {
		 List[i].m_HostNo = -1;
		 List[i].m_bConnected = false;
	 }
	 else	 
	   if (List[i].m_HostNo > SentNo)
	  	  List[i].m_HostNo --;

	 if (List[i].m_ParatNo == SentNo)
	 {
		 List[i].m_ParatNo = -1;
		 List[i].m_bConnected = false;
	 }
	 else	 
	   if (List[i].m_ParatNo > SentNo)
	  	  List[i].m_ParatNo--;
	};
	List.erase(List.begin()+SentNo);
};

// =========  Примитив, который объединяет текущую МЕ с последующим. 
// Пояснение: Текущее МЕ становится больше ровно на длину последующей МЕ.
// Меняются параметры МЕ. 
void GobbleNextCS (CSList& List, long SentNo)
{
	assert (SentNo < List.size() - 1);
	CConSent& C1 =  List[SentNo];
	CConSent& C2 =  List[SentNo+1];
	if (!C1.m_bConnected)
	{
	  C1.m_ParatNo  = C2.m_ParatNo;
	  C1.m_HostNo  = C2.m_HostNo;
	};
	C1.m_EndNo = C2.m_EndNo;
    C1.m_HardGraphEndNo = C2.m_HardGraphEndNo;
    C1.m_GraLineNoToMark = C2.m_GraLineNoToMark;
    C1.m_NumberOfLines += C2.m_NumberOfLines;
	C1.m_CountFullStops += C2.m_CountFullStops;
	C1.m_LowerRubiconType = C2.m_LowerRubiconType;
	DelCS(List, SentNo+1);
}


//== Примитив, который подчиняет  текущую МЕ заданному МЕ   ==
// если МЕ входит в неатомарное поле эквивалентности, тогда подчинаяется все поле
inline void MakeObey (CSList& List, long SentNo, long m_HostNo)
{
   if (List[SentNo].m_SimilarFieldNo)
     SubdueSimilarField(List, List[SentNo].m_SimilarFieldNo, m_HostNo);
   else
   {
	   List[SentNo].m_HostNo = m_HostNo;
	   List[SentNo].m_bConnected = true;
   };
}

//== Примитив, находящий вводящие условно предложения
void FindParents (CSList& List)
{
   for (long i=1; i < List.size(); i++)
    if (List[i].LastChar() == ':')  
		List[i].m_Type = CS_Parent;
}


inline bool TestHeadingNumberOfLines(int x)    
{
	return x<14;
}

inline bool TestHeadinfLeftMargin(int x) 
{
	return x>0 && x<40;
}


/* если последний символ данного кусочка текста, не может заканчиваить
   имя документа */
inline bool TestHeadingLastLetter (int  ch)
{
    return strchr(",;:'<>?`-\0",ch) == NULL;
}


/* если первый символ данного кусочка текста, не может начинать
      имя документа */
inline bool TestHeadingFirstLetter (int ch, const CGraphmatFile* GraFile) 
{
	return (strchr("-;({\0",ch) == NULL) && !GraFile->is_lowercase(ch); 
}


//== Примитив, находящий заголовки
void FindHeadings (CSList& List)
{

  const CGraphmatFile* GraFile = List[0].m_GraFile;
  size_t CountHeadings = 0;
  size_t SumOfMiddles = 0;
 
  long SentNo=0;
  for (; SentNo < List.size()-1; SentNo++)
	 if (List[SentNo].m_Type == CS_Undef)   // если оно еще не было определено
	  if (   (List[SentNo].m_bFirstUpper != false) 
		  || isdigit (List[SentNo].FirstChar()) 
		  || List[0].GetUnit(List[SentNo].m_HardGraphStartNo).IsParagraphChar()
		 )
		if (     (List[SentNo].m_UpperRubiconType == rELine)
			  || (List[SentNo].m_UpperRubiconType == rBeg)
		   )
		  if (   (List[SentNo].m_UsualLawDocumentDivision != CConSent :: None)
			  ||(
					 TestHeadingFirstLetter(List[SentNo].FirstChar(), GraFile) 
				  && (List[SentNo].LastChar()  != '.')
				)
			 )
 			  { 
				if (    (List[SentNo].m_NumberOfLines > 4) 
					 &&  GraFile->HasDescr (List[SentNo].m_HardGraphStartNo,OPar) 
					 && (List[SentNo].m_CountFullStops > 1)
				   )
				  continue;

				if (    (List[SentNo].m_NumberOfLines > 5) 
					 && (List[SentNo].m_CountFullStops > 1)
				   )
				  continue;
			  
				List[SentNo].m_Type = CS_Heading;
				CountHeadings ++; 
		  };

 

}  //  FindHeadings

//== Примитив, находящий примечания
// Примечание - это УП, заключенное в скобки, идущее после заголовка
void FindExplanatory (CSList& List)
{
 const CGraphmatFile* GraFile = List[0].m_GraFile;

 
 for (long i=0; i <  List.size();i++ )
 {
	long m_HostNo = -1;

	if ( (List[i].m_Type == CS_Heading)  ||  (List[i].m_Type == CS_Parent))
	  m_HostNo = i;
	else
	if (List[i].m_Type == CS_Explan )  
		m_HostNo = List[i].m_HostNo;
	else
		continue;

	i++;
	if ( i == List.size()) continue;

	if (List[i].m_Type != CS_Undef) continue;

	// если есть отркрывающая
	if (List[i].m_GraFile->HasDescr (List[i].m_HardGraphStartNo,OOpn))
		for (long k=List[i].m_HardGraphEndNo;  // ищем закрывающую в конце
				(k> 0)  
			&&	(     List[i].m_GraFile->HasDescr(k,OPun) 
				  ||  List[0].GetUnit(k).IsSoft()
				); 
			k--)
		 if ( List[i].m_GraFile->HasDescr (k,OCls))
		 {  
			List[i].m_Type = CS_Explan;
			List[i].m_HostNo =  m_HostNo;
			List[i].m_bConnected =  true;
			break;
		 }
 }
}

inline bool IsUpperRomanOne(const CGraLine& L)
{
	return L.IsString("1") || L.IsString("I");
};



//== Примитив, находящий множества заголовков, начинающихся стандартным способом
void FindSimilarHeadings (CSList& List)
{
  int NSF = 1; // Текущий номер поля эквивалентности
  const CGraphmatFile* GraFile = List[0].m_GraFile;

  long SentNo=0;
  for (; SentNo <  List.size();SentNo++ )
    List[SentNo].m_SimilarFieldNo = 0;
  

  for (SentNo=0; SentNo <  List.size();SentNo++ )
    if (List[SentNo].m_SimilarFieldNo == 0)
      if (
		       (List[SentNo].m_Type   == CS_Heading) 
		   &&  (   (List[SentNo].m_UsualLawDocumentDivision != CConSent :: None) 
		        || IsUpperRomanOne(List[SentNo].FirstUnit()) 
			   ) 
		  ) 
		{
			bool IsParagraph = List[0].GetUnit(List[SentNo].m_HardGraphStartNo).IsParagraphChar();
			const char *s = List[SentNo].FirstUnit().GetToken();
			// Идем дальше по тексту и ищем похожие МЕ
			for (long SentNo1 = SentNo; (SentNo1 <  List.size()) && (List[SentNo1].m_SimilarFieldNo == 0); SentNo1++)
				 if  	(List[SentNo1].m_Type == List[SentNo].m_Type) 
					 if (
							(
								(List[SentNo1].FirstUnit().GetToken() != NULL ) && 
								(s != NULL) && 
								(strscmp(s,List[SentNo1].FirstUnit().GetToken(),4, List[SentNo1].m_GraFile->m_Language) == 0) 
							) || // типа "Глава 1 .... Глава 2"
							(
							      IsParagraph 
                               && List[0].GetUnit(List[SentNo1].m_HardGraphStartNo).IsParagraphChar()
							) ||// Или все начинаются с символа параграфа
							( 
							     List[SentNo1].IsBullet()
							  && List[SentNo].IsBullet () 
							  && SimilarBullets (List[SentNo1], List[SentNo])	
							) // или заголовки с пп одинаковой глубины
						)
	        			List[SentNo1].m_SimilarFieldNo = NSF;
	
		    NSF ++;
		}
}

// =============================================================
// ========    Установление макросинтаксических отношений(МЕ) ==
// =============================================================


// Структура MacroSynStackItem используется для сохранения 
// ситуаций макросинтаксического анализа, к которым  алгоритм 
// откатывается, когда завершает построение поддерева основного дерева.
struct MacroSynStackItem {
  long  m_HostNo;        // корень строящегося поддерева 
  long  m_LastBrotherNo; // последний пройденный сын корня строящегося поддерева
  long  m_ParatTail;  // последнее МЕ, к которому можно примкнуть 
  bool	m_bBrother;    // может ли первый сын корня строящегося поддерева иметь братьев

  MacroSynStackItem (long h)
  {
	  m_LastBrotherNo = -1;
	  m_bBrother = true;
	  m_ParatTail = -1;
	  m_HostNo = h;
  };
};

void AssertValid (CSList& List) {

 for (long i=0; i < List.size(); i++)
   assert  ( (List[i].m_HostNo ==-1) ||  (List[i].m_ParatNo == -1));
};
  


// === Основная процедура построения макросинтаксического дерева
// возвращает истину, если удалось пропарсить весь документ.
bool SetMacroSyntDependcies (const CGraphmatFile& G,  CSList& List)
{
   bool bCanIterate = true;
   stack<MacroSynStackItem> Parents; // стек родителей
   Parents.push(MacroSynStackItem(0)); // добавляем туда корневой узел

   long SentNo = 1;
   for (; SentNo < List.size(); SentNo++)
   {
	 CycleStart: 
   
     if (!List[SentNo].m_bConnected) 
      if (Parents.top().m_LastBrotherNo == -1) // первое предложение всегда подчиняется хозяину
	  {
		  MakeObey(List,SentNo, Parents.top().m_HostNo);
 	      Parents.top().m_LastBrotherNo = SentNo;
		  Parents.top().m_bBrother =  CanHaveBrotherAfterItself(List, SentNo);
	  }
	  else	  
       if (Parents.top().m_bBrother && CanBeBrothers(List[SentNo], List[Parents.top().m_LastBrotherNo]))
		   // пришли к брату  текущего хоязина
	   {
			MakeObey(List,SentNo, Parents.top().m_HostNo);
			Parents.top().m_LastBrotherNo = SentNo;
			Parents.top().m_bBrother =  CanHaveBrotherAfterItself(List, SentNo);
	   }
		else
			  if (CanBeParataxed(List[SentNo])) // примыкает к текущему хоязину
					LinkParat(List,SentNo, Parents.top().m_ParatTail);
				else
                 if (G.m_bSubdueWrongMacroSynUnitToMainRoot) 
				  // если пользователем было указано, что нужно насильно подчинять все главному хозяину 
				  // пытаемся подняться наверх по стеку, пробуя подчинить текущий узел каким-то
				  // более высоким родителям.
				 {  
                  if (Parents.size() == 1)  //когда стек родителей состоит из одного узла, подчиняем насильно
				  {
					  MakeObey(List,SentNo, Parents.top().m_HostNo);
					  Parents.top().m_LastBrotherNo = SentNo;
					  List[SentNo].m_bForceConnected = true; 
				  }
				  else
			          {		
					        Parents.pop();	 // поднимаемся вверх по стеку
							goto CycleStart;
						
				      }
				 }
		 		 else
				 {
                   // поиск в стеке УП, с которым можно связать текущее
				   for (int i=Parents.size() -  1; i>0; i--)
					   if ( 
						       (Parents.top().m_bBrother && CanBeBrothers(List[SentNo],List[Parents.top().m_LastBrotherNo])) 
							|| CanBeParataxed(List[SentNo])
						   ) 
							   goto CycleStart;
					       else
							  Parents.pop();;	

                     // если в стеке не было найдено нужного УП, то связываем текущее УП примыканием с последним
					 LinkParat(List, SentNo, Parents.top().m_ParatTail); 
					   
					 List[SentNo].m_bForceConnected = true; 
				 }
                    
       else // List[SentNo].m_bConnected
       if (     (List[SentNo].m_HostNo != Parents.top().m_HostNo) 
		     && (Parents.size() > 1)
		  )  //если мы не на брате, надо  подняться вверх по стеку и дойти до родителя, который является
		  // родителем текущего УП
		  {
			  Parents.pop();
			  goto CycleStart;
	      };

	  //если не сын и не брат, тогда только примыкание
      Parents.top().m_ParatTail = SentNo;

	  if (CanBeHost(List[SentNo]))
			Parents.push(MacroSynStackItem(SentNo));
   };

   return SentNo == List.size();
}


const char* GetEnglishTag(Descriptors D)
{
 switch (D) 
 {
	case CS_Parent: return "PAC";
	case CS_Heading: return "TWH";
	case CS_Explan		  : return "EXPL";
	case CS_Doc		  : return "DOC";
 };
 return "SMPL";
};


void  PrintXmlSubtree (const vector<CConSent>& CSL, FILE* fp, int SentNo)
{
	const char* Type = GetEnglishTag(CSL[SentNo].m_Type);
	fprintf (fp, "<%s>\r\n",  Type);
	fprintf (fp, "%s", CSL[SentNo].GetString().c_str());

	//  prints all  children
	int i = SentNo+1;
	for (; i<CSL.size(); i++)
	{
		if (CSL[i].m_HostNo == SentNo)
			PrintXmlSubtree(CSL, fp, i);
	};
	fprintf (fp, "</%s>\r\n", Type);

	//  prints all  parataxed
	for (i = SentNo+1; i<CSL.size(); i++)
	{
		if (CSL[i].m_ParatNo == SentNo)
			PrintXmlSubtree(CSL, fp, i);
	};

};


void  WriteXmlMacSyn (const vector<CConSent>& CSL, const  char* Filename)
{
  if (CSL.size() == 0) return;
  
  FILE * fp = fopen (Filename, "wb");
  if (!fp) return;
  fprintf (fp, "<?xml version=\"1.0\" encoding=\"windows-1251\" ?>\r\n");
  PrintXmlSubtree(CSL, fp, 0);
  fclose (fp);
};	


void CGraphmatFile :: MacSynHierarchy ()
{
// ===============================================================
// ===================			Recognition		========================
// ===============================================================

	vector<CConSent> CSL;

	// если файл пуст надо выйти
	if (PSoft(1, GetUnits().size()) == GetUnits().size()) return;
	


	// Построение списка ME
	RecognizeCS (*this, CSL, 1,GetUnits().size() );

	CSL.insert(CSL.begin(),InitDoc(this));

	int i = 1;
	for (; i < CSL.size(); i++)
	{
		CSL[i].InitBasicInformation ();

		if (CSL[i].IsBullet())
			SetDepthOfBullet (CSL[i]);
	};



	// ===============================================================
	// ===================      Analyse       ========================
	// ===============================================================
	if (CSL.size() == 1)  return;

	if (m_bMacSynHierarchy)
	if (CSL[1].IsSoft())
	{
		for (long i=1; i < CSL.size(); i++)
		{
		  CSL[i].m_HostNo = 0;;
		  CSL[i].m_Type = CS_Simple;
		};
	}
	else
	{
		FindParents(CSL);

		FindHeadings(CSL);

		FindExplanatory(CSL);

		FindSimilarHeadings (CSL);

		SetMacroSyntDependcies(*this, CSL);

	};

	// ===============================================================
	// ===================    Writing results ========================
	// ===============================================================

	for (i=0; i < CSL.size(); i++)
		if (CSL[i].m_GraLineNoToMark < GetUnits().size()) 
			SetDes(CSL[i].m_GraLineNoToMark, CSL[i].m_Type);
		   


	if (!m_XmlMacSynOutputFile.empty())
		WriteXmlMacSyn (CSL, m_XmlMacSynOutputFile.c_str());


} 







