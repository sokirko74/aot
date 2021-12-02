#include <stdio.h> 
#include <map>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <string>
#include <vector>
#include <ctime>
#include <set>
#include "morph_dict/common/util_classes.h"
#include "morph_dict/AgramtabLib/eng_consts.h"
#include "morph_dict/LemmatizerBaseLib/MorphanHolder.h"
#include "dicts/BinaryDictsLib/BinaryDictionary.h"

CMorphanHolder MorphHolderRus;
CMorphanHolder MorphHolderEng;
CBinaryDictionary BinaryDictionary;


struct word_pair{
	uint32_t e, r;
	word_pair(uint32_t a, uint32_t b)
		:e(a), r(b){}
	bool operator < (const word_pair &p) const{
		if(e < p.e) return true;
		if(e == p.e && r < p.r) return true;
		return false;
	}
};

typedef std::set<std::pair<uint32_t, uint32_t> > small_dict_t;

//--------------------------------------------------------------------------------
void init_dicts()
{
	MorphHolderRus.LoadLemmatizer(morphRussian);
	MorphHolderEng.LoadLemmatizer(morphEnglish);
	BinaryDictionary.Load();
	std::cerr << "dictionaries are loaded" << std::endl;
}

small_dict_t read_small_dict(const char* filename) {
	/*
	 читаем малый словарь, который состоит из пар слов
	 отдельно на каждой строке типа
	   "аббат  abbot"
	 считается, что если пара попала в этот словарь, то у этой
	 пары хороший вес для перевода.
	 Параллелльно считаем скорость морфологии7
	*/
	small_dict_t smallDictSet;

	std::cerr << "Opening small dictionary\n";
	std::ifstream SmallDict(filename);
	assert(SmallDict.is_open());
	while (true) {
		char buff[2048];
		SmallDict.getline(buff, 2047);
		if (!SmallDict) break;
		/*
		  на самом деле, здесь возможны словосочетания
		  как с русской, так и с англ. стороны
		  От словосочетания всегда берем только первое слово.
		*/
		StringTokenizer tok(convert_from_utf8(buff, morphRussian).c_str(), " \t");
		StringVector vec;
		const char* word;
		while ((word = tok())) vec.push_back(word);

		std::string r, e;
		if (vec.size() < 2) continue;
		if (vec.size() == 2) {
			r = vec[0];
			e = vec[1];
		}
		else {

			r = vec[0];
			int i = 1;
			// проходим все русские слова
			while ((unsigned char)vec[i][0] > 128 && i < vec.size()) i++;
			if (i == vec.size() - 1)
				e = vec[i];
			else continue;
		}

		// лемматизируем

		clock_t     ltime1 = clock();
		DwordVector r_id, e_id;
		MorphHolderRus.string_to_ids(r.c_str(), r_id, true);
		MorphHolderEng.string_to_ids(e.c_str(), e_id, true);
		clock_t ltime2 = clock();

		for (int R = 0; R < r_id.size(); R++)
			for (int E = 0; E < e_id.size(); E++)
				smallDictSet.insert(std::make_pair(r_id[R], e_id[E]));
	}
	SmallDict.close();
	std::cerr << "Done" << smallDictSet.size() << "\n";
	return smallDictSet;
}


size_t write_other_small_dict_pair(const small_dict_t& small_dict, const small_dict_t& used_pairs, std::ofstream& out) {
	/*
	 проходим по малому словарю, если пара из этого словаря
	 не была записана в выходной файл, пишем с частотой пять
	 берутся только существительные, глаголы, прил. и нар,
	*/
	size_t written = 0;
	for (small_dict_t::iterator shira_it = small_dict.begin(); shira_it != small_dict.end(); ++shira_it)
	{
		if (used_pairs.count(*shira_it)) continue;
		CFormInfo rp = MorphHolderRus.id_to_paradigm(shira_it->first);
		CFormInfo ep = MorphHolderEng.id_to_paradigm(shira_it->second);

		BYTE r_pos = MorphHolderRus.m_pGramTab->GetPartOfSpeech(rp.GetAncode(0).c_str());
		BYTE e_pos = MorphHolderEng.m_pGramTab->GetPartOfSpeech(ep.GetAncode(0).c_str());
		BYTE t_pos = 0xff;

		if (r_pos == NOUN && e_pos == eNOUN)t_pos = 0;

		if (r_pos == ADJ_FULL && e_pos == eADJ)t_pos = 2;
		if (r_pos == ADJ_SHORT && e_pos == eADJ)t_pos = 2;

		if (r_pos == ADV && e_pos == eADV)t_pos = 3;
		if (r_pos == PREDK && e_pos == eADV)t_pos = 3;

		if (r_pos == INFINITIVE && e_pos == eVERB)t_pos = 1;

		if (t_pos == 0xff) continue;

		uint32_t id = ep.GetParadigmId();
		out.write((char*)&(id), sizeof(uint32_t));
		id = rp.GetParadigmId();
		out.write((char*)&(id), sizeof(uint32_t));
		uint16_t  freq = 5;
		out.write((char*)&(freq), sizeof(unsigned short));

		std::string rus = MorphHolderRus.id_to_string(shira_it->first);
		std::string eng = MorphHolderEng.id_to_string(shira_it->second);

		written++;
	}
	return written;
}

