// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru), 
// ==========  Copyright by Alexey Sokirko (2004)

#include "StdMorph.h"
#include "MorphDictBuilder.h"

const size_t MaxLemmaPrefixCount = 0x200;
const size_t MaxLemmaCount = 0x800000;
const size_t MaxFlexiaModelsCount = 0x8000;
const size_t MaxNumberFormsInOneParadigm = 0x200;

CMorphDictBuilder::CMorphDictBuilder(MorphLanguageEnum Language) 
:	CMorphDict(Language)
{
	InitAutomat( new CMorphAutomatBuilder(Language, MorphAnnotChar) );
};

CMorphDictBuilder::~CMorphDictBuilder() 
{
};

//bool  operator <  (const char*& s1, const char* s2)
//{
//	return strcmp(s1,s2) < 0;
//}
bool CMorphDictBuilder::GenerateLemmas(const MorphoWizard& Wizard) 
{
	printf ("GenerateLemmas\n");
	vector<set<string> > InfoToBases;
	{	// creaing CMorphDict::m_Bases
		set<string> Bases;
		
		for( const_lemma_iterator_t lemm_it= Wizard.m_LemmaToParadigm.begin(); lemm_it!=Wizard.m_LemmaToParadigm.end(); lemm_it++ )
		{
			set<string> curr_bases;

			if (lemm_it->second.m_PrefixSetNo != UnknownPrefixSetNo)
			{
				const set<string>& s = Wizard.m_PrefixSets[lemm_it->second.m_PrefixSetNo];
				for(set<string>::const_iterator it_s = s.begin(); it_s != s.end(); it_s++)
					curr_bases.insert(*it_s+Wizard.get_base_string(lemm_it));
			}
			else
				curr_bases.insert(Wizard.get_base_string(lemm_it));

			InfoToBases.push_back(curr_bases);
			Bases.insert(curr_bases.begin(), curr_bases.end());
		};

		m_Bases.CreateFromSet(Bases);
	};

	{
		// creating vector<CLemmaInfoAndLemma>	m_LemmaInfos
		size_t Index = 0;
		for( const_lemma_iterator_t lemm_it= Wizard.m_LemmaToParadigm.begin(); lemm_it!=Wizard.m_LemmaToParadigm.end(); lemm_it++ )
		{
			CLemmaInfoAndLemma I;
			
			for(set<string>::const_iterator it = InfoToBases[Index].begin(); it != InfoToBases[Index].end(); it++)
			{
				vector<CShortString>::const_iterator base_it =  lower_bound(m_Bases.begin(), m_Bases.end(), it->c_str(), IsLessShortString());
				assert (base_it != m_Bases.end());
				assert (*it == base_it->GetString());
				I.m_LemmaStrNo = base_it - m_Bases.begin();
				I.m_LemmaInfo = lemm_it->second;
				m_LemmaInfos.push_back(I);
			}
			Index++;
		};
		sort(m_LemmaInfos.begin(), m_LemmaInfos.end());
	};

	
	if (m_LemmaInfos.size() >= MaxLemmaCount)
	{
		fprintf (stderr, "Cannot be more than %i lemmas\n", MaxLemmaCount-1); 
		return false;
	};


	return true;
}



bool  CMorphDictBuilder::GenerateUnitedFlexModels(const MorphoWizard& Wizard)
{
	printf ("GenerateUnitedFlexModels\n");
	//===========================
	// Creating m_ModelInfo
	m_ModelInfo.clear();
	m_FlexiaModels.clear();
	m_NPSs.clear();
	if (Wizard.m_FlexiaModels.size() >=  MaxFlexiaModelsCount)
	{
		fprintf (stderr, "Cannot be more than %i flexia models\n", MaxFlexiaModelsCount-1); 
		return false;
	};

	for( size_t ModelNo=0; ModelNo < Wizard.m_FlexiaModels.size(); ModelNo++ )
	{
		CFlexiaModel p = Wizard.m_FlexiaModels[ModelNo];

		
		m_NPSs.push_back(GetPredictionPartOfSpeech(Wizard.get_pos_string(p.get_first_code()), 
													Wizard.m_Language));
		

		m_ModelInfo.push_back(vector<bool>(p.m_Flexia.size(), true));

		if ( p.m_Flexia.size() >=  MaxNumberFormsInOneParadigm)
		{
			fprintf (stderr, "Error: flexia No %i contains more than %i forms. !\n", MaxNumberFormsInOneParadigm); 
			return false;
		};

		for (size_t i=0; i <p.m_Flexia.size(); i++)
			if (m_ModelInfo.back()[i])
				for (size_t j=i+1; j <p.m_Flexia.size(); j++)
					if (		(p.m_Flexia[j].m_FlexiaStr ==  p.m_Flexia[i].m_FlexiaStr)
							&&	(p.m_Flexia[j].m_PrefixStr ==  p.m_Flexia[i].m_PrefixStr)
						)
						{
							m_ModelInfo.back()[j] = false;
							p.m_Flexia[i].m_Gramcode.insert(p.m_Flexia[i].m_Gramcode.end(),p.m_Flexia[j].m_Gramcode.begin(),p.m_Flexia[j].m_Gramcode.end());
						};
		m_FlexiaModels.push_back(p);
	};
	
	return true;
};

