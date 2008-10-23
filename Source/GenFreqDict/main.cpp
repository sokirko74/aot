#include <stdio.h> 


#include <map>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <string>
#include <vector>
#include <ctime>
#include <set>
#include "../common/util_classes.h"
#include "../AgramtabLib/eng_consts.h"
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include "../common/MorphologyHolder.h"
#include "../BinaryDictsLib/BinaryDictionary.h"

CMorphologyHolder MorphHolderRus;
CMorphologyHolder MorphHolderEng;
CBinaryDictionary BinaryDictionary;



std::ofstream out;


struct word_pair{
	long e, r;
	word_pair(long a, long b)
		:e(a), r(b){}
	bool operator < (const word_pair &p) const{
		if(e < p.e) return true;
		if(e == p.e && r < p.r) return true;
		return false;
	}
};
std::map<word_pair, int> word_map;
std::set<std::pair<long, long> > SmallDictSet;


//--------------------------------------------------------------------------------
void init_dicts()
{
	if (!MorphHolderRus.LoadGraphanAndLemmatizer(morphRussian))
		throw CExpc( "cannot load Russian morphologyn");
	if (!MorphHolderEng.LoadGraphanAndLemmatizer(morphEnglish))
		throw CExpc( "cannot load English morphology\n");
	
	if (!BinaryDictionary.Load())
		throw CExpc( "cannot load binary dictionary\n");

	std::cerr << "dictionaries are loaded" << std::endl;

}


