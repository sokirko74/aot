// graphtest3View.cpp : implementation of the CSemanticStrView class
//

#include "StdAfx.h"
#include "RossDev.h"


#include "SemanticStrDoc.h"
#include "resource.h"
#include "ProgressForm.h"
#include "SemanticStrView.h"
#include <atlbase.h>
#include <sstream>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSemanticStrView

IMPLEMENT_DYNCREATE(CSemanticStrView, CView)

BEGIN_MESSAGE_MAP(CSemanticStrView, CView)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CSemanticStrView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()



CSemanticStrView::CSemanticStrView() :  m_TclInterp(((CRossDevApp*)AfxGetApp())->m_TclInterp)
{
}

CSemanticStrDoc* CSemanticStrView::GetDoc() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSemanticStrDoc)));
	return (CSemanticStrDoc*)m_pDocument;
}


BOOL CSemanticStrView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_TopTKWindow = m_TclInterp.UseTopWindow(m_hWnd);
	SwitchMainTkWindow();
	m_TclInterp.RunTcl("source $env(GRAPHLET_DIR)/lib/graphscript/ross/semstruct.tcl");
	m_TclInterp.RunTcl("initialize_graphic $main");
	CView::OnCreate(lpCreateStruct);
	return TRUE;
}


void CSemanticStrView::OnDraw(CDC* pDC)
{
}


/////////////////////////////////////////////////////////////////////////////
// CSemanticStrView diagnostics

#ifdef _DEBUG
void CSemanticStrView::AssertValid() const
{
	CView::AssertValid();
}

void CSemanticStrView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

void CSemanticStrView::ShowGraph() 
{
	m_TclInterp.RunTcl("$GT($main,graph) draw");
};


void CSemanticStrView::SwitchMainTkWindow()
{
	m_TclInterp.RunTcl(Format("set main %s", m_TopTKWindow.m_tkname.c_str()));
};


/*BOOL CSemanticStrView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, uint32_t dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
   m_Menu.LoadMenu(IDR_ROSSDETYPE);
   pParentWnd->SetMenu( &m_Menu );
   CRect R = rect;
   pParentWnd->SetWindowPos(0, 120, 120, 840, 500,0);
	
   BOOL ok = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, R, pParentWnd, nID, pContext);

   if (ok) {
	}
	return ok;
	
}*/

void CSemanticStrView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if (m_TopTKWindow.m_tkwin) {
		CRect clrect;
		GetClientRect(clrect);
		Tk_MoveResizeWindow(m_TopTKWindow.m_tkwin, 0,0,clrect.Width(),clrect.Height());
		m_TclInterp.RunTcl(Format("$GT($main,canvas) configure -height %u -width %u", clrect.Height(), clrect.Width() - 100));
		m_TclInterp.RunTcl("ResizeChildControls");
	}
	
}

BOOL CSemanticStrView::OnEraseBkgnd(CDC* pDC) 
{
	return 0; // let tk do it
}

CSemanticStrView::~CSemanticStrView()
{
};

void CSemanticStrView::OnDestroy() 
{
	m_TclInterp.RunTcl(Format("destroy %s", m_TopTKWindow.m_tkname.c_str()));
	CView::OnDestroy();
}

LRESULT CSemanticStrView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// pass some messages thru to Tk child window
	if (m_TopTKWindow.m_tkhwnd) {
		switch (message) {
			case WM_KEYDOWN: ::SendMessage(m_TopTKWindow.m_tkhwnd,message,wParam,lParam); break;
			case WM_KEYUP: ::SendMessage(m_TopTKWindow.m_tkhwnd,message,wParam,lParam); break;
			case WM_CHAR:
            case WM_COPY:
			case WM_CLEAR:
			case WM_CUT: 
			case WM_GETTEXT: 
			case WM_SETFOCUS: ::SendMessage(m_TopTKWindow.m_tkhwnd,message,wParam,lParam); break;
				
				break;
		}
	}

	return CView::WindowProc(message, wParam, lParam);
}


