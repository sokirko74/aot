#include  "SimpleGrammarLib.h"
#include  "GLRTable.h"
#include  "LR-items.h"
#include  "SimpleGrammar.h"
#include "../common/bserialize.h"



void	CGLRRuleInfo::Init(const CWorkRule* p)
{
	m_RuleId = p->m_RuleId;
	m_LeftPart = p->m_LeftPart;
	m_RuleLength = p->m_RightPart.m_Items.size();
	m_SynMainItemNo = p->m_RightPart.m_SynMainItemNo;
	m_RuleFeaturesNo = p->m_RuleFeaturesNo;
};

CGLRTable::CGLRTable()
{
	m_pWorkGrammar = 0;

};


size_t CGLRTable::GetRuleInfo (const CWorkRule* R) const
{
	CGLRRuleInfo I;
	I.m_RuleId = R->m_RuleId;
	vector<CGLRRuleInfo>::const_iterator it = lower_bound(m_RuleInfos.begin(), m_RuleInfos.end(), I);
	assert (it != m_RuleInfos.end());
	assert (it->m_RuleId == R->m_RuleId);
	return it - m_RuleInfos.begin();


};
/*
Constructing the SLR parsing table
 * Compute the canonical LR collection set of LR(0) items for grammr G
   let it be C = {I0, I1,...,In}
 * For terminal a, if A->X.aY in Ii anf goto(Ii,a) = Ij, then set action[i,a] to shift j
 * if A->X. is in Ii and A != S',  for all terminals  a in Follow(A) set action [i,a] 
   to  reduce A->X (S' is a new specially  inserted root of the grammar)
 * if S'->S. is in Ii, then set action[i,$]=accept (this is not implemented because we use 
   this grammar for longest match search)
 * For non-terminal symbol A, if goto(Ii,A) = Ij, then set goto(i,A)=j
 * set all other table entries to "error"

 Since we should use this table for GLR, we should not stop when 
 a conflict occurrs, to the contrary we should proceed collection all possible 
 actions in one table  cell.
*/
bool CGLRTable::BuildGLRTable()
{
	assert (m_pWorkGrammar != 0);

	// initializing m_RuleInfos, in order to be independent from class CWorkGrammar
	// while  parsing
	m_RuleInfos.clear();
	for (set<CWorkRule>::const_iterator it = m_pWorkGrammar->m_EncodedRules.begin(); it!=m_pWorkGrammar->m_EncodedRules.end(); it++)
	{
		CGLRRuleInfo I;
		I.Init(&*it);
		m_RuleInfos.push_back(I);
	};
	sort(m_RuleInfos.begin(), m_RuleInfos.end());
	// creating a canonical LR(0) index item set collection
    
	CLRCollectionSet S;
	S.Compute(m_pWorkGrammar);

	m_BuildTable.clear();

	// building m_Table by  CLRCollectionSet
	for (size_t i=0;  i<S.m_ItemSets.size(); i++)
	{
		const set<CLRItem>& ItemSet = S.m_ItemSets[i];

		// TableLine is a new table line in SLR table, which should  be added to the end of the table
		vector < CSLRCellBuild > TableLine;

		// we resize it to contain all symbols 
		TableLine.resize(m_pWorkGrammar->m_UniqueGrammarItems.size());

		// initializing shift actions and reduce actions
		for (set<CLRItem>::const_iterator it = ItemSet.begin(); it != ItemSet.end(); it++)
		{
			// "For terminal a, if A->X.aY in Ii and goto(Ii,a) = Ij, then set action[i,a] to shift j"
			if	(		!it->IsFinished()
					&&	!m_pWorkGrammar->m_UniqueGrammarItems[it->GetSymbolNo()].m_bMeta 
				)
			{
				int SymbolNo = it->GetSymbolNo();
				size_t NextState;
				if (S.GetGotoValue(SymbolNo, i, NextState))
				{
					if (TableLine[SymbolNo].m_GotoLine != NextState)
					{
						assert ( TableLine[SymbolNo].m_GotoLine == -1 );
						assert ( !TableLine[SymbolNo].m_bShift );
						TableLine[SymbolNo].m_bShift = true;
						TableLine[SymbolNo].m_GotoLine = NextState;
					};
				};

			};
			// if A->X. is in Ii and A != S',  for all terminals  a in Follow(A) set action [i,a] 
			// to  reduce A->X 
			if	(		it->IsFinished() 
						// "and A != S'"
						&&	it->m_pRule->m_LeftPart != m_pWorkGrammar->GetNewRoot()
				)
			{				
				const set<size_t>& Follow = m_pWorkGrammar->m_FollowSets[it->m_pRule->m_LeftPart];
				for (set<size_t>::const_iterator it1 = Follow.begin(); it1 != Follow.end(); it1++)
				{
					int SymbolNo = *it1;
					TableLine[SymbolNo].m_ReduceRules.push_back( GetRuleInfo(it->m_pRule) );
				};
			};
		};
		//  going  through non-terminal symbols, initializing GOTO table  for non terminals
		for (size_t SymbolNo = 0; SymbolNo<m_pWorkGrammar->m_UniqueGrammarItems.size(); SymbolNo++)
		if	(m_pWorkGrammar->m_UniqueGrammarItems[SymbolNo].m_bMeta)
		{
			size_t NextState;

			if (!S.GetGotoValue(SymbolNo,i, NextState)) continue;

			assert (TableLine[SymbolNo].m_GotoLine == -1);

			TableLine[SymbolNo].m_GotoLine =  NextState;
			

		};

		m_BuildTable.push_back(TableLine);	
		
	};

	return true;
};