int  mymain(int argc, char **argv)
{
		init_dicts();
	if (argc != 4)
	{
		throw CExpc ("bad number of arguments");
	}


/*
 читаем малый словарь, который состоит из пар слов 
 отдельно на каждой строке типа
   "аббат  abbot"
 считается, что если пара попала в этот словарь, то у этой
 пары хороший вес для перевода.
 Параллелльно считаем скорость морфологии7
*/
	std::cerr << "Opening small dictionary\t\t\t\r";
	std::ifstream SmallDict(argv[3]);
	assert(SmallDict.is_open());
	clock_t MorphTime = 0;
	while(true){
		char buff[2048];
		SmallDict.getline(buff, 2047);
		if(!SmallDict) break;
		/*
		  на самом деле, здесь возможны словосочетания
		  как с русской, так и с англ. стороны
		  От словосочетания всегда берем только первое слово.
		*/
		StringTokenizer tok(buff, " \t");
		StringVector vec;
		const char *word;
		while((word = tok())) vec.push_back(word);

		std::string r, e;
		if(vec.size() < 2) continue;
		if(vec.size() == 2){
			r = vec[0];
			e = vec[1];
		}else{
			
			r = vec[0];
			int i = 1;
			// проходим все русские слова
			while((unsigned char)vec[i][0] > 128 && i < vec.size()) i++;
			if(i == vec.size() - 1)
				e = vec[i];
			else continue;
		}

		// лемматизируем
		
		clock_t     ltime1 = clock();
		DwordVector r_id, e_id;
		MorphHolderRus.string_to_ids(r.c_str(), r_id, true);
		MorphHolderEng.string_to_ids(e.c_str(), e_id, true);
		clock_t ltime2  = clock();
		MorphTime += ltime2 - ltime1;



		for(int R = 0; R < r_id.size(); R++)
		for(int E = 0; E < e_id.size(); E++)
			SmallDictSet.insert(std::make_pair(r_id[R], e_id[E]));
	}
	SmallDict.close();
	std::cerr << "Done\t" << SmallDictSet.size() << "\t\t\t\n";
	if (MorphTime > 0)		
	{
		MorphTime /= CLOCKS_PER_SEC;
		if (MorphTime > 0)		
			std::cerr << "Morphology of norm norm/second)\t" << double(SmallDictSet.size()*2/MorphTime) << "\t\t\t\n";
	};
		

	std::cerr << "Parsing aligned sentences\t\t\t\r";
	const char *in_str = argv[1];
	const char *out_str = argv[2];
	std::ifstream file_list(in_str);
	out.open(out_str, std::ios::binary);
	if (!out.is_open())
	{
		std::cerr << "cannot open output file!" << out_str << std::endl;
		return -1;
	};
	if (!file_list.is_open())
	{
		std::cerr << "cannot open list-file !" << in_str << std::endl;
		return -1;
	};
		
	while(true)
	{
		std::string file_name;
		file_list >> file_name;
		if(!file_list) break;
		std::ifstream in(file_name.c_str());
		assert(in.is_open());
		int sent_no = 0;
		while(true){
			char buff[2][4096];
			in.getline(buff[0], 4095);
			in.getline(buff[1], 4095);
			if(!in) break;
			const char *word;
			StringTokenizer r_tok(buff[0], " \t");
			StringTokenizer e_tok(buff[1], " \t");
			std::vector<CFormInfo> e_id, r_id;
			while((word = r_tok()))
			{
				std::vector<CFormInfo> p;
				string w = word;
				if (MorphHolderRus.m_pLemmatizer->CreateParadigmCollection(false,w, false, false, p))
					r_id.insert(r_id.end(), p.begin(), p.end());

			}
			while((word = e_tok()))
			{
				std::vector<CFormInfo> p;
				string w = word;
				if (MorphHolderEng.m_pLemmatizer->CreateParadigmCollection(false,w, false, false, p))
					e_id.insert(e_id.end(), p.begin(), p.end());
			}

			for(int r = 0; r < r_id.size(); r++)
				for(int e = 0; e < e_id.size(); e++)
					if (BinaryDictionary.HavePair(e_id[e].GetParadigmId(), r_id[r].GetParadigmId()))
					{
						word_map[word_pair(e_id[e].GetParadigmId(),r_id[r].GetParadigmId())]++;
						break;
					}



		}
		in.close();
	}

	/*
	  вычисляю максимальную частоту и увеличиваю
      частоту пары слов, которая была найдена в малом словаре
	*/
	long MaxFreq = 0;
	std::set<std::pair<long, long> > SmallDictUsed;
	std::map<word_pair,int>::iterator it = word_map.begin();
	for(; it != word_map.end(); ++it)
	{
		if(SmallDictSet.count(std::make_pair(it->first.r, it->first.e)))
		{
			it->second *= 3;
			if(it->second < 6) it->second = 6;
			SmallDictUsed.insert(std::make_pair(it->first.r, it->first.e));
		}
		if (it->second > MaxFreq)
			MaxFreq =  it->second;
	};

	// writting out file and dump
	int written = 0;
	std::cout << "Frequence of Pair\tRussian Lemma\tEnglish Lemma";
	for(it = word_map.begin(); it != word_map.end(); ++it)
	{
		std::string rus = MorphHolderRus.id_to_string(it->first.r);
		std::string eng = MorphHolderEng.id_to_string(it->first.e);
		out.write((char*)&(it->first.e), sizeof(long));
		out.write((char*)&(it->first.r), sizeof(long));

		unsigned short freq = it->second;
		/*
		  частота не должна превышать  USHRT_MAX, поэтому
		  нужно нормализовать частоту 
		*/
		if (MaxFreq > USHRT_MAX)
			freq *= USHRT_MAX/MaxFreq; 
		out.write((char*)&(freq), sizeof(unsigned short ));

		// выводим дамп
		std::cout << it->second << '\t' << rus.c_str() << '\t' << eng.c_str() << std::endl;
		written++;
	}







	/*
	 проходим по малому словарю, если пара из этого словаря
	 не была записана в выходной файл, пишем с частотой пять
	 берутся только существительные, глаголы, прил. и нар,
	*/
	
	for(std::set<std::pair<long, long> >::iterator shira_it = SmallDictSet.begin(); shira_it != SmallDictSet.end(); ++shira_it)
	{
		if(SmallDictUsed.count(*shira_it)) continue;
		CFormInfo rp = MorphHolderRus.id_to_paradigm(shira_it->first);
		CFormInfo ep = MorphHolderEng.id_to_paradigm(shira_it->second);

		BYTE r_pos = MorphHolderRus.m_pGramTab->GetPartOfSpeech(rp.GetAncode(0).c_str());
		BYTE e_pos = MorphHolderEng.m_pGramTab->GetPartOfSpeech(ep.GetAncode(0).c_str());
		BYTE t_pos = 0xff;

		if(r_pos == NOUN && e_pos == eNOUN)t_pos = 0;

		if(r_pos == ADJ_FULL && e_pos == eADJ)t_pos = 2;
		if(r_pos == ADJ_SHORT && e_pos == eADJ)t_pos = 2;

		if(r_pos == ADV && e_pos == eADV)t_pos = 3;
		if(r_pos == PREDK && e_pos == eADV)t_pos = 3;

		if(r_pos == INFINITIVE && e_pos == eVERB)t_pos = 1;

		if(t_pos ==  0xff ) continue;

		long id = ep.GetParadigmId();
		out.write((char*)&(id), sizeof(long));
		id = rp.GetParadigmId();
		out.write((char*)&(id), sizeof(long));
		unsigned short  freq = 5;
		out.write((char*)&(freq), sizeof(unsigned short));

		std::string rus = MorphHolderRus.id_to_string(shira_it->first);
		std::string eng = MorphHolderEng.id_to_string(shira_it->second);
		
		if(!BinaryDictionary.HavePair(shira_it->second, shira_it->first))
		{
			std::cerr << rus << " " << 
				rp.GetAncode(0).c_str() << " "
				<< eng ;
			std::cerr << " " << 
				ep.GetAncode(0).c_str() << " " << t_pos
				<< " 0 0 0 0"<< std::endl;
		}
		std::cout << 5 << '\t' << rus << '\t' << eng	 << std::endl;
		written++;
	}



	std::cerr << "Written " << written << "\t\t\t" << std::endl;


	out.close();
	file_list.close();
	
	return 0;

}

int  main(int argc, char **argv)
{
	try 
	{
		return mymain(argc, argv);
	}
	catch (CExpc e)
	{
		fprintf (stderr, "exception occurred: %s!\n", e.m_strCause.c_str());
		return 1;
	}
	catch (...)
	{
		fprintf (stderr, "some exception occurred!\n");
		return 1;
	}


}