void CSemanticStrView::PasteClipboard() 
{
	OpenClipboard();
    HGLOBAL hMem = ::GetClipboardData(CF_UNICODETEXT );
    if (!hMem) {        
        CloseClipboard();        
        return;    
    }

	wchar_t wstr  [5000];
	if ( GlobalSize(hMem) > 4999) 
	{
		AfxMessageBox (_T(" Text is too big!"));
	};

	CopyMemory( wstr, hMem, GlobalSize(hMem));
	wstr[GlobalSize(hMem)] = 0;
	std::string utf8 = wstring_to_utf8(std::wstring(wstr));
	std::string str1251 = convert_from_utf8(utf8.c_str(), morphRussian);
    CString Q = str1251.c_str();
	char cmd[5200];
	strcpy (cmd, "$main.controls.mainEntry insert 0.0 \"");
    for (size_t i=0; i < Q.GetLength();i++)
		if (Q[i] == '"')
			strcat (cmd, "\\\"");
		else
		{   size_t l = strlen(cmd);
			cmd[l] = Q[i];
			cmd[l+1] = 0; };

    strcat (cmd, "\"");
	try {
		m_TclInterp.RunTcl(cmd);
	}
	catch (CExpc c) {
		AfxMessageBox(_T("Cannot set value"));
	}
	CloseClipboard();

}

void CSemanticStrView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	SwitchMainTkWindow();
	
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	CRossDevApp* App =  (CRossDevApp*)::AfxGetApp();
	App->m_pMainWnd->SendMessage(WM_MDISETMENU, (WPARAM)0, (LPARAM)m_Menu.m_hMenu);
	App->m_pMainWnd->DrawMenuBar();

}





static void log(std::string  s)
{
	std::string FileName = "rossdev.log";

	try {
		std::string log_path  = GetRegistryString( "Software\\Dialing\\Logs\\Main" );
		FileName = log_path + FileName;
	}
	catch (...) {
	};

    FILE* fp  = fopen (FileName.c_str(), "a");
	fprintf (fp, "%s\n", s.c_str());
	fclose(fp);
}

void CSemanticStrView::BuildTclGraph(std::string s_utf8)
{  
   log (std::string("\nBuildTclGraphn"));
   if (s_utf8.empty()) return;

   char* str = new char[s_utf8.length() + 1];
   assert (str);
   std::string CurrCommand;

   try {
	 strcpy  (str, s_utf8.c_str());
	 char* s = str;
	 for (;;)
	 {
		char* q  = strchr(s, 1);
		if (!q) break;
		*q = 0;
		assert (strlen(s) > 0) ;
		try {
			m_TclInterp.RunTcl(s);
		}
		catch (std::exception& e) {
			auto m = Format("Cannot interpret TCL Code \"%s\". Proceed?", e.what());
			int Result = ::MessageBox(0, _U16(m), _T("Seman"), MB_YESNO);
			if (Result == IDYES)
				break;
		};
		s = q+1;
	}
   }
	catch (...)
	{
		::MessageBox(0, _T("Cannot interpret TCL Code"), _T("Seman"), MB_OK);
	};

	delete str;
};


int CreateSynStr (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])
{
	CRossDevApp* App =  (CRossDevApp*)::AfxGetApp();
	if  (App->m_bGraphanIsBusy)
	{
		AfxMessageBox (_T("Graphan is busy (getting minus list)"));
		return TCL_OK;
	};

    std::string utf8text;
    if (argv[2] != 0) utf8text = argv[2];
	Trim(utf8text);
	if (utf8text.empty())
		return TCL_OK;
	CSemanticStrDoc* Doc = FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate());
	if (Doc->m_bBusy) return TCL_OK;
	Doc->m_bBusy = true;
	std::string Graph;
	auto& rusStr = GetSemBuilder().m_RusStr;
	if (!rusStr.GetSyntaxTreeByText(utf8text, atoi(argv[3]), Graph))
	{
		Doc->m_bBusy = false;
		return TCL_ERROR;
	}
    Doc->GetView()->BuildTclGraph (Graph.c_str());
	Doc->m_bBusy = false;

	return TCL_OK;

}


int ShowArticle (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])
{
	try {
		CSemanticStrDoc* Doc = FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate());
		DictTypeEnum Type = (DictTypeEnum)atoi (argv[2]);
		uint16_t UnitNo  = atoi (argv[3]);
		CRossDoc* RossDoc = ((CRossDevApp*)AfxGetApp())->FindRossDoc(Type);
		if (RossDoc)
		{
			if (UnitNo >= RossDoc->GetRoss()->GetUnitsSize())  return TCL_OK;
			GlobalOpenArticle(RossDoc,UnitNo);
		}
	}
	catch (...) {
		AfxMessageBox (_T("Cannot show article"));
	};

	return TCL_OK;

}


