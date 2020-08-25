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
	for (int i = 0; i < NumeralToNumberCount; i++)
	{
		if (NumeralToNumber[i].m_Number == 0)
		{
			GenitFormsOfCardinal.push_back(_R("НУЛЬ"));
			continue;
		};
		if (NumeralToNumber[i].m_Number == 1)
		{
			GenitFormsOfCardinal.push_back(_R("ОДНО"));
			continue;
		};
		if (NumeralToNumber[i].m_Number == 100)
		{
			GenitFormsOfCardinal.push_back(_R("СТО"));
			continue;
		};
		if (NumeralToNumber[i].m_Number == 1000)
		{
			GenitFormsOfCardinal.push_back(_R("ТЫСЯЧЕ"));
			continue;
		};
		if (NumeralToNumber[i].m_Number == 1000000)
		{
			GenitFormsOfCardinal.push_back(_R("МИЛЛИОНО"));
			continue;
		};
		if (NumeralToNumber[i].m_Number == 1000000000)
		{
			GenitFormsOfCardinal.push_back(_R("МИЛЛИАРДНО"));
			continue;
		};
		if (NumeralToNumber[i].m_Number == 1000000000000.0)
		{
			GenitFormsOfCardinal.push_back(_R("ТРИЛЛИОНО"));
			continue;
		};
		if (NumeralToNumber[i].m_Number == 1000000000000000.0)
		{
			GenitFormsOfCardinal.push_back(_R("КВАДРИЛЛИОНО"));
			continue;
		};
		std::vector<CFormInfo> ParadigmCollection;
		std::string WordForm = NumeralToNumber[i].m_Cardinal;
		piRusLemmatizer->CreateParadigmCollection(true, WordForm, false, false, ParadigmCollection);
		// ищем числительное
		long k = 0;
		for (; k < ParadigmCollection.size(); k++)
		{
			std::string AnCode = ParadigmCollection[k].GetAncode(0);
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
		assert(k < ParadigmCollection.size());
		const CFormInfo& P = ParadigmCollection[k];
		// ищем родительный падеж
		for (k = 0; k < P.GetCount(); k++)
		{
			std::string AnCode = P.GetAncode(k);
			QWORD Grammems;
			if (!Agramtab->GetGrammems(AnCode.c_str(), Grammems))
				throw CExpc("Bad ancode in  BuildGenitFormOfCardinal");
			if ((Grammems & _QM(rGenitiv)) > 0)
				break;
		};
		assert(k < P.GetCount());
		GenitFormsOfCardinal.push_back(P.GetWordForm(k));
	};
	return true;
};



double GetGenitFormNumeral(std::string word)
{
	for (size_t i = 0; i < GenitFormsOfCardinal.size(); i++)
		if (word == GenitFormsOfCardinal[i])
			return NumeralToNumber[i].m_Number;
	return -1;
};


// выдает длину максимального префикса слова word, который совпадает с одним из 
// генитивных вариантов числительного.
// Если такого генитивного  варианта не было обнаружено, то 
// проверяются некоторые фонетические  варианты  этих префиксов.
int GetNumeralPrefixGenitForm(std::string word, int& maxPrefixLength)
{
	maxPrefixLength = -1;
	long NumLine = -1;


	for (int i = 0; i < GenitFormsOfCardinal.size(); i++)
	{
		long len = GenitFormsOfCardinal[i].length();
		if (len > maxPrefixLength)
			if (startswith(word, GenitFormsOfCardinal[i]))
			{
				NumLine = i;
				maxPrefixLength = len;
			};
	};

	if (maxPrefixLength < 3)
		if (startswith(word, _R("ДВУ"))) // двуногий
		{
			NumLine = 2;
			maxPrefixLength = 3;
			assert(NumeralToNumber[NumLine].m_Number == 2);
		};

	if (maxPrefixLength < 5)
		if (startswith(word, _R("ДВУХЪ"))) // двухъярусный
		{
			NumLine = 2;
			maxPrefixLength = 5;
			assert(NumeralToNumber[NumLine].m_Number == 2);
		};

	if (maxPrefixLength < 5)
		if (startswith(word, _R("ТРЕХЪ"))) // трехъярусный
		{
			NumLine = 3;
			maxPrefixLength = 5;
			assert(NumeralToNumber[NumLine].m_Number == 3);
		};

	if (maxPrefixLength < 8)
		if (startswith(word, _R("ЧЕТЫРЕХЪ"))) // четырехъярусный
		{
			NumLine = 4;
			maxPrefixLength = 8;
			assert(NumeralToNumber[NumLine].m_Number == 4);
		};

	if (maxPrefixLength < 4)
		if (startswith(word, _R("ОДНО"))) // одноэтажный
		{
			NumLine = 0;
			maxPrefixLength = 4;
			assert(NumeralToNumber[NumLine].m_Number == 1);
		};

	if (maxPrefixLength < 4)
		if (startswith(word, _R("НУЛЬ"))) // нульмодемный
		{
			NumLine = NumeralToNumberCount;
			maxPrefixLength = 4;
			assert(NumeralToNumber[NumLine].m_Number == 0);
		};

	return NumLine;
};

