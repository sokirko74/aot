// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "StdMorph.h"
#include "Predict.h"
#include "MorphAutomBuilder.h"
#include "MorphDictBuilder.h"
#include "LemmaInfoSerialize.h"



struct CPredictWord 
{
	WORD					m_ItemNo;
	WORD					m_Freq;
	int						m_LemmaInfoNo;
	WORD					m_nps; 
	


	CPredictWord(WORD Freq, int LemmaInfoNo,WORD nps, WORD ItemNo)
		: m_Freq(Freq),m_LemmaInfoNo(LemmaInfoNo),m_nps(nps),m_ItemNo(ItemNo)
	{

	}
};

struct CModelPostfix 
{
	string m_Postfix;
	size_t m_ModelNo;
	CModelPostfix(string Postfix, size_t ModelNo)
	{
		m_Postfix = Postfix;
		m_ModelNo = ModelNo;
	};

	bool operator < (const CModelPostfix& X) const
	{
		if (m_ModelNo != X.m_ModelNo)
			return m_ModelNo < X.m_ModelNo;
		else
			return m_Postfix < X.m_Postfix;
	};

};


typedef map<string, vector<CPredictWord> > Flex2WordMap;
typedef map<CModelPostfix, size_t > Postfix2FreqMap;

#ifdef DETECT_MEMORY_LEAK
	#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#endif
#endif


/*
 this function add  a new item to svMapRaw.
 svMapRaw is  from a postfix to  a vector of possible predict words.
 
*/
void AddElem(	Flex2WordMap& svMapRaw, 
				const string &Postfix, 
				int LemmaInfoNo, 
				const WORD nps,
				const WORD ItemNo,
				Postfix2FreqMap& ModelFreq, 
				const vector<CLemmaInfoAndLemma>&	LemmaInfos
				)
{

	// ============= determine the part of speech (nps)
	const CLemmaInfo& LemmaInfo =  LemmaInfos[LemmaInfoNo].m_LemmaInfo;
	

	// ============= adding to  svMapRaw, if not exists, otherwise update frequence
	CPredictWord set2(1, LemmaInfoNo, nps, ItemNo);
	Flex2WordMap::iterator svMapIt = svMapRaw.find(Postfix);

	if( svMapIt==svMapRaw.end() )
	{
		vector<CPredictWord> set2vec;
		set2vec.push_back(set2);
		svMapRaw[Postfix] = set2vec;
	}
	else
	{
		int i=0;
		CModelPostfix new_p(Postfix,  LemmaInfo.m_FlexiaModelNo);
		for( ; i<svMapIt->second.size(); i++ )
		{
			// if postfix, flexia and PartOfSpeech are equal then we should update frequence and exit
			if( svMapIt->second[i].m_nps == set2.m_nps )
			{
				svMapIt->second[i].m_Freq++;

				//  if  the new example is more frequent then we should predict using this example
				const CLemmaInfo& OldLemmaInfo =  LemmaInfos[svMapIt->second[i].m_LemmaInfoNo].m_LemmaInfo;
				
				if (ModelFreq[new_p] > ModelFreq[CModelPostfix(Postfix,  OldLemmaInfo.m_FlexiaModelNo)])
				{
					svMapIt->second[i].m_LemmaInfoNo = LemmaInfoNo;
					svMapIt->second[i].m_ItemNo = ItemNo;

				};

				break;
			}
		}
		// if no such part of speech for this postfix and flexia is found
		// then add new item
		if( i>= svMapIt->second.size() )
			svMapIt->second.push_back(set2);
	}
}




