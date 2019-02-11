#include  "SimpleGrammarLib.h"
#include  "SimpleGrammar.h"



// building m_FirstSets.
// m_FirstSets[i] is a set of all terminals which can start a terminal sequence, 
// which can be produced from  nonterminal symbol  i
//==============
//  1. We first initialize the FIRST sets to the empty set
// 2. Then we process each grammar rule in the following way: if the right-hand side
// starts with a teminal  symbol. we add this symbol to the FIRST set  of the 
//  left-hand side, since it  can be the firts symbol of a sentential form 
// derived from the left side.  If the right-hand side starts with a non-terminal symbol
// we add all symbols of the present FIRST set of this non-terminal to the FIRST set
// of the left-hand side. These are all symbols that can be the first  terminal of
//  a sentential for derived from teh left-hand side.
//  3. The previuos  step is repeated until no more new symbols are added to any of the
// FIRST sets.
void CWorkGrammar::Build_FIRST_Set() 
{
	m_FirstSets.clear();
	m_FirstSets.resize(m_UniqueGrammarItems.size());
	bool bChanged = true;
	while (bChanged)
	{
		bChanged = false;
		for (CWRI it = m_EncodedRules.begin(); it!= m_EncodedRules.end(); it++)
		{
			const CWorkRule& R = (*it);
			int SymbolNo = R.m_RightPart.m_Items[0];
			if (m_UniqueGrammarItems[SymbolNo].m_bMeta)
			{
				const set<size_t>& s = m_FirstSets[SymbolNo];
				for (set<size_t>::const_iterator it = s.begin(); it != s.end(); it++)
					if (m_FirstSets[R.m_LeftPart].insert(*it).second)
						bChanged  = true;			
			}
			else
				if (m_FirstSets[R.m_LeftPart].insert(SymbolNo).second)
					bChanged  = true;			
		};
	};

	/*
	// print

	for (size_t SymbolNo=0; SymbolNo<m_UniqueGrammarItems.size(); SymbolNo++)
	if	(m_UniqueGrammarItems[SymbolNo].m_Type == siMeta)
	{
		printf ("FIRST(%s) = ",m_UniqueGrammarItems[SymbolNo].GetDumpString().c_str());
		const set<size_t>& s = m_FirstSets[SymbolNo];
		for (set<size_t>::const_iterator it = s.begin(); it != s.end(); it++)
			printf ("%s,",m_UniqueGrammarItems[*it].GetDumpString().c_str());
		printf ("\n");

	};*/
	
};

/*
	this procedure builds map First_k.  First_k[i] contains all possible sequence of terminals
	which can be a prefix of length PrefixLength or less of a string which is  derived from meta symbol i.
	The process of computing First_k() is described in many sources about Parsing and is similar
	to function CWorkGrammar::Build_FIRST_Set.
*/

void CWorkGrammar::Build_MAP_Node_To_FIRST_Set_k(size_t PrefixLength, map<size_t, CPrefixSet >& First_k)  const
{
	First_k.clear();
	bool bChanged = true;
	while (bChanged ) 
	{
		bChanged = false;
		for (CWRI it = m_EncodedRules.begin(); it!= m_EncodedRules.end(); it++)
		{
			const CWorkRule& R = (*it);
			string DumpRule = GetRuleStr(R);
			CPrefixSet ThisRuleResult;
			//  we should add an empty prefix
			ThisRuleResult.insert(CPrefix());

			for (size_t i=0; i < min (PrefixLength,R.m_RightPart.m_Items.size()); i++)
			{
				CPrefixSet Additions;
				int SymbolNo = R.m_RightPart.m_Items[i];
				if (m_UniqueGrammarItems[SymbolNo].m_bMeta)
					Additions = First_k[SymbolNo];
				else
					Additions.insert( CPrefix(1, SymbolNo) );

				if (Additions.empty())
				{
					ThisRuleResult.clear();
					break;
				};

				CPrefixSet NewRuleResult;

				for (CPrefixSet::const_iterator it =  Additions.begin(); it != Additions.end(); it++)
				{
					const CPrefix& Addition = *it;
					CPrefixSet R;
					for (CPrefixSet::const_iterator it1 =  ThisRuleResult.begin(); it1 != ThisRuleResult.end(); it1++)
					{
						const CPrefix& Prefix = *it1;
						if	(Prefix.size() < PrefixLength) 
						{
							CPrefix UpdatedPrefix = Prefix;

							int AddLen = min(Addition.size(), PrefixLength - Prefix.size());

							UpdatedPrefix.insert (UpdatedPrefix.end(), Addition.begin(), Addition.begin()+AddLen);

							assert (!UpdatedPrefix.empty());

							// if P.size() < PrefixLength then we can add to P some new 
							//  projections since P is a finished projection 
							assert (UpdatedPrefix.size() <= PrefixLength);
							
							R.insert(UpdatedPrefix);
						}
						else
							R.insert(Prefix);

					};
					NewRuleResult.insert(R.begin(), R.end());
				};
				ThisRuleResult = NewRuleResult;

			}; // end of the cycle through the right-hand part

			//  updating Firstk() for this rule 
			{ 
				CPrefixSet& s =  First_k[R.m_LeftPart];
				size_t SaveSize = s.size();
				s.insert (ThisRuleResult.begin(), ThisRuleResult.end());
				if (SaveSize != s.size())
					bChanged = true;
			}

		}


	};

};

