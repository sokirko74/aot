// graphtest3View.cpp : implementation of the CSemanticStrView class
//

#include "stdafx.h"
#include "..\RossDev\RossDev.h"


#include "SemanticStrDoc.h"
#include "resource.h"
#include "ProgressForm.h"
#include "SemanticStrView.h"
#include <atlbase.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern Tcl_Interp* theInterp;

/////////////////////////////////////////////////////////////////////////////
// CSemanticStrView

IMPLEMENT_DYNCREATE(CSemanticStrView, CView)

BEGIN_MESSAGE_MAP(CSemanticStrView, CView)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CSemanticStrView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CSemanticStrView construction/destruction

CSemanticStrView::CSemanticStrView() //!: m_EngStr(*(CSemanticStructure*)this)
{
	// TODO: add construction code here
    m_tkwin=0;
	m_tkhwnd=0; 	
//!  m_EngStr.SetAutoDeleteRossHolders(false);

}



void CSemanticStrView::OnDraw(CDC* pDC)
{
	CSemanticStrDoc* pDoc = GetDoc();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}
CSemanticStrDoc* CSemanticStrView::GetDoc() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSemanticStrDoc)));
	return (CSemanticStrDoc*)m_pDocument;
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

/////////////////////////////////////////////////////////////////////////////
// CSemanticStrView message handlers








void CSemanticStrView::ShowGraph() 
{
	char s[200];
	sprintf (s, "$GT($main,graph) draw");
    Tcl_Eval(theInterp,s);
};


void SwitchGraph(const char* tkname) 
{
	char s[2000];

    sprintf (s, "set main %s", (const char*)tkname);
    int retcode = Tcl_Eval(theInterp,s);

};

extern int TclWindowCounter;
BOOL CSemanticStrView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
   m_Menu.LoadMenu(IDR_ROSSDETYPE);
   pParentWnd->SetMenu( &m_Menu );
   char s[2000];
   CRect R = rect;
   //R.right = rect.right + 100;
   pParentWnd->SetWindowPos(0, 120, 120, 840, 500,0);
	
   BOOL ok = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, R, pParentWnd, nID, pContext);

   if (ok) {

		int retcode;
		CString winhnd, cmd;

		m_tkname.Format(".tk%d",++TclWindowCounter);   // name this window .tk1 .tk2 etc

		winhnd.Format("0x%08X",m_hWnd);
		cmd.Format("toplevel %s -use %s",m_tkname,winhnd);
        strcpy (s, cmd);
		retcode = Tcl_Eval(theInterp,s);
		if (retcode!=TCL_OK) {
			Tcl_SetResult(theInterp,"Cannot create TK child window",TCL_VOLATILE);
			return ok;
		}

		Tk_Window tkmain = Tk_MainWindow(theInterp);
		if (tkmain) {
			
			//get the tk window token
			strcpy (s, m_tkname);
			m_tkwin = Tk_NameToWindow(theInterp, s, tkmain);
			Tcl_DoOneEvent(TCL_ALL_EVENTS|TCL_DONT_WAIT);   //force window creation
			
			// get the HWND
			cmd.Format("winfo id %s",m_tkname);
			strcpy (s, cmd);
			retcode = Tcl_Eval(theInterp,s);
			if (retcode==TCL_OK) {
				int i;
				retcode = Tcl_GetInt(theInterp,theInterp->result,&i);
				if (retcode==TCL_OK) {
				  m_tkhwnd = (HWND)i;
				}
			}
			SwitchGraph(m_tkname);

			CString cmd;
            cmd.Format("source $env(GRAPHLET_DIR)/lib/graphscript/ross/semstruct.tcl");
            strcpy (s, cmd);
            retcode = Tcl_Eval(theInterp,s);
	        if (retcode!=TCL_OK) {
				AfxMessageBox (theInterp->result);
				return FALSE;
			};

            cmd.Format("initialize_graphic $main");
            strcpy (s, cmd);
            retcode = Tcl_Eval(theInterp,s);
	        if (retcode!=TCL_OK) {
				AfxMessageBox (theInterp->result);
				return FALSE;
			};

			

		}
	}
	return ok;
	
}

void CSemanticStrView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if (m_tkwin) {
		CRect clrect;
		GetClientRect(clrect);
		Tk_MoveResizeWindow(m_tkwin,0,0,clrect.Width(),clrect.Height());
		char s[2000];
		sprintf (s, "$GT($main,canvas) configure -height %u -width %u", clrect.Width()-100);;
		Tcl_Eval(theInterp,s);
		Tcl_Eval(theInterp,"ResizeChildControls");

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
    CString cmd;
	cmd.Format("destroy %s",m_tkname);
	char s[1000];
	strcpy (s, cmd);
	Tcl_Eval(theInterp,s);

	CView::OnDestroy();
	
	// TODO: Add your message handler code here

}

LRESULT CSemanticStrView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// pass some messages thru to Tk child window
	if (m_tkhwnd) {
		switch (message) {
			case WM_KEYDOWN: ::SendMessage(m_tkhwnd,message,wParam,lParam); break;
			case WM_KEYUP: ::SendMessage(m_tkhwnd,message,wParam,lParam); break;
			case WM_CHAR:
            case WM_COPY:
			case WM_CLEAR:
			case WM_CUT: 
			case WM_GETTEXT: 
			case WM_SETFOCUS: ::SendMessage(m_tkhwnd,message,wParam,lParam); break;
			//case WM_PASTE: PasteClipboard(); break;

 				
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

	wchar_t s  [5000];
	if ( GlobalSize(hMem) > 4999) 
	{
		AfxMessageBox (" Text is too big!");
	};

	CopyMemory( s, hMem, GlobalSize(hMem));
	s[GlobalSize(hMem)] = 0;
    CString Q = s;
	char cmd[5200];
	strcpy (cmd, "$main.controls.mainEntry insert 0.0 \"");
    for (size_t i=0; i < Q.GetLength();i++)
		if (s[i] == '"')
			strcat (cmd, "\\\"");
		else
		{   size_t l = strlen(cmd);
			cmd[l] = Q[i];
			cmd[l+1] = 0; };

    strcat (cmd, "\"");
    if (Tcl_Eval(theInterp,cmd)!=0)
		AfxMessageBox ("Cannot set value");
	CloseClipboard();

}

void CSemanticStrView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	SwitchGraph(m_tkname); 
	//GetParent()->SetMenu( &m_Menu );
	
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	CRossDevApp* App =  (CRossDevApp*)::AfxGetApp();
	App->m_pMainWnd->SendMessage(WM_MDISETMENU, (WPARAM)0, (LPARAM)m_Menu.m_hMenu);
	App->m_pMainWnd->DrawMenuBar();

}





static void log(string  s)
{
	string FileName = "rossdev.log";

	try {
		string log_path  = GetRegistryString( "Software\\Dialing\\Logs\\Main" );
		FileName = log_path + FileName;
	}
	catch (...) {
	};

    FILE* fp  = fopen (FileName.c_str(), "a");
	fprintf (fp, "%s\n", s.c_str());
	fclose(fp);
}
void CSemanticStrView::BuildTclGraph(CString S)
{  
   log (string("\nBuildTclGraphn"));
   if (S.IsEmpty()) return;

   char* str = new char[S.GetLength()+1]; 
   assert (str);
   string CurrCommand;

   try {
	 strcpy  (str, S);
	 char* s = str;
	 for (;;)
	 {
		char* q  = strchr(s, 1);
		if (!q) break;
		*q = 0;
		assert (strlen(s) > 0) ;
		int retcode = Tcl_Eval(theInterp,s);
		if (retcode != 0) 
		{
			char q[5000];
			sprintf (q, "Cannot interpret TCL Code \"%s\". Proceed?", s);
			int Result = ::MessageBox(0, q, "Seman", MB_YESNO);
			if (Result == IDYES)
			break;
		};
		s = q+1;
	}
   }
	catch (...)
	{
		::MessageBox(0, "Cannot interpret TCL Code", "Seman", MB_OK);
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
			AfxMessageBox ("Graphan is busy (получение минус перечня)");
			return TCL_OK;
	};

    CString text = "";
    if (argv[2] != 0) text = argv[2];
	text.TrimLeft();
	text.TrimRight();
	if (text.IsEmpty()) 
		return TCL_OK;
	CSemanticStrDoc* Doc = FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate());
	if (Doc->m_bBusy) return TCL_OK;
	Doc->m_bBusy = true;
	CSemanticStrView& SemStr = *((CSemanticStrView*)Doc->GetView());
	string Graph;
	if (!GetSemBuilder().m_RusStr.GetSyntaxTreeByText((const char*)text, atoi(argv[3]), Graph))
	{
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
		WORD UnitNo  = atoi (argv[3]);
		CRossDoc* RossDoc = Doc->GetView()->FindRossDoc(Type);
		if (RossDoc)
		{
			if (UnitNo >= RossDoc->GetRoss()->GetUnitsSize())  return TCL_OK;
			GlobalOpenArticle(RossDoc,UnitNo);
		}
	}
	catch (...) {
		AfxMessageBox ("Cannot show article");
	};

	return TCL_OK;

}


