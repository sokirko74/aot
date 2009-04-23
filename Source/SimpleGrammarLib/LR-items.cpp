#include  "SimpleGrammarLib.h"

#include  "SimpleGrammar.h"
#include  "LR-items.h"

bool CLRItem::IsFinished() const
{
	return m_pRule->m_RightPart.m_Items.size() == m_DotSymbolNo;
};


int CLRItem::GetSymbolNo() const
{
	return m_pRule->m_RightPart.m_Items[m_DotSymbolNo];
};


void AddToItemSet(const CWorkGrammar* pWorkGrammar, set<CLRItem>& ItemSet, int SymbolNo) 
{
	for (CWRI it = pWorkGrammar->m_EncodedRules.begin(); it!= pWorkGrammar->m_EncodedRules.end(); it++)
		if (it->m_LeftPart == SymbolNo) 
		{
			CLRItem I;
			I.m_pRule = &(*it);
			I.m_DotSymbolNo = 0;
			ItemSet.insert(I);
		};
};



 //this function closes an item set:
 //if there is an item of the form A->v*Bw in an item set and  in the grammar there is a rule 
 //of form B->w' then  the item B->*w' should also be in the item set.

void CloseItemSet(const CWorkGrammar* pWorkGrammar, set<CLRItem>& ItemSet) 
{
	size_t SaveSize;
	set <int> AlreadyAdded;
	do 
	{
		SaveSize = ItemSet.size();
		for (set<CLRItem>::iterator it = ItemSet.begin(); it != ItemSet.end(); it++)
		if (!it->IsFinished())
		{
			int SymbolNo = it->GetSymbolNo();
			if	(		(pWorkGrammar->m_UniqueGrammarItems[SymbolNo].m_bMeta)
					&&	(AlreadyAdded.find(SymbolNo) == AlreadyAdded.end())
				)
			{
				AddToItemSet(pWorkGrammar, ItemSet, SymbolNo);
				AlreadyAdded.insert(SymbolNo);
			};
		};

	}
	while (SaveSize < ItemSet.size());
};



void PrintItemSet(const CWorkGrammar* pWorkGrammar, const set<CLRItem>& ItemSet) 
{
	for (set<CLRItem>::const_iterator it = ItemSet.begin(); it != ItemSet.end(); it++)
	{
		string s = pWorkGrammar->GetRuleStr(*it->m_pRule, it->m_DotSymbolNo);
		printf ("%s\n", s.c_str() ); 
	};

};


void CLRCollectionSet::Compute(const CWorkGrammar* pWorkGrammar)
{
	assert (pWorkGrammar != 0);

	m_GotoFunction.clear();
	m_ItemSets.clear();
	
	{	// constructing Item set0
		set<CLRItem> ItemSet0;
		AddToItemSet(pWorkGrammar, ItemSet0, pWorkGrammar->GetNewRoot());
		CloseItemSet(pWorkGrammar, ItemSet0);
		m_ItemSets.push_back(ItemSet0);
	}

	for (size_t i=0;  i<m_ItemSets.size(); i++)
	{
		//printf ("===== ItemSet N %i\n",i);
		//PrintItemSet(pWorkGrammar, m_ItemSets[i]);
		//  add a new state
		vector <CTableItem> NewState (pWorkGrammar->m_UniqueGrammarItems.size());
	
		//  going  through all symbols, except the last one which is a symbol for non-existing terminal 
		for (size_t SymbolNo = 0; SymbolNo<pWorkGrammar->m_UniqueGrammarItems.size(); SymbolNo++)
		{
			if (SymbolNo == pWorkGrammar->GetEndOfStreamSymbol()) continue;

			set<CLRItem> NewItemSet;
		
			for (set<CLRItem>::const_iterator it = m_ItemSets[i].begin(); it != m_ItemSets[i].end(); it++)
			if (!it->IsFinished())
			{
				int CurrSymbolNo = it->GetSymbolNo();
				if (SymbolNo == CurrSymbolNo)
				{
					CLRItem I = *it;
					I.m_DotSymbolNo++;
					NewItemSet.insert(I);
				};
			};
			
			if (!NewItemSet.empty())
			{
				CloseItemSet(pWorkGrammar, NewItemSet);
				int NewStateNo = -1;
				vector<set<CLRItem> >::iterator it = find(m_ItemSets.begin(), m_ItemSets.end(), NewItemSet);
				if (it != m_ItemSets.end())
					NewStateNo = it - m_ItemSets.begin();
				else
				{
					NewStateNo = m_ItemSets.size();
					m_ItemSets.push_back(NewItemSet);
				};
				CStateAndSymbol p;
				p.m_StateNo = i;
				p.m_SymbolNo = SymbolNo;
				m_GotoFunction[p] = NewStateNo;
			};

		};

	};
	
};

bool CLRCollectionSet::GetGotoValue(int SymbolNo, size_t StateNo, size_t& ResultStateNo) const
{
	CStateAndSymbol S;
	S.m_StateNo = StateNo;
	S.m_SymbolNo = SymbolNo;
	map< CStateAndSymbol, size_t>::const_iterator it = m_GotoFunction.find(S);
	if (it == m_GotoFunction.end()) return  false;
	ResultStateNo = it->second;
	return  true;

};


