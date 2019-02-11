// TempArticle.h : Declaration of the CTempArticle

#ifndef __COMTEMPARTICLE_H_
#define __COMTEMPARTICLE_H_

#include "../StructDictLib/ross.h"       
#include "../StructDictLib/TextField.h"       
#include "../StructDictLib/TempArticle.h"       

#include "resource.h"       
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE CComTempArticle : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CComTempArticle, &CLSID_TempArticle>,
	public IDispatchImpl<ITempArticle, &IID_ITempArticle, &LIBID_STRUCTDICTLib>,
	public CTempArticle
{
public:

DECLARE_REGISTRY_RESOURCEID(IDR_TEMPARTICLE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CComTempArticle)
	COM_INTERFACE_ENTRY(ITempArticle)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


	STDMETHOD(get_LastError)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(AttachToDictionary)(/*[in]*/IDictionary* Dictionary);
	STDMETHOD(IsPartOf)(/*[in]*/ ITempArticle* Article, /*[in]*/ BOOL UseWildCards, /*[out]*/ BOOL* result);
	STDMETHOD(AddArticle)(/*[in]*/ITempArticle* Article);
	STDMETHOD(IntersectByFields)(/*[in]*/ ITempArticle* Article, /*[retval, out]*/ LONG* result  );
	STDMETHOD(GetCortegesSize)(/*[out]*/ LONG* result);
	STDMETHOD(GetCortege)(LONG i, /*[out]*/ BYTE* FieldNo, /*[out]*/ BYTE* SignatNo, /*[out]*/ BYTE*LevelId, /*[out]*/ BYTE* LeafId, BYTE* BracketLeafId);
	STDMETHOD(GetCortegeItem)(LONG CortegeNo, BYTE PositionInCortege, LONG* ItemId);
	STDMETHOD(IsModified)(/*[out]*/ BOOL* result);
	STDMETHOD(WriteToDictionary)();
	STDMETHOD(BuildCortegeList)();
	STDMETHOD(GetFieldsSize)(/*[out]*/ WORD* result);
	STDMETHOD(MarkUp)();
	STDMETHOD(get_ArticleStr)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ArticleStr)(/*[in]*/ BSTR newVal);
	STDMETHOD(ReadFromDictionary)(/*[in]*/ WORD UnitNo, BOOL VisualOrder, BOOL ReadOnly);
	STDMETHOD(get_Fields)(/*[in]*/ LONG FieldNo, /*[out, retval]*/ ITextField* *pVal);
	STDMETHOD(get_MeanNum)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(get_UnitStr)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ErrorLine)(int* pVal);
};

#endif //__TEMPARTICLE_H_
