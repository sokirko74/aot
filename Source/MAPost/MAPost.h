

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Oct 23 10:33:13 2008
 */
/* Compiler settings for .\MAPost.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __MAPost_h__
#define __MAPost_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMAPost_FWD_DEFINED__
#define __IMAPost_FWD_DEFINED__
typedef interface IMAPost IMAPost;
#endif 	/* __IMAPost_FWD_DEFINED__ */


#ifndef __MAPost_FWD_DEFINED__
#define __MAPost_FWD_DEFINED__

#ifdef __cplusplus
typedef class MAPost MAPost;
#else
typedef struct MAPost MAPost;
#endif /* __cplusplus */

#endif 	/* __MAPost_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IMAPost_INTERFACE_DEFINED__
#define __IMAPost_INTERFACE_DEFINED__

/* interface IMAPost */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IMAPost;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6AD16946-1802-11D3-8C31-00105A68ADF3")
    IMAPost : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProcessData( 
            /* [in] */ IUnknown *piInputTextItems,
            /* [retval][out] */ IUnknown **piOutputTextItems) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ int Language,
            /* [in] */ IUnknown *Lemmatizer,
            /* [in] */ IUnknown *GramTab) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_bCanChangeInputText( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_bCanChangeInputText( 
            /* [in] */ BOOL pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_bHumanFriendlyOutput( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_bHumanFriendlyOutput( 
            /* [in] */ BOOL pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMAPostVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMAPost * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMAPost * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMAPost * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMAPost * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMAPost * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMAPost * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMAPost * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProcessData )( 
            IMAPost * This,
            /* [in] */ IUnknown *piInputTextItems,
            /* [retval][out] */ IUnknown **piOutputTextItems);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Init )( 
            IMAPost * This,
            /* [in] */ int Language,
            /* [in] */ IUnknown *Lemmatizer,
            /* [in] */ IUnknown *GramTab);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_bCanChangeInputText )( 
            IMAPost * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_bCanChangeInputText )( 
            IMAPost * This,
            /* [in] */ BOOL pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_bHumanFriendlyOutput )( 
            IMAPost * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_bHumanFriendlyOutput )( 
            IMAPost * This,
            /* [in] */ BOOL pVal);
        
        END_INTERFACE
    } IMAPostVtbl;

    interface IMAPost
    {
        CONST_VTBL struct IMAPostVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMAPost_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMAPost_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMAPost_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMAPost_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMAPost_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMAPost_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMAPost_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMAPost_ProcessData(This,piInputTextItems,piOutputTextItems)	\
    (This)->lpVtbl -> ProcessData(This,piInputTextItems,piOutputTextItems)

#define IMAPost_Init(This,Language,Lemmatizer,GramTab)	\
    (This)->lpVtbl -> Init(This,Language,Lemmatizer,GramTab)

#define IMAPost_get_bCanChangeInputText(This,pVal)	\
    (This)->lpVtbl -> get_bCanChangeInputText(This,pVal)

#define IMAPost_put_bCanChangeInputText(This,pVal)	\
    (This)->lpVtbl -> put_bCanChangeInputText(This,pVal)

#define IMAPost_get_bHumanFriendlyOutput(This,pVal)	\
    (This)->lpVtbl -> get_bHumanFriendlyOutput(This,pVal)

#define IMAPost_put_bHumanFriendlyOutput(This,pVal)	\
    (This)->lpVtbl -> put_bHumanFriendlyOutput(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMAPost_ProcessData_Proxy( 
    IMAPost * This,
    /* [in] */ IUnknown *piInputTextItems,
    /* [retval][out] */ IUnknown **piOutputTextItems);


void __RPC_STUB IMAPost_ProcessData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMAPost_Init_Proxy( 
    IMAPost * This,
    /* [in] */ int Language,
    /* [in] */ IUnknown *Lemmatizer,
    /* [in] */ IUnknown *GramTab);


void __RPC_STUB IMAPost_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IMAPost_get_bCanChangeInputText_Proxy( 
    IMAPost * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IMAPost_get_bCanChangeInputText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IMAPost_put_bCanChangeInputText_Proxy( 
    IMAPost * This,
    /* [in] */ BOOL pVal);


void __RPC_STUB IMAPost_put_bCanChangeInputText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IMAPost_get_bHumanFriendlyOutput_Proxy( 
    IMAPost * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IMAPost_get_bHumanFriendlyOutput_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IMAPost_put_bHumanFriendlyOutput_Proxy( 
    IMAPost * This,
    /* [in] */ BOOL pVal);


void __RPC_STUB IMAPost_put_bHumanFriendlyOutput_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMAPost_INTERFACE_DEFINED__ */



#ifndef __MAPOSTLib_LIBRARY_DEFINED__
#define __MAPOSTLib_LIBRARY_DEFINED__

/* library MAPOSTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MAPOSTLib;

EXTERN_C const CLSID CLSID_MAPost;

#ifdef __cplusplus

class DECLSPEC_UUID("6AD16947-1802-11D3-8C31-00105A68ADF3")
MAPost;
#endif
#endif /* __MAPOSTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


