// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Posmorphological Module (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Alexey Sokirko (1999-2002)

#include "MAPostMain.h"
#include "../common/PlmLine.h"



typedef CSmallVector<CHomonym*,5> SmallHomonymsVec;
typedef CSmallVector<CLineIter,5> SmallWordsVec;


enum FioItemTypeEnum {fiName, fiSurname, fiMiddle,fiAbbr,fiStop,fiRoman,fiProbName, fiString, fiOrdinal};
struct CFIOItem {
	FioItemTypeEnum m_fiType;
	string			m_ItemStr;
};


struct CFIOFormat 
{
	string m_FormatStr;
	bool   m_GleicheCase;
	vector<CFIOItem> m_Items;
	CFIOFormat () {};

	CFIOFormat (string  FormatStr, bool GleicheCase) 
	{
		m_FormatStr = FormatStr;
		m_GleicheCase = GleicheCase;
		StringTokenizer tok(FormatStr.c_str(), " ");
		while (tok()) 
		{
			string s = tok.val();
			FioItemTypeEnum t;
		    if  (s == "ИМЯ")
				t = fiName;
			else
			if (s == "ФАМИЛИЯ")
				t = fiSurname;
			else
			if (s == "ОТЧЕСТВО")
				t = fiMiddle;
		    else
			if (s == "ИНИЦИАЛ")
				t = fiAbbr;	
			else
			if (s == "ТОЧКА")
				t = fiStop;
		    else
			if (s == "РИМСК_ЦК")
				t = fiRoman;
			else
		 	if (s == "ИМ?")
				t = fiProbName;
			else
		 	if (s == "ПОРЯД_ЧИСЛ")
				t = fiOrdinal;
		    else
			{
		       t = fiString;
			};
			CFIOItem I;
			I.m_fiType = t;
			I.m_ItemStr = s;
			m_Items.push_back(I);
		};	
	};
};


static void GetCommonVariants(const vector<SmallHomonymsVec>&  Parents, 
									   SmallHomonymsVec&   V,
							  vector<SmallHomonymsVec>&  Variants, 
								long       Position)
{ 
    if (Variants.size() > 1000) return;
	if (Position < V.size())
	for (long i=0; i< Parents[Position].size(); i++)
	{
			V.m_Items[Position] = Parents[Position].m_Items[i];
			GetCommonVariants(Parents, V, Variants, Position+1);
	}
	else
			Variants.push_back(V);

};


static bool IsPartFio(const CMAPost& C, const CFIOItem& I, const CPostLemWord& Word, const CHomonym* pH)  
{

    if ( Word.HasDes(OFAM1) || Word.HasDes(OFAM2)  )    return false;

    switch (I.m_fiType)
    {
        case fiName     :  return pH->HasGrammem(rName);
        case fiSurname  :  return pH->HasGrammem(rSurName);
        case fiMiddle   :  return pH->HasGrammem(rPatronymic);

        case fiAbbr     :  return      Word.m_bFirstUpperAlpha
                                    && (Word.m_strWord.size() == 1)
                                    && Word.HasDes(ORLE);

        case fiStop     :  return Word.m_strWord == ".";
        case fiRoman    :  return Word.HasDes(ORoman);
        case fiOrdinal  :  return pH->HasPos(NUMERAL_P);
        case fiProbName :  return Word.HasDes(ONam);
        default         :  return pH->m_strLemma == I.m_ItemStr;
    }                


};


bool IsPartOfNonSingleOborot(const CHomonym* pH)
{
    return pH->m_bOborot1 != pH->m_bOborot2;
}

