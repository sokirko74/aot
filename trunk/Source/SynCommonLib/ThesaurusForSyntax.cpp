// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "stdafx.h"
#include "ThesaurusForSyntax.h"
#include "SyntaxInit.h"
#include "../RmlThesLib/RmlThesConsts.h"
#include "../AgramtabLib/EngGramTab.h"



CThesaurusForSyntax::CThesaurusForSyntax(CSyntaxOpt* Opt) 
{ 
	m_pSyntaxOptions = Opt;
	m_pEngGramTab  = new CEngGramTab;
	m_pEngGramTab->LoadFromRegistry();
};

CThesaurusForSyntax::~CThesaurusForSyntax() 
{ 
	if (m_pEngGramTab) 
		delete m_pEngGramTab;
};


CThesaurus* CThesaurusForSyntax::LoadThesaurus(const char* ThesName) const
{
	CThesaurus* T = new CThesaurus;
	if (!T) return false;
	if (!T->SetDatabase(ThesName)) return false;
	assert (GetOpt()->GetOborDic() != 0);
	T->m_MainLanguage = GetOpt()->m_Language;
	T->m_pOborDic = GetOpt()->GetOborDic();
	assert (GetOpt()->GetGramTab() != 0);
	T->m_pMainGramTab = GetOpt()->GetGramTab();
	assert (m_pEngGramTab != 0);
	T->m_pEngGramTab = m_pEngGramTab;
	if (!T->ReadThesaurusFromDisk()) return false;
	if (!T->ReadRelationsFromDisk()) return false;

	return T;
};

bool CThesaurusForSyntax::ReadThesaurusForSyntax(const char* strDBName,  const CThesaurus* Thes, StringVector& p_vectorAccost)
{
	try
	{

		
		EThesType eThesType;
		eThesType = GetThesTypeByStr(strDBName);
		if( eThesType == NoneThes)
			return false;

		

		
		//для форматки  format_for_rank_surname (ДОЛЖ-ФИО) нужно 
		//прочитать перечент всех должностей из общего тезауруса
		
		if ( !strcmp(strDBName, "RML_THES_OMNI") )
		{
			try 
			{
				string s_accost = "PROF";
				vector<int> Res;
				Thes->QueryLowerTermins(s_accost.c_str(), morphRussian, Res);
				for (int i=0; i <Res.size(); i++)
				{
					string TerminStr =  Thes->m_Termins[Res[i]].m_TerminStr;
					RmlMakeLower(	TerminStr, GetOpt()->m_Language);                    
					p_vectorAccost.push_back(TerminStr);
				};
				sort( p_vectorAccost.begin(), p_vectorAccost.end() );
			}
			catch(...)
			{
				ErrorMessage("SynAn", "Probably, RMLTHESLib::QueryLowerTermins function failed!");
			}
		}
		//nim

		bool bRes;

		
		bRes = ReadModels(*Thes, eThesType);
		if( !bRes )
		{
			ErrorMessage (Format ("Cannot read models for %s", Thes->m_Name.c_str()));
			return false;
		};
		
		bRes = ReadTermins(Thes, eThesType);

		return bRes;
	}
	catch(...)
	{
		return false;
	}
	return false;
}



void CThesaurusForSyntax::SortIndexes() 
{
		CTerminSort_less pred(&m_Termins);
		
		for( CInt2Vector::iterator iter = m_TerminsGrouppedByLength.begin();
			 iter != m_TerminsGrouppedByLength.end();
			 iter++ )
		{
			CIntVector& IndexVector = iter->second;
			sort(IndexVector.begin(), IndexVector.end(), pred);
		}
};

