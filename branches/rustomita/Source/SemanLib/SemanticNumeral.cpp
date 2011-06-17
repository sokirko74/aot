#include "stdafx.h"
#include "SemanticRusStructure.h"
#include "../common/rus_numerals.h"


static StringVector GenitFormsOfCardinal;



// порождает по числительному генитивный вариант.
// например, два=>двух
// это генитивный вариант используется в слвоах типа "двухламповый"
bool BuildGenitFormOfCardinal(const CLemmatizer* piRusLemmatizer, const CRusGramTab* Agramtab) 
{
	GenitFormsOfCardinal.clear();
	for(int i = 0 ; i < NumeralToNumberCount; i++ )
	{
		if (NumeralToNumber[i].m_Number == 1)
		{
			GenitFormsOfCardinal.push_back("ОДНО");
			continue;
		};
		if (NumeralToNumber[i].m_Number == 100)
		{
			GenitFormsOfCardinal.push_back("СТО");
			continue;
		};
		if (NumeralToNumber[i].m_Number == 1000)
		{
			GenitFormsOfCardinal.push_back("ТЫСЯЧЕ");
			continue;
		};
		if (NumeralToNumber[i].m_Number == 1000000)
		{
			GenitFormsOfCardinal.push_back("МИЛЛИОНО");
			continue;
		};
		vector<CFormInfo> ParadigmCollection;
		string WordForm = NumeralToNumber[i].m_Cardinal;
		piRusLemmatizer->CreateParadigmCollection(true, WordForm, false, false, ParadigmCollection);
		// ищем числительное
		long k=0;
		for (; k < ParadigmCollection.size(); k++)
		{
			string AnCode = ParadigmCollection[k].GetAncode(0);
			BYTE POS = Agramtab->GetPartOfSpeech(AnCode.c_str());
            if (NumeralToNumber[i].m_bNoun)
            {
                if (POS == NOUN)
                    break;
            }
            else
			    if (POS == NUMERAL)
				    break;
		};
		assert (k < ParadigmCollection.size());
		const CFormInfo& P = ParadigmCollection[k];
		// ищем родительный падеж
		for (k=0; k < P.GetCount(); k++)
		{
			string AnCode = P.GetAncode(k);
			QWORD Grammems;
			if (!Agramtab->GetGrammems(AnCode.c_str(), Grammems))
				throw CExpc ("Bad ancode in  BuildGenitFormOfCardinal");
			if ( (Grammems & _QM(rGenitiv)) > 0 )
				break;
		};
		assert (k < P.GetCount());
		GenitFormsOfCardinal.push_back(P.GetWordForm(k));
	};
	return true;
};



double GetGenitFormNumeral(string word)
{
	for(int i = 0 ; i < GenitFormsOfCardinal.size() ; i++ )
		if( word == GenitFormsOfCardinal[i] )
			return NumeralToNumber[i].m_Number;
	return -1;
};


