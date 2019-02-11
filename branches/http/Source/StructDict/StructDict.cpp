// StructDict.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f StructDictps.mk in the project directory.

#include "StdPch.h"
#include "resource.h"
#include <initguid.h>
#include "StructDict.h"

#include "StructDict_i.c"
#include "ComDictionary.h"
#include "ComSignat.h"
#include "ComDomen.h"
#include "ComField.h"
#include "ComTempArticle.h"
#include "ComTextField.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_Signat, CComSignat)
OBJECT_ENTRY(CLSID_Dictionary, CComDictionary)
OBJECT_ENTRY(CLSID_Domain, CComDomen)
OBJECT_ENTRY(CLSID_Field, CComField)
OBJECT_ENTRY(CLSID_TempArticle, CComTempArticle)
OBJECT_ENTRY(CLSID_TextField, CComTextField)
END_OBJECT_MAP()


/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{

	if (dwReason == DLL_PROCESS_ATTACH)
    {
       if (time(0) == -1)
	   {
		   ::MessageBox (0,"Structdict objects can be used between January 1, 1970, and January 18, 2038;\n It's too late, too late, late...", "StructDict", MB_OK);
		   return FALSE;

	   };
       _Module.Init(ObjectMap, hInstance, &LIBID_STRUCTDICTLib);
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


