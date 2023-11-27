#include "StdAfx.h"
#include "EngSemStructure.h"




std::string CEngSemStructure::GetPrep(const TCortege& cortege, DictTypeEnum type)
{
	int iNum = 0;
	std::string strPrep;
	strPrep.erase();

	if( cortege.is_null(0))
		return strPrep;

	std::string str = GetItemStr(cortege.GetItem(0), type);
	

	if( (iNum = str.find("+")) != string::npos )
	{		
		strPrep = str.substr(0, iNum);
	}
	else
		if( !cortege.is_null(1))
		{
			str = GetItemStr(cortege.GetItem(1), type);
			if( (iNum = str.find("+")) != string::npos )
				strPrep = str.substr(0, iNum);
		}

	return strPrep;
}

//переводит русский предлог с помощью поля PREP
std::string CEngSemStructure::HasParticularPrepInField( CStructDictHolder* pRossDoc,int  iRusActant,CEngSemNode&  engActantm)
{
	assert( iRusActant != -1 );
	const CSemNode& rusActant = RusStr.GetNode(iRusActant);
	std::string strPrep;
	strPrep.erase();
	if( rusActant.m_MainWordNo == -1 )
		return strPrep;

	long UnitNo = rusActant.GetUnitNo();
	std::vector<TCortege> vectorCortege;

	pRossDoc->GetFieldValues("PREP", UnitNo, vectorCortege);

	for( int i = 0 ; i < vectorCortege.size() ; i++ )
	{

		const CSignat& Signat = pRossDoc->GetRoss()->GetSignat(vectorCortege[i]);
		if( Signat.GetDomsWoDelims().size() != 3 )
			return strPrep;

		long DomNo = Signat.GetDomsWoDelims()[0];
		if( pRossDoc->GetRoss()->m_Domens[DomNo].GetDomStr() == "D_1" )
		{
			std::string strPattern = pRossDoc->GetRoss()->GetDomItemStr(vectorCortege[i].GetItem(0));
			long PrepNo;
			if( RusStr.CheckGroupBeginAndCase(strPattern, iRusActant, PrepNo) )
			{

				dom_item_id_t prepItemNo = vectorCortege[i].GetItem(2);
				if( !is_null(prepItemNo))
					return pRossDoc->GetRoss()->GetDomItemStr(prepItemNo);
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
	CStructDictHolder* pRossDoc = GetRossHolder(rusActant.GetType());
	if( semEngRel.m_RusRel == -1 )
		return false;
	const CSemRelation& semRusRel = *RusStr.GetRelation(semEngRel.m_RusRel);	

	//если статье описан особый случай перевода этого предлога
	std::string strPrep = HasParticularPrepInField(pRossDoc, iRusActant, engActant);
	if( !strPrep.empty())
	{
		SetSimpleEngPrep(strPrep, -1, RelationNo);
		return true;
	}
	return false;
}


bool CEngSemStructure::Rule_TranslatePrepNounGroup( int iRusActant, long EngRelNo, const std::vector<TCortege>& GramCorteges, int iEngNode)
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

	CStructDictHolder* pLocRossDoc = GetRossHolder(LocRoss);

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
	auto iPrepPhr = NumPrepPhr(GramCorteges, engNode.GetType());
	//если нет PREP_PHR то берем первый попавшийся предлог( вообще лучше сначала свериться с Путринским переводом и выбрать 
	//тот предлого, который тот вернет( если ок один из тех, что в АОССе))
	if( iPrepPhr == nullptr )
	{
		std::string strPrep = GetPrep(GramCorteges[0], engNode.GetType());

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
		semEngRel.m_SynReal.m_Cortege = *iPrepPhr;
		return true;
	}

	return false;
}




const TCortege* CEngSemStructure::NumPrepPhr(const std::vector<TCortege>& GramCorteges, DictTypeEnum type) const
{
	if( type == NoneRoss)
		return nullptr;
	for(auto& c: GramCorteges)
	{
		if( (GetItemStr(c.GetItem(0), type) == "PREP_PHR") && c.is_null(1) )
			return &c;
	}
	return nullptr;	
}

