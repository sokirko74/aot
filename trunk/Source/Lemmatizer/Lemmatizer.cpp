// Lemmatizer.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f Lemmatizerps.mk in the project directory.


#include "StdAfx.h"
#include "lemmatizer_resource.h"
#include <initguid.h>
#include "Lemmatizer.h"
#include "ComPlmLineCollection.h"

#include "Lemmatizer_i.c"

CComModule _Module;


BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_Paradigm, CCOMParadigm)
OBJECT_ENTRY(CLSID_ParadigmCollection, CCOMParadigmCollection)
OBJECT_ENTRY(CLSID_LemmatizerRussian, CCOMLemmatizerRussian)
OBJECT_ENTRY(CLSID_LemmatizerEnglish, CCOMLemmatizerEnglish)
OBJECT_ENTRY(CLSID_LemmatizerGerman, CCOMLemmatizerGerman)
OBJECT_ENTRY(CLSID_PLMLineCollection, CCOMPLMLineCollection)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
   if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_LEMMATIZERLib);
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