bool  CMorphDictBuilder::GeneratePrefixes(const MorphoWizard& Wizard)
{
	printf ("GeneratePrefixes\n");
	m_Prefixes.clear();
	// add the empty prefix
	m_Prefixes.push_back("");
	for (size_t i=0; i< Wizard.m_PrefixSets.size(); i++)
	{
		m_PrefixSets.push_back(DwordVector());
		
		for (set<string>::const_iterator it =  Wizard.m_PrefixSets[i].begin();  it != Wizard.m_PrefixSets[i].end(); it++)
		{
			string prefix = *it;
			StringVector::iterator it_c = find(m_Prefixes.begin(), m_Prefixes.end(), prefix);
			if (it_c == m_Prefixes.end())
				it_c = m_Prefixes.insert(m_Prefixes.end(), prefix);
			m_PrefixSets.back().push_back(it_c - m_Prefixes.begin());
		};
		if (m_PrefixSets.back().empty())
		{
			fprintf (stderr, "PrefixSet %i  has no prefixes\n", i); 
			return false;
		};

	};
	if (m_Prefixes.size() >= MaxLemmaPrefixCount)
	{
		fprintf (stderr, "Cannot be more than %i prefixes\n", MaxLemmaPrefixCount-1); 
		return false;
	};

	return true;
};



extern size_t RegisterSize;

bool  CMorphDictBuilder::CreateAutomat(const MorphoWizard& Wizard)
{
	
	GetFormBuilder()->InitTrie();
	
	m_AccentModels = Wizard.m_AccentModels;

	

	if (!GeneratePrefixes(Wizard))
		return false;
	

	// Creating tries for paradigms
	size_t RuleNo = 0; 
	size_t LemmaNo = 0;
	size_t ReusedNodes = 0;
	
	
	DwordVector EmptyGlobalPrefixes(1, 0);
	
	printf ("Generate the main automat ...\n");
	size_t FormsCount = 0;
	
	for( const_lemma_iterator_t it=Wizard.m_LemmaToParadigm.begin(); it!=Wizard.m_LemmaToParadigm.end(); it++ )
	{

		if (!(LemmaNo % 100))
			fprintf (stderr,"Lemma %i/%i  RegisterSize = %i \r", LemmaNo, Wizard.m_LemmaToParadigm.size(), RegisterSize);
			
		
		//if (LemmaNo  >  12000)
		//	break;

		/*if (LemmaNo  <  39000)
			continue;*/

		size_t ModelNo = it->second.m_FlexiaModelNo;
		if (ModelNo  > Wizard.m_FlexiaModels.size())
		{
			fprintf (stderr,"Bad flexia model  : %s\n", Wizard.get_lemm_string(it).c_str());
			return false;
		};


		DwordVector* pPrefixVector = &EmptyGlobalPrefixes;
		if (it->second.m_PrefixSetNo != UnknownPrefixSetNo)
			pPrefixVector = & (m_PrefixSets[it->second.m_PrefixSetNo] );

		assert (!pPrefixVector->empty());

		const CFlexiaModel&p = Wizard.m_FlexiaModels[ModelNo];
		const vector <bool>& Infos = m_ModelInfo[ModelNo];
		
		for (size_t PrefixNo = 0; PrefixNo < pPrefixVector->size();PrefixNo++)
		{
			string base  = Wizard.get_base_string(it);
			
			for (size_t ItemNo=0; ItemNo <p.m_Flexia.size(); ItemNo++)
			if (Infos[ItemNo])
			{
				string WordForm = m_Prefixes[(*pPrefixVector)[PrefixNo]];
				WordForm += p.m_Flexia[ItemNo].m_PrefixStr;
				WordForm += base;
				WordForm += p.m_Flexia[ItemNo].m_FlexiaStr;

				WordForm += GetFormBuilder()->m_AnnotChar;
				{
					FormsCount++;
					DWORD info = 	GetFormBuilder()->EncodeMorphAutomatInfo(ModelNo, ItemNo, (*pPrefixVector)[PrefixNo]);

					{	// checking encoding
						size_t checkModelNo, checkItemNo,checkPrefixNo;
						GetFormBuilder()->DecodeMorphAutomatInfo(info, checkModelNo, checkItemNo, checkPrefixNo);

						if (		(checkModelNo != ModelNo)
								||	(checkItemNo != ItemNo)
								||	(checkPrefixNo != (*pPrefixVector)[PrefixNo])
							)
						{
							fprintf (stderr, "General annotation encoding error!\n");
							return false;
						};

					};

					WordForm += GetFormBuilder()->EncodeIntToAlphabet(info);	
				};

				

				
				if (!GetFormBuilder()->AddStringDaciuk(WordForm))
					return false;
			};
		};


		LemmaNo++;

		
	};
	fprintf (stderr,"Lemma %i/%i  RegisterSize=%i   \n", LemmaNo, Wizard.m_LemmaToParadigm.size(), RegisterSize);

	if (LemmaNo >  0xffffff)
	{
		fprintf (stderr, "Cannot be more than 0xffffff lemmas\n"); 
		return false;
	};
	fprintf (stderr,"Count of word forms =  %i \n", FormsCount);
	

	bool bResult = true;

	/*printf ("Testing ...\n");
	bResult = IsValid();*/
		
	GetFormBuilder()->ClearRegister();

	printf ("ConvertBuildRelationsToRelations for word forms...  \n");
	GetFormBuilder()->ConvertBuildRelationsToRelations();


	return bResult;
};

