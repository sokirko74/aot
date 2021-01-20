#include "../../TckTkWrapper.h"

int main(int argc, const char* argv[])
{
	CTclTkWrapper W;
	W.InitializeTcl();
	W.RunTcl("puts {tcl: hello, world}");
	W.InitializeTk();
	return 0;
}