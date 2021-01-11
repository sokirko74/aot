#include "../../TckTkWrapper.h"

int main(int argc, const char* argv[])
{
	CTclTkWrapper W;
	W.IniatializeTcl();
	W.RunTcl("puts {tcl: hello, world}");
	W.InitializeTk();
	return 0;
}