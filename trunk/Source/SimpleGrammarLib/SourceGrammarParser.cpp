// This module contains all functions which should parse and process 
// source of the grammar. The main function   of this module is BuildWorkGrammar,
// which contains all necssary calls to build a WorkGrammar from a source file.

#include "SimpleGrammarLib.h"
#include "SimpleGrammar.h"
#include "ChunkParser.h"
extern string CurrentSourceFileName;


bool BuildGrammarItem(const CChunkNode* pNode, const CAgramtab* GramTab, MorphLanguageEnum Language, CGrammarItem& I, string& ErrorStr )
{
	I.InitGrammarItem();
	I.m_bMeta = (pNode->m_AtomicName != "TOKEN");
	I.m_Source = 	pNode->m_AtomicName;
	I.m_ItemStrId = 	pNode->m_AtomicName;
	
	if (pNode->m_pAttributes)
	{
		for (list<CNodeAttribute*>::const_iterator it = pNode->m_pAttributes->m_Items.begin(); it != pNode->m_pAttributes->m_Items.end(); it++)
		{
			const CNodeAttribute* A = (*it);
			if (!I.AddAttribute(A->m_Name,A->m_Value, Language, ErrorStr))
			{
				return false;
			};
		};

	};

	if (!I.m_MorphPattern.Init(GramTab, ErrorStr)) return false;

	return true;	
};

bool AttributesToFeatures (CWorkGrammar& WorkGrammar, const size_t CurrentItemNo, CGrammarItem& I, vector<CRuleFeature>& Features, string& ErrorStr)
{
	// convert attribute "grm" to a feature "has_grammem"
	if (		(I.m_bMeta)  
			&&	!I.m_MorphPattern.m_GrmAttribute.empty()
		)
	{
		CRuleFeature F;
		{
			CAttribAndId A;
			A.m_AttribName = "grm";
			A.m_Id = CurrentItemNo;
			F.m_RightItems.push_back(A);
		}
		{
			CAttribAndId A;
			A.m_MorphPattern.CopyFrom(I.m_MorphPattern);
			F.m_RightItems.push_back(A);
		};
		F.m_Type = foeCheck;
		bool b = F.InitFuncName("has_grammem");
		if (!b)
		{
			assert (b);
			ErrorStr = "Cannot convert \"has_grammem\" to a feature function name"; 
			return false;
		};
		I.m_MorphPattern.SetNull();

		Features.push_back(F);
	};

	// convert attribute "det" to a feature is_true or is_false
	if (		(I.m_bMeta)  
			&&	(I.m_Attributes.find("det") != I.m_Attributes.end())
		)
	{
		CRuleFeature F;
		{
			CAttribAndId A;
			A.m_AttribName = "det";
			A.m_Id = CurrentItemNo;
			F.m_RightItems.push_back(A);
		}
		F.m_Type = foeCheck;
		map<string, string>::iterator it  = I.m_Attributes.find("det");
		assert (it != I.m_Attributes.end());
		bool b = F.InitFuncName(it->second);
		if (!b)
		{
			assert (b);
			ErrorStr = Format("Cannot convert %s to a feature function name", it->second.c_str()); 
			return false;
		};
		I.m_Attributes.erase(it);

		Features.push_back(F);
	};

	// convert attribute "has_one_grm" to a feature "has_one_grm"
	if	(		(I.m_bMeta)  
			&&	(I.m_Attributes.find("has_one_grm") != I.m_Attributes.end())
		)
	{
		map<string, string>::iterator it  = I.m_Attributes.find("has_one_grm");
		assert (it != I.m_Attributes.end());
		CRuleFeature F;
		{
			CAttribAndId A;
			A.m_AttribName = "grm";
			A.m_Id = CurrentItemNo;
			F.m_RightItems.push_back(A);
		}
		{
			CAttribAndId A;
			A.m_MorphPattern.m_GrmAttribute =  it->second;
			if (!A.m_MorphPattern.Init(WorkGrammar.m_pGramTab, ErrorStr))
				return false;
			F.m_RightItems.push_back(A);
		};
		F.m_Type = foeCheck;
		bool b = F.InitFuncName("has_one_grm");
		if (!b)
		{
			assert (b);
			ErrorStr = Format("Cannot convert \"has_one_grm\" to a feature function name"); 
			return false;
		};
		I.m_Attributes.erase(it);

		Features.push_back(F);

	};

	// convert attribute "atomic" to a feature "is_true"
	if	(		(I.m_bMeta)  
			&&	(I.m_Attributes.find("atomic") != I.m_Attributes.end())
		)
	{
		map<string, string>::iterator it  = I.m_Attributes.find("atomic");
		CRuleFeature F;
		{
			CAttribAndId A;
			A.m_AttribName = "size";
			A.m_Id = CurrentItemNo;
			F.m_RightItems.push_back(A);
		}
		F.m_Type = foeCheck;
		bool b = F.InitFuncName("is_atomic");
		if (!b)
		{
			assert (b);
			ErrorStr = Format("Cannot convert \"atomic\" to a feature function name"); 
			return false;
		};
		I.m_Attributes.erase(it);

		Features.push_back(F);

	};


	return true;
};



