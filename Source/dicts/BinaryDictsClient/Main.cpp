#include "morph_dict/common/utilit.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"
#include  "dicts/StructDictLib/StructDictConsts.h"
#include  "dicts/BinaryDictsLib/AspPairDict.h"
#include  "dicts/BinaryDictsLib/FreqDict.h"
#include  "dicts/BinaryDictsLib/DerivDict.h"
#include  "dicts/BinaryDictsLib/BinaryDictionary.h"
#include  "dicts/BinaryDictsLib/SetOfWordPairs.h"

#include <iostream>
#include <ctime>

static CBinaryDictionary Dict;
CMorphanHolder RussianHolder, EnglishHolder;


const char g_strFreqCompPath[] =    "Software\\Dialing\\EngRusFreqDict\\Comp";
const char g_strFreqFinPath[] =    "Software\\Dialing\\EngRusFreqDict\\Fin";
const char g_strFreqHudPath[] =    "Software\\Dialing\\EngRusFreqDict\\Hud";


CFreqDict				HudFreq;
CFreqDict				CompFreq;
CFreqDict				FinFreq;


void string_to_id(const char *str, DwordVector &ids, const CMorphanHolder& Holder)
{
	bool is_capital = is_russian_upper((BYTE)str[0]) != 0;
	if (is_capital)
	{
		fprintf (stderr,"First char is uppercase\n");
	}
	std::vector<CFormInfo> ParadigmCollection;
	std::string input = str;
	if (!Holder.m_pLemmatizer->CreateParadigmCollection(false, input, is_capital, false, ParadigmCollection))
	{
		std::cerr << "Catch " << str<< std::endl;
		return;
	}
	if(ParadigmCollection.empty())
		std::cerr << "No paradigm" << std::endl;
	for(int i = 0; i < ParadigmCollection.size(); i++){
		const CFormInfo& Paradigm = ParadigmCollection[i];
		if(!Paradigm.m_bFound) continue;

		ids.push_back(Paradigm.GetParadigmId());
	}
}


int main()
{
	
	try{
		long tt;
		std::cout << "Please wait ..." << std::endl;
		tt = -clock();
		RussianHolder.LoadLemmatizer(morphRussian);
		EnglishHolder.LoadLemmatizer(morphEnglish);

		tt += clock();
		std::cout << "Loaded two lemmatizers at " << tt << " tiks" << std::endl; //"\r";

		//!!! Создаем и загружаем словарь
		tt = -clock();
		Dict.Load();

        //std::cerr << "Number of pairs in the dictionary: " << Dict.eng_vec.size() << std::endl;
		

		CompFreq.Load(GetRegistryString(g_strFreqCompPath));
		FinFreq.Load(GetRegistryString(g_strFreqFinPath));
		HudFreq.Load(GetRegistryString(g_strFreqHudPath));
		tt += clock();
		std::cout << "Loaded Dictionary at " << tt << " ticks" << std::endl;
	}
	catch (CExpc e) {
		std::cerr << "An error occured" << e.what() << std::endl;
		return 1;
	}
	catch(...){
		std::cerr << "An error occured" << std::endl;
		return 1;
	}

	bool Direct = false;
	while(true){
		std::cout << ">> ";
		int i;
		std::string alt;
		std::cin >> alt;
		if(!std::cin) break;
		if(alt == "-quit")
			break;
		if (alt == "")
			break;

		if( alt[0] < 128)
			Direct = true;
		else {
			Direct = false;
			alt = convert_from_utf8(alt.c_str(), morphRussian);
		}
		{
			std::string s(alt);
			size_t slash = s.find('/');
			if(slash >= 0 && slash < s.size()){
				std::string eng = s.substr(0, slash);
				std::string rus = s.substr(slash+1, s.size());
				DwordVector e, r;
				string_to_id(eng.c_str(), e, EnglishHolder);
				string_to_id(rus.c_str(), r, RussianHolder);
				for(int i = 0; i < e.size(); i++)
					for(int j = 0; j < r.size(); j++){
						if(Dict.HavePair(e[i], r[j])){
							std::cout << "Found: " << e[i] << " " <<  r[j] << std::endl;
							std::cout << CompFreq.GetFreq(e[i], r[j]) << " ";
							std::cout << FinFreq.GetFreq(e[i], r[j]) << " ";
							std::cout << HudFreq.GetFreq(e[i], r[j]) << std::endl;
						}
					}				
				continue;
			}
		}


		DwordVector ids;
		const CMorphanHolder& Holder =  (Direct ? EnglishHolder : RussianHolder);
		const CMorphanHolder& TransHolder =  (Direct ?  RussianHolder : EnglishHolder);
		string_to_id(alt.c_str(), ids, Holder);

		for(i = 0; i < ids.size(); i++)
		{
			int j;
			std::string code;
			CFormInfo FormInfo;
			if (Holder.m_pLemmatizer->CreateParadigmFromID(ids[i], FormInfo))
			{
				code += FormInfo.GetWordForm(0);
				code += " ";
				code += FormInfo.GetAncode(0);
				if (!Direct) 	
					code = convert_to_utf8(code, morphRussian);
				std::cout << code << std::endl;

			}

			//!!! получаем набор пар
			CSetOfWordPairs pairs = Direct 
				? Dict.TranslateDirect(ids[i])
				: Dict.TranslateIndirect(ids[i]);
			//!!! число пар
			int count = pairs.get_Count();
			if(count == 0)
				std::cerr << "No entry" << std::endl;

			for(j = 0; j < count; j++){
				//!!! получаем перевод
				long to_id = pairs.GetId(j);
				std::string to;
				TransHolder.id_to_string(to_id, to);

				CFormInfo TransFormInfo;
				if (TransHolder.m_pLemmatizer->CreateParadigmFromID(to_id, TransFormInfo))
				{
					std::string code = TransFormInfo.GetAncode(0);
					if (Direct) 	
						code = convert_to_utf8(code.c_str(), morphRussian);
					std::cout << (j+1) << ") " << to << " " << code << " ";;
				}

				{
					long id1 = ids[i];
					long id2 = to_id;
					if(!Direct) std::swap(id1, id2);
					std::cout << " (";
					std::cout << CompFreq.GetFreq(id1, id2) << " ";
					std::cout << FinFreq.GetFreq(id1, id2) << " ";
					std::cout << HudFreq.GetFreq(id1, id2) << ") ";
				}

				std::cout << " --";
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	}
}

