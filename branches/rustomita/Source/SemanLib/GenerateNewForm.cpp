#include "stdafx.h"
#include "SemanticRusStructure.h"


static void GetMaxSimilarWordForm (const CRusSemStructure& R, CRusSemWord& Word, QWORD OldGrammems)
{
	
	Word.m_Word = Word.m_Lemma;

	CFormInfo Paradigm;
	//  calculating piParadigm by Lemma and poses from NodeNo
	{
		long ParadigmId = R.m_pData->GetFirstParadigmId(morphRussian, Word.m_Lemma, Word.m_Poses);

		if ((ParadigmId == -1) && (Word.m_Poses != 0))
			ParadigmId = R.m_pData->GetFirstParadigmId(morphRussian, Word.m_Lemma, 0);

		if (ParadigmId == -1)
			return;

		R.m_pData->GetRusLemmatizer()->CreateParadigmFromID(ParadigmId, Paradigm);
	}

	// finding word form, whose grammems has the maximal intersection with R.m_Nodes[NodeNo].GetGrammems()
	size_t MaxSizeOfIntersection = 0;
	const CRusGramTab* A = R.m_pData->GetRusGramTab();
	string debug_old = A->GrammemsToStr(OldGrammems);
	

	for (long k=0; k < Paradigm.GetCount(); k++)
	{
		string  AnCode = Paradigm.GetAncode(k);
		QWORD currGrammems;
		A->GetGrammems(AnCode.c_str(), currGrammems);
		QWORD Intersection = currGrammems & OldGrammems;
 		string debug = A->GrammemsToStr(Intersection);
		size_t SizeOfIntersection = 0;

		{
			for (size_t i=0; i <sizeof(QWORD)*8; i++)
				if (_QM(i) & Intersection)
					SizeOfIntersection++;
		}
		
		if (SizeOfIntersection > MaxSizeOfIntersection)
		{
			MaxSizeOfIntersection = SizeOfIntersection;
			Word.m_Word = Paradigm.GetWordForm(k);
			Word.SetFormGrammems( currGrammems );
		}
	};
	

};

static void GenerateCoordinatedNodes(CRusSemStructure& R, int RelNo, 	QWORD NewGrammems, 	QWORD OldGrammems)
{
	const CRusSemRelation& Rel = R.m_Relations[RelNo];
	vector<long> SubNodes;

	if (R.m_Nodes[Rel.m_TargetNodeNo].m_NodeType != MNA)
		SubNodes.push_back(Rel.m_TargetNodeNo);
	else
	{
		vector<long> SubRels;
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
			QWORD Grammems = (W.GetAllGrammems() & ~rAllGenders & ~rAllAnimative);
			Grammems |= rAllGenders & NewGrammems;
			Grammems |= rAllAnimative & NewGrammems;
			GetMaxSimilarWordForm(R,  W, Grammems);
		};
	};
}

static void GeneratePredicateBySubject(CRusSemStructure& R, int RelNo, 	QWORD NewGrammems, 	QWORD OldGrammems)
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
		QWORD Grammems = (W.GetAllGrammems() & ~rAllGenders & ~rAllNumbers);
		Grammems |= rAllGenders & NewGrammems;
		Grammems |= rAllNumbers & NewGrammems;
		GetMaxSimilarWordForm(R,  W, Grammems);
	}
	else
		if	(		( (rAllPersons & NewGrammems) != (rAllPersons & OldGrammems))
				||	( (rAllNumbers & NewGrammems) != (rAllNumbers & OldGrammems))
			)
		{
			QWORD Grammems = (W.GetAllGrammems() & ~rAllPersons & ~rAllNumbers);
			Grammems |= rAllPersons & NewGrammems;
			Grammems |= rAllNumbers & NewGrammems;
			GetMaxSimilarWordForm(R,  W, Grammems);

		};

};


static void GenerateNewWordFormAndAdjustCoordination (CRusSemStructure& R, int NodeNo, int WordNo)
{
	QWORD NewGrammems;
	QWORD OldGrammems;
	{
		CRusSemNode& Node = R.m_Nodes[NodeNo];
		OldGrammems = Node.m_Words[WordNo].GetAllGrammems();
		if (WordNo == Node.m_MainWordNo)
			OldGrammems = Node.GetGrammems();

		GetMaxSimilarWordForm(R,  Node.m_Words[WordNo], OldGrammems);

		NewGrammems = Node.m_Words[WordNo].GetAllGrammems();
		if (WordNo != Node.m_MainWordNo) return;
	}

	vector<long> Rels;
	R.GetOutcomingRelations(NodeNo, Rels);
	for (size_t i=0; i <Rels.size(); i++)
	{
		if (R.m_Relations[Rels[i]].m_SyntacticRelation == "ÏÐÈË_ÑÓÙ")
			GenerateCoordinatedNodes(R,Rels[i], NewGrammems, OldGrammems);

	};

	
	CRelSet RelSet = R.GetIncomingRelations(NodeNo, false);

	for (size_t i=0; i <RelSet.m_RelsCount; i++)
	{
		long RelNo = RelSet.m_Rels[i];
		if (R.m_Relations[RelNo].m_bDopRelation) continue;

		if (R.m_Relations[RelNo].m_SyntacticRelation == "ïîäë")
			GeneratePredicateBySubject(R,RelNo, NewGrammems, OldGrammems);
	};

};



void GenerateRecursive(CRusSemStructure& R, int NodeNo)
{

	vector<long> Rels;
	R.GetOutcomingRelations(NodeNo, Rels, false);
	for (size_t  i=0; i < Rels.size(); i++)
	{
		const CRusSemRelation& Rel = R.m_Relations[Rels[i]];
		GenerateRecursive(R,Rel.m_TargetNodeNo);
	};

	CRusSemNode& Node = R.m_Nodes[NodeNo];
	for (size_t i=0; i < Node.m_Words.size(); i++)
	{
		
		map<string,string>::const_iterator it = R.m_SynthLemmaToReplace.find(Node.m_Words[i].m_Lemma);
		if (it == R.m_SynthLemmaToReplace.end()) continue;
		Node.m_Words[i].m_Lemma = it->second;
		GenerateNewWordFormAndAdjustCoordination(R, NodeNo, i);
	};
};