void CGLRTable::ConvertBuildCellsToWordCells()
{
	m_WorkTable.clear();
	m_ReduceRuleSets.clear();
	for (size_t LineNo=0; LineNo < m_BuildTable.size(); LineNo++)
	{
		for (size_t i=0; i < m_BuildTable[LineNo].size(); i++)
		{
			const CSLRCellBuild& OldCell =  m_BuildTable[LineNo][i];
			CSLRCellWork NewCell;
			NewCell.m_bShift = OldCell.m_bShift;
			NewCell.m_GotoLine = OldCell.m_GotoLine;
			NewCell.m_ReduceRulesSetStart = m_ReduceRuleSets.size();
			m_ReduceRuleSets.insert(m_ReduceRuleSets.end(), OldCell.m_ReduceRules.begin(), OldCell.m_ReduceRules.end());
			m_WorkTable.push_back(NewCell);
		};
	};
};

inline size_t get_size_in_bytes (const CGLRRuleInfo& t)
{
	return		get_size_in_bytes(t.m_RuleId) 
			+	get_size_in_bytes(t.m_RuleLength) 
			+	get_size_in_bytes(t.m_LeftPart) 
			+	get_size_in_bytes(t.m_SynMainItemNo)
			+   get_size_in_bytes(t.m_RuleFeaturesNo);
			
};

inline size_t save_to_bytes(const CGLRRuleInfo& t, BYTE* buf)
{
	buf += save_to_bytes(t.m_RuleId, buf);
	buf += save_to_bytes(t.m_RuleLength, buf);
	buf += save_to_bytes(t.m_LeftPart, buf);
	buf += save_to_bytes(t.m_SynMainItemNo, buf);
	buf += save_to_bytes(t.m_RuleFeaturesNo, buf);
	return get_size_in_bytes(t);
};

inline size_t restore_from_bytes(CGLRRuleInfo& t, const BYTE* buf)
{
	buf += restore_from_bytes(t.m_RuleId, buf);
	buf += restore_from_bytes(t.m_RuleLength, buf);
	buf += restore_from_bytes(t.m_LeftPart, buf);
	buf += restore_from_bytes(t.m_SynMainItemNo, buf);
	buf += restore_from_bytes(t.m_RuleFeaturesNo, buf);
	
	return get_size_in_bytes(t);
};

inline size_t get_size_in_bytes (const CSLRCellWork& t)
{
	return		1 // get_size_in_bytes(t.m_bShift) 
			+	get_size_in_bytes(t.m_ReduceRulesSetStart) 
			+	get_size_in_bytes(t.m_GotoLine) 
;
		
};

inline size_t save_to_bytes(const CSLRCellWork& t, BYTE* buf)
{
	BYTE b= t.m_bShift? 1:0;
	buf += save_to_bytes(b, buf);
	buf += save_to_bytes(t.m_ReduceRulesSetStart, buf);
	buf += save_to_bytes(t.m_GotoLine, buf);
	return get_size_in_bytes(t);
};

inline size_t restore_from_bytes(CSLRCellWork& t, const BYTE* buf)
{
	BYTE b;
	buf += restore_from_bytes(b, buf);
	t.m_bShift = (b==1);
	buf += restore_from_bytes(t.m_ReduceRulesSetStart, buf);
	buf += restore_from_bytes(t.m_GotoLine, buf);
	
	return get_size_in_bytes(t);
};



bool CGLRTable::ConvertAndSaveGLRTable(string FileName)
{
	ConvertBuildCellsToWordCells();

	FILE* fp = fopen (FileName.c_str(), "wb");
	if (!fp) 
	{
		ErrorMessage (Format("Cannot write to %s",FileName.c_str()));
		return false;
	};
	fprintf (fp, "%i\n", m_RuleInfos.size());
	WriteVectorInner(fp, m_RuleInfos);

	fprintf (fp, "%i\n", m_WorkTable.size());
	WriteVectorInner(fp, m_WorkTable);

	fprintf (fp, "%i\n", m_ReduceRuleSets.size());
	WriteVectorInner(fp, m_ReduceRuleSets);

	fclose(fp);


	return true;
};

const CSLRCellWork& CGLRTable::GetCell(size_t  StateNo, size_t SymbolNo)  const
{
	return m_WorkTable[StateNo*m_WorkLineSize+SymbolNo];
};

