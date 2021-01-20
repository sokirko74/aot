//================================================================
// =====   COM Test Example for Dialing Lemmatizer (www.aot.ru) ==
// =====   Author: Alexey Sokirko, sokirko@yandex.ru, 2003  =======
//================================================================

#include <stdio.h>
#include <comdef.h>
#include <string>
#include <fstream>
#include <iostream>
#include <assert.h>
#import "../Lemmatizer/Lemmatizer.tlb"


int main(int argc, char* argv[])
{
	::CoInitialize(NULL);

	//  creating and loading Russian dictionary
	LEMMATIZERLib::ILemmatizerPtr piLemmatizer;
	HRESULT hr = piLemmatizer.CreateInstance(__uuidof(LEMMATIZERLib::LemmatizerRussian));
	if (FAILED(hr))
	{
		std::cerr << "Fatal Error: Lemmatizer creation failed";
		return 1;
	}
	// loading dictionary into the memory
	hr = piLemmatizer->LoadDictionariesRegistry();
	if (FAILED(hr))
	{
		std::cerr << "Fatal Error: Lemmatizer loading failed";
		return 1;
	}

	std::string inputFile = argv[1];
	std::cout << "Loading file " << inputFile << "\n";
	std::ifstream inp(inputFile);
	assert(inp.is_open());

	std::ofstream outp(inputFile + ".lemma", std::ios::binary);
	assert(outp.is_open());

	// lemmatizing a word in utf8
	std::string word;
	while (std::getline(inp, word))
	{
		auto piParadigmCollection = piLemmatizer->CreateParadigmCollectionFromForm(word.c_str(), FALSE, FALSE);

		for (int j = 0; j < piParadigmCollection->Count; j++)
		{
			auto p = piParadigmCollection->Item[j];
			outp << p->Norm <<  " " << p->SrcAncode <<  "\n";
		};
		outp << "\n";
	}

	piLemmatizer = 0;
	::CoUninitialize();

}
