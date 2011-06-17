// GraphmatFile.h : Declaration of the CGraphmatFile

#ifndef __COMGRAPHMATFILE_H_
#define __COMGRAPHMATFILE_H_


#include "../Graphan/graphan.h"       // main symbols
#include "resource.h"       // main symbols
#include "../GraphanLib/GraphmatFile.h"
#include "../GraphanLib/Consent.h"
#include "../GraphanLib/GraphanDicts.h"
/////////////////////////////////////////////////////////////////////////////
// CComGraphmatFile
class ATL_NO_VTABLE CComGraphmatFile : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CComGraphmatFile, &CLSID_GraphmatFile>,
	public IDispatchImpl<IGraphmatFile, &IID_IGraphmatFile, &LIBID_GRAPHANLib>,
	public CGraphmatFile
{
public:

//DECLARE_CLASSFACTORY2(CDialingLicense) 	

DECLARE_REGISTRY_RESOURCEID(IDR_GRAPHMATFILE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CComGraphmatFile)
	COM_INTERFACE_ENTRY(IGraphmatFile)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IGraphmatFile
public:

	STDMETHOD(get_bUseIndention)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_bUseIndention)(/*[in]*/ BOOL newVal);

	STDMETHOD(get_bUseParagraphTagToDivide)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_bUseParagraphTagToDivide)(/*[in]*/ BOOL newVal);

	STDMETHOD(get_bEmptyLineIsSentenceDelim)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_bEmptyLineIsSentenceDelim)(/*[in]*/ BOOL newVal);

	STDMETHOD(get_Language)(/*[out, retval]*/ int *pVal);
	STDMETHOD(put_Language)(/*[in]*/ int newVal);
	STDMETHOD(get_XmlMacSynOutputFile)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_XmlMacSynOutputFile)(/*[in]*/ BSTR newVal);
	STDMETHOD(GetUnitLength)(/*[in]*/ int LineNo, /*[out, retval]*/ BYTE* Offset );
	STDMETHOD(GetUnitOffset)(/*[in]*/ int LineNo, /*[out, retval]*/ int* Offset );
	STDMETHOD(get_bMacSynHierarchy)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_bMacSynHierarchy)(/*[in]*/ BOOL newVal);
	STDMETHOD(FreeDicts)();
	STDMETHOD(get_IsTableLoaded)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(GetOborotNo)(/*[in]*/ UINT LineNo, /*[out]*/ LONG* result);
    STDMETHOD(LoadDicts)();
	STDMETHOD(get_GraOutputFile)(BSTR *pVal);
	STDMETHOD(put_GraOutputFile)( BSTR newVal);
	STDMETHOD(LoadStringToGraphan)(BSTR szBuffer);
	STDMETHOD(GetLine)(UINT LineNo, BSTR* result);
	STDMETHOD(GetRubiconTypeStr)(UINT RubiconTypeNo, BSTR* result);
	STDMETHOD(GetDescriptorStr)(UINT DescriptorNo, BSTR* result);
	STDMETHOD(GetWord)(UINT LineNo, BSTR* s);
	STDMETHOD(GetLineCount)(UINT* Count);
	STDMETHOD(HasDescr)(UINT LineNo,UINT D, BOOL* result);
	STDMETHOD(FreeTable)();
	STDMETHOD(LoadFileToGraphan (BSTR  CommandLine)); 
	STDMETHOD(SetOborDic (IUnknown *Dic));
	STDMETHOD(get_bSubdueWrongMacroSynUnitToMainRoot)(BOOL* pVal);
	STDMETHOD(put_bSubdueWrongMacroSynUnitToMainRoot)(BOOL newVal);
	STDMETHOD(GetTokenLanguage)(int LineNo, int* Language);
};

#endif //__GRAPHMATFILE_H_
