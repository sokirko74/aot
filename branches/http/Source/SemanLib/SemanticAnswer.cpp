#include "stdafx.h"
#include "SemanticRusStructure.h"
#include "HierarchyHolder.h"

bool AreEqualNodes(const CRusSemNode& AnswerNode, const CRusSemNode& QuestionNode)
{
	//  checking dictionary interpretations
	const CDictUnitInterp* i1 =  AnswerNode.GetInterp();
	const CDictUnitInterp* i2 =  QuestionNode.GetInterp();
	if (i1 && i2 &&  (*i1 == *i2))
		return true;

	//  checking word by word 
	if (AnswerNode.m_Words.size() != QuestionNode.m_Words.size()) return false;
	for (size_t i=0; i <AnswerNode.m_Words.size(); i++)
		if (AnswerNode.m_Words[i].m_Lemma != QuestionNode.m_Words[i].m_Lemma) 
			return false;

	return true;
};
	

bool  CheckTreeRecursive(const CRusSemStructure& AnswerStruct, int AnswerRoot, const CRusSemStructure& QuestionStruct, int QuestionRoot, int& AnswerNode)
{
	if (!AreEqualNodes(AnswerStruct.m_Nodes[AnswerRoot], QuestionStruct.m_Nodes[QuestionRoot]))
		return false;

	vector<long> QuestRels;
	QuestionStruct.GetOutcomingRelations(QuestionRoot, QuestRels, false);

	vector<long> AnswerRels;
	AnswerStruct.GetOutcomingRelations(AnswerRoot, AnswerRels, false);

	for (size_t  i=0; i < QuestRels.size(); i++)
	{
		const CRusSemRelation& QuestRel = QuestionStruct.m_Relations[QuestRels[i]];
		if (QuestRel.m_bDopRelation) continue;
		bool bQuestionWord = QuestionStruct.m_Nodes[QuestRel.m_TargetNodeNo].m_bQuestionWord;

		string QuestRelName( QuestRel.m_Valency.m_RelationStr.c_str() );
		vector<string> PossibleRels(1,  QuestRelName);
		if (bQuestionWord)
		{
			IncludeLowerInHierarchy (&QuestionStruct.m_pData->m_HierarchySemRelDoc, PossibleRels);
			IncludeHigherInHierarchy (&QuestionStruct.m_pData->m_HierarchySemRelDoc, PossibleRels);
		}

		int BestRelNo= AnswerRels.size();
		
		for (int AnsRelNo=0;  AnsRelNo < AnswerRels.size(); AnsRelNo++)
		{
			string AnswRel(AnswerStruct.m_Relations[AnswerRels[AnsRelNo]].m_Valency.m_RelationStr.c_str());
			// if QuestRelName is equal to AnswRel, then it the best case
			if (QuestRelName == AnswRel)
			{
				BestRelNo = AnsRelNo;
				break;
			};

			// if QuestRelName is equal to AnswRel using hierarchy, then it can be also true
			if (_find(PossibleRels, AnswRel))
			{
				BestRelNo = AnsRelNo;
			};
		};
		if (BestRelNo == AnswerRels.size()) return false;
		const CRusSemRelation& AnswRel = AnswerStruct.m_Relations[AnswerRels[BestRelNo]];

		if (bQuestionWord)
			AnswerNode = AnswRel.m_TargetNodeNo;
		else
		{
			if (!CheckTreeRecursive(AnswerStruct, AnswRel.m_TargetNodeNo,QuestionStruct, QuestRel.m_TargetNodeNo,AnswerNode))
				return false;
		};
		
	};
	return true;

}

int	FindAnswer(const CRusSemStructure& AnswerStr, const CRusSemStructure& QuestionStr)
{
	vector<long> roots;	
	QuestionStr.GetRoots(roots);
	if ((roots.size() > 1) ||  roots.empty())
	{
		//  too complicated
		return -1;
	};
	const CRusSemNode& N = QuestionStr.m_Nodes[roots[0]];
	if (N.m_MainWordNo == -1)
		return -1;

	string RootLemma = N.m_Words[N.m_MainWordNo].m_Lemma;
	int AnswerNodeNo = -1;
	for (size_t i=0; i < AnswerStr.m_Nodes.size(); i++)
		if (CheckTreeRecursive(AnswerStr, i, QuestionStr, roots[0], AnswerNodeNo))
		{
			assert (AnswerNodeNo != -1);
			return AnswerNodeNo;
		}
		else
			AnswerNodeNo = -1;

	return -1;
};

bool	CRusSemStructure::SemanticAnswer(string& Result, const vector<CRusSemStructure>&	SavedSentences) const
{
	try {
		for (size_t i=0; i < SavedSentences.size(); i++)
		{
			int NodeNo = FindAnswer(SavedSentences[i], *this);
			if (NodeNo == -1) continue;

			// generating answer
			CRusSemStructure NewStructure = SavedSentences[i];
			Result = NewStructure.RussianSynthesisOfNode(NodeNo);
			::SetSpacesAndRegisterInSentence(Result, morphRussian);
			return true;
		};
	}
	catch (...)
	{
		ErrorMessage ("An exception in CRusSemStructure::SemanticAnswer has occurred");
	};

	return false;
};
