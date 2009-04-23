#include "../common/utilit.h"
#include "../common/MorphologyHolder.h"
#include  "../StructDictLib/StructDictConsts.h"
#include  "../BinaryDictsLib/AspPairDict.h"
#include  "../BinaryDictsLib/FreqDict.h"
#include  "../BinaryDictsLib/DerivDict.h"
#include  "../BinaryDictsLib/SynDictionary.h"
#include  "../BinaryDictsLib/BinaryDictionary.h"
#include  "../BinaryDictsLib/SetOfWordPairs.h"

#include <iostream>
#include <ctime>

static CBinaryDictionary Dict;
CMorphologyHolder RussianHolder, EnglishHolder;


const char g_strFreqCompPath[] =    "Software\\Dialing\\EngRusFreqDict\\Comp";
const char g_strFreqFinPath[] =    "Software\\Dialing\\EngRusFreqDict\\Fin";
const char g_strFreqHudPath[] =    "Software\\Dialing\\EngRusFreqDict\\Hud";


CFreqDict				HudFreq;
CFreqDict				CompFreq;
CFreqDict				FinFreq;

void id_to_string(long id, std::string &str, const CMorphologyHolder& Holder)
{
	
	CFormInfo pVal;
	if (!Holder.m_pLemmatizer->CreateParadigmFromID(id, pVal))
	{
		assert(false);
		return ;
	}
	str = pVal.GetWordForm(0);
}

void string_to_id(const char *str, DwordVector &ids, const CMorphologyHolder& Holder)
{
	bool is_capital = is_russian_upper((BYTE)str[0]) != 0;
	if (is_capital)
	{
		fprintf (stderr,"First char is uppercase\n");
	}
	vector<CFormInfo> ParadigmCollection;
	string input = str;
	if (!Holder.m_pLemmatizer->CreateParadigmCollection(false, input, is_capital, ParadigmCollection))
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

void RussianConvertToDictionary (std::string& S)
{
	#ifdef WIN32
		OemToCharBuff((char*)S.c_str(), (char*)S.c_str(), S.length());		 
	#else
		KOI8ToWin(S);
	#endif

};

void RussianConvertToScreen (std::string& S)
{
	#ifdef WIN32
		CharToOemBuff((char*)S.c_str(), (char*)S.c_str(), S.length());		 
	#else
		WinToKOI8(S);
	#endif

};


int main()
{
	
	try{
		long tt;
		std::cout << "Please wait ..." << std::endl;
		tt = -clock();
		if (!RussianHolder.LoadGraphanAndLemmatizer(morphRussian))
		{
			std::cerr << "Cannot load Russian morphology" << std::endl;
			return 1;
		}
		if (!EnglishHolder.LoadGraphanAndLemmatizer(morphEnglish))
		{
			std::cerr << "Cannot load English morphology" << std::endl;
			return 1;
		}
		tt += clock();
		std::cout << "Loaded two lemmatizers at " << tt << " tiks" << std::endl; //"\r";

		//!!! Создаем и загружаем словарь
		tt = -clock();
		if (!Dict.Load())
		{
			std::cerr << "Cannot create dictionary" << std::endl;
			return 1;
		}
		std::cerr << "Number of pairs in the dictionary: " << Dict.eng_vec.size() << std::endl;
		

		if (!CompFreq.Load(GetRegistryString(g_strFreqCompPath).c_str()))
		{
			std::cerr << "Cannot load CompFreq" << std::endl;
			return 1;
		}
		if (!FinFreq.Load(GetRegistryString(g_strFreqFinPath).c_str() ))
		{
			std::cerr << "Cannot load FinFreq" << std::endl;
			return 1;
		}
		if (!HudFreq.Load(GetRegistryString(g_strFreqHudPath).c_str()))
		{
			std::cerr << "Cannot load HudFreq" << std::endl;
			return 1;
		}
		tt += clock();
		std::cout << "Loaded Dictionary at " << tt << " ticks" << std::endl;
	}catch(...){
		std::cerr << "An error occured" << std::endl;
		return 1;
	}

	bool Direct = false;
	//!!! число флагов 
	int flag_count = Dict.GetFlagCount();
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

		if((BYTE)alt[0] >= 'a' && (BYTE)alt[0] <= 'z' || (BYTE)alt[0] >= 'A' && (BYTE)alt[0] <= 'Z')
			Direct = true;
		else
			Direct = false;
		if (!Direct) 	
			RussianConvertToDictionary(alt);


		{
			std::string s(alt);
			int slash = s.find('/');
			if(slash >= 0 && slash < s.size()){
				std::string eng = s.substr(0,slash);
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
		const CMorphologyHolder& Holder =  (Direct ? EnglishHolder : RussianHolder);
		const CMorphologyHolder& TransHolder =  (Direct ?  RussianHolder : EnglishHolder);
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
						RussianConvertToScreen(code);
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
				id_to_string(to_id, to, TransHolder);

				if (Direct) 	
						RussianConvertToScreen(to);

				CFormInfo TransFormInfo;
				if (TransHolder.m_pLemmatizer->CreateParadigmFromID(to_id, TransFormInfo))
				{
					std::string code = TransFormInfo.GetAncode(0);
					if (Direct) 	
						RussianConvertToScreen(code);
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
				for(int fl = 0; fl < flag_count; fl++){
					
					//!!! получаем флаг
					std::string fl_str;
					DWORD flag = pairs.GetFlag(j, fl, fl_str);
					RussianConvertToScreen(fl_str);
					std::cout << " " << fl_str ;
				}
				std::cout << std::endl;				
			}
			std::cout << std::endl;
		}
	}

	//!!! уничтожаем словарь

}