void log (string s)
{
	printf ("%s", s.c_str());
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


string GetPlugLemmabyLanguage (MorphLanguageEnum Langua)
{
	switch (Langua) {
		case morphRussian: return  "ÍÅÓÁÈÂÀÉÌÅÍß";
		case morphEnglish: return  "DURNOVO";
		case morphGerman: return  "UNKNOWN";
		default :return "unk";
	}
};

struct IsLessByModelNoAndBase 
{
	const MorphoWizard& m_Wizard;

	IsLessByModelNoAndBase(const MorphoWizard& wizard) : m_Wizard(wizard) 
	{
	};

	bool operator() (const_lemma_iterator_t _X1,const_lemma_iterator_t _X2) const
	{
		if (_X1->second.m_FlexiaModelNo != _X2->second.m_FlexiaModelNo)
			return _X1->second.m_FlexiaModelNo < _X2->second.m_FlexiaModelNo;

		return m_Wizard.get_base_string( _X1 ) < m_Wizard.get_base_string( _X2 );
	};
};

const size_t MinimalFlexiaModelFrequence = 10;

bool CMorphDictBuilder::GenPredictIdx(const MorphoWizard& wizard, int PostfixLength, int MinFreq, string path)
{
	fprintf (stderr, "CMorphDictBuilder::GenPredictIdx\n");
	DwordVector ModelFreq(wizard.m_FlexiaModels.size(), 0);
	//  building frequences of flexia models
	for(const_lemma_iterator_t lnMapIt = wizard.m_LemmaToParadigm.begin(); lnMapIt != wizard.m_LemmaToParadigm.end(); lnMapIt++)
		ModelFreq[lnMapIt->second.m_FlexiaModelNo]++;

	bool bSparsedDictionary;
	{
		int Count=0;
		for (size_t ModelNo=0; ModelNo<ModelFreq.size(); ModelNo++)
			if (ModelFreq[ModelNo] >= MinimalFlexiaModelFrequence)
				Count++;
		bSparsedDictionary = 2*Count < ModelFreq.size();
		if (bSparsedDictionary)
			fprintf (stderr, "Flexia models are too sparsed\n");
	};
	//  going through all words
	Postfix2FreqMap Postfix2Freq;
	for(size_t lin =0; lin < m_LemmaInfos.size(); lin++)
	{
		if (!(lin%1000))
			log ( Format("Prepare data...%i            \r", lin) ) ;
		const CLemmaInfo& LemmaInfo = m_LemmaInfos[lin].m_LemmaInfo;
		size_t ModelNo = LemmaInfo.m_FlexiaModelNo;
		const CFlexiaModel& paradigm = m_FlexiaModels[ModelNo];
		string	base = m_Bases[m_LemmaInfos[lin].m_LemmaStrNo].GetString();
		const vector <bool>& Infos = m_ModelInfo[ModelNo];
		for (size_t i=0; i<paradigm.m_Flexia.size(); i++)
			if (Infos[i])
			{
				string flexia = paradigm.m_Flexia[i].m_FlexiaStr;
				string wordform = base + flexia;
				if (wordform.length() < PostfixLength) continue;
				CModelPostfix p(wordform.substr(wordform.length() - PostfixLength), ModelNo);
				Postfix2FreqMap::iterator it = Postfix2Freq.find(p);
				if (it != Postfix2Freq.end())
					it->second++;
				else
					Postfix2Freq[p] = 1;
			};

	};
		
	log ( Format("Prepare data...%i            \n", m_LemmaInfos.size()) ) ;
	
	/*FILE *fp = fopen ("dump", "w");
	for (Postfix2FreqMap::const_iterator it = Postfix2Freq.begin();it != Postfix2Freq.end(); it++)
	{
		fprintf(fp, "%i %s %i\n", it->first.m_ModelNo, it->first.m_Postfix.c_str(), it->second);
	};
	fclose (fp);*/




	string PlugLemma = GetPlugLemmabyLanguage(wizard.m_Language);
	int PlugLemmaInfoNo = -1;

	Flex2WordMap svMapRaw;
	//  going through all words
	for(size_t lin =0; lin < m_LemmaInfos.size(); lin++)
	{
		
		if (!(lin%1000))
			log ( Format("Pick up data...%i            \r", lin) ) ;

		const CLemmaInfo& LemmaInfo = m_LemmaInfos[lin].m_LemmaInfo;
		size_t ModelNo = LemmaInfo.m_FlexiaModelNo;
		const CFlexiaModel& paradigm = m_FlexiaModels[ModelNo];
		string	base = m_Bases[m_LemmaInfos[lin].m_LemmaStrNo].GetString();

		if (base+paradigm.get_first_flex() == PlugLemma)
		{
			PlugLemmaInfoNo = lin;
			continue;
		};
		
		if (!bSparsedDictionary)
			if (ModelFreq[ModelNo] < MinimalFlexiaModelFrequence)
				continue;

		string pos = wizard.get_pos_string(paradigm.get_first_code());
		WORD nps =  GetPredictionPartOfSpeech(pos, wizard.m_Language);
		if (nps == UnknownPartOfSpeech)
			continue;


		const vector <bool>& Infos = m_ModelInfo[ModelNo];
		for (size_t i=0; i<paradigm.m_Flexia.size(); i++)
		if (Infos[i])
		{
			string flexia = paradigm.m_Flexia[i].m_FlexiaStr;
			string wordform = base + flexia;
			if (wordform.length() < PostfixLength) continue;
			string Postfix = wordform.substr(wordform.length() - PostfixLength);
			AddElem(svMapRaw, Postfix, lin, nps, i, Postfix2Freq, m_LemmaInfos);
		}
		
	}
	log ( Format("Pick up data...%i            \n", m_LemmaInfos.size()) ) ;

	if (PlugLemmaInfoNo == -1)
	{
		ErrorMessage (Format("Cannot find a word for the default noun prediction (\"%s\") while  generating %s prediction base",PlugLemma.c_str(), GetStringByLanguage(wizard.m_Language).c_str()));
		return false;
	};

	log("Saving...\n");

	CMorphAutomatBuilder R(wizard.m_Language, MorphAnnotChar);
	R.InitTrie();

	// adding crtitical noun
	{
		string s = R.GetCriticalNounLetterPack();
		s += R.m_AnnotChar;
		s += R.EncodeIntToAlphabet(0); // noun
		s += R.m_AnnotChar;
		s += R.EncodeIntToAlphabet(PlugLemmaInfoNo);
		s += R.m_AnnotChar;
		s += R.EncodeIntToAlphabet(0);
		R.AddStringDaciuk(s);
	};

	for( Flex2WordMap::const_iterator it=svMapRaw.begin(); it!=svMapRaw.end(); it++ )
	{
		for( int i=0; i<it->second.size(); i++ )
		{
			const CPredictWord& W = it->second[i];
			// checking minimal frequence

			if (W.m_Freq < MinFreq) continue;
			
			string s = it->first;
			reverse(s.begin(), s.end());
			s += R.m_AnnotChar;
			s += R.EncodeIntToAlphabet(W.m_nps);
			s += R.m_AnnotChar;
			s += R.EncodeIntToAlphabet(W.m_LemmaInfoNo);
			s += R.m_AnnotChar;
			s += R.EncodeIntToAlphabet(W.m_ItemNo);
			R.AddStringDaciuk(s);
		}
	};

	R.ConvertBuildRelationsToRelations();
	R.Save(path + PREDICT_BIN_PATH);
	

	svMapRaw.clear();
	return true;
	
}



