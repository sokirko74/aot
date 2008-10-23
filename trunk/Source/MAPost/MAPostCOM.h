// MAPostCOM.h : Declaration of the CCOMMAPost

#ifndef __MAPOST_H_
#define __MAPOST_H_


class CCOMMAPost;


#include "resource.h"       // main symbols
#include "../MAPostLib/PostMorphInterface.h" 

/////////////////////////////////////////////////////////////////////////////
// CCOMMAPost
class ATL_NO_VTABLE CCOMMAPost : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMMAPost, &CLSID_MAPost>,
	public IDispatchImpl<IMAPost, &IID_IMAPost, &LIBID_MAPOSTLib>
{
	CPostMorphInteface* m_pPostMorphInterface;
public:
	CCOMMAPost();
	~CCOMMAPost();
	
DECLARE_REGISTRY_RESOURCEID(IDR_MAPOST)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMMAPost)
	COM_INTERFACE_ENTRY(IMAPost)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IMAPost
public:
	STDMETHOD(Init)(int Language, /*[in]*/IUnknown* RusLemmatizer, IUnknown* RusGramTab);
	STDMETHOD(ProcessData)(IUnknown *piInputTextItems, IUnknown ** piOutputTextItems);
	STDMETHOD(get_bCanChangeInputText) (BOOL *pVal);
	STDMETHOD(put_bCanChangeInputText) (BOOL pVal);
	STDMETHOD(get_bHumanFriendlyOutput) (BOOL *pVal);
	STDMETHOD(put_bHumanFriendlyOutput) (BOOL pVal);


};






#endif //__MAPOST_H_
