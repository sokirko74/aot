#include <stdio.h>
#include <comdef.h>
#include <string>
#include <fstream>
#include <iostream>
#include <assert.h>
#import "windows/Lemmatizer/Lemmatizer.tlb"


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

	// lemmatizing a word in utf8
	std::string word = "мама";
	auto piParadigmCollection = piLemmatizer->CreateParadigmCollectionFromForm(word.c_str(), FALSE, FALSE);

	for (int j = 0; j < piParadigmCollection->Count; j++)
	{
		auto p = piParadigmCollection->Item[j];
		std::cout << p->Norm <<  " " << p->SrcAncode <<  "\n";
	};
	std::cout << "\n";
	piParadigmCollection = nullptr;

	piLemmatizer = nullptr;
	::CoUninitialize();

}