//	This functions builds the slot CWorkGrammar::m_RootPrefixes.
//	This slot contains  all terminal sequences (length < PrefixLength)
//	which can start the root symbol of the grammar. 

void CWorkGrammar::BuildRootPrefixes(size_t PrefixLength) 
{
	map<size_t, CPrefixSet > First_k;
	Build_MAP_Node_To_FIRST_Set_k(PrefixLength, First_k);


	//  Building CWorkGrammar::m_RootPrefixes
	size_t RuleId = 1;
	for  (map<size_t, CPrefixSet >::const_iterator it=First_k.begin(); it != First_k.end(); it++)
	if (m_UniqueGrammarItems[it->first].m_bGrammarRoot)
	{
		for (CPrefixSet::const_iterator it1 =  it->second.begin(); it1 != it->second.end(); it1++)
		{
			CWorkRule R;
			R.m_LeftPart = it->first;
			R.m_RuleId = RuleId++;
			R.m_RightPart.m_Items = *it1;
			R.m_RightPart.m_SynMainItemNo = 0;
			R.m_RuleFeaturesNo = 0xffffffff;
			m_RootPrefixes.insert(R);
			//printf ("First_k -> %s\n",GetRuleStr(R).c_str() );
		};

	};
	
};



// building m_FollowSets.
// m_FollowSets[i] is a set of all terminals which can be after a terminal sequence, 
// which can be produced from  nonterminal symbol  i
void CWorkGrammar::Build_FOLLOW_Set() 
{
	//  FIRST sets should  be already generated
	assert (m_FirstSets.size () == m_UniqueGrammarItems.size());
	m_FollowSets.clear();
	m_FollowSets.resize(m_UniqueGrammarItems.size());

	

	bool bChanged = true;
	while (bChanged)
	{
		bChanged = false;
		for (CWRI it = m_EncodedRules.begin(); it!= m_EncodedRules.end(); it++)
		{
			const CWorkRule& R = (*it);
			for (size_t i=0; i+1 < R.m_RightPart.m_Items.size(); i++)
			{
				int SymbolNo = R.m_RightPart.m_Items[i+1];

				if (m_UniqueGrammarItems[SymbolNo].m_bMeta)
				{
					//  adding FIRST(SymbolNo) to  FOLLOW(R.m_RightPart[i])
					const set<size_t>& s = m_FirstSets[SymbolNo];
					for (set<size_t>::const_iterator it = s.begin(); it != s.end(); it++)
						if (m_FollowSets[R.m_RightPart.m_Items[i]].insert(*it).second)
							bChanged  = true;			
				}
				else
					//  adding SymbolNo itself to  FOLLOW(SymbolNo)
					if (m_FollowSets[R.m_RightPart.m_Items[i]].insert(SymbolNo).second)
						bChanged  = true;			
			};

			int SymbolNo = R.m_RightPart.m_Items.back();
			if (m_UniqueGrammarItems[SymbolNo].m_bMeta) 
			{
				//  adding FOLLOW(R.m_LeftPart) to  FOLLOW(SymbolNo)
				const set<size_t>& s = m_FollowSets[R.m_LeftPart];
				for (set<size_t>::const_iterator it = s.begin(); it != s.end(); it++)
					if (m_FollowSets[SymbolNo].insert(*it).second)
						bChanged  = true;			
			};
		};
	};



	//  print
	//for (size_t SymbolNo=0; SymbolNo<m_UniqueGrammarItems.size(); SymbolNo++)
	//if	(m_UniqueGrammarItems[SymbolNo].m_bMeta)
	//{
	//	printf ("FOLLOW(%s) = ",m_UniqueGrammarItems[SymbolNo].GetDumpString().c_str());
	//	const set<size_t>& s = m_FollowSets[SymbolNo];
	//	for (set<size_t>::const_iterator it = s.begin(); it != s.end(); it++)
	//		printf ("%s,",m_UniqueGrammarItems[*it].GetDumpString().c_str());
	//	printf ("\n");

	//};
	
 };