int fc_argc;
char** fc_argv;
bool ThreadFinish;
std::string fc_Graph;
bool bAnswer = false;

UINT FindSituationsInThread(LPVOID pParam )
{
	bAnswer = false; 
	log (std::string("\nFindSituationsInThread"));
	int argc = fc_argc; 
	char** argv = fc_argv;

	try {
		CRossDevApp* App =  (CRossDevApp*)::AfxGetApp();
		if  (App->m_bGraphanIsBusy)
		{
			AfxMessageBox (_T("Graphan is busy (getting minus list)"));
			return TCL_OK;
		};
		std::string text = "";
		if (argv[2] != 0) text = argv[2];
		Trim(text);
		//text = convert_to_utf8(text, morphRussian);
		if (text.empty()) 
		{
			ThreadFinish = true;
			return TCL_OK;
		};

		CSemanticStrDoc* Doc = FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate());
		if (Doc->m_bBusy) return TCL_OK;
		Doc->m_bBusy = true;
		CSemanticStrView& SemStr = *((CSemanticStrView*)Doc->GetView());
		SemStr.OpenAllRosses();
		log ("SemStr.m_RusStr->FindSituations");
		
		{
			int index = text.find("##");
			if (index != std::string::npos)
			{
				std::string LemmasToReplace = text.substr(index+2);
				text.erase(index);
				GetSemBuilder().m_RusStr.SetLemmasToReplace(LemmasToReplace.c_str());
			};
		}
		std::string Question;
		{
			int index = text.find("#mem");
			if (index != std::string::npos)
			{
				Question = text.substr(index+4);
				text.erase(index);
				bAnswer = true;
			};

		}

        
        std::string Result;
		bool bRes = 
			GetSemBuilder().FindSituations(text,
				atoi(argv[5]), 
				(const char*)argv[6],
				atoi(argv[7]),
				atoi(argv[8]),
				argv[10],
				Result);
		if (!bRes)
		{
			ErrorMessage("FindSituations has crushed!");
		}
		else
		{
			fc_Graph = Result;
			if (!Question.empty())
			{
				GetSemBuilder().SaveThisSentence();

				bool bRes = 
						GetSemBuilder().FindSituations(Question.c_str(),
						atoi(argv[5]), 
						(const char*)argv[6],
						atoi(argv[7]),
						atoi(argv[8]),
						(const char*)argv[10],
						Result);
				if (!bRes)
				{
					ErrorMessage("FindSituations has crushed!");
				}

			};
		}

		Doc->m_bBusy = false;
	}
	catch  (...)
	{
		AfxMessageBox (_T(" Semantics has crushed!"));
	};


	ThreadFinish = true;
	return TCL_OK;
};

int AnswerBySavedSentences (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])
{
	  CSemanticStrDoc* Doc = FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate());
	  if (Doc->m_bBusy) return TCL_OK;
	  Doc->m_bBusy = true;
	  
	  CSemanticStrView& SemStrView = *((CSemanticStrView*)Doc->GetView());

	  std::string Sent = GetSemBuilder().Answer();

	  ::GlobalOpenReport(Sent.c_str(), "Answer:");
	  Doc->m_bBusy = false;

	  return TCL_OK;

}


int FindSituations    (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])

{
	log (std::string("\nFindSituations"));
	fc_argc = argc;
	fc_argv = argv;
	ThreadFinish = false;
	try {
		
		CSemanticStrDoc* Doc = FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate());
		if (Doc->m_bBusy) return TCL_OK;
		CSemanticStrView& SemStr = *((CSemanticStrView*)Doc->GetView());
		AfxBeginThread(FindSituationsInThread, 0, THREAD_PRIORITY_NORMAL);
		CProgressForm P;
		P.DoModal();
		Doc->GetView()->BuildTclGraph (fc_Graph);

		if (bAnswer)
			AnswerBySavedSentences(clienData, interp, argc, argv);

	} catch (...) 
	{
			AfxMessageBox (_T(" FindSituations!"));

	};
	return TCL_OK;
};


int GetOtherRelations (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])

{
	return TCL_OK;
};

int TranslateToEnglish    (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])

