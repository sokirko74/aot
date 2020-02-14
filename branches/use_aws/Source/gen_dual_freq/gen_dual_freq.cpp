// sort.cpp : Defines the entry point for the console application.
//

#pragma warning(disable:4786)
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <ctime>
#include <algorithm>

#include "..\common\util_classes.h"
#include "../FreqDictionary/ParadigmPairFreq.h"
#include "../AgramtabLib/eng_consts.h"
#include "../common/MorphologyHolder.h"

CMorphologyHolder MorphHolderRus;

template<int max_size>
inline bool get_line_(std::istream &is, std::string &s)
{
	char buff[max_size];
	is.getline(buff, max_size - 1);
	if(!is) return false;
	s = buff;
	return true;
}

inline bool get_line(std::istream &is, std::string &s)
{
	return get_line_<10240>(is, s);
}




void string_to_id(const char *str, DwordVector &ids, DwordVector &pos_vec)
{
	bool is_capital = isupper((BYTE)str[0]) != 0;

		
	vector<CFormInfo > ParadigmCollection;
	
	string Word = str;
	if (!MorphHolderRus.m_pLemmatizer->CreateParadigmCollection(false, Word, is_capital, ParadigmCollection))
	{
		fprintf (stderr,"cannot lemmatize %s\n", str);
		return;
	}
	
		
	for(int i = 0; i < ParadigmCollection.size(); i++)
	{
		const CFormInfo& Paradigm = ParadigmCollection[i];
		if(!Paradigm.m_bFound) continue;
		BYTE pos = MorphHolderRus.m_pGramTab->GetPartOfSpeech(Paradigm.GetAncode(0).c_str());
		if(ParadigmCollection.size() == 1 || std::find(pos_vec.begin(), pos_vec.end(), pos) != pos_vec.end())
			ids.push_back(Paradigm.GetParadigmId());
	}
}




struct fff{
	std::string label;
	DwordVector pos1;
	DwordVector pos2;
	int count;
	struct  w_pair: public std::pair<long,long>{
		int count;
		w_pair(std::pair<long,long> p)
			:std::pair<long,long>(p), count(1)
		{}
	};

	std::set<w_pair> w_pair_set;
	typedef std::set<w_pair>::iterator iterator;

	fff():count(0){}
};






int main(int argc, char* argv[])
{
try 
{
	if(argc != 3)
	{
		fprintf (stderr, "bad number of arguments\n");
		return 1;
	}
	if (!MorphHolderRus.LoadGraphanAndLemmatizer(morphRussian))
	{
		fprintf(stderr, "cannot load Russian morphology\n");
		return 1;
	}


	fff MVa_s, A_s, E_s;

	MVa_s.label = "MVa";
	A_s.label = "A";
	E_s.label = "E";

	MVa_s.pos1.push_back(eVERB);
	MVa_s.pos1.push_back(eVBE);
	MVa_s.pos2.push_back(eADV);

	A_s.pos1.push_back(eADJ);
	A_s.pos2.push_back(eNOUN);

	E_s.pos1.push_back(eADV);
	E_s.pos2.push_back(eVERB);
	E_s.pos2.push_back(eVBE);


	std::vector<fff> fff_vec;
	fff_vec.push_back(MVa_s);
	fff_vec.push_back(E_s);
	fff_vec.push_back(A_s);


	assert(argc == 2);
	std::ifstream in(argv[1]);
	assert(in.is_open());

	std::string line;
	int line_no = 0;
	while(get_line(in, line))
	{
		line_no++;
		if(line_no % 10000 == 0) std::cerr << ".";

		StringTokenizer tok(line.c_str(), " \t");
		tok();

		for(int f = 0; f < fff_vec.size(); f++)
		{
			fff &ff = fff_vec[f];
			if(ff.label != tok.val()) continue;

			// we have the right link name found
			tok();
			tok();
			const char *w1 = tok();
			const char *w2 = tok();
			assert(w1 && w2);

			DwordVector id1, id2;
			string_to_id(w1, id1, ff.pos1);
			string_to_id(w2, id2, ff.pos2);

			for(int i1 = 0; i1 < id1.size(); i1++){
				for(int i2 = 0; i2 < id2.size(); i2++)
				{
					std::pair<long,long> p = std::make_pair(id1[i1],id2[i2]);

					fff::iterator it = ff.w_pair_set.find(p);
					if(it == ff.w_pair_set.end()){
						ff.w_pair_set.insert(p);
					}else{
						fff::w_pair q = *it;
						q.count++;
						ff.w_pair_set.erase(it);
						ff.w_pair_set.insert(q);
					}
				}
			}

			ff.count++;
			break;
		}		
	}
	
	
	std::cout << std::endl;
	for(int f = 0; f < fff_vec.size(); f++)
	{
		std::vector<CParadigmPairAndFreq> pp_vec;
		for(fff::iterator it = fff_vec[f].w_pair_set.begin(); it != fff_vec[f].w_pair_set.end(); ++it)
		{
			CParadigmPairAndFreq p;
			p.m_EngParadigmId = it->first;

			// writing to Russian paradigm, because we are lazy to create a new structure  for frequences
			p.m_RusParadigmId = it->second;
			if (it->count  > 0xffff)
				p.m_Freq = 0xffff;
			else
				p.m_Freq = it->count;

			pp_vec.push_back(p);
		}
		std::sort(pp_vec.begin(), pp_vec.end());
		WriteVector("link" + fff_vec[f].label + ".bin", pp_vec);
		std::cerr << pp_vec.size() << " items written" << std::endl;
	}
	
	return 0;
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
