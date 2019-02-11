// =====================================================
// Перевод терминов. Файл создан Алексеем Сокирко 20  ноября 2000 года.

#include "StdAfx.h"
#include "EngSemStructure.h"

// Выдает главное слово термина(слов в термине =атомарная группа).
// проходим по атомарным группам и ищем группу, в которую не входит ни одной стрелки
int get_termin_main_word(const CInnerTermin* termin, const CThesaurus* thes)
{
	long _modelnum = termin->m_ModelNo;
	if( _modelnum == -1 )
		return -1;
	const CInnerModel&	Model =	thes->m_Models[_modelnum];
	vector<long>	_relations;
	long _atomgroupscount = Model.m_AtomGroups.size();
	_relations.resize(_atomgroupscount);
	for (int i = 0; i < _atomgroupscount; i++)
		_relations[i] = 0;
	long _relationscount = Model.m_SynRelations.size();
	
	for (int i = 0; i <  _relationscount; i++)
	{
		long w  =  Model.m_SynRelations[i].m_Word2 - 1;
		_relations[w]++;
	};

	int res = -1;
	vector<long>::iterator	_iterator = find(_relations.begin(), _relations.end(), 0);
	if (_iterator != _relations.end())
		res = _iterator - _relations.begin();

	return res;
}



void CEngSemStructure::translate_abbr_termin_node(int MainNodeNo)
{
	if (!m_Nodes[MainNodeNo].m_ThesAbbrForm) return;
	int ThesId = m_Nodes[MainNodeNo].m_ThesaurusId;
	vector<int> EnglishTranslations;
	helper.GetThes(ThesId)->QueryEnglishTranslations(m_Nodes[MainNodeNo].m_TerminId, EnglishTranslations);
	if( EnglishTranslations.empty()) return;
	long eng_num = EnglishTranslations[0];
	
	const CInnerTermin& Term = helper.GetThes(ThesId)->m_Termins[eng_num];
	string  Form = Term.m_AbbrForm;
	if (Form == "")
		Form = Term.m_TerminStr;
	m_Nodes[MainNodeNo].m_Words[0].m_Word = Form;
	m_Nodes[MainNodeNo].m_Words[0].m_bDoNotChangeForm = true;  
	m_Nodes[MainNodeNo].m_Words[0].m_Poses = 1<<eNOUN;
};

bool  CEngSemStructure::set_multiword_termin(int NodeNo, const CInnerTermin* eng_termin, int ThesId)
{
	int eng_main_word_no = get_termin_main_word(eng_termin, helper.GetThes(ThesId));
	if( eng_main_word_no==-1 )
		return false;

	int MainPos = -1;
	long _modelnum = eng_termin->m_ModelNo;
	const CInnerModel& piModel = helper.GetThes(ThesId)->m_Models[_modelnum];
	long word_count = eng_termin->m_Items.size();

	// здесь будет накоплена результирующая строка
	string result;
	string AllLemmas;

	// переводим по атомарным группам
	for(int i=0; i<word_count; i++ )
	{
		const CInnerSynItem&  SynItem = helper.GetThes(ThesId)->m_SynItems[eng_termin->m_Items[i]];

		string lemma = SynItem.m_ItemStr;
		AllLemmas += " ";
		AllLemmas +=  lemma;
		const CAtomGroup& atom = piModel.m_AtomGroups[i];
		QWORD grammems =  atom.m_Grammems;
		BYTE  pos =  atom.m_PartOfSpeech;
		
		if( SynItem.m_Flags & siPlural ) 
		{
			grammems |=  _QM( ePlural);
		};
		
		if( i == eng_main_word_no )
			MainPos = pos;

		if (!m_Nodes[NodeNo].m_bProper)
		{
			if( (word_count == 1) 
				&& (    m_Nodes[NodeNo].m_Words[eng_main_word_no].HasOneGrammem(ePlural) 
				     || has_plural_rel(NodeNo)
				   )
			  ) 
			{
				grammems |= _QM( ePlural);
			}
			if( (word_count > 1) 
				&& (i == eng_main_word_no)
				&& (m_Nodes[NodeNo].RusNode != -1)
				&& (RusStr.GetNode(m_Nodes[NodeNo].RusNode).GetGrammems() & _QM( rPlural)) 
				&& (pos == eNOUN)
			  )
			{
				grammems |= _QM( ePlural);
			}
		};

		string form = lemma;
		try	{ // get  word form by thesaurus entry in the proper form
				vector<CFormInfo>  ParadigmCollection;
				m_pData->GetEngLemmatizer()->CreateParadigmCollection(true, lemma, true, false, ParadigmCollection);
				for(int i = 0; i < ParadigmCollection.size(); i++)
				{
					const CFormInfo& piParadigm = ParadigmCollection[i];


					string Ancode = piParadigm.GetAncode(0);
					if  (		(pos != 255) 
							&&	( m_pData->GetEngGramTab()->GetPartOfSpeech(Ancode.c_str()) != pos)
						)
						continue;	

					if	(		!m_Nodes[NodeNo].m_bProper 
							&&  (m_pData->GetEngGramTab()->GetAllGrammems(Ancode.c_str()) & _QM(eProper))
						)
						continue;


					form = helper.create_form_by_id(piParadigm.GetParadigmId(), grammems);
					break;
				};
			}
		catch(...)
		{
		}

		EngRusMakeLower(form); // по умолчанию в lowercase
		if( SynItem.m_Flags & siUpUp ) 
		{	
			EngRusMakeUpper(form);
		}

		if( SynItem.m_Flags & siUpLw ) 
			form[0] = toupper((unsigned char)form[0]);
		
		result += form;
		
		if( i<word_count-1 )
			result += " ";
	}

	m_Nodes[NodeNo].m_Words.resize(1);
	m_Nodes[NodeNo].m_Words[0].m_Word = result;  
	m_Nodes[NodeNo].m_Words[0].m_bDoNotChangeForm = true;  
	m_Nodes[NodeNo].m_Words[0].m_Poses = 1<<MainPos;
	Trim(AllLemmas);
	m_Nodes[NodeNo].m_Words[0].m_Lemma = AllLemmas;
	m_Nodes[NodeNo].m_MainWordNo = 0;
	return true;

};

