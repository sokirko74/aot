#pragma warning(disable:4786)
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include "../common/util_classes.h"
#include "../common/MorphologyHolder.h"


static const char *default_path_suff = "troiki_suff";
static const char *default_path_pref = "troiki_pref";

CMorphologyHolder MorphHolderRus;




//----------------------------------------------------------------------------------------
StringSet suffixes_set, prefixes_set;
struct rec{
	std::string w1, w2, r;
	rec(std::string &a, std::string &b, std::string &c)
		:w1(a), w2(b), r(c){}
};

std::vector<rec> suff_vec, pref_vec;



//----------------------------------------------------------------------------------------
void handle(std::istream &in, std::ostream &out, StringSet &rest_set, std::vector<rec> &rec_vec)
{
	StringSet little_roots;
	little_roots.insert("ад");
	little_roots.insert("юг");
	little_roots.insert("ют");
	little_roots.insert("як");
	little_roots.insert("ям");
    
	while(true){
		char Line[1000];
		in.getline(Line, 1000);
		std::string s = Line;
		if(in.eof()) break;
		StringTokenizer tok(s.c_str(), " ;");
		std::string left = tok();
		Trim(left);

		std::string right = tok();
		Trim(right);
		if(left.size() < 3 || right.size() < 3){
			std::cerr << left.c_str() << '\t' << right.c_str() << std::endl;
			if(little_roots.count(left) <= 0) continue;
		}
		std::string rest = tok();
		Trim(rest);
		rest_set.insert(rest);
		rec_vec.push_back(rec(left, right, rest));
	}

	StringVector rest_vec;
	out << rest_set.size() << std::endl;
	for(StringSet::iterator it = rest_set.begin(); it != rest_set.end(); ++it){
		rest_vec.push_back(*it);
		out << (*it).c_str() << std::endl;
	}
	std::cout << "Written " << rest_set.size() << " afixes" << std::endl;



	int pair_count = 0;
	for(int i = 0; i < rec_vec.size(); i++){
		rec &r = rec_vec[i];
		//out << r.w1.c_str() << "\t" << r.w2.c_str() << '\t';
		UINT index = std::lower_bound(rest_vec.begin(), rest_vec.end(), r.r) - rest_vec.begin();
		//out << index << endl;

		DwordVector id1, id2;
		MorphHolderRus.string_to_ids(r.w1.c_str(), id1, true);
		MorphHolderRus.string_to_ids(r.w2.c_str(), id2, true);
		for(int i1 = 0; i1 < id1.size(); i1++)
			for(int i2 = 0; i2 < id2.size(); i2++){
				out.write((char*)&(id1[i1]), sizeof(UINT));
				out.write((char*)&(id2[i2]), sizeof(UINT));
				out.write((char*)&index, sizeof(UINT));
				pair_count++;
			//	cout << r.w1.c_str() << " " << r.w2.c_str() << " "
			//		<< id1[i1] << " " << id2[i2] << endl;
			}
	}
	std::cout << "Written " << pair_count << " pairs" << std::endl;
}


/**
Этот класс -- хранилище бызы данных словаря словобразования.
Бинарный файл устроен так:
Сначала идет текстовая часть:
N  -- число
афикс1
...
афиксN
Потом стоит символ 0 и далее в бинарном виде идут тройки UINTов
id1 id2 index

id1 и id2 -- ParadigmId слов из пары. 
index -- индекс афикса.

*/
class AfixDerivDict{
	struct deriv_pair{
		UINT id1, id2;
		UINT index;
		deriv_pair(UINT a, UINT b, UINT i = 0)
			:id1(a),  id2(b), index(i){}
		struct Less1{
			bool operator()(const deriv_pair &p1, const deriv_pair &p2) const{
				return p1.id1 < p2.id1;
			}
		};
		struct Less2{
			bool operator()(const deriv_pair &p1, const deriv_pair &p2) const{
				return p1.id2 < p2.id2;
			}
		};

	};
	std::vector<deriv_pair> vec1, vec2;
	typedef std::vector<deriv_pair>::iterator Iter;
protected:
	StringVector pref; // prefixes or suffixes -- afix
public:
	//! read the database
	AfixDerivDict(std::istream &in){
		UINT pref_size;
		in >> pref_size;
		pref.reserve(pref_size);
		int i;
		for(i = 0; i < pref_size; i++){
			std::string s;
			in >> s;
			pref.push_back(s);
		}
		while(in.get() != 0);
		while(true){
			UINT id1, id2, index;
			in.read((char*)&id1, sizeof(UINT));
			in.read((char*)&id2, sizeof(UINT));
			in.read((char*)&index, sizeof(UINT));
			if(in.eof()) break;
			vec1.push_back(deriv_pair(id1, id2, index));
		}
		vec2 = vec1;
		std::sort(vec1.begin(), vec1.end(), deriv_pair::Less1());
		std::sort(vec2.begin(), vec2.end(), deriv_pair::Less2());
	}