int fc_argc;
char** fc_argv;
bool ThreadFinish;
CString fc_Graph;
bool bAnswer = false;

UINT FindSituationsInThread    ( LPVOID pParam )
{
	bAnswer = false; 
	log (string("\nFindSituationsInThread"));
	int argc = fc_argc; 
	char** argv = fc_argv;

	try {
		CRossDevApp* App =  (CRossDevApp*)::AfxGetApp();
		if  (App->m_bGraphanIsBusy)
		{
			AfxMessageBox ("Graphan is busy (получение минус перечня)");
			return TCL_OK;
		};
		string text = "";
		if (argv[2] != 0) text = argv[2];
		Trim(text);
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
			if (index != string::npos)
			{
				string LemmasToReplace = text.substr(index+2);
				text.erase(index);
				GetSemBuilder().m_RusStr.SetLemmasToReplace(LemmasToReplace.c_str());
			};
		}
		string Question;
		{
			int index = text.find("#mem");
			if (index != string::npos)
			{
				Question = text.substr(index+4);
				text.erase(index);
				bAnswer = true;
			};

		}

        
        string Result;
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
			fc_Graph = Result.c_str();
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
		AfxMessageBox (" Semantics has crushed!");
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

	  string Sent = GetSemBuilder().Answer();

	  ::GlobalOpenReport(Sent.c_str(), "Ответ:");
	  Doc->m_bBusy = false;

	  return TCL_OK;

}


int FindSituations    (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])

{
	log (string("\nFindSituations"));
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
			AfxMessageBox (" FindSituations!");

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
	  string Graph;
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

	  string Sent;
	  if (GetSemBuilder().BuildSentence(Sent))
	  {
		::GlobalOpenReport(Sent.c_str(), "Перевод:");
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

	  string Sent;
	  if (GetSemBuilder().SyntRusSentence(Sent))
	  {
		  ::GlobalOpenReport(Sent.c_str(), "Русский синтез:");
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





int Update    (ClientData clienData, 
			   Tcl_Interp* interp, 
			   int argc, char* argv[])

{
/*	FindDoc<CSemanticStrDoc>(argv[1], GetSemanticStructureTemplate())->GetView()->Update();
*/
	return TCL_OK;
};



CString CSemanticStrView::GetNormalizedRossPath(DictTypeEnum RossType) const
{
	CAllRossesHolder* Trans = GetSemBuilder().m_RusStr.m_pData;
	const CString _strPathName  = Trans->GetRossPath(RossType).c_str();
	CString strPathName = _strPathName;
	CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
	strPathName.MakeLower();
	assert(pRossDocTemplate);
	strPathName.Replace('/','\\');
	return strPathName;
}


CRossDoc* CSemanticStrView::FindRossDoc(DictTypeEnum RossType)
{
	CString strPathName = GetNormalizedRossPath(RossType);

	CDocTemplate* pRossDocTemplate = GetRossDocTemplate();
	assert(pRossDocTemplate);

	CDocument* pDoc;
	POSITION pos = pRossDocTemplate->GetFirstDocPosition();
	while( pos )
	{
		pDoc = pRossDocTemplate->GetNextDoc(pos);	
		CString strPathNameDoc = pDoc->GetPathName();
		strPathNameDoc.MakeLower();
		if( strPathNameDoc == strPathName )
		{
			return (CRossDoc*)pDoc;
		}				
	}

	return 0;
}


void CSemanticStrView::OpenRossDocIfNeeded(DictTypeEnum RossType)
{
	try
	{
		if (!FindRossDoc(RossType))
		{
			((CRossDevApp*)AfxGetApp())->m_OpenRossDocReadOnly = true;
			GetRossDocTemplate()->OpenDocumentFile(GetNormalizedRossPath(RossType));
			((CRossDevApp*)AfxGetApp())->m_OpenRossDocReadOnly = false;
		};
	}
	catch(...)
	{
		CString Mess = "Cannot open " + GetNormalizedRossPath(RossType);
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
		AfxMessageBox ("Cannot open some semantic dictionary .");
		return;
	};
};

	// семантических словарей 



void  CSemanticStrView::GetJavaGraph()
{
	int retcode = Tcl_Eval(theInterp,"GetJavaGraph");
	string s = theInterp->result;
	TRACE (s.c_str());
};
