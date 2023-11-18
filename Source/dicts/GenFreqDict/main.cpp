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
#include "morph_dict/agramtab/EngGramTab.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"
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
	*/
	small_dict_t smallDictSet;

	std::cerr << "Opening small dictionary\n";
	std::ifstream inp(filename);
	assert(inp.is_open());
	std::string line;
	while (std::getline(inp, line) {
		auto items = split_string(line, '\t');
		auto r = items[0];
		auto e = items[1];
		DwordVector r_id = MorphHolderRus.GetLemmaIds(r);
		DwordVector e_id = MorphHolderEng.GetLemmaIds(e);
		for (auto r : r_id)
			for (auto e : e_id)
				smallDictSet.insert(std::make_pair(r, e));
	}
	inp.close();
	std::cerr << "Done" << smallDictSet.size() << "\n";
	return smallDictSet;
}


CFormInfo id_to_paradigm(CMorphanHolder& holder, long id) const
{
	CFormInfo Res;
	if (!holder.m_pLemmatizer->CreateParadigmFromID(id, Res))
		throw CExpc(Format("cannot get lemma  by id %i", id));
	return Res;
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
		CFormInfo rp = id_to_paradigm(MorphHolderRus, shira_it->first);
		CFormInfo ep = id_to_paradigm(MorphHolderEng, shira_it->second);

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

	
		written++;
	}
	return written;
}

void update_word_map_by_file(std::string filename, std::map<word_pair, int>& word_map) {
	std::ifstream in(filename);
	assert(in.is_open());
	int sent_no = 0;
	while (true) {
		std::string r_str, e_str;
		std::getline(in, r_str);
		std::getline(in, e_str);
		StringTokenizer r_tok(buff[0], " \t");
		StringTokenizer e_tok(buff[1], " \t");
		std::vector<uint32_t> e_id, r_id;
		const char* word;
		while ((word = r_tok()))
		{
			auto ids = MorphHolderRus.GetWordFormIds(word);
			r_id.insert(r_id.end(), ids.begin(), ids.end());
		}
		while ((word = e_tok()))
		{
			auto ids = MorphHolderEng.GetWordFormIds(word);
			e_id.insert(e_id.end(), ids.begin(), ids.end());
		}

		for (auto r : r_id)
			for (auto e: e_id)
				if (BinaryDictionary.HavePair(e, r))
				{
					word_map[word_pair(e, r)]++;
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
	std::string mainFolder = GetParentPath(file_list.c_str());

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
	catch (std::exception e)
	{
		std::cerr << "exception occurred: " << e.what() << "\n";
		return 1;
	}
	catch (...)
	{
		std::cerr << "unkown exception occurred\n";
		return 1;
	}


}


