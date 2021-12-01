#include "StdAfx.h"
#include "../../SemanLib/SemanticRusStructure.h"


static void GetMaxSimilarWordForm (const CRusSemStructure& R, CRusSemWord& Word, uint64_t OldGrammems)
{
	
	Word.m_Word = Word.m_Lemma;

	IParadigmPtr piParadigm;
	//  calculating piParadigm by Lemma and poses from NodeNo
	{
		long ParadigmId = GetFirstParadigmId(Word.m_Lemma, R.m_pData->m_piRusLemmatizer, R.m_pData->m_Agramtab, Word.m_Poses);

		if ((ParadigmId == -1) && (Word.m_Poses != 0))
			ParadigmId = GetFirstParadigmId(Word.m_Lemma, R.m_pData->m_piRusLemmatizer, R.m_pData->m_Agramtab, 0);

		if (ParadigmId == -1)
			return;

		piParadigm = R.m_pData->m_piRusLemmatizer->CreateParadigmFromID(ParadigmId);
	}

	// finding word form, whose grammems has the maximal intersection with R.m_Nodes[NodeNo].GetGrammems()
	size_t MaxSizeOfIntersection = 0;
	IGramTabPtr A = R.m_pData->m_Agramtab;
	std::string debug_old = A->GrammemsToStr(OldGrammems);
	

	for (long k=0; k < piParadigm->Count; k++)
	{
		std::string  AnCode = (const char*)piParadigm->Ancode[k];
		uint64_t currGrammems = A->GetGrammems(AnCode.c_str());
		uint64_t Intersection = currGrammems & OldGrammems;
 		std::string debug = A->GrammemsToStr(Intersection);
		size_t SizeOfIntersection = 0;

		{
			for (size_t i=0; i <sizeof(uint64_t)*8; i++)
				if (_QM(i) & Intersection)
					SizeOfIntersection++;
		}
		
		if (SizeOfIntersection > MaxSizeOfIntersection)
		{
			MaxSizeOfIntersection = SizeOfIntersection;
			Word.m_Word = (const char*)piParadigm->Form[k];
			Word.SetFormGrammems( currGrammems );
		}
	};
	

};

static void GenerateCoordinatedNodes(CRusSemStructure& R, int RelNo, 	uint64_t NewGrammems, 	uint64_t OldGrammems)
{
	const CRusSemRelation& Rel = R.m_Relations[RelNo];
	std::vector<long> SubNodes;

	if (R.m_Nodes[Rel.m_TargetNodeNo].m_NodeType != MNA)
		SubNodes.push_back(Rel.m_TargetNodeNo);
	else
	{
		std::vector<long> SubRels;
		R.GetOutcomingRelations(Rel.m_TargetNodeNo, SubRels);
		for (size_t j=0; j <SubRels.size(); j++)
			SubNodes.push_back(R.m_Relations[SubRels[j]].m_TargetNodeNo);

	};


	for (size_t j=0; j<SubNodes.size(); j++)
	{
		CRusSemNode& Target = R.m_Nodes[ SubNodes[j] ];
		if	(		( (rAllGenders & NewGrammems) != (rAllGenders & OldGrammems))
				&&	(Target.m_MainWordNo != -1)
			)
		{
			CRusSemWord& W = Target.m_Words[Target.m_MainWordNo];
			uint64_t Grammems = (W.GetAllGrammems() & ~rAllGenders & ~rAllAnimative);
			Grammems |= rAllGenders & NewGrammems;
			Grammems |= rAllAnimative & NewGrammems;
			GetMaxSimilarWordForm(R,  W, Grammems);
		};
	};
}

static void GeneratePredicateBySubject(CRusSemStructure& R, int RelNo, 	uint64_t NewGrammems, 	uint64_t OldGrammems)
{
	const CRusSemRelation& Rel = R.m_Relations[RelNo];
	CRusSemNode& PredNode = R.m_Nodes[Rel.m_SourceNodeNo];
	if (PredNode.m_MainWordNo == -1) return;
	CRusSemWord& W = PredNode.m_Words[PredNode.m_MainWordNo];

	
	if	(		(PredNode.GetGrammems() & _QM(rPastTense))
			&&	(		( (rAllGenders & NewGrammems) != (rAllGenders & OldGrammems))
					||	( (rAllNumbers & NewGrammems) != (rAllNumbers & OldGrammems))
				)
		)
	{
		uint64_t Grammems = (W.GetAllGrammems() & ~rAllGenders & ~rAllNumbers);
		Grammems |= rAllGenders & NewGrammems;
		Grammems |= rAllNumbers & NewGrammems;
		GetMaxSimilarWordForm(R,  W, Grammems);
	}
	else
		if	(		( (rAllPersons & NewGrammems) != (rAllPersons & OldGrammems))
				||	( (rAllNumbers & NewGrammems) != (rAllNumbers & OldGrammems))
			)
		{
			uint64_t Grammems = (W.GetAllGrammems() & ~rAllPersons & ~rAllNumbers);
			Grammems |= rAllPersons & NewGrammems;
			Grammems |= rAllNumbers & NewGrammems;
			GetMaxSimilarWordForm(R,  W, Grammems);

		};

};


static void GenerateNewWordFormAndAdjustCoordination (CRusSemStructure& R, int NodeNo, int WordNo)
{
	uint64_t NewGrammems;
	uint64_t OldGrammems;
	{
		CRusSemNode& Node = R.m_Nodes[NodeNo];
		OldGrammems = Node.m_Words[WordNo].GetAllGrammems();
		if (WordNo == Node.m_MainWordNo)
			OldGrammems = Node.GetGrammems();

		GetMaxSimilarWordForm(R,  Node.m_Words[WordNo], OldGrammems);

		NewGrammems = Node.m_Words[WordNo].GetAllGrammems();
		if (WordNo != Node.m_MainWordNo) return;
	}

	std::vector<long> Rels;
	R.GetOutcomingRelations(NodeNo, Rels);
	for (size_t i=0; i <Rels.size(); i++)
	{
		if (R.m_Relations[Rels[i]].m_SyntacticRelation == _R("ПРИЛ_СУЩ"))
			GenerateCoordinatedNodes(R,Rels[i], NewGrammems, OldGrammems);

	};

	
	CRelSet RelSet = R.GetIncomingRelations(NodeNo, false);

	for (size_t i=0; i <RelSet.m_RelsCount; i++)
	{
		long RelNo = RelSet.m_Rels[i];
		if (R.m_Relations[RelNo].m_bDopRelation) continue;

		if (R.m_Relations[RelNo].m_SyntacticRelation == _R("подл"))
			GeneratePredicateBySubject(R,RelNo, NewGrammems, OldGrammems);
	};

};



void GenerateRecursive(CRusSemStructure& R, int NodeNo)
{

	std::vector<long> Rels;
	R.GetOutcomingRelations(NodeNo, Rels, false);
	for (size_t  i=0; i < Rels.size(); i++)
	{
		const CRusSemRelation& Rel = R.m_Relations[Rels[i]];
		GenerateRecursive(R,Rel.m_TargetNodeNo);
	};

	CRusSemNode& Node = R.m_Nodes[NodeNo];
	for (size_t i=0; i < Node.m_Words.size(); i++)
	{
		
		std::map<std::string,std::string>::const_iterator it = R.m_SynthLemmaToReplace.find(Node.m_Words[i].m_Lemma);
		if (it == R.m_SynthLemmaToReplace.end()) continue;
		Node.m_Words[i].m_Lemma = it->second;
		GenerateNewWordFormAndAdjustCoordination(R, NodeNo, i);
	};
};