{
	try
	{
	  CSemanticStrDoc* Doc = FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate());
	  CString sent; 
	  
	  //bool bShowUnusedValencies = (atoi(argv[2]) == 1) ?  true : false;
	  if (Doc->m_bBusy) return TCL_OK;
	  Doc->m_bBusy = true;
	  CSemanticStrView& RusStr = *((CSemanticStrView*)Doc->GetView());
	  Doc->GetView()->OpenAllRosses();
	  std::string Graph;
	  if (GetSemBuilder().TranslateToEnglish(Graph))
	  {
		Doc->GetView()->BuildTclGraph (Graph.c_str());
	  }
	  Doc->m_bBusy = false;
	}
	catch (...)
	{
	};
	return TCL_OK;
};

int BuildSentence (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])
{
	  CSemanticStrDoc* Doc = FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate());
	  if (Doc->m_bBusy) return TCL_OK;
	  Doc->m_bBusy = true;
	  
	  CSemanticStrView& RusStr = *((CSemanticStrView*)Doc->GetView());

	  std::string Sent;
	  if (GetSemBuilder().BuildSentence(Sent))
	  {
		::GlobalOpenReport(Sent.c_str(), "Translation:");
	  }
	  Doc->m_bBusy = false;

	  return TCL_OK;

}

int SyntRusSentence (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])
{
	  CSemanticStrDoc* Doc = FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate());
	  if (Doc->m_bBusy) return TCL_OK;
	  Doc->m_bBusy = true;
	  
	  CSemanticStrView& RusStr = *((CSemanticStrView*)Doc->GetView());

	  std::string Sent;
	  if (GetSemBuilder().SyntRusSentence(Sent))
	  {
		  ::GlobalOpenReport(Sent.c_str(), "Russian synthesis:");
	  }
	  Doc->m_bBusy = false;

	  return TCL_OK;

}



int ClearSavedSentences (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])
{
	  CSemanticStrDoc* Doc = FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate());
	  if (Doc->m_bBusy) return TCL_OK;
	  CSemanticStrView& RusStr = *((CSemanticStrView*)Doc->GetView());
	  GetSemBuilder().ClearSavedSentences();
	  return TCL_OK;

}

int SaveSentence (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])
{
	  CSemanticStrDoc* Doc = FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate());
	  if (Doc->m_bBusy) return TCL_OK;
	  CSemanticStrView& RusStr = *((CSemanticStrView*)Doc->GetView());
	  GetSemBuilder().SaveThisSentence();
	  return TCL_OK;

}







int PasteClipboard (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])

{
	FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate())->GetView()->PasteClipboard();
	return TCL_OK;
};



int ShowMessageMicrosoftWindows(ClientData clienData,
	Tcl_Interp* interp,
	int argc, char* argv[])
{
	AfxMessageBox(_U16(argv[1]));
	return 1;
}

int Update    (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])

{
	return TCL_OK;
};

void CSemanticStrView::OpenRossDocIfNeeded(DictTypeEnum RossType)
{
	CAllRossesHolder* Trans = ((CRossDevApp*)AfxGetApp())->m_SemBuilder.m_RusStr.m_pData;
	std::string dir = GetRossPath(RossType).c_str();
	std::string path = (fs::path(dir) / "config.rcf").string();
	try
	{
		if (!((CRossDevApp*)AfxGetApp())->FindRossDoc(RossType))
		{
			((CRossDevApp*)AfxGetApp())->m_OpenRossDocReadOnly = true;
			GetRossDocTemplate()->OpenDocumentFile(_U16(path));
			((CRossDevApp*)AfxGetApp())->m_OpenRossDocReadOnly = false;
		};
	}
	catch(...)
	{
		CString Mess = CString("Cannot open ") + path.c_str();
		AfxMessageBox (Mess);
	}
}


void  CSemanticStrView::OpenAllRosses() 
{
	try {
		for (int i =0; i < NoneRoss; i++)
		{
			OpenRossDocIfNeeded((DictTypeEnum)i);
		}
	}
	catch (...)
	{
		AfxMessageBox (_T("Cannot open some semantic dictionary."));
		return;
	};
};

	// семантических словарей 



void  CSemanticStrView::GetJavaGraph()
{
	try {
		m_TclInterp.RunTcl("GetJavaGraph");
		std::string s = m_TclInterp.TclInterp->result;
		TRACE(s.c_str());
	}
	catch (CExpc c) {
		AfxMessageBox(_T("failed"));
	}
};
