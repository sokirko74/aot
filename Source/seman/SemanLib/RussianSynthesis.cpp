#include "stdafx.h"
#include "SemanticRusStructure.h"



std::string GetStringByNode (const CRusSemStructure& R, int NodeNo)
{
	const CRusSemNode& Node = R.m_Nodes[NodeNo];
	std::string Result;
	for (auto& w: Node.m_Words)
	{
		std::string str = w.GetWord();
		if (w.m_CharCase == LowLow) {
			MakeLowerUtf8(str);
		}
		else if (w.m_CharCase == UpLow) {
			MakeTitleUtf8(str);
		}
		else {
			// already uppercase
		}
		Result += str + " ";
		
	};
	if (Node.m_bQuoteMarks)
		Result = "<Quote> " + Result + "</Quote> ";

	return Result;
};

bool IsSynthesLess(const CRusSemStructure& R, size_t NodeNo1, size_t NodeNo2)
{
	return R.m_Nodes[NodeNo1].GetMinWordNo() < R.m_Nodes[NodeNo2].GetMinWordNo();
};

std::string CRusSemStructure::GetPrepOrConjFromSynReal (const CSynRealization&  SynReal) const
{
	std::string Result;
    if (!SynReal.m_Preps.empty())
	{
		std::string PrepAndCase = GetRoss(SynReal.m_Preps[0].m_DictType)->GetEntryStr(SynReal.m_Preps[0].m_UnitNo);
		if (PrepAndCase.find("+") != std::string::npos)
			PrepAndCase.erase(PrepAndCase.find("+"));
		Result += PrepAndCase;		
	};

    if (SynReal.m_Conj.m_UnitNo != ErrUnitNo)
	     Result += std::string(" ")+GetRoss(SynReal.m_Conj.m_DictType)->GetEntryStr(SynReal.m_Conj.m_UnitNo);

	//  change '_' to ' ' in compound prepositions an conjunctions
	for (size_t i =0; i<Result.size(); i++)
		if (Result[i] == '_')
			Result[i] = ' ';

	return Result;
};

std::string CRusSemStructure::GetPrepOrConjOfNode (const CRusSemNode&  Node) const
{
	std::string Result  = GetPrepOrConjFromSynReal(Node.m_SynReal);



	return Result;
};

void AddRelOperators (const CRusSemNode& Node, std::string& Result)
{
	// Добавляем частицы ("не", "ни", "даже")
	for (size_t i=0; i < Node.m_RelOperators.size(); i++)
	{
			std::string s = Node.m_RelOperators[i];
			MakeLowerUtf8(s);
			Result = s + " " + Result;
	};
};

std::string SynthRecursive(const CRusSemStructure& R, int NodeNo, bool bRoot)
{
	const CRusSemNode& Node = R.m_Nodes[NodeNo];

	std::vector<long> Rels;
	R.GetOutcomingRelations(NodeNo, Rels, false);
	

	std::string left,right;
	for (size_t  i=0; i < Rels.size(); i++)
	{
		const CRusSemRelation& Rel = R.m_Relations[Rels[i]];
		if (Rel.m_bDopRelation) continue;
		int SubNodeNo =  Rel.m_TargetNodeNo;
		std::string Word = SynthRecursive(R, SubNodeNo, false);
		Word = R.GetPrepOrConjFromSynReal(Rel.m_SynReal) + " "+ Word;

		if (Node.m_ClauseNo !=  R.m_Nodes[SubNodeNo].m_ClauseNo)
			Word = ", "+ Word;


		if (IsSynthesLess(R, SubNodeNo, NodeNo))
			left += " " + Word;
		else
			right += " " + Word;
	};

	

	// if a node was found in CollocDictionary and it has a preposition then 
	//  we should restore it before this node
	if (Node.GetType() == CollocRoss || bRoot)
		left = R.GetPrepOrConjOfNode(Node) + " "+ left;
	
	std::string NodeItself = 	GetStringByNode(R, NodeNo);

	//  relational operators goes before all subordinates, if the node  is a noun
	if (R.IsNounOrMNANoun(NodeNo))
		AddRelOperators(Node, left);
	else
		AddRelOperators(Node, NodeItself);

	std::string Result = left +" "+ NodeItself + right;

	//  специальная обработка разрывных союзов
	if	(		(Node.m_MNAType == RepeatMNA)
			&&	(Node.GetType() == OborRoss)
		)
	{
		std::string UnitStr = R.GetRoss(OborRoss)->GetEntryStr(Node.GetInterp()->m_UnitNo);
		UnitStr = convert_from_utf8(UnitStr.c_str(), morphRussian);
		std::string Delim = "...";
		int index = UnitStr.find(Delim);
		MakeLowerUtf8(UnitStr);
		if (index != std::string::npos)
			Result = UnitStr.substr(0, index) +std::string(" ")+ left + std::string(" , ") + UnitStr.substr(index+Delim.length()) +" "+ right;
	};
	return Result;
	
};


extern void GenerateRecursive(CRusSemStructure& R, int NodeNo);

std::string	CRusSemStructure::RussianSynthesisOfNode(int NodeNo)
{
	GenerateRecursive(*this, NodeNo);
	std::string Result = SynthRecursive(*this, NodeNo, true);
	{
		//  if  it is a clause root then we should add the clause conjunction to the beginning
		// if it exists
		std::vector<long> Rels;
		CSemanticStructure::GetIncomingRelations(NodeNo,Rels, false);
		for (size_t i=0; i<Rels.size(); i++)
			if (IsRelBetweenClauses(m_Relations[Rels[i]]))
			{
				Result = GetPrepOrConjFromSynReal(m_Relations[Rels[i]].m_SynReal) + " "+Result;
			};
	};

	return Result;
};

bool	CRusSemStructure::RussianSynthesis(std::string& Result) const
{
	std::string str;
	CRusSemStructure NewStructure = *this;
    // берем все вершины клаузы
	std::vector<long> roots;	
	GetRoots(roots);
	for (size_t RootNo =0;  RootNo <roots.size(); RootNo++)
	{
		str += NewStructure.RussianSynthesisOfNode(roots[RootNo]);
	};
	::SetSpacesAndRegisterInSentence(str);
	NewStructure.SetLastSentencePunctuationMark(str);
	Result = str;
	return true;
};
