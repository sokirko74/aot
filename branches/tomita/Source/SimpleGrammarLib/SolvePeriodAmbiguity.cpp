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
1.  "Solution", which is the maximal possiblle unambiguous subset of  
Occurrences which includes period Start. 
2.  "MaxRightBorder" is  the maximal right border, to which extends the ambigous set of periods.
MaxRightBorder can be 
	1. the end of the whole text;
	2. the start of the first unambiguous period;
	3. the point which doesn't belong to any period.
The return value is coverage of this maximal solution i.e.
length(Solution[0]) + ... length(Solution[Solution.size() - 1])
*/

size_t SolveAmbiguityRecursive (const vector< COccurrence >& Occurrences, size_t Start, vector<size_t>& Solution, size_t& MaxRightBorder)
{
	// if this period has no, intersection  then exit
	if (IsUnambiguous(Occurrences, Start)) 
	{
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
	vector<size_t> SaveSolution = Solution;
	
	for (i=0; i < InconsistentPeriods.size(); i++)
	{
		size_t  k = InconsistentPeriods[i];
		
		size_t Coverage = Occurrences[k].second - Occurrences[k].first;
		vector<size_t> CurrSolution = SaveSolution;

		// try k as a possible  variant of continuing
		CurrSolution.push_back(k);

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
				Coverage += SolveAmbiguityRecursive(Occurrences, j,  CurrSolution, MaxRightBorder);
				break;
			};

		if	(		(Coverage >  MaxCoverage)
				||	(		(Coverage == MaxCoverage)
						&&	(CurrSolution.size() < Solution.size())
					)
			)
		{
			MaxCoverage = Coverage;
			Solution = CurrSolution;
		};
	
	};

	return MaxCoverage;
};


void SolveAmbiguity (vector< COccurrence >& Occurrences)
{
	
	sort (Occurrences.begin(), Occurrences.end());

	for (size_t i=0; i+1 < Occurrences.size(); i++)
		if	(Occurrences[i+1].first < Occurrences[i].second)
		{
			vector<size_t> Solution;

			size_t MaxRightBorder = Occurrences[i].second;
			SolveAmbiguityRecursive(Occurrences,i, Solution, MaxRightBorder);

			assert (!Solution.empty());

			int j=Solution.back()+1;

			for (; j <Occurrences.size(); j++)
				if (Occurrences[j].first >= Occurrences[Solution.back()].second)
					break;

			Occurrences.erase(Occurrences.begin()+Solution.back()+1, Occurrences.begin()+ j);

			
			for (size_t  k=Solution.size() - 1; k>0 ; k--)
			{
				int start = Solution[k-1]+1;
				int end = Solution[k];
				Occurrences.erase(Occurrences.begin()+ start,Occurrences.begin()+ end);
			};
			Occurrences.erase(Occurrences.begin()+ i, Occurrences.begin()+ Solution[0]);
		};

};





