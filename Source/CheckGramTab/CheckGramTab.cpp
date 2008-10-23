// CheckGramTab.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../AgramtabLib/RusGramTab.h"
#include "../AgramtabLib/EngGramTab.h"
#include "../AgramtabLib/GerGramTab.h"


void PrintUsage()
{
	printf ("Usage: CheckGramTab  (Russian|English|German)\n");
	printf ("Example: CheckGramTab  Russian\n");
	exit(-1);
};

int main(int argc, char* argv[])
{
	if (argc != 2)
		PrintUsage();
		
	string Lng = argv[1];

	if (Lng == "Russian")
	{
		CRusGramTab P;
		if (!P.LoadFromRegistryAndCheck()) return 1;
	}
	else
		if (Lng == "English")
		{
			CEngGramTab P;
			if (!P.LoadFromRegistryAndCheck()) return 1;
		}
	else
		if (Lng == "German")
		{
			CGerGramTab P;
			if (!P.LoadFromRegistryAndCheck()) return 1;
		}
		else
			PrintUsage();

	printf ("no errors");
	return 0;
}