DWORD CGLRTable::GetReduceRuleEnd(size_t  StateNo, size_t SymbolNo)  const
{
	if (StateNo*m_WorkLineSize + SymbolNo + 1 == m_WorkTable.size())
		return m_ReduceRuleSets.size();
	else
		return m_WorkTable[StateNo*m_WorkLineSize+SymbolNo+1].m_ReduceRulesSetStart;
};


bool CGLRTable::LoadGLRTable(string FileName) 
{
	if (!m_pWorkGrammar)
		return false;
	FILE* fp = fopen (FileName.c_str(), "rb");
	if (!fp) 
	{
		ErrorMessage (Format("Cannot read to %s",FileName.c_str()));
		return false;
	};
	size_t Count;
	char buffer[512];

	if (!fgets(buffer, 512, fp)) return false;
	if (sscanf (buffer, "%i\n", &Count) != 1) return false;
	if (!Count) return false;
	m_RuleInfos.clear();
	ReadVectorInner(fp, m_RuleInfos, Count);


	if (!fgets(buffer, 512, fp)) return false;
	if (sscanf (buffer, "%i\n", &Count) != 1) return false;
	if (!Count) return false;
	m_WorkLineSize = m_pWorkGrammar->m_UniqueGrammarItems.size();
	if (m_WorkLineSize == 0)
	{
		ErrorMessage ("The grammar should be first initialized");
		return false;
	};

	if ((Count %  m_WorkLineSize) != 0)
	{
		ErrorMessage ("The number of lines in the parse table doesn't correspond to grammar symbols number");
		return false;
	};
	ReadVectorInner(fp, m_WorkTable, Count);
	
	if (!fgets(buffer, 512, fp)) return false;
	if (sscanf (buffer, "%i\n", &Count) != 1) return false;
	ReadVectorInner(fp, m_ReduceRuleSets, Count);

	fclose(fp);

	return true;
};

void CGLRTable::PrintGLRTable(FILE* fp) const
{
	fprintf (fp, "%2s"," ");
	for (size_t i=0; i<m_pWorkGrammar->m_UniqueGrammarItems.size(); i++)
	{
		fprintf (fp, "%9s ",m_pWorkGrammar->m_UniqueGrammarItems[i].m_ItemStrId.c_str());
	};
	fprintf (fp, "\n");
	for (size_t i=0; i<m_BuildTable.size(); i++)
	{
		assert  (m_pWorkGrammar->m_UniqueGrammarItems.size() == m_BuildTable[i].size());

		fprintf (fp, "%2i ", i);
		
		for (size_t SymbolNo=0; SymbolNo < m_BuildTable[i].size(); SymbolNo++)
		{
			string s = " ";
			const CSLRCellBuild& I = m_BuildTable[i][SymbolNo];
			if (I.m_bShift )
				s += "sh,";

			for (size_t k=0; k<I.m_ReduceRules.size(); k++)
			{
				const CGLRRuleInfo& Info = m_RuleInfos[I.m_ReduceRules[k]];
				s += Format("r%i,", Info.m_RuleId).c_str();
			};
			
			if (s != " ")
				s = s.substr(1, s.length()-2);

			if (I.m_GotoLine != -1)
				s += Format(",g%i", I.m_GotoLine);

			fprintf (fp, "%9s ",s.c_str());
		};
		fprintf (fp, "\n");

	};
	

};

/*
bool CGLRTable::SaveOldGLRTable(string FileName) const
{
	FILE* fp = fopen (FileName.c_str(), "wb");
	if (!fp) 
	{
		ErrorMessage (Format("Cannot write to %s",FileName.c_str()));
		return false;
	};
	fprintf (fp, "%i\n", m_RuleInfos.size());
	for (size_t i=0; i<m_RuleInfos.size(); i++)
	{
		const CGLRRuleInfo& R = m_RuleInfos[i];
		fprintf (fp,"%i %i %i %i %i\n", R.m_RuleId, R.m_LeftPart,R.m_RuleLength,R.m_SynMainItemNo, R.m_RuleFeaturesNo);
	};
	fprintf (fp, "%i %i\n", m_WorkTable.size()/m_pWorkGrammar->m_UniqueGrammarItems.size(), m_pWorkGrammar->m_UniqueGrammarItems.size());
	for (size_t i=0; i<m_WorkTable.size(); i++)
	{
		const CSLRCellWork& C = m_WorkTable[i];
		size_t end = i+1 == m_WorkTable.size() ? m_ReduceRuleSets.size(): m_WorkTable[i+1].m_ReduceRulesSetStart;
		fprintf (fp, "%i %i %i", C.m_bShift ? 1 : 0, C.m_GotoLine, end - C.m_ReduceRulesSetStart);
		for (size_t j=C.m_ReduceRulesSetStart; j< end; j++)
			fprintf (fp, " %i", m_ReduceRuleSets[j]);
		fprintf (fp, "\n");
		
	};
	fclose(fp);


	return true;
};
*/