	//! find target by source. Return number of found targets.
	int SourceToTarget(UINT id, DwordVector &ids, DwordVector &afixes){
		std::pair<Iter, Iter> range = std::equal_range
			(vec1.begin(), vec1.end(), deriv_pair(id, 0), deriv_pair::Less1());

		for(Iter it = range.first; it != range.second; ++it){
			ids.push_back(it->id2);
			afixes.push_back(it->index);
		}
		return ids.size();		
	}

	//! find source by target. Return number of found sources.
	int TargetToSource(UINT id, DwordVector &ids, DwordVector &afixes){
		std::pair<Iter, Iter> range = std::equal_range
			(vec2.begin(), vec2.end(), deriv_pair(0, id), deriv_pair::Less2());
		for(Iter it = range.first; it != range.second; ++it){
			ids.push_back(it->id1);
			afixes.push_back(it->index);
		}
		return ids.size();		
	}


	//! Number of afixes
	int AfixCount(){
		return pref.size();
	}
	//! Get Afix of source by number. 
	virtual std::string GetSourceAfix(int i) = 0;
	//! Get Afix of target by  nuber
	virtual std::string GetTargetAfix(int i) = 0;


	// 
};


class PrefixDerivDict: public AfixDerivDict{
public:
	PrefixDerivDict(std::istream &in)
		:AfixDerivDict(in){}
	//! Get Afix of source by number. 
	std::string GetSourceAfix(int){
		return "";
	}
	//! Get Afix of target by  nuber
	std::string GetTargetAfix(int i){
		return pref.at(i);
	}
};


class SuffixDerivDict: public AfixDerivDict{
public:
	SuffixDerivDict(std::istream &in)
		:AfixDerivDict(in){}
	//! Get Afix of source by number. 
	std::string GetSourceAfix(int i){
		std::string &s = pref.at(i);
		s.find('-');
		return s.substr(0, s.find('-'));
	}
	//! Get Afix of target by  nuber
	std::string GetTargetAfix(int i){
		std::string &s = pref.at(i);
		s.find('-');
		return s.substr(s.find('-') + 1, s.size());
	}
};

void create_nest(UINT id, std::set<UINT> &id_set, AfixDerivDict &d1, AfixDerivDict &d2)
{
	int size = id_set.size();
	id_set.insert(id);
	if(size == id_set.size())
		return;
	DwordVector ids, afixes;
	d1.SourceToTarget(id, ids, afixes);
	d2.SourceToTarget(id, ids, afixes);
	d1.TargetToSource(id, ids, afixes);
	d2.TargetToSource(id, ids, afixes);
	for(int i = 0; i < ids.size(); i++)
		create_nest(ids[i], id_set, d1, d2);
}



//----------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	assert(argc == 3);
	if (!MorphHolderRus.LoadGraphanAndLemmatizer(morphRussian))
	{
		fprintf(stderr, "cannot load Russian morphology\n");
		return 1;
	}

	try
	{
		std::ifstream suff(argv[1]);
		assert(suff.is_open());
		fprintf (stderr,"Opening outfile %s\n", argv[2]);
		std::ofstream suff_out(argv[2], std::ios::binary);
		assert(suff_out.is_open());
		handle(suff, suff_out, suffixes_set, suff_vec);
		suff.close();
		suff_out.close();
		
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










