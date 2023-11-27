// graphtest3Doc.cpp : implementation of the CHierarchyDoc class
//

#include "StdAfx.h"
#include "morph_dict/common/utilit.h"
#include "HierarchyDoc.h"
#include "Hierarchy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHierarchyDoc

IMPLEMENT_DYNCREATE(CHierarchyDoc, CDocument)

BEGIN_MESSAGE_MAP(CHierarchyDoc, CDocument)
    //{{AFX_MSG_MAP(CHierarchyDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHierarchyDoc construction/destruction


CHierarchyDoc::~CHierarchyDoc()
{
}

BOOL CHierarchyDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CHierarchyDoc serialization





int SaveToRossdev(ClientData clienData,
    Tcl_Interp* interp,
    int argc, char* argv[])
{

    if (argc > 3) {
        interp->result = "Bad usage: random ? range";
        return TCL_ERROR;
    };
    int limit = 0;
    if (argc == 3)
    {
        CHierarchyDoc* D = FindDoc<CHierarchyDoc>(argv[2], GetHierarchyTemplate());
        if (D)
        {
            if (argv[1] == 0) TCL_OK;
            for (int i = 0; i < strlen(argv[1]); i++)
                if (argv[1][i] == '|') argv[1][i] = '\n';
            D->WriteToRoss(argv[1]);
            D->ReadFromRoss(true);
            ((CRossDoc*)D->m_pRossDoc)->SetModifiedFlag();
        };
    };

    return TCL_OK;
};

//int GetNewNode(ClientData clienData,
//    Tcl_Interp* interp,
//    int argc,
//    char* argv[])
//{
//    if (argc == 3)
//    {
//        CHierarchyDoc* D = FindDoc<CHierarchyDoc>(argv[2], GetHierarchyTemplate());
//        if (D)
//        {
//            strcpy(interp->result, D->GetNewNode(argv[1]));
//        };
//    };
//    return TCL_OK;
//};
//



void CHierarchyDoc::Serialize(CArchive& ar)
{
    MySerialize(ar.IsStoring() != 0, false);
}



bool CHierarchyDoc::OpenHierarchy(CRossDoc* pRossDoc, CHierarchyEnum Type)
{

    m_Type = Type;
    m_pRossDoc = pRossDoc->GetRossHolder();

    BOOL bAutoDelete = m_bAutoDelete;
    m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
    CFrameWnd* pFrame = GetDocTemplate()->CreateNewFrame(this, NULL);
    m_bAutoDelete = bAutoDelete;

    if (pFrame == NULL)
    {
        AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
        delete this;       // explicit delete on error
        return FALSE;
    };

    ASSERT_VALID(pFrame);

    switch (Type)
    {
        case SemFet: SetPathName(_T("Semantic features hierarchy")); break;
        case SemRel: SetPathName(_T("Semantic relations hierarchy")); break;
    };

    GetDocTemplate()->InitialUpdateFrame(pFrame, this, TRUE);
    MySerialize(false, false);

    GetView()->ShowGraph();

    return true;
};

/////////////////////////////////////////////////////////////////////////////
// CHierarchyDoc diagnostics

#ifdef _DEBUG
void CHierarchyDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CHierarchyDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHierarchyDoc commands

BOOL CHierarchyDoc::DoFileSave()
{
    try
    {
        if (IsModified())
            return DoSave(m_strPathName);
        else
            return TRUE;
    }
    catch (...)
    {
        ::MessageBox(0, _T("Cannot save Hierarchy"), _T("Error"), MB_OK);
        return FALSE;
    };
};


void CHierarchyDoc::CreateNode(const char* name, CNodePlace p, bool WithoutView)
{
    if (WithoutView)
        CHierarchyHolder::CreateNode(name);
    else {
        POINT point;
        point.x = p.x;
        point.y = p.y;
        GetView()->CreateNode(name, point);
    }
};

void CHierarchyDoc::CreateRelation(int node1, int node2, bool bWork, bool WithoutView)
{
    if (WithoutView)
        CHierarchyHolder::CreateRelation(node1, node2, bWork);
    else
        GetView()->CreateRelation(node1, node2, bWork);
};