static bool SetFioFormat (CMAPost& C, const CFIOFormat& Format, CLineIter it) 
{
   vector<SmallHomonymsVec> Hypots;
   SmallWordsVec FioWords;

   Hypots.resize(Format.m_Items.size());
   
   int CountOfVariants = 1;
   for (long ItemNo = 0; ItemNo < Format.m_Items.size() && it != C.m_Words.end(); ItemNo++, it=C.NextNotSpace(it))
   {
        FioWords.Add(it);
		CPostLemWord& W = *it;
        
        for (int HomNo=0; HomNo < W.GetHomonymsCount();  HomNo++)
		{
            CHomonym* pH = W.GetHomonym(HomNo);
			// иначе "Т.Е. ОТКАЗАТЬСЯ" будет ФИО
            if (IsPartOfNonSingleOborot(pH)) return false;

			if (IsPartFio(C, Format.m_Items[ItemNo], W, pH))
					Hypots[ItemNo].Add(pH);
		};
        if ( Hypots[ItemNo].empty() ) return false;
		CountOfVariants *= Hypots[ItemNo].size();
   };

   if (FioWords.size() != Format.m_Items.size()) return false; // не достроилось

   SmallHomonymsVec V; // текущий вариант 
   vector<SmallHomonymsVec> Variants;
   Variants.reserve(CountOfVariants);
   V.m_ItemsCount = Hypots.size();
   GetCommonVariants(Hypots, V, Variants, 0);

   if (Format.m_GleicheCase)
	for (long VarNo=0; VarNo < Variants.size(); VarNo++)
	{
		QWORD Grammems = rAllCases;
		for (long i=0; i < Variants[VarNo].size(); i++)
		{
            Grammems  &=   Variants[VarNo].m_Items[i]->m_iGrammems;
		};
		if (Grammems  == 0)
		{
			Variants.erase(Variants.begin()+VarNo);
			VarNo--;
		};

	};

   if (Variants.empty()) return false;

   for (size_t i=0; i <FioWords.size(); i++)
   {
       CPostLemWord& W = *FioWords[i];
       CHomonym* pH = Variants[0].m_Items[i];
       W.SetHomonymsDel(true);
       pH->m_bDelete = false;
       pH->DeleteOborotMarks(); // удаляем однословные оборотыб (многословных там быть не может)
       W.DeleteMarkedHomonymsBeforeClauses();
       if (W.HasDes(OSentEnd))
       {
           /*
             если ФИО содержало конец предложения, а после ФИО нет ни одной пометы 
	         конца предложения, тогда надо поставить SENT_END на последнюю строчку ФИО.
           */
           W.DelDes(OSentEnd);
           FioWords.back()->AddDes(OSentEnd);
       }
   }

   // ставим графем. пометы
   FioWords[0]->AddDes(OFAM1);
   FioWords.back()->AddDes(OFAM2);

   return true;

};


/*
Петр Владимирович Иванов,
Иванов Петр Владимирович,
Петр Иванов,
Иванов Петр,
Петр Владимирович,
Райнер Мария Рильке,
Иоанн Павел II
М.Горбачев
*/

void CMAPost::Rule_Fio() 
{
    vector<CFIOFormat> FioFormats;
    FioFormats.push_back(CFIOFormat("ИМЯ ОТЧЕСТВО ФАМИЛИЯ", true));
    FioFormats.push_back(CFIOFormat("ФАМИЛИЯ ИМЯ ОТЧЕСТВО", true));
    FioFormats.push_back(CFIOFormat("ИМЯ ФАМИЛИЯ", true));
    FioFormats.push_back(CFIOFormat("ФАМИЛИЯ ИМЯ", true));
    FioFormats.push_back(CFIOFormat("ИМЯ ОТЧЕСТВО", true));
    FioFormats.push_back(CFIOFormat("ИМЯ ИМЯ ИМ?", false)); // Эрих Мария Ремарк
    FioFormats.push_back(CFIOFormat("ИНИЦИАЛ ТОЧКА ИНИЦИАЛ ТОЧКА ФАМИЛИЯ", false)); // М.Горбачев
    FioFormats.push_back(CFIOFormat("ИНИЦИАЛ ТОЧКА ФАМИЛИЯ", false)); // М.Горбачев
    FioFormats.push_back(CFIOFormat("ИМЯ ИМЯ РИМСК_ЦК", false)); // Иоанн Павел II
    FioFormats.push_back(CFIOFormat("ИМЯ РИМСК_ЦК", false)); // Александр II
    FioFormats.push_back(CFIOFormat("ИМЯ ПОРЯД_ЧИСЛ", false)); // Александр Второй
    FioFormats.push_back(CFIOFormat("ИМЯ ИМЯ ПОРЯД_ЧИСЛ", false)); // Иоанн Павел Второй
    FioFormats.push_back(CFIOFormat("ДОН ЖУАН", false)); // Дон Жуан

    
    for (CLineIter it=m_Words.begin(); it !=  m_Words.end(); it++)
    {
        for (long FormatNo=0; FormatNo < FioFormats.size(); FormatNo++)
            if (SetFioFormat (*this, FioFormats[FormatNo], it))
            {
                break;
            }
    };

};