bool CEngSemStructure::translate_termin_node(int MainNodeNo)
{
	if( !m_Nodes[MainNodeNo].IsThesNode() )
		return false;
	const CThesPartialInterp& colloc = m_Nodes[MainNodeNo].m_Colloc.GetThesInterp();
	if( !colloc.m_bMainWord )
		return false;
	if( !m_Nodes[MainNodeNo].IsWordContainer() )
		return false;
	
	SetNodeToDeleteFalse();

	for( int i=0; i<m_Nodes.size(); i++ )
	{
		if(m_Nodes[i].m_CollocId != m_Nodes[MainNodeNo].m_CollocId)
			continue;

		if( i != MainNodeNo )
		{
			m_Nodes[i].m_bToDelete = true;
		}
	}

	vector<int> EnglishTranslations;
	helper.GetThes(colloc.m_ThesaurusId)->QueryEnglishTranslations(colloc.m_TerminId, EnglishTranslations);
	if( EnglishTranslations.empty())
		return false;

	// get translation
	long eng_num = EnglishTranslations[0];
	const CInnerTermin& eng_termin= helper.GetThes(colloc.m_ThesaurusId)->m_Termins[eng_num];


	if (!set_multiword_termin (MainNodeNo, &eng_termin, colloc.m_ThesaurusId))
		return false;

	CEngSemNode& Main = m_Nodes[MainNodeNo];
	// счистаем, что уже это не открытое словосочетание, а просто узел,
	// чтобы не заходить в эту процедуру еще раз.
	Main.m_Colloc.m_Type = NoneType;

	// если артикль был поставлен по синтаксическим причинам, тогда приходится  отказатся от этого артикля,
	// поскольку синтаксическая структура можект претерпеть значительные изменения в термине,
	// например, "нарушение налоговых правил".-> "tax offence"
	if (!Main.m_ArticleCauseHistory.empty() ) 
		if (Main.m_ArticleCauseHistory.back() == DefArticleBecauseOfNominalSupplement)
		{
			m_Nodes[MainNodeNo].m_ArticleStr = "";
			m_Nodes[MainNodeNo].m_ArticleCauseHistory.push_back(OverwriteArticleForTerminNodes);
		};

	// все отношения, идущие к узлам, не вошедшим в эту таум-группу 
	// подвешиваем к главному узлу (узлы вошедшие в группу  помечены m_bToDelete),
	// потому что они потом будут удалены. 
	for (int i=0; i < m_Nodes.size(); i++)
	if  (m_Nodes[i].m_CollocId == m_Nodes[MainNodeNo].m_CollocId)
	{
		vector <long> Rels;
		GetOutcomingRelations(i,Rels);
	    for (int k=0; k < Rels.size(); k++)
	     if (!m_Nodes[m_Relations[Rels[k]].m_TargetNodeNo].m_bToDelete)
		 {
		 	 if  (i != MainNodeNo)
			 m_Relations[Rels[k]].m_SourceNodeNo = MainNodeNo;
 	// считаем, что все отношения не пришли из 
	// из русского графа.
	// это нужно,например, чтобы переводить "кредитовании промышленности и торговли",
	// где отноешение между "кредитовании" и "промышленности"  было ГЕНИТ_ИГ, и поэтому 
	// синтез ставил "of" после trade

			 m_Relations[Rels[k]].m_RusRel = -1;
		 };
	};
    //удаляем все узлы, вошедшие в группу
	DelNodesToDelete();

	return true;
}