// выдает длину максимального префикса слова word, который совпадает с одним из 
// генитивных вариантов числительного.
// Если такого генитивного  варианта не было обнаружено, то 
// проверяются некоторые фонетические  варианты  этих префиксов.
int GetNumeralPrefixGenitForm(string word, int& PrefixLength)
{
	PrefixLength = -1;
	long NumLine = -1;

	
	for(int i = 0 ; i < GenitFormsOfCardinal.size(); i++ )
	{
		long len =  GenitFormsOfCardinal[i].length();
		if (len > PrefixLength)
		if( !strncmp(word.c_str(), GenitFormsOfCardinal[i].c_str(), len) )
		{
			NumLine = i;
			PrefixLength = len;
		};
	};

	if (PrefixLength < 3)
  	 if( !strncmp(word.c_str(), "ДВУ", 3) ) // двуногий
		{
			NumLine = 2;
			PrefixLength = 3;
			assert (NumeralToNumber[NumLine].m_Number == 2);
		};

	if (PrefixLength < 5)
  	 if( !strncmp(word.c_str(), "ДВУХЪ", 5) ) // двухъярусный
		{
			NumLine = 2;
			PrefixLength = 5;
			assert (NumeralToNumber[NumLine].m_Number == 2);
		};

	 if (PrefixLength < 5)
  	 if( !strncmp(word.c_str(), "ТРЕХЪ", 5) ) // трехъярусный
		{
			NumLine = 3;
			PrefixLength = 5;
			assert (NumeralToNumber[NumLine].m_Number == 3);
		};

    if (PrefixLength < 8)
  	 if( !strncmp(word.c_str(), "ЧЕТЫРЕХЪ", 8) ) // четырехъярусный
		{
			NumLine = 4;
			PrefixLength = 8;
			assert (NumeralToNumber[NumLine].m_Number == 4);
		};

	if (PrefixLength < 4)
  	 if( !strncmp(word.c_str(), "ОДНО", 4) ) // одноэтажный
		{
			NumLine = 0;
			PrefixLength = 4;
			assert (NumeralToNumber[NumLine].m_Number == 1);
		};

	
	return NumLine;
};

double FindInTable(string word)
{
	double Result = GetOrdinalNumeral(word);
	if (Result == -1)
	{
		Result = GetCardinalNumeral(word);

		if (Result == -1)
		  Result = GetNounNumeral(word);
	};
    return Result;
};


double ConvertNumeralByTable (vector<CRusSemWord>::const_iterator Start, vector<CRusSemWord>::const_iterator End) 
{

  double Result = 0;
  
  
  long LastLength = 20;
  for (vector<CRusSemWord>::const_iterator Word = Start; Word < End;Word++)
  {
	  double Res;

	  
	  if (    !Word->m_Word.empty()
	 	 && isdigit((BYTE)Word->m_Word[0])
	   )
	   //  "15 тысяч граждан"
		Res = atoi(Word->m_Word.c_str());
	  else
		//  "пятнадцать тысяч граждан"
		Res = FindInTable(Word->m_Lemma);

	  if (Res == -1) return -1;

	  // порядок должен уменьшаться 
	  // например "сто двадцать шесть" - нормально, а "шесть двадцать сто" - нет
	  long CurrLength = IntToStr(Res).length();
	  if (CurrLength >= LastLength)	  return -1;
	  LastLength = CurrLength;

	  Result += Res;

  };
  return Result;
};


string ConvertOneWordOrdinalNumeral (const string&  InputOrdinal)
{
  string Result = IntToStr(GetOrdinalNumeral(InputOrdinal));
  if (Result == "-1") 
  {
    string Word = InputOrdinal;
	long PrefixNumber = 0;

    for (;;)
	{
		int PrefixLength = -1;
		int LineNo =  GetNumeralPrefixGenitForm(Word, PrefixLength);
		if (LineNo == -1) break;
		Word.erase (0, PrefixLength);
		PrefixNumber += (long)NumeralToNumber[LineNo].m_Number;
	};

	if (Word != InputOrdinal)
	{
		QWORD LastRes = GetOrdinalNumeral (Word);
		if ( LastRes >= 1000)  //"тысячный" или "миллионный"
			Result = IntToStr (PrefixNumber*LastRes);
		else
		    Result = IntToStr (PrefixNumber)+'#' + Word;
	};
  };

  return Result;   	
};


string ConvertHugeNumeral  (vector<CRusSemWord>::const_iterator Start, vector<CRusSemWord>::const_iterator End, QWORD Order)
{
  string OrderStr = FindByNumber(Order);
  string Result;
  if (Order == 1)
  {
       Result = IntToStr(ConvertNumeralByTable (Start, End));
  }
  else 
  {
	  vector<CRusSemWord>::const_iterator Word = Start;
	  for (; Word < End;Word++)
		if (Word->m_Lemma ==  OrderStr)
			break;

     if (Word < End) 
          if  (Word == Start)
		    Result  = IntToStr(Order+atoi(ConvertHugeNumeral (Word+1, End,  Order/1000).c_str() ));
		  else
			Result  = IntToStr(Order*ConvertNumeralByTable (Start,Word)+atof(ConvertHugeNumeral (Word+1, End, Order/1000).c_str()));
     else
          Result = ConvertHugeNumeral (Start, End, Order/1000);
  }

  return Result;
};