void update_word_map_by_file(std::string filename, std::map<word_pair, int>& word_map) {
	std::ifstream in(filename);
	assert(in.is_open());
	int sent_no = 0;
	while (true) {
		char buff[2][4096];
		in.getline(buff[0], 4095);
		in.getline(buff[1], 4095);
		if (!in) break;
		const char* word;
		StringTokenizer r_tok(convert_from_utf8(buff[0], morphRussian).c_str(), " \t");
		StringTokenizer e_tok(convert_from_utf8(buff[1], morphRussian).c_str(), " \t");
		std::vector<CFormInfo> e_id, r_id;
		while ((word = r_tok()))
		{
			std::vector<CFormInfo> p;
			std::string w = word;
			if (MorphHolderRus.m_pLemmatizer->CreateParadigmCollection(false, w, false, false, p))
				r_id.insert(r_id.end(), p.begin(), p.end());

		}
		while ((word = e_tok()))
		{
			std::vector<CFormInfo> p;
			std::string w = word;
			if (MorphHolderEng.m_pLemmatizer->CreateParadigmCollection(false, w, false, false, p))
				e_id.insert(e_id.end(), p.begin(), p.end());
		}

		for (int r = 0; r < r_id.size(); r++)
			for (int e = 0; e < e_id.size(); e++)
				if (BinaryDictionary.HavePair(e_id[e].GetParadigmId(), r_id[r].GetParadigmId()))
				{
					word_map[word_pair(e_id[e].GetParadigmId(), r_id[r].GetParadigmId())]++;
					break;
				}
	}
	in.close();
}


std::vector<std::string> read_file_list(const char* file_name) {
	std::vector<std::string> v;
	std::ifstream file_list(file_name);
	if (!file_list.is_open())
	{
		throw CExpc("cannot open output file %s", file_name);
	};
	std::string s;
	while (std::getline(file_list, s))
	{
		Trim(s);
		v.push_back(s);
	}
	return v;
}

size_t write_out_file(const long maxFreq, const std::map<word_pair, int>& word_map, std::ofstream& out) {
	size_t written = 0;

	for (auto it = word_map.begin(); it != word_map.end(); ++it)
	{
		std::string rus = MorphHolderRus.id_to_string(it->first.r);
		std::string eng = MorphHolderEng.id_to_string(it->first.e);
		out.write((char*)&(it->first.e), sizeof(uint32_t));
		out.write((char*)&(it->first.r), sizeof(uint32_t));

		uint16_t freq = it->second;
		/*
		  частота не должна превышать  USHRT_MAX, поэтому
		  нужно нормализовать частоту
		*/
		if (maxFreq > USHRT_MAX)
			freq *= USHRT_MAX / maxFreq;
		out.write((char*)&(freq), sizeof(unsigned short));

		written++;
	}
	return written;
}

int  mymain(int argc, char **argv)
{
	init_dicts();
	if (argc != 4)
	{
		throw CExpc ("bad number of arguments");
	}

	auto small_dict = read_small_dict(argv[3]);
	std::ofstream out;

	std::cerr << "Parsing aligned sentences\n";
	std::string file_list = argv[1];
	std::string mainFolder = GetPathByFile(file_list.c_str());

	const char *out_str = argv[2];
	out.open(out_str, std::ios::binary);
	if (!out.is_open())
	{
		throw CExpc("cannot open output file %s", out_str);
	};
	std::map<word_pair, int> word_map;
	for (auto file_name: read_file_list(file_list.c_str())) {
		update_word_map_by_file(mainFolder + file_name, word_map);
	}

	/*
	  вычисляю максимальную частоту и увеличиваю
      частоту пары слов, которая была найдена в малом словаре
	*/
	long MaxFreq = 0;
	small_dict_t small_dict_used;
	std::map<word_pair,int>::iterator it = word_map.begin();
	for(; it != word_map.end(); ++it)
	{
		if(small_dict.count(std::make_pair(it->first.r, it->first.e)))
		{
			it->second *= 3;
			if(it->second < 6) it->second = 6;
			small_dict_used.insert(std::make_pair(it->first.r, it->first.e));
		}
		if (it->second > MaxFreq)
			MaxFreq =  it->second;
	};

	size_t written = write_out_file(MaxFreq, word_map, out);
	written += write_other_small_dict_pair(small_dict, small_dict_used, out);
	std::cerr << "Written " << written << std::endl;

	out.close();
	
	
	return 0;

};

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


