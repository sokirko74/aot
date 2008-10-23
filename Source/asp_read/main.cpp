#pragma warning(disable:4786)
#include "../common/utilit.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "../common/MorphologyHolder.h"



static const char *default_path = "ASP.txt";
CMorphologyHolder MorphHolderRus;

//----------------------------------------------------------------------------------------
int get_id(std::string str, DwordVector &res, bool is_left)
{
	
    vector<CFormInfo> ParadigmCollection;

	string s = str;
	if (!MorphHolderRus.m_pLemmatizer->CreateParadigmCollection(false, s, false, false,  ParadigmCollection))
		throw CExpc(Format("Cannot lemmatize %s by Russian lemmatizer" , str.c_str()));

	for(int i = 0; i < ParadigmCollection.size(); i++)
	{
		const CFormInfo Paradigm = ParadigmCollection[i];
		if(!Paradigm.m_bFound) continue;
		UINT part = MorphHolderRus.m_pGramTab->GetPartOfSpeech(Paradigm.GetSrcAncode().c_str());
		if(part != INFINITIVE)
			continue;
		QWORD gr = MorphHolderRus.m_pGramTab->GetAllGrammems(Paradigm.GetCommonAncode().c_str());
		bool is_perfective = (gr & _QM(rPerfective)) != 0;
		bool is_nonperfective = (gr & _QM(rNonPerfective)) != 0;
		std::string norm = Paradigm.GetWordForm(0);
		for(int i = 0; i < norm.size(); i++)
			norm[i] = norm[i] - 'А' + 'а';
		if(norm != str) continue;

		if(is_perfective && is_nonperfective) 
		{
			std::cerr << "двувидовой:" << str.c_str() << std::endl;
		}
		if(!is_perfective && !is_nonperfective) 
		{
			std::cerr << "нет граммемы вида:" << str.c_str() << std::endl;
		};
		
		if(is_left && is_nonperfective || !is_left && is_perfective)
			res.push_back(Paradigm.GetParadigmId());
	}
	return res.size();
}


//----------------------------------------------------------------------------------------
/**
This class is a storage of Aspect Pairs database.
The database is a binary file of the following structure:
UINT -- number of pairs
UINT UINT -- first pair
.........
UINT UINT -- last pair

First index in pair is ParadigmId of NonPerfective verb, the second index - 
is ParadigmId of Perfective verb.

*/
class AspectPairs{
	//--------------
	struct asp_pair{
		UINT NonPerf;
		UINT Perf;
		asp_pair(UINT n, UINT p)
			:NonPerf(n), Perf(p){}
		struct Less1{
			bool operator()(const asp_pair &p1, const asp_pair &p2) const{
				return p1.NonPerf < p2.NonPerf;
			}
		};

		struct Less2{
			bool operator()(const asp_pair &p1, const asp_pair &p2) const{
				return p1.Perf < p2.Perf;
			}
		};
	};
	//--------------
	std::vector<asp_pair> pair_vec1;
	std::vector<asp_pair> pair_vec2;
	//--------------
	UINT read_uint(std::ifstream& in){
		UINT data;
		in.read((char*)&data, sizeof(data));
		return data;
	}
	//--------------
	typedef std::vector<asp_pair>::iterator Iter;
public:
	//! Read Aspect Database from file 'path'.
	AspectPairs(const char *path){
		std::ifstream in(path, std::ios::binary);
		if(!in.is_open()){
			throw CExpc("Can not load file");
		}
		UINT size = read_uint(in);
		pair_vec1.reserve(size);
		for(int i = 0; i < size; i++){
			UINT NonPerf = read_uint(in);
			UINT Perf = read_uint(in);
			pair_vec1.push_back(asp_pair(NonPerf, Perf));
		}
		in.close();
		pair_vec2 = pair_vec1;
		std::sort(pair_vec1.begin(), pair_vec1.end(), asp_pair::Less1());
		std::sort(pair_vec2.begin(), pair_vec2.end(), asp_pair::Less2());
	}
	//--------------
	//!Get Perfective verbs by NonPerfective one. Return number of such verbs.
	int nonperf2perf(UINT nonperf, std::vector<UINT> &res)
	{
		std::pair<Iter, Iter> range = std::equal_range(
			pair_vec1.begin(), pair_vec1.end(),
			asp_pair(nonperf, 0), asp_pair::Less1());

		for(Iter i = range.first; i != range.second; ++i)
			res.push_back(i->Perf);
		return res.size();
	}
	//--------------
	//!Get NonPerfective verbs by Perfective one. Return number of such verbs.
	int perf2nonperf(UINT perf, std::vector<UINT> &res)
	{
		std::pair<Iter, Iter> range = std::equal_range(
			pair_vec2.begin(), pair_vec2.end(),
			asp_pair(0, perf), asp_pair::Less2());

		for(Iter i = range.first; i != range.second; ++i)
			res.push_back(i->NonPerf);
		return res.size();
	}
};

//----------------------------------------------------------------------------------------

int main(int argc, char ** argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "bad number of arguments\n");
		return 1;
	}

	if (!MorphHolderRus.LoadGraphanAndLemmatizer(morphRussian))
	{
		fprintf(stderr, "cannot load Russian morphology\n");
		return 1;
	}

	try
	{
		DwordVector left,right;

		{	// reading

			std::ifstream in(argv[1]);
			assert(in.is_open());
			while(true)
			{
				std::string l,r;
				bool l_ast = false, r_ast = false;
				
				in >> l >> r;
				if(in.eof()) break;

				if(l[l.size() - 1] == '*') {
					l.erase(l.end() - 1);
					l_ast = true;
				}

				if(r[r.size() - 1] == '*') {
					r.erase(r.end() - 1);
					r_ast = true;
				}

				DwordVector l_vec, r_vec;

				get_id(l, l_vec, true);
				get_id(r, r_vec, false);

				std::cout << l.c_str() << " " << r.c_str() << l_vec.size() << '\t' << r_vec.size()<< std::endl;
				for(int i = 0; i < l_vec.size(); i++)
				{
					for(int j = 0; j < r_vec.size(); j++){
						left.push_back(l_vec[i]);
						right.push_back(r_vec[j]);
						std::cout << l.c_str() << " " << r.c_str() << " " << l_vec[i] << " " << r_vec[j] << std::endl;
					}
				}
			}
			in.close();
		}


		{ // writing
			std::ofstream bin_out(argv[2], std::ios::binary);
			assert(bin_out.is_open());
			if (!bin_out.is_open())
			{
				std::cerr << "Cannot open output file "<< argv[2]<< std::endl;
				return  -1;

			};
			
			for(int i = 0; i < left.size(); i++)
			{
				UINT data = left[i];
				bin_out.write((char *)&data, sizeof(UINT));
				data = right[i];
				bin_out.write((char *)&data, sizeof(UINT));
			}
			bin_out.close();
		};
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

	
	
	return 0;
}
