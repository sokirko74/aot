// ClauseBuilder.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f ClauseBuilderps.mk in the project directory.

#include "stdafx.h"

#include "resource.h"
#include <initguid.h>
#include "SynAn.h"

#include "SynAn_i.c"
#include "COMSentencesCollection.h"

#include "COMSentence.h"
#include "COMWord.h"
#include "COMGroup.h"
#include "COMHomonym.h"
#include "COMRelation.h"
#include "COMClauseType.h"
#include "ComRelationIterator.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_Sentence, CCOMSentence)
OBJECT_ENTRY(CLSID_Word, CCOMWord)
OBJECT_ENTRY(CLSID_Clause, CCOMClause)
OBJECT_ENTRY(CLSID_Group, CCOMGroup)
OBJECT_ENTRY(CLSID_ClauseVariant, CCOMClauseVariant)
OBJECT_ENTRY(CLSID_Relation, CCOMRelation)
OBJECT_ENTRY(CLSID_Homonym, CCOMHomonym)
OBJECT_ENTRY(CLSID_ClauseRoot, CCOMClauseType)
OBJECT_ENTRY(CLSID_SentencesCollection, CCOMSentencesCollection)
OBJECT_ENTRY(CLSID_SyntaxUnit, CCOMUnit)
OBJECT_ENTRY(CLSID_RelationsIterator, CCOMRelationsIterator)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_SYNANLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}


#include "COMClause.h"
#include "COMGroup.h"
#include "COMClauseVariant.h"
#include "ComRelation.h"
#include "COMHomonym.h"
#include "COMClauseType.h"
#include "Unit.h"
#include "ComRelationIterator.h"