bool FullAdjWithNumeralPrefix (const CRusSemNode& Node)
{
      if (   !Node.IsPrimitive() ) return false;
	  if (  !Node.m_Words[0].HasPOS(ADJ_FULL)  ) return false;
	  int dummy;
	  if ( GetNumeralPrefixGenitForm(Node.m_Words[0].m_Word, dummy) == -1) return false;
	  if (Node.m_Words[0].m_Word.substr (0, 3) == "СТО")
	  {
		  string Word = Node.m_Words[0].m_Word.substr(3);
		  if (GetNumeralPrefixGenitForm(Word, dummy) != -1) return true;
	  };
	  if  (   (Node.m_Words[0].m_Word.substr (0, 3) == "СТО")
		   || (Node.m_Words[0].m_Word.substr (0, 4) == "ОДНО")
		  )
	  if (Node.m_Words[0].m_ParadigmId != -1) return false;
	  return true;
};

string ConvertRusNumeral(const CRusSemNode& Node)
{
  string Result;
  Result = ConvertHugeNumeral (Node.m_Words.begin(),Node.m_Words.end(), 1000000000);
  return Result;
};


void InterpretAdjectivesWithNumeral(CRusSemStructure& R, long NodeNo)
{
   // пробуем проинтерпретировать с "-" в начале
   R.m_Nodes[NodeNo].m_Words[0].m_Lemma = "-"+R.m_Nodes[NodeNo].m_Words[0].m_Lemma;
   R.InitInterps(R.m_Nodes[NodeNo].m_Words[0], false, R.m_Nodes[NodeNo].m_ClauseNo, R.m_Nodes[NodeNo], false);
   // если не получилось, отрзаем  "-" и интерпретируем обычно
   if (R.m_Nodes[NodeNo].GetInterps().empty())
   {
	   R.m_Nodes[NodeNo].m_Words[0].m_Lemma.erase(0,1);
	   R.InitInterps(R.m_Nodes[NodeNo].m_Words[0], false, R.m_Nodes[NodeNo].m_ClauseNo, R.m_Nodes[NodeNo], true);
   };

};