double FindInTable(std::string word)
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


double ConvertNumeralByTable(std::vector<CRusSemWord>::const_iterator Start, std::vector<CRusSemWord>::const_iterator End)
{

	double Result = 0;


	long LastLength = 20;
	for (std::vector<CRusSemWord>::const_iterator Word = Start; Word < End; Word++)
	{
		double Res;


		if (!Word->m_Word.empty()
			&& isdigit((BYTE)Word->m_Word[0])
			)
			//  "15 тысяч граждан"
			Res = atof(Word->m_Word.c_str());
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


static bool ConvertOneWordOrdinalNumeral(const std::string& InputOrdinal, string& result)
{
	result = IntToStr(GetOrdinalNumeral(InputOrdinal));
	if (result != "-1") {
		return true;
	}
	std::string Word = InputOrdinal;
	long PrefixNumber = 0;

	for (;;)
	{
		int PrefixLength = -1;
		int LineNo = GetNumeralPrefixGenitForm(Word, PrefixLength);
		if (LineNo == -1) break;
		Word.erase(0, PrefixLength);
		PrefixNumber += (long)NumeralToNumber[LineNo].m_Number;
	};

	if (Word == InputOrdinal) {
		return false;
	}
	QWORD lastRes = GetOrdinalNumeral(Word);
	if (lastRes >= 1000)  //"тысячный" или "миллионный"
		result = IntToStr(PrefixNumber * lastRes);
	else
		result = IntToStr(PrefixNumber) + '#' + Word;
	return true;
};


double ConvertNumeralByOrderRecursive(std::vector<CRusSemWord>::const_iterator Start, std::vector<CRusSemWord>::const_iterator End, QWORD Order)
{
	std::string OrderStr = FindByNumber(Order);
	std::string Result;
	if (Order == 1)
	{
		return ConvertNumeralByTable(Start, End);
	}
	else
	{
		for (auto i = Start; i < End; ++i) {
			if (i->m_Lemma == OrderStr) {
				double left_coef = 1.0;
				if (i > Start) {
					left_coef = ConvertNumeralByTable(Start, i);
				}
				double left = Order * left_coef;
				double right = ConvertNumeralByOrderRecursive(i + 1, End, Order / 1000);
				return left + right;
			}
		}
		return ConvertNumeralByOrderRecursive(Start, End, Order / 1000);
	}
};

bool FullAdjWithNumeralPrefix(const CRusSemNode& Node)
{
	if (!Node.IsPrimitive()) return false;
	if (!Node.m_Words[0].HasPOS(ADJ_FULL)) return false;
	int dummy;
	if (GetNumeralPrefixGenitForm(Node.m_Words[0].m_Word, dummy) == -1) return false;
	if (Node.m_Words[0].m_Word.substr(0, 3) == _R("СТО"))
	{
		std::string Word = Node.m_Words[0].m_Word.substr(3);
		if (GetNumeralPrefixGenitForm(Word, dummy) != -1) return true;
	};
	if ((Node.m_Words[0].m_Word.substr(0, 3) == _R("СТО"))
		|| (Node.m_Words[0].m_Word.substr(0, 4) == _R("ОДНО"))
		)
		if (Node.m_Words[0].m_ParadigmId != -1) return false;
	return true;
};


static bool ConvertRusNumeral(const CRusSemNode& Node, double& number)
{
	if (Node.m_Words.size() == 1 && (FindFloatingPoint(Node.m_Words[0].m_Word.c_str()) != -1 || Node.m_Words[0].HasOneGrammem(rComparative)))
		return false;
	number =  ConvertNumeralByOrderRecursive(Node.m_Words.begin(), Node.m_Words.end(), 1000000000000);
	return number != -1;
};


void InterpretAdjectivesWithNumeral(CRusSemStructure& R, CRusSemNode& node)
{
	// пробуем проинтерпретировать с "-" в начале
	node.m_Words[0].m_Lemma = "-" + node.m_Words[0].m_Lemma;
	R.InitInterps(node.m_Words[0], false, node.m_ClauseNo, node, false);
	// если не получилось, отрзаем  "-" и интерпретируем обычно
	if (node.GetInterps().empty())
	{
		node.m_Words[0].m_Lemma.erase(0, 1);
		R.InitInterps(node.m_Words[0], false, node.m_ClauseNo, node, true);
	};

};

void ConvertOneRusNumeralsToArabic (CRusSemStructure& R, CRusSemNode& node) {
	std::string LemmaAfterHyphen;
	int pos = node.m_Words[0].m_Lemma.find("-");

	if (pos != -1)
	{
		LemmaAfterHyphen = node.m_Words[0].m_Word.substr(pos + 1);
		node.m_Words[0].m_Lemma = node.m_Words[0].m_Lemma.substr(0, pos);
	};

	std::string  ConvertedWord;
	int dummy;
	if ((node.IsPrimitive())
		&& (node.m_Words[0].HasPOS(NUMERAL_P)
			|| (
				node.m_Words[0].HasPOS(ADJ_FULL)
				&& (GetNumeralPrefixGenitForm(node.m_Words[0].m_Word, dummy) != -1)
				)
			)
		)
	{
		if (!ConvertOneWordOrdinalNumeral(node.m_Words[0].m_Lemma, ConvertedWord)) {
			return;
		}
		int j = ConvertedWord.find('#');

		if (j != -1) // по этой же схеме идут числительные "двухтысячный", которые не обрабатывется в этом if
		{
			int len = ConvertedWord.length() - j - 1;
			node.m_Words[0].m_Lemma.erase(0, node.m_Words[0].m_Lemma.length() - len);
			node.m_Words[0].m_ParadigmId = R.m_pData->GetFirstParadigmId(morphRussian, node.m_Words[0].m_Lemma, 0);
			node.m_Words[0].m_NumeralPrefix = ConvertedWord.substr(0, j);
			ConvertedWord.erase(0, j + 1);
			InterpretAdjectivesWithNumeral(R, node);
		};
	}
	else {
		double number;
		if (!ConvertRusNumeral(node, number))
		{
			// слова типа "оба", "несколько" являются числительными, но не конвертируются,
			// а оставляются нетронутыми
			//assert (node.IsPrimitive());

			//еще могут быть просто ряды числительных, которые нельзя считать целым QUANTIT, например:		
			// "один два"
			if (node.m_Words.size() > 1)
				for (long i = 0; i < node.m_Words.size(); i++)
				{
					float Res = FindInTable(node.m_Words[i].m_Lemma);
					if (Res != -1)
					{
						node.m_Words[i].m_Word = IntToStr(Res);;
						node.m_Words[i].m_Lemma = "";
						node.m_Words[i].m_ParadigmId = -1;
					};

				};
			return;
		};
		ConvertedWord = IntToStr(number);
	}

	std::string  ConvertedWordHyphen = "-1";

	// разберем случай "полтора-два миллиона баррелей"
	if (LemmaAfterHyphen != "") {
		node.m_Words[0].m_Lemma = LemmaAfterHyphen;
		double number;
		if (ConvertRusNumeral(node, number)) {
			ConvertedWordHyphen = IntToStr(number);
		}
	};


	// берем этот слоту первого слова
	RegisterEnum FirstCase = node.m_Words[0].m_CharCase;
	// удаляем все слова кроме последнего слова
	node.m_Words.erase(node.m_Words.begin(), node.m_Words.end() - 1);
	// обнуляем лемму последнего слова
	if (node.m_Words[0].m_NumeralPrefix == "")
	{
		node.m_Words[0].m_Lemma = "";
		node.m_Words[0].m_ParadigmId = -1;
		node.DelAllInterps();
	};
	node.m_Words[0].m_Word = ConvertedWord;
	if (R.HasRichPOS(node, NOUN)) // миилион, миллиард
	{
		node.m_Words[0].m_Poses = 1 << NUMERAL;
		node.m_Words[0].SetFormGrammems(node.GetGrammems());
		node.m_Words[0].m_GramCodes = node.m_GramCodes;
	}
	if (ConvertedWordHyphen != "-1")
	{
		node.m_Words[0].m_Word += "(" + ConvertedWordHyphen + ")";
	};
	node.m_Words[0].m_CharCase = FirstCase;
	node.SetMainWordNo(0);
}

void CRusSemStructure::ConvertRusNumeralsToArabic()
{
	// прилагательные типа 2-кратный берутся на Maposte
	// их  надо преобразовать в слот  NumeralPrefix
	for (auto& node : m_Nodes)
		if (   node.IsPrimitive()
			&& HasRichPOS(node, ADJ_FULL)
			&& isdigit((BYTE)node.m_Words[0].m_Word[0])
			)
		{
			int i = node.m_Words[0].m_Word.find("#");
			node.m_Words[0].m_NumeralPrefix = node.m_Words[0].m_Word.substr(0, i);
			node.m_Words[0].m_Word.erase(0, i + 1);
			InterpretAdjectivesWithNumeral(*this, node);
		};

	//	А теперь основной цикл ...
	for (auto& node : m_Nodes) {
		if (   (node.m_SynGroupTypeStr == NUMERALS)
			|| (node.IsPrimitive() && HasRichPOS(node, NUMERAL))
			|| (node.IsPrimitive() && HasRichPOS(node, NUMERAL_P))
			|| FullAdjWithNumeralPrefix(node)
			)
		{
			if (node.m_Words.size() == 1 && node.m_Words[0].m_ArabicNumber) {
				continue;
			}
			ConvertOneRusNumeralsToArabic(*this, node);
		};
	}

};

// 1) Всемером мы подошли к  зданию.
//    Мы втроем можем сделать это за пять минут.
//2) Они отправили нас всемером искать грибы.
//    Они всемером отправили нас  искать грибы.


bool CanBeNumeralAdverbHostMorph(const CRusSemNode& Node)
{
	return Node.HasOneGrammem(rPlural)
		&& (Node.HasPOS(NOUN)
			|| Node.HasPOS(PRONOUN)
			|| (Node.IsPrimitive()
				&& Node.m_Words[0].IsRusSubstPronounP()
				)
			);
};

void CRusSemStructure::NumeralAdverbRule()
{
	for (long NumNodeNo = 0; NumNodeNo < m_Nodes.size(); NumNodeNo++)
		if (m_Nodes[NumNodeNo].IsPrimitive())
		{
			std::string Word = m_Nodes[NumNodeNo].m_Words[0].m_Word;
			EngRusMakeUpper(Word);
			long NumeralValue = IsAdverbRule(Word);
			if (NumeralValue == -1)  continue;
			m_Nodes[NumNodeNo].m_Words[0].m_Lemma = "";
			m_Nodes[NumNodeNo].m_Words[0].m_Word = IntToStr(NumeralValue);
			m_Nodes[NumNodeNo].m_Words[0].m_ParadigmId = -1;
			m_Nodes[NumNodeNo].SetMainWordNo(0);


			long ClauseNo = m_Nodes[NumNodeNo].m_ClauseNo;
			std::vector<long> Hypots;
			for (long i = NumNodeNo - 1; i >= m_Clauses[ClauseNo].m_BeginNodeNo; i--)
				if (CanBeNumeralAdverbHostMorph(m_Nodes[i]))
					Hypots.push_back(i);

			// вшестером мы этого не сделаем
			if (Hypots.empty())
				for (long i = NumNodeNo + 1; i < m_Clauses[ClauseNo].m_EndNodeNo; i++)
					if (CanBeNumeralAdverbHostMorph(m_Nodes[i]))
						Hypots.push_back(i);

			if (Hypots.empty()) continue;

			long HypotNodeNo;
			if (Hypots.size() == 1)
				HypotNodeNo = Hypots[0];
			else
			{
				long i = 0;
				for (i = 0; i < Hypots.size(); i++)
					if (HasSemFetOrLower(m_Nodes[Hypots[i]], "ANIM"))
						break;

				if (i < Hypots.size())
					HypotNodeNo = Hypots[i];
				else
					HypotNodeNo = Hypots[0];
			};

			std::vector<long> Rels;
			GetIncomingInClauseRelations(NumNodeNo, Rels);
			if (Rels.size() == 1)
			{
				m_Relations[Rels[0]].m_SourceNodeNo = HypotNodeNo;
				m_Relations[Rels[0]].m_Valency = CValency("QUANTIT", A_C);
			}
			else
				AddRelation(CRusSemRelation(CValency("QUANTIT", A_C), HypotNodeNo, NumNodeNo, ""));

		};

};
