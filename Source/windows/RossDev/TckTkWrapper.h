#pragma once
#include <windows.h>
#include "morph_dict/common/utilit.h"
#include <tcl.h>
#include <tk.h>


struct CTopTkWindow {
	std::string   m_tkname;
	Tk_Window m_tkwin;
	HWND      m_tkhwnd;
	CTopTkWindow() {
		m_tkwin = 0;
		m_tkhwnd = 0;
	}
};

class CTclTkWrapper {
public:
	Tcl_Interp* TclInterp;
	int TclMajor;
	int TclMinor;
	int TclWindowCounter; 

	CTclTkWrapper() {
		TclInterp = NULL;
		TclMajor = 0;
		TclMinor = 0;
		TclWindowCounter = 0;
	}

	inline void RunTcl(std::string cmd) {
		char buffer[10240];
		assert(cmd.length() < 10240);
		strcpy(buffer, cmd.c_str());
		if (Tcl_Eval(TclInterp, buffer) != TCL_OK) {
			std::string mess = Format("tcl command failed: %s, error message: %s", cmd.c_str(), TclInterp->result);
			throw CExpc(mess);
		};
	}

	inline int InitializeTcl() {
		TclInterp = Tcl_CreateInterp();
		if (TclInterp == NULL)
		{
			throw CExpc("Could not create tcl interpreter!");
		}

		RunTcl(Format("set tcl_library \"%s/Bin/Lib/tcl8.0\"", GetRmlVariable().c_str()));
		if (Tcl_Init(TclInterp) == TCL_ERROR)
		{
			throw CExpc(Format("Tcl_Init failed:\n%s\n", Tcl_GetStringResult(TclInterp)));
		}

		RunTcl(Format("source \"%s/Bin/Lib/tcl8.0/init.tcl\"", GetRmlVariable().c_str()));
		RunTcl("info library");
		std::string s = TclInterp->result;
		int i = s.find("/tcl");
		if (i == -1) {
			throw CExpc("bad tcl version format");
		}
		sscanf(s.c_str() + i, "/tcl%d.%d", &TclMajor, &TclMinor);
		if (TclMajor < 8) {
			throw CExpc("tcl version must be greater than 8");
		}
	};

	inline int InitializeTk() {
		RunTcl(Format("set tk_library \"%s/Bin/Lib/tk%d.%d\"", GetRmlVariable().c_str(), TclMajor, TclMinor));

		if (Tk_Init(TclInterp) != TCL_OK) {
			throw CExpc("Tk_init failed");
		}
	};
	CTopTkWindow UseTopWindow(HWND hwnd) {
		CTopTkWindow w;
		std::string winhnd = Format("0x%08X", hwnd);
		w.m_tkname= Format(".tk%d", ++TclWindowCounter);   // name this window .tk1 .tk2 etc
		RunTcl(Format("toplevel %s -use %s", w.m_tkname.c_str(), winhnd.c_str()));
		Tk_Window tkmain = Tk_MainWindow(TclInterp);
		if (!tkmain) {
			throw CExpc("cannot find main tk window");
		}

		//get the tk window token
		char s[100];
		strcpy(s, w.m_tkname.c_str());
		w.m_tkwin = Tk_NameToWindow(TclInterp, s, tkmain);
		Tcl_DoOneEvent(TCL_ALL_EVENTS | TCL_DONT_WAIT);   //force window creation

	    // get the HWND
		RunTcl(Format("winfo id %s", w.m_tkname.c_str()));
		int i;
		if (Tcl_GetInt(TclInterp, TclInterp->result, &i) != TCL_OK) {
			throw CExpc("cannot read winfo result");
		}
		w.m_tkhwnd = (HWND)i;
		return w;
	}
};