void CRusSemStructure::ConvertRusNumeralsToArabic()
{
	// прилагательные типа 2-кратный берутся на Maposte
	// их  надо преобразовать в слот  NumeralPrefix
	for (long NodeNo=0; NodeNo< m_Nodes.size(); NodeNo++)
	if  (    m_Nodes[NodeNo].IsPrimitive() 
		  && HasRichPOS (NodeNo, ADJ_FULL)
		  && isdigit((BYTE)m_Nodes[NodeNo].m_Words[0].m_Word[0])
		)
	{
		int i = m_Nodes[NodeNo].m_Words[0].m_Word.find ("#");
		m_Nodes[NodeNo].m_Words[0].m_NumeralPrefix = m_Nodes[NodeNo].m_Words[0].m_Word.substr(0, i);
		m_Nodes[NodeNo].m_Words[0].m_Word.erase(0, i+1);
		InterpretAdjectivesWithNumeral(*this, NodeNo);
		
	};

	/*
		А теперь основной цикл ...
	*/
	
	for (long NodeNo=0; NodeNo< m_Nodes.size(); NodeNo++)
	if  (    (m_Nodes[NodeNo].m_SynGroupTypeStr == NUMERALS)
		  || (m_Nodes[NodeNo].IsPrimitive() && HasRichPOS (NodeNo, NUMERAL))
		  || (m_Nodes[NodeNo].IsPrimitive() && HasRichPOS (NodeNo, NUMERAL_P))
	      ||   FullAdjWithNumeralPrefix(m_Nodes[NodeNo])
		)
	{
		string LemmaAfterHyphen;
		int pos =  m_Nodes[NodeNo].m_Words[0].m_Lemma.find ("-");
		
		if (pos != -1)
		{
			LemmaAfterHyphen = m_Nodes[NodeNo].m_Words[0].m_Word.substr(pos+1);
			m_Nodes[NodeNo].m_Words[0].m_Lemma = m_Nodes[NodeNo].m_Words[0].m_Lemma.substr(0,pos);
		};

	  string  ConvertedWord;
	  int dummy;
	  if (     (m_Nodes[NodeNo].IsPrimitive())
	      &&  (    m_Nodes[NodeNo].m_Words[0].HasPOS(NUMERAL_P)
	           || ( 
			          m_Nodes[NodeNo].m_Words[0].HasPOS(ADJ_FULL)
			       && (GetNumeralPrefixGenitForm(m_Nodes[NodeNo].m_Words[0].m_Word, dummy) != -1)
			 	 )
	       )
	      )
		{
           ConvertedWord = ConvertOneWordOrdinalNumeral (m_Nodes[NodeNo].m_Words[0].m_Lemma);
		   int j = ConvertedWord.find('#');

		   if (j != -1) // по этой же схеме идут числительные "двухтысячный", которые не обрабатывется в этом if
		   {
			   int len = ConvertedWord.length() - j -1;
			   m_Nodes[NodeNo].m_Words[0].m_Lemma.erase(0, m_Nodes[NodeNo].m_Words[0].m_Lemma.length() - len);
			   m_Nodes[NodeNo].m_Words[0].m_ParadigmId = m_pData->GetFirstParadigmId(morphRussian, m_Nodes[NodeNo].m_Words[0].m_Lemma, 0);
			   m_Nodes[NodeNo].m_Words[0].m_NumeralPrefix = ConvertedWord.substr(0, j);
			   ConvertedWord.erase(0, j+1);
			   InterpretAdjectivesWithNumeral(*this, NodeNo);
		   };
		}
       else
		 ConvertedWord = ConvertRusNumeral(m_Nodes[NodeNo]);

		string  ConvertedWordHyphen = "-1";
		
		if (ConvertedWord == "-1") 
		{
			// слова типа "оба", "несколько" являются числительными, но не конвертируются,
			// а оставляются нетронутыми
			//assert (m_Nodes[NodeNo].IsPrimitive());

			//еще могут быть просто ряды числительных, которые нельзя считать целым QUANTIT, например:		
			// "один два"
			if (m_Nodes[NodeNo].m_Words.size() > 1)
				for (long i=0;i < m_Nodes[NodeNo].m_Words.size(); i++)
				{
					float Res = FindInTable(m_Nodes[NodeNo].m_Words[i].m_Lemma);
					if (Res != -1)
					{
						m_Nodes[NodeNo].m_Words[i].m_Word = IntToStr(Res);;
						m_Nodes[NodeNo].m_Words[i].m_Lemma = "";
						m_Nodes[NodeNo].m_Words[i].m_ParadigmId = -1;
					};

				};
			continue;
		};
		// разберем случай "полтора-два миллиона баррелей"
		if (LemmaAfterHyphen != "")
		{
			m_Nodes[NodeNo].m_Words[0].m_Lemma =  LemmaAfterHyphen;
			ConvertedWordHyphen = ConvertRusNumeral(m_Nodes[NodeNo]);
		};


		// берем этот слоту первого слова
		RegisterEnum FirstCase = m_Nodes[NodeNo].m_Words[0].m_CharCase;
		// удаляем все слова кроме последнего слова
		m_Nodes[NodeNo].m_Words.erase(m_Nodes[NodeNo].m_Words.begin(), m_Nodes[NodeNo].m_Words.end()-1);
		// обнуляем лемму последнего слова
		if (m_Nodes[NodeNo].m_Words[0].m_NumeralPrefix == "")
		{
			  m_Nodes[NodeNo].m_Words[0].m_Lemma = "";
			  m_Nodes[NodeNo].m_Words[0].m_ParadigmId = -1;
			  m_Nodes[NodeNo].DelAllInterps();
		};
		m_Nodes[NodeNo].m_Words[0].m_Word = ConvertedWord;
		if (HasRichPOS (NodeNo, NOUN)) // миилион, миллиард
		  m_Nodes[NodeNo].m_Words[0].m_Poses = 1<<NUMERAL;
		if (ConvertedWordHyphen != "-1")
		{
			m_Nodes[NodeNo].m_Words[0].m_Word += "("+ConvertedWordHyphen+")";
		};
		m_Nodes[NodeNo].m_Words[0].m_CharCase = FirstCase;
		
		m_Nodes[NodeNo].SetMainWordNo(0);
		// case
	};

};

// 1) Всемером мы подошли к  зданию.
//    Мы втроем можем сделать это за пять минут.
//2) Они отправили нас всемером искать грибы.
//    Они всемером отправили нас  искать грибы.


bool CanBeNumeralAdverbHostMorph(const CRusSemNode& Node)
{
	return Node.HasOneGrammem(rPlural)
	  &&(   Node.HasPOS(NOUN) 
		 || Node.HasPOS(PRONOUN) 
		 || (    Node.IsPrimitive()
			  && Node.m_Words[0].IsRusSubstPronounP()
			)
	    );
};

void CRusSemStructure::NumeralAdverbRule()
{
	for (long NumNodeNo=0; NumNodeNo < m_Nodes.size();NumNodeNo++)
		if (m_Nodes[NumNodeNo].IsPrimitive())
		{
			string Word = m_Nodes[NumNodeNo].m_Words[0].m_Word;
			EngRusMakeUpper(Word);
			long NumeralValue = IsAdverbRule(Word);
			if  (NumeralValue == -1)  continue;
			m_Nodes[NumNodeNo].m_Words[0].m_Lemma = "";
			m_Nodes[NumNodeNo].m_Words[0].m_Word = IntToStr(NumeralValue);
			m_Nodes[NumNodeNo].m_Words[0].m_ParadigmId = -1;
			m_Nodes[NumNodeNo].SetMainWordNo(0);


			long ClauseNo = m_Nodes[NumNodeNo].m_ClauseNo;
			vector<long> Hypots;
			for (long i=NumNodeNo - 1; i >= m_Clauses[ClauseNo].m_BeginNodeNo; i--)
				if (   CanBeNumeralAdverbHostMorph(m_Nodes[i]) )
					Hypots.push_back(i);

			// вшестером мы этого не сделаем
			if (Hypots.empty())
				for (long i=NumNodeNo + 1; i < m_Clauses[ClauseNo].m_EndNodeNo; i++)
					if (   CanBeNumeralAdverbHostMorph(m_Nodes[i]) )
						Hypots.push_back(i);

			if (Hypots.empty()) continue;

			long HypotNodeNo;
			if (Hypots.size() == 1)
				HypotNodeNo = Hypots[0];
			else	
			{
				long i=0;
				for (i=0;i < Hypots.size(); i++)
					if ( HasSemFetOrLower(m_Nodes[Hypots[i]], "ANIM") )					  
						break;

				if (i < Hypots.size())
					HypotNodeNo = Hypots[i];  
				else
					HypotNodeNo = Hypots[0];  
			};

			vector<long> Rels;
			GetIncomingInClauseRelations(NumNodeNo, Rels);
			if (Rels.size() == 1)
			{
				m_Relations[Rels[0]].m_SourceNodeNo = HypotNodeNo;
				m_Relations[Rels[0]].m_Valency = CValency("QUANTIT",A_C);		  
			}
			else
				AddRelation(CRusSemRelation(CValency("QUANTIT",A_C), HypotNodeNo, NumNodeNo ,   ""));

		};

};
