#include "StdAfx.h"
#include "EngSemStructure.h"




string CEngSemStructure::GetPrep(const TCortege& cortege, DictTypeEnum type)
{
	int iNum = 0;
	string strPrep;
	strPrep.erase();

	if( cortege.m_DomItemNos[0] == -1 )
		return strPrep;

	string str = GetItemStr(cortege.m_DomItemNos[0], type);
	

	if( (iNum = str.find("+")) != -1 )
	{		
		strPrep = str.substr(0, iNum);
	}
	else
		if( cortege.m_DomItemNos[1] != -1 )
		{
			str = GetItemStr(cortege.m_DomItemNos[1], type);
			if( (iNum = str.find("+")) != -1 )
				strPrep = str.substr(0, iNum);
		}

	return strPrep;
}

//переводит русский предлог с помощью поля PREP
string CEngSemStructure::HasParticularPrepInField( CRossHolder* pRossDoc,int  iRusActant,CEngSemNode&  engActantm)
{
	assert( iRusActant != -1 );
	const CSemNode& rusActant = RusStr.GetNode(iRusActant);
	string strPrep;
	strPrep.erase();
	if( rusActant.m_MainWordNo == -1 )
		return strPrep;

	long UnitNo = rusActant.GetUnitNo();
	vector<TCortege> vectorCortege;

	pRossDoc->GetFieldValues("PREP", UnitNo, vectorCortege);

	for( int i = 0 ; i < vectorCortege.size() ; i++ )
	{

		CSignat& Signat = pRossDoc->GetRoss()->Fields[vectorCortege[i].m_FieldNo].m_Signats[vectorCortege[i].GetSignatNo()];
		if( Signat.Doms.size() != 3 )
			return strPrep;

		long DomNo = Signat.Doms[0];
		if( string(pRossDoc->GetRoss()->m_Domens[DomNo].DomStr) == "D_1" )
		{
			string strPattern = (const char*)pRossDoc->GetRoss()->GetDomItemStr(vectorCortege[i].m_DomItemNos[0]);
			long PrepNo;
			if( RusStr.CheckGroupBeginAndCase(strPattern, iRusActant, PrepNo) )
			{

				int prepItemNo = vectorCortege[i].m_DomItemNos[2];
				if( prepItemNo != -1)
					return (const char*)pRossDoc->GetRoss()->GetDomItemStr(prepItemNo);
			}
		}
	}
	return strPrep;
}




bool CEngSemStructure::Rule_TranslateRelWithPrepField( int iRusActant, long RelationNo, int iEngNode)
{
	CEngSemRelation& semEngRel  = m_Relations[RelationNo];
	CEngSemNode& engNode = m_Nodes[iEngNode];
	const CSemRelation& rusRel = *RusStr.GetRelation(semEngRel.m_RusRel);
	int iEngActant = semEngRel.m_TargetNodeNo;
	CEngSemNode& engActant = m_Nodes[iEngActant];	
	const CSemNode& rusActant = RusStr.GetNode(iRusActant);
	if (rusActant.GetType() == NoneRoss)  return false;
	CRossHolder* pRossDoc = GetRossHolder(rusActant.GetType());
	if( semEngRel.m_RusRel == -1 )
		return false;
	const CSemRelation& semRusRel = *RusStr.GetRelation(semEngRel.m_RusRel);	

	//если статье описан особый случай перевода этого предлога
	string strPrep = HasParticularPrepInField(pRossDoc, iRusActant, engActant);
	if( !strPrep.empty())
	{
		SetSimpleEngPrep(strPrep, -1, RelationNo);
		return true;
	}
	return false;
}


bool CEngSemStructure::Rule_TranslatePrepNounGroup( int iRusActant, long EngRelNo, const vector<TCortege>& GramCorteges, int iEngNode)
{
	CEngSemRelation& semEngRel  = m_Relations[EngRelNo];
	assert(iRusActant != -1 );
	CEngSemNode& engNode = m_Nodes[iEngNode];
	int iEngActant = semEngRel.m_TargetNodeNo;
	CEngSemNode& engActant = m_Nodes[iEngActant];	
	const CSemNode& rusActant = RusStr.GetNode(iRusActant);
	if( rusActant.m_MainWordNo == -1 )
		return false;
	
	if( !RusStr.IsNounOrMNANoun(iRusActant) 
		&& !rusActant.GetWord(rusActant.m_MainWordNo).HasPOS( PRONOUN_PREDK) 
		&& !rusActant.GetWord(rusActant.m_MainWordNo).HasPOS( PRONOUN)  
		&& !rusActant.GetWord(rusActant.m_MainWordNo).IsRusSubstPronounP()  
	  )
		return false;

	CRossHolder* pLocRossDoc = GetRossHolder(LocRoss);

	//если есть лексическая функция Loc у англ. слова
	CLexicalFunctionField LexicalFunct("","");
	CDictUnitInterp UnitInterp;
	UnitInterp.m_DictType = engActant.GetType();
	UnitInterp.m_UnitNo = engActant.GetUnitNo();
	if( HasThisLexFunc( "Loc", UnitInterp, LexicalFunct) )
	{
		SetSimpleEngPrep(LexicalFunct.m_Lemma,-1,EngRelNo);
		return true;
	}
//
	if( GramCorteges.size()==0 )
		return false;
//
	int iPrepPhr = NumPrepPhr(GramCorteges, engNode.GetType());
	//если нет PREP_PHR то берем первый попавшийся предлог( вообще лучше сначала свериться с Путринским переводом и выбрать 
	//тот предлого, который тот вернет( если ок один из тех, что в АОССе))
	if( iPrepPhr==-1 )
	{
		string strPrep = GetPrep(GramCorteges[0], engNode.GetType());

		if( !strPrep.empty())
		{
			SetSimpleEngPrep(strPrep,-1,EngRelNo);
			semEngRel.m_SynReal.m_Cortege = GramCorteges[0]; 
			return true;
		}
	}
	//если есть PREP_PHR, то выбрать ничего не можем - переводим синтаксисом	
	else
	{
		semEngRel.m_SynReal.m_Cortege = GramCorteges[iPrepPhr]; 
		return true;
	}

	return false;
}




int CEngSemStructure::NumPrepPhr(const vector<TCortege>& GramCorteges, DictTypeEnum type)
{
	if( type == NoneRoss)
		return false;
	for( int i=0; i<GramCorteges.size(); i++ )
	{
		const TCortege& cortege = GramCorteges[i];
		if( (GetItemStr(cortege.m_DomItemNos[0], type) == "PREP_PHR") && (cortege.m_DomItemNos[1] == -1) )
			return i;
	}

	return -1;	
}

