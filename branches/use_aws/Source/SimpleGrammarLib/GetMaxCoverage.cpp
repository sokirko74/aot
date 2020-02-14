#include  "SimpleGrammarLib.h"
#include "AhoKorasick.h"



inline int IsUnambiguous (const vector< COccurrence >& Occurrences, size_t PeriodNo)
{
	return		(	(PeriodNo+1 ==	Occurrences.size())
					||	(Occurrences[PeriodNo+1].first >= Occurrences[PeriodNo].second)
				)
			&&	(		(PeriodNo ==	0)
					||	(Occurrences[PeriodNo-1].second <= Occurrences[PeriodNo].first)
				);
};

/*
SolveAmbiguityRecursive finds the most longest non-intersected set of periods,
which  includes period A.

This function recieves the following:
1. A base set of periods  "Occurrences", which has intersections. 
2. A position in the set of periods "Start". from which an ambiguous ( = with intersections)
part starts;


The output parameter are:
1.  "BestOccurrences" contains all occurrences, which are included in the solutions with the maximal weights
2.  "MaxRightBorder" is  the maximal right border, to which extends the ambigous set of periods.
MaxRightBorder can be 
	1. the end of the whole text;
	2. the start of the first unambiguous period;
	3. the point which doesn't belong to any period.
The return value is coverage of this maximal solution;
*/

size_t GetMaxCoverageRecursive (const vector< COccurrence >& Occurrences, size_t Start, set<size_t>& BestOccurrences, size_t& MaxRightBorder)
{
	// if this period has no, intersection  then exit
	if (IsUnambiguous(Occurrences, Start)) 
	{
		BestOccurrences.insert(Start);
		return 0;
	};

	// find the first period, which intersects with period Start, say First, and collect
	// all periods which have intersecion with intersection of Start and First.
	// Therefore we find the maximal subset which includes period Start
	// and each two members of this subset have an intersection.
	vector<size_t> InconsistentPeriods;
	

	// "EndIntersection" is the end of the common intersecion, it is not 
	// obligatory to check the start point of the common intersection due the order in the periods  
	size_t EndIntersection = Occurrences[Start].second;
	size_t i=Start;
	for (; i < Occurrences.size(); i++)
	{
		if (Occurrences[i].first < EndIntersection)
		{
			InconsistentPeriods.push_back(i);
			// initializing  EndInterection
			EndIntersection = min(Occurrences[Start].second, Occurrences[i].second);
			if (MaxRightBorder <  Occurrences[i].second)
				MaxRightBorder = Occurrences[i].second;

		}
		else
			break;
	};


	/*
		going through all possible variants of continuing this  solution,
		i.e. cycling through the   InconsistentPeriods.
	*/

	size_t MaxCoverage = 0;

	set<size_t> BestOccurrencesInner;

	for (i=0; i < InconsistentPeriods.size(); i++)
	{
		size_t  k = InconsistentPeriods[i];
		
		set<size_t> CurrBestOccurrences;
		CurrBestOccurrences.insert(k);
		size_t Coverage = Occurrences[k].second - Occurrences[k].first;
	
		//  disabling all periods which have the "left" intersection
		// with  period k, i.e. the have intersection and the start point
		// of the  period k is less or equal than the start point of these periods.
		size_t  j;
		for (j=k+1; j < Occurrences.size(); j++)
			if (	Occurrences[j].first >=  Occurrences[k].second 
				&&	(Occurrences[j].first < MaxRightBorder)
				)
			{
				// The first non-intersected period is found,
				// we should include it to current solution and get
				// the  coverage  of  this variant
				Coverage += GetMaxCoverageRecursive(Occurrences, j,  CurrBestOccurrences, MaxRightBorder);
				break;
			};

		if	(Coverage >  MaxCoverage)
		{
			MaxCoverage = Coverage;
			BestOccurrencesInner = CurrBestOccurrences;
		}
		else
			if	(Coverage ==  MaxCoverage)
			{
				BestOccurrencesInner.insert(CurrBestOccurrences.begin(), CurrBestOccurrences.end());

			};
	
	};
	BestOccurrences.insert( BestOccurrencesInner.begin(), BestOccurrencesInner.end());
	return MaxCoverage;
};

bool IsLessBySize (const COccurrence& O1, const COccurrence& O2)
{
	return O1.second - O1.first  < O2.second - O2.first;
};
bool IsPartOf (const COccurrence& O1, const COccurrence& O2)
{
	return		(O2.first <= O1.first)
			&&	(O1.second <= O2.second);
};

void GetMaxCoverage (vector< COccurrence >& Occurrences)
{
	if (Occurrences.empty()) return;
	
	sort (Occurrences.begin(), Occurrences.end());

	//  find all groups which are more than 10 in length and delete all groups, which intersects with them
	{
		set<COccurrence> MaxOccurrences;
		COccurrence MaxOcc = *max_element(Occurrences.begin(), Occurrences.end(), IsLessBySize);
		while (MaxOcc.second - MaxOcc.first > 10)
		{
			MaxOccurrences.insert(MaxOcc);
			COccurrence NewMaxOcc;
			NewMaxOcc.first = NewMaxOcc.second = 0;
			for (size_t i=0; i < Occurrences.size(); i++)
				if	(MaxOccurrences.find(Occurrences[i]) == MaxOccurrences.end())
					if	(		(Occurrences[i].first <  MaxOcc.first && MaxOcc.first <  Occurrences[i].second)
							||	(Occurrences[i].first <  MaxOcc.second && MaxOcc.second <  Occurrences[i].second)
						)
					{
						//  found an intersection
						Occurrences.erase(Occurrences.begin()+i);
						i--;
					}
					else
						if (IsLessBySize(NewMaxOcc, Occurrences[i]))
						{
							//  found a new maximal element
							NewMaxOcc = Occurrences[i];
						};

			MaxOcc = NewMaxOcc;
		};
	};


	set<size_t> BestOccurrences;

	for (size_t i=0; i < Occurrences.size(); i++)
	{
		set<size_t> CurrBestOccurrences;
		size_t MaxRightBorder = Occurrences[i].second;
		GetMaxCoverageRecursive(Occurrences,i, CurrBestOccurrences, MaxRightBorder);
		BestOccurrences.insert(CurrBestOccurrences.begin(), CurrBestOccurrences.end());

		int max_occur_no = *max_element(CurrBestOccurrences.begin(), CurrBestOccurrences.end());

		for (i++; i < Occurrences.size(); i++)
			if (Occurrences[i].first >= Occurrences[max_occur_no].second)
				break;

		i--;
	}

	for (int i=Occurrences.size()-1; i>=0; i--)
		if (BestOccurrences.find(i) == BestOccurrences.end())
			Occurrences.erase(Occurrences.begin()+i);

};




