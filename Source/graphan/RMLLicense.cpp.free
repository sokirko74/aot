#include "string.h"
#include <stdio.h>
#include "../common/utilit.h"


int CalculateCode(const char* s)
{
    return 1; // dummy
};


bool CheckPersonalLicense()
{
	//ErrorMessage ("CheckPersonalLicense");
	string Path;
	try {
		Path  = GetIniFilePath();
		Path += "/rml.lic";
		//ErrorMessage (Path);
	}
	catch (...)
	{
		return false;
	};
	FILE * fp  = fopen (Path.c_str(), "r");
	if (!fp) return false;
	char buffer[1000];
	fgets(buffer, 1000, fp);
	string q = buffer;
	Trim(q);
	if (strlen(buffer) < 3) return false;
	int Code;
	fscanf (fp, "%i", &Code);
	fclose (fp);

	if (Code != CalculateCode(q.c_str())) return false;
	return true;
};