bool GetRightPartRecursive(const CChunkSequenceNode* pNode, CWorkGrammar& WorkGrammar, CWorkRule& NewRule, string& ErrorStr )
{
	const vector<CChunkNode*>& V = pNode->GetChildren();	

	for (size_t i=0; i<V.size(); i++)
	{
		const CChunkNode* P = V[i];
		if (P->m_AtomicName.empty())
		{
			if (!GetRightPartRecursive((const CChunkSequenceNode*)P, WorkGrammar, NewRule, ErrorStr ))
				return false;
		}
		else
		{
			CGrammarItem I;
			if (!BuildGrammarItem(P, WorkGrammar.m_pGramTab, WorkGrammar.m_Language, I, ErrorStr))
				return false;

			if (!AttributesToFeatures(	WorkGrammar, 
										NewRule.m_RightPart.m_Items.size()+1, 
										I,
										WorkGrammar.m_RuleFeatures[NewRule.m_RuleFeaturesNo], 
										ErrorStr))
				return false;


			size_t id = WorkGrammar.GetItemId(I);

			if (I.m_bSynMain)
				NewRule.m_RightPart.m_SynMainItemNo = NewRule.m_RightPart.m_Items.size();

			NewRule.m_RightPart.m_Items.push_back(id);

			WorkGrammar.m_UniqueGrammarItems[id].m_bGrammarRoot = false;
		};
	};
	return true;

};

bool ConvertToWorkGrammar(const CChunkGrammar& ChunkGrammar, CWorkGrammar& WorkGrammar)
{
	WorkGrammar.m_EncodedRules.clear();
	WorkGrammar.m_UniqueGrammarItems.clear();
	WorkGrammar.m_RuleFeatures.clear();

	int RuleId = 1;
	
	for (list<CChunkRule*>::const_iterator it = ChunkGrammar.m_Rules.begin(); it != ChunkGrammar.m_Rules.end(); it++)
	{
		
		const CChunkRule* Rule = *it;
		CGrammarItem I;
		string ErrorStr;
		CurrentSourceFileName = Rule->m_SourceFileName;
		
		if (!BuildGrammarItem(Rule->m_pLeftHand, WorkGrammar.m_pGramTab, WorkGrammar.m_Language, I, ErrorStr))
		{
			fprintf (stderr, "%s at line %i in file %s\n", ErrorStr.c_str(), Rule->m_SourceLineNo, Rule->m_SourceFileName.c_str());
			return false;
		};
		size_t LeftPartId = WorkGrammar.GetItemId(I);

		for (list<CChunkSequenceNode*>::const_iterator it1 = Rule->m_pRightHand.begin(); it1 != Rule->m_pRightHand.end(); it1++)
		{
			WorkGrammar.m_RuleFeatures.push_back(Rule->m_FeatureExprs);
			CWorkRule WorkRule;
			WorkRule.m_LeftPart = LeftPartId;
			WorkRule.m_RuleId = RuleId;
			WorkRule.m_RightPart.m_SynMainItemNo = 0xffffffff;
			WorkRule.m_RuleFeaturesNo = WorkGrammar.m_RuleFeatures.size() - 1;
			if (!GetRightPartRecursive(*it1, WorkGrammar, WorkRule, ErrorStr))
			{
				fprintf (stderr, "%s at line %i in file %s\n", ErrorStr.c_str(), Rule->m_SourceLineNo, Rule->m_SourceFileName.c_str());
				return false;
			};

			if (WorkRule.m_RightPart.m_SynMainItemNo == 0xffffffff)
				WorkRule.m_RightPart.m_SynMainItemNo = 0;



			if (WorkGrammar.m_EncodedRules.insert(WorkRule).second)
				RuleId++;
		};

	};

	return true;
};


bool BuildWorkGrammar(CWorkGrammar& WorkGrammar, bool bPrintRules)
{
	fprintf (stderr, "reading from the source file %s\n",WorkGrammar.m_SourceGrammarFile.c_str());
	CChunkParser Parser;
	Parser.m_pGramTab = WorkGrammar.m_pGramTab;
	bool bResult = Parser.ParseGrammarInFile(WorkGrammar.m_SourceGrammarFile);
	if (!bResult)
	{
		fprintf(stderr, "cannot parse %s\n", WorkGrammar.m_SourceGrammarFile.c_str());
		return false;
	};
	fprintf(stderr, "Number of rules = %i\n", Parser.m_ChunkGrammar.m_Rules.size());

	
	if (!ConvertToWorkGrammar(Parser.m_ChunkGrammar, WorkGrammar))
		return false;

	
	size_t CountOfRoots = WorkGrammar.GetCountOfRoots();
	if (CountOfRoots == 0)
	{
		ErrorMessage("No roots found in simple grammar; exit!\n");
		return false;
	};

	if (bPrintRules)
		WorkGrammar.Print();

	string ErrorStr;
	fprintf (stderr, "creating all variants of nodes from nodes with work attributes\n");
	if (!WorkGrammar.CreateNodesForNodesWithWorkAttributes(ErrorStr))
	{
		ErrorMessage(ErrorStr);
		return false;
	};

	fprintf(stderr, "Number of rules = %i\n", WorkGrammar.m_EncodedRules.size());

	fprintf (stderr, "checking coherence\n");
	if (!WorkGrammar.CheckCoherence())
		return false;

	//printf ("Found roots: %i\n", CountOfRoots);

	
	if (bPrintRules)
	{
		WorkGrammar.Print();
	};
	fprintf (stderr, "validation\n");
	bool bValid = WorkGrammar.IsValid();
	return bValid;
};
