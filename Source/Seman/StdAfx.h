#pragma once 

#define _ATL_APARTMENT_THREADED


#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;

#include <atlcom.h>
#include <comdef.h>
#include "../common/cortege.h"
#include "../common/utilit.h"
#include "../SemanLib/TranslatorHolder.h"
#include "../SemanLib/SemanticsHolder.h"
#include "../SemanLib/HierarchyHolder.h"
