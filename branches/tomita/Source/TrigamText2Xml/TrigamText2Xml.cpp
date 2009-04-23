// TrigamText2Xml.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void PrintUsageAndExit()
{
	fprintf (stderr,"TrigramText2Xml, creates morph-xml files by plain-texts\n");
	fprintf (stderr,"Usage: TrigramText2Xml <trigram-config> <input-file-list>\n");
	fprintf (stderr,"Example: Trigram partofspeech.config moshkow.con\n");
	exit(1);
};


int main(int argc, char* argv[])
{
	return 0;
}

