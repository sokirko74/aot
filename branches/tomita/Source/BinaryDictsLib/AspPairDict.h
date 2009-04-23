// BinaryDictionary.h : Declaration of the CBinaryDictionary

#ifndef __ABINARYDICTIONARY_H_
#define __ABINARYDICTIONARY_H_
#pragma warning(disable:4786)
#pragma warning(disable:4530)


#include "../common/utilit.h"





struct asp_pair
{
	DWORD NonPerf;
	DWORD Perf;

	asp_pair()		{};

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


class CAspPairDict 
{
	

	//--------------
	std::vector<asp_pair> pair_vec1;
	std::vector<asp_pair> pair_vec2;
	//--------------
	typedef std::vector<asp_pair>::iterator Iter;
	typedef std::vector<asp_pair>::const_iterator CIter;


public:
	CAspPairDict();
	bool Load();
	//!Get Perfective verbs by NonPerfective one. Return number of such verbs.
	size_t nonperf2perf(UINT nonperf, DwordVector &res) const;
	//!Get NonPerfective verbs by Perfective one. Return number of such verbs.
	size_t perf2nonperf(UINT perf, DwordVector &res) const;

};

#endif //__BINARYDICTIONARY_H_