bool CThesaurusForSyntax::ReadTermins(const CThesaurus* piThes, EThesType eThesType)
{
	map<EThesType, vector<CGroups> >::const_iterator it = m_AllThesModels.find(eThesType);
	assert (it != m_AllThesModels.end());
	const vector<CGroups>& Models = it->second;

	int i;
	try
	{
		

		size_t Count =	piThes->m_Termins.size();
		size_t iTerminsCount = m_Termins.size();
		CInt2Vector::iterator iter;		

		size_t iModelsCount = piThes->m_Models.size();

		bool bBadTermin; 


		for( i = 0 ; i < Count ; i++ )
		{
			try
			{
				bBadTermin = false;
				const CInnerTermin& piTermin =  piThes->m_Termins[i];									
				const char* TerminStr = piTermin.m_TerminStr.c_str();
				long ModelNo = piTermin.m_ModelNo;
				long iTerminID = piTermin.m_TerminId;		

				CTermin termin;
				long WordCountInTermin = piTermin.m_Items.size();

				bool bRus = (ModelNo >= 0) && ( piThes->m_Models[ModelNo].IsRussian() );

				if( ( ModelNo != -1 ) && ( ModelNo < iModelsCount ) && ( WordCountInTermin > 0 ) && bRus)
				{

					termin.m_pModel = & (Models[ModelNo]);
					termin.m_TerminId = iTerminID;
					bool bInOb = true;
					for (long j=0; j < WordCountInTermin; j++)
					{
						const CInnerSynItem& piItem = piThes->m_SynItems[piTermin.m_Items[j]];;
						const char* str = piItem.m_ItemStr.c_str();
						
						//если оболочка тезауруса не собрала части дефисного слова в один элемент,
						//тогда будем игнорировать такой термин.
						//Например, слово "красно-белый" -  должно быть одним элементом.
						
						if(		str == 0
							||	(strlen(str) == 0)
							||	(!strcmp(str,"-") )
						  )
						{
							bBadTermin = true;

							break;
						}

						termin.push_back(str);
					};


					if( bBadTermin )
					{
						//printf ("%s - bad termin\n", TerminStr);
						continue;
					};

					termin.m_strTermin = TerminStr;
					termin.m_ThesType = eThesType;
					m_Termins.push_back(termin);
					iter = m_TerminsGrouppedByLength.find(WordCountInTermin);
					if(iter == m_TerminsGrouppedByLength.end() )
					{
						CIntVector IndexVector;
						IndexVector.push_back(iTerminsCount);
						m_TerminsGrouppedByLength.insert(CInt2Vector::value_type(WordCountInTermin, IndexVector));
						//printf ("The maximal length of termin is %i (%s) \n",WordCountInTermin, TerminStr);
					}
					else
					{
						iter->second.push_back(iTerminsCount);
					}

					iTerminsCount++;
				}
			}
			catch(...)
			{
				continue;
			}	

		}		

		
	}
	catch(...)
	{
		return false;
	}
	return true;
}


bool InitWordScheme(const CSyntaxOpt* Opt, CSynPlmLine& WordScheme,	const CAtomGroup& piWordScheme) 
{
	return true;
}

bool CThesaurusForSyntax::ReadModels(const CThesaurus& Thes, EThesType eThesType)
{
	

	size_t ModelNo = 0;
	string Name = Thes.m_Name;
	try {
		vector<CGroups>& Models = m_AllThesModels[eThesType];

		long iModelsCount = Thes.m_Models.size();
		//printf ("iModelsCount = %i\n",iModelsCount);

		for(; ModelNo < iModelsCount ; ModelNo++ )
		{
			const CInnerModel& piModel = Thes.m_Models[ModelNo];
			CGroups model(GetOpt());
			long DatabaseModelId = piModel.m_ModelId;

			long iAtomGroupsCount = piModel.m_AtomGroups.size();
			bool bRus = false;
			int j = 0 ;
            for(; j < iAtomGroupsCount ; j++ )
            {
                const CAtomGroup& ThesWordScheme = piModel.m_AtomGroups[j];

                CSynPlmLine WordScheme;
                WordScheme.SetGrammems (ThesWordScheme.m_Grammems);
                WordScheme.SetPoses((1 << ThesWordScheme.m_PartOfSpeech));
                if (ThesWordScheme.m_LanguageId == morphRussian)
                    WordScheme.SetFlag(fl_is_russian);
                if (ThesWordScheme.m_bDigCmpl) WordScheme.SetFlag(fl_dg_ch);
                if (GetOpt()->GetGramTab()->IsMorphNoun(WordScheme.GetPoses())) WordScheme.SetFlag(fl_morph_noun);
                bRus = bRus || WordScheme.HasFlag(fl_is_russian);
                model.AddWord(WordScheme);		
            }

			

			if( bRus )
			{
				long iGroupsCount = piModel.m_SynGroups.size();
				
				
				for(j = 0 ; j < iGroupsCount ; j++ )
				{
					const CThesSynGroup& piGroup = piModel.m_SynGroups[j];		
					assert (piGroup.m_First > 0);
					assert (piGroup.m_Last > 0);
					CGroup group (piGroup.m_First-1, piGroup.m_Last-1, 0);
					group.m_GroupType = GetOpt()->GetGroupTypebyName(piGroup.m_TypeStr.c_str());
					//  by default the main word is the first fort, later it will be changed
					group.m_MainWordNo = group.m_iFirstWord;
					model.create_group(group);
				}
				
				//printf ("ModelId %i, iGroupsCount %i \n",piModel.m_ModelId, iGroupsCount);
				
				AssignMainGroupsToModel(model, piModel);
			}

			Models.push_back(model);

		}	

	}
	catch (...)
	{
		ErrorMessage ("Synan", Format ("Cannot read models for %s ModelNo=%i", Thes.m_Name.c_str(), ModelNo));
		return false;
	};
	return true;
}

