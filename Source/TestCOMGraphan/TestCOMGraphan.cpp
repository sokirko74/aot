/*
 This example loads the input file into graphematics and prints
 all found Russian words.
*/

#include <stdio.h>
#include "string.h"
#include <fstream>
#include <string>
#include <iostream>
#include <io.h>
#import "../graphan/graphan.tlb"


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "TestComGraphan, Test for Dialing Graphematical Analysis(COM)(www.aot.ru)\n";
		std::cout << "Usage: TestComGraphan <file>\n";
		std::cout << "where file is an text or html file\n";
		return 1;
	};
	// init COM
	CoInitialize(NULL);

	GRAPHANLib::IGraphmatFilePtr piGraphan;

	// creating COM-object
	HRESULT hr = piGraphan.CreateInstance(__uuidof(GRAPHANLib::GraphmatFile));
	if (FAILED(hr))
	{
		std::cerr << "Graphan is not registered\n";
		CoUninitialize();
		return 1;
	};

	// loading dictionaries 
	hr = piGraphan->LoadDicts();

	piGraphan->Language = 1;

	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);

	std::string inputFile = argv[1];
	assert(access(inputFile.c_str(), 0) != -1);

	std::cout << "Loading file " << inputFile << "\n";
	piGraphan->LoadFileToGraphan(inputFile.c_str());


	// get the number of rows in the graphematical table
	size_t Count = piGraphan->GetLineCount();
	std::cout << "Found Units " << Count << "\n";

	// print all words which are written in Russian ABC
	std::ofstream outp(inputFile + ".tokens");
	for (size_t i = 0; i < Count; i++) {
		if (piGraphan->HasDescr(i, GRAPHANLib::ORLE) == TRUE)
		{
			std::string s = (const char*)piGraphan->GetWord(i);
			outp << s << "\n";
		};
	}
	outp.close();
	piGraphan = 0;
	CoUninitialize();
	return 0;
}


