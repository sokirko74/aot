

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Oct 23 10:49:31 2008
 */
/* Compiler settings for .\Lemmatizer.idl:
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

#ifndef __Lemmatizer_h__
#define __Lemmatizer_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IParadigm_FWD_DEFINED__
#define __IParadigm_FWD_DEFINED__
typedef interface IParadigm IParadigm;
#endif 	/* __IParadigm_FWD_DEFINED__ */


#ifndef __IParadigmCollection_FWD_DEFINED__
#define __IParadigmCollection_FWD_DEFINED__
typedef interface IParadigmCollection IParadigmCollection;
#endif 	/* __IParadigmCollection_FWD_DEFINED__ */


#ifndef __ILemmatizer_FWD_DEFINED__
#define __ILemmatizer_FWD_DEFINED__
typedef interface ILemmatizer ILemmatizer;
#endif 	/* __ILemmatizer_FWD_DEFINED__ */


#ifndef __IPLMLineCollection_FWD_DEFINED__
#define __IPLMLineCollection_FWD_DEFINED__
typedef interface IPLMLineCollection IPLMLineCollection;
#endif 	/* __IPLMLineCollection_FWD_DEFINED__ */


#ifndef __Paradigm_FWD_DEFINED__
#define __Paradigm_FWD_DEFINED__

#ifdef __cplusplus
typedef class Paradigm Paradigm;
#else
typedef struct Paradigm Paradigm;
#endif /* __cplusplus */

#endif 	/* __Paradigm_FWD_DEFINED__ */


#ifndef __ParadigmCollection_FWD_DEFINED__
#define __ParadigmCollection_FWD_DEFINED__

#ifdef __cplusplus
typedef class ParadigmCollection ParadigmCollection;
#else
typedef struct ParadigmCollection ParadigmCollection;
#endif /* __cplusplus */

#endif 	/* __ParadigmCollection_FWD_DEFINED__ */


#ifndef __LemmatizerRussian_FWD_DEFINED__
#define __LemmatizerRussian_FWD_DEFINED__

#ifdef __cplusplus
typedef class LemmatizerRussian LemmatizerRussian;
#else
typedef struct LemmatizerRussian LemmatizerRussian;
#endif /* __cplusplus */

#endif 	/* __LemmatizerRussian_FWD_DEFINED__ */


#ifndef __LemmatizerEnglish_FWD_DEFINED__
#define __LemmatizerEnglish_FWD_DEFINED__

#ifdef __cplusplus
typedef class LemmatizerEnglish LemmatizerEnglish;
#else
typedef struct LemmatizerEnglish LemmatizerEnglish;
#endif /* __cplusplus */

#endif 	/* __LemmatizerEnglish_FWD_DEFINED__ */


#ifndef __LemmatizerGerman_FWD_DEFINED__
#define __LemmatizerGerman_FWD_DEFINED__

#ifdef __cplusplus
typedef class LemmatizerGerman LemmatizerGerman;
#else
typedef struct LemmatizerGerman LemmatizerGerman;
#endif /* __cplusplus */

#endif 	/* __LemmatizerGerman_FWD_DEFINED__ */


#ifndef __PLMLineCollection_FWD_DEFINED__
#define __PLMLineCollection_FWD_DEFINED__

#ifdef __cplusplus
typedef class PLMLineCollection PLMLineCollection;
#else
typedef struct PLMLineCollection PLMLineCollection;
#endif /* __cplusplus */

#endif 	/* __PLMLineCollection_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_Lemmatizer_0000 */
/* [local] */ 

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_Lemmatizer_0000_0001
    {	idlFinance	= 1,
	idlComputer	= 2,
	idlLiterature	= 4
    } 	idlSubjectEnum;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_Lemmatizer_0000_0002
    {	idlRussian	= 1,
	idlEnglish	= 2,
	idlGerman	= 3
    } 	idlMorphLanguageEnum;



extern RPC_IF_HANDLE __MIDL_itf_Lemmatizer_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_Lemmatizer_0000_v0_0_s_ifspec;

#ifndef __IParadigm_INTERFACE_DEFINED__
#define __IParadigm_INTERFACE_DEFINED__

/* interface IParadigm */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IParadigm;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8D229F36-FD73-11D2-9C9F-00105A68BAF2")
    IParadigm : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParadigmID( 
            /* [retval][out] */ DWORD *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ DWORD *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Ancode( 
            /* [in] */ DWORD pos,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Norm( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Form( 
            /* [in] */ DWORD pos,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TypeAncode( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Founded( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SrcAncode( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SrcNorm( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HomonymWeight( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_WordWeight( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HomonymWeightWithForm( 
            /* [in] */ DWORD pos,
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Accent( 
            /* [in] */ DWORD pos,
            /* [retval][out] */ BYTE *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BaseLength( 
            /* [retval][out] */ UINT *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LemmaPrefixLength( 
            /* [retval][out] */ int *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IParadigmVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IParadigm * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IParadigm * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IParadigm * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IParadigm * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IParadigm * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IParadigm * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IParadigm * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ParadigmID )( 
            IParadigm * This,
            /* [retval][out] */ DWORD *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IParadigm * This,
            /* [retval][out] */ DWORD *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Ancode )( 
            IParadigm * This,
            /* [in] */ DWORD pos,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Norm )( 
            IParadigm * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Form )( 
            IParadigm * This,
            /* [in] */ DWORD pos,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TypeAncode )( 
            IParadigm * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Founded )( 
            IParadigm * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SrcAncode )( 
            IParadigm * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SrcNorm )( 
            IParadigm * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HomonymWeight )( 
            IParadigm * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WordWeight )( 
            IParadigm * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HomonymWeightWithForm )( 
            IParadigm * This,
            /* [in] */ DWORD pos,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Accent )( 
            IParadigm * This,
            /* [in] */ DWORD pos,
            /* [retval][out] */ BYTE *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BaseLength )( 
            IParadigm * This,
            /* [retval][out] */ UINT *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LemmaPrefixLength )( 
            IParadigm * This,
            /* [retval][out] */ int *pVal);
        
        END_INTERFACE
    } IParadigmVtbl;

    interface IParadigm
    {
        CONST_VTBL struct IParadigmVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IParadigm_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IParadigm_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IParadigm_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IParadigm_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IParadigm_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IParadigm_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IParadigm_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IParadigm_get_ParadigmID(This,pVal)	\
    (This)->lpVtbl -> get_ParadigmID(This,pVal)

#define IParadigm_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IParadigm_get_Ancode(This,pos,pVal)	\
    (This)->lpVtbl -> get_Ancode(This,pos,pVal)

#define IParadigm_get_Norm(This,pVal)	\
    (This)->lpVtbl -> get_Norm(This,pVal)

#define IParadigm_get_Form(This,pos,pVal)	\
    (This)->lpVtbl -> get_Form(This,pos,pVal)

#define IParadigm_get_TypeAncode(This,pVal)	\
    (This)->lpVtbl -> get_TypeAncode(This,pVal)

#define IParadigm_get_Founded(This,pVal)	\
    (This)->lpVtbl -> get_Founded(This,pVal)

#define IParadigm_get_SrcAncode(This,pVal)	\
    (This)->lpVtbl -> get_SrcAncode(This,pVal)

#define IParadigm_get_SrcNorm(This,pVal)	\
    (This)->lpVtbl -> get_SrcNorm(This,pVal)

#define IParadigm_get_HomonymWeight(This,pVal)	\
    (This)->lpVtbl -> get_HomonymWeight(This,pVal)

#define IParadigm_get_WordWeight(This,pVal)	\
    (This)->lpVtbl -> get_WordWeight(This,pVal)

#define IParadigm_get_HomonymWeightWithForm(This,pos,pVal)	\
    (This)->lpVtbl -> get_HomonymWeightWithForm(This,pos,pVal)

#define IParadigm_get_Accent(This,pos,pVal)	\
    (This)->lpVtbl -> get_Accent(This,pos,pVal)

#define IParadigm_get_BaseLength(This,pVal)	\
    (This)->lpVtbl -> get_BaseLength(This,pVal)

#define IParadigm_get_LemmaPrefixLength(This,pVal)	\
    (This)->lpVtbl -> get_LemmaPrefixLength(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_ParadigmID_Proxy( 
    IParadigm * This,
    /* [retval][out] */ DWORD *pVal);


void __RPC_STUB IParadigm_get_ParadigmID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_Count_Proxy( 
    IParadigm * This,
    /* [retval][out] */ DWORD *pVal);


void __RPC_STUB IParadigm_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_Ancode_Proxy( 
    IParadigm * This,
    /* [in] */ DWORD pos,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IParadigm_get_Ancode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_Norm_Proxy( 
    IParadigm * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IParadigm_get_Norm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_Form_Proxy( 
    IParadigm * This,
    /* [in] */ DWORD pos,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IParadigm_get_Form_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_TypeAncode_Proxy( 
    IParadigm * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IParadigm_get_TypeAncode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_Founded_Proxy( 
    IParadigm * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IParadigm_get_Founded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_SrcAncode_Proxy( 
    IParadigm * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IParadigm_get_SrcAncode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_SrcNorm_Proxy( 
    IParadigm * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IParadigm_get_SrcNorm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_HomonymWeight_Proxy( 
    IParadigm * This,
    /* [retval][out] */ int *pVal);


void __RPC_STUB IParadigm_get_HomonymWeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_WordWeight_Proxy( 
    IParadigm * This,
    /* [retval][out] */ int *pVal);


void __RPC_STUB IParadigm_get_WordWeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_HomonymWeightWithForm_Proxy( 
    IParadigm * This,
    /* [in] */ DWORD pos,
    /* [retval][out] */ int *pVal);


void __RPC_STUB IParadigm_get_HomonymWeightWithForm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_Accent_Proxy( 
    IParadigm * This,
    /* [in] */ DWORD pos,
    /* [retval][out] */ BYTE *pVal);


void __RPC_STUB IParadigm_get_Accent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_BaseLength_Proxy( 
    IParadigm * This,
    /* [retval][out] */ UINT *pVal);


void __RPC_STUB IParadigm_get_BaseLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigm_get_LemmaPrefixLength_Proxy( 
    IParadigm * This,
    /* [retval][out] */ int *pVal);


void __RPC_STUB IParadigm_get_LemmaPrefixLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IParadigm_INTERFACE_DEFINED__ */


#ifndef __IParadigmCollection_INTERFACE_DEFINED__
#define __IParadigmCollection_INTERFACE_DEFINED__

/* interface IParadigmCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IParadigmCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9FCC72D0-FE0A-11D2-9CA0-00105A68BAF2")
    IParadigmCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long pos,
            /* [retval][out] */ IParadigm **pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IParadigmCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IParadigmCollection * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IParadigmCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IParadigmCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IParadigmCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IParadigmCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IParadigmCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IParadigmCollection * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IParadigmCollection * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IParadigmCollection * This,
            /* [in] */ long pos,
            /* [retval][out] */ IParadigm **pVal);
        
        END_INTERFACE
    } IParadigmCollectionVtbl;

    interface IParadigmCollection
    {
        CONST_VTBL struct IParadigmCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IParadigmCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IParadigmCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IParadigmCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IParadigmCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IParadigmCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IParadigmCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IParadigmCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IParadigmCollection_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IParadigmCollection_get_Item(This,pos,pVal)	\
    (This)->lpVtbl -> get_Item(This,pos,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigmCollection_get_Count_Proxy( 
    IParadigmCollection * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IParadigmCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IParadigmCollection_get_Item_Proxy( 
    IParadigmCollection * This,
    /* [in] */ long pos,
    /* [retval][out] */ IParadigm **pVal);


void __RPC_STUB IParadigmCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IParadigmCollection_INTERFACE_DEFINED__ */


#ifndef __ILemmatizer_INTERFACE_DEFINED__
#define __ILemmatizer_INTERFACE_DEFINED__

/* interface ILemmatizer */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ILemmatizer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BBE83451-FBD6-11D2-9C98-00105A68BAF2")
    ILemmatizer : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateParadigmCollectionFromNorm( 
            /* [in] */ BSTR norm,
            /* [in] */ BOOL capital,
            /* [in] */ BOOL UsePrediction,
            /* [retval][out] */ IParadigmCollection **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateParadigmCollectionFromForm( 
            /* [in] */ BSTR form,
            /* [in] */ BOOL capital,
            /* [in] */ BOOL UsePrediction,
            /* [retval][out] */ IParadigmCollection **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateParadigmFromID( 
            /* [in] */ long id,
            /* [retval][out] */ IParadigm **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadDictionariesRegistry( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UseStatistic( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UseStatistic( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadStatisticRegistry( 
            /* [in] */ idlSubjectEnum subj) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MaximalPrediction( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CheckABC( 
            /* [in] */ BSTR Word,
            /* [retval][out] */ BOOL *Result) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILemmatizerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILemmatizer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILemmatizer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILemmatizer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ILemmatizer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ILemmatizer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ILemmatizer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ILemmatizer * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateParadigmCollectionFromNorm )( 
            ILemmatizer * This,
            /* [in] */ BSTR norm,
            /* [in] */ BOOL capital,
            /* [in] */ BOOL UsePrediction,
            /* [retval][out] */ IParadigmCollection **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateParadigmCollectionFromForm )( 
            ILemmatizer * This,
            /* [in] */ BSTR form,
            /* [in] */ BOOL capital,
            /* [in] */ BOOL UsePrediction,
            /* [retval][out] */ IParadigmCollection **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateParadigmFromID )( 
            ILemmatizer * This,
            /* [in] */ long id,
            /* [retval][out] */ IParadigm **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadDictionariesRegistry )( 
            ILemmatizer * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UseStatistic )( 
            ILemmatizer * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UseStatistic )( 
            ILemmatizer * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadStatisticRegistry )( 
            ILemmatizer * This,
            /* [in] */ idlSubjectEnum subj);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MaximalPrediction )( 
            ILemmatizer * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CheckABC )( 
            ILemmatizer * This,
            /* [in] */ BSTR Word,
            /* [retval][out] */ BOOL *Result);
        
        END_INTERFACE
    } ILemmatizerVtbl;

    interface ILemmatizer
    {
        CONST_VTBL struct ILemmatizerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILemmatizer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILemmatizer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILemmatizer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILemmatizer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ILemmatizer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ILemmatizer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ILemmatizer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ILemmatizer_CreateParadigmCollectionFromNorm(This,norm,capital,UsePrediction,pVal)	\
    (This)->lpVtbl -> CreateParadigmCollectionFromNorm(This,norm,capital,UsePrediction,pVal)

#define ILemmatizer_CreateParadigmCollectionFromForm(This,form,capital,UsePrediction,pVal)	\
    (This)->lpVtbl -> CreateParadigmCollectionFromForm(This,form,capital,UsePrediction,pVal)

#define ILemmatizer_CreateParadigmFromID(This,id,pVal)	\
    (This)->lpVtbl -> CreateParadigmFromID(This,id,pVal)

#define ILemmatizer_LoadDictionariesRegistry(This)	\
    (This)->lpVtbl -> LoadDictionariesRegistry(This)

#define ILemmatizer_get_UseStatistic(This,pVal)	\
    (This)->lpVtbl -> get_UseStatistic(This,pVal)

#define ILemmatizer_put_UseStatistic(This,newVal)	\
    (This)->lpVtbl -> put_UseStatistic(This,newVal)

#define ILemmatizer_LoadStatisticRegistry(This,subj)	\
    (This)->lpVtbl -> LoadStatisticRegistry(This,subj)

#define ILemmatizer_put_MaximalPrediction(This,newVal)	\
    (This)->lpVtbl -> put_MaximalPrediction(This,newVal)

#define ILemmatizer_CheckABC(This,Word,Result)	\
    (This)->lpVtbl -> CheckABC(This,Word,Result)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILemmatizer_CreateParadigmCollectionFromNorm_Proxy( 
    ILemmatizer * This,
    /* [in] */ BSTR norm,
    /* [in] */ BOOL capital,
    /* [in] */ BOOL UsePrediction,
    /* [retval][out] */ IParadigmCollection **pVal);


void __RPC_STUB ILemmatizer_CreateParadigmCollectionFromNorm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILemmatizer_CreateParadigmCollectionFromForm_Proxy( 
    ILemmatizer * This,
    /* [in] */ BSTR form,
    /* [in] */ BOOL capital,
    /* [in] */ BOOL UsePrediction,
    /* [retval][out] */ IParadigmCollection **pVal);


void __RPC_STUB ILemmatizer_CreateParadigmCollectionFromForm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILemmatizer_CreateParadigmFromID_Proxy( 
    ILemmatizer * This,
    /* [in] */ long id,
    /* [retval][out] */ IParadigm **pVal);


void __RPC_STUB ILemmatizer_CreateParadigmFromID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILemmatizer_LoadDictionariesRegistry_Proxy( 
    ILemmatizer * This);


void __RPC_STUB ILemmatizer_LoadDictionariesRegistry_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ILemmatizer_get_UseStatistic_Proxy( 
    ILemmatizer * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB ILemmatizer_get_UseStatistic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILemmatizer_put_UseStatistic_Proxy( 
    ILemmatizer * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB ILemmatizer_put_UseStatistic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILemmatizer_LoadStatisticRegistry_Proxy( 
    ILemmatizer * This,
    /* [in] */ idlSubjectEnum subj);


void __RPC_STUB ILemmatizer_LoadStatisticRegistry_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ILemmatizer_put_MaximalPrediction_Proxy( 
    ILemmatizer * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB ILemmatizer_put_MaximalPrediction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILemmatizer_CheckABC_Proxy( 
    ILemmatizer * This,
    /* [in] */ BSTR Word,
    /* [retval][out] */ BOOL *Result);


void __RPC_STUB ILemmatizer_CheckABC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILemmatizer_INTERFACE_DEFINED__ */


#ifndef __IPLMLineCollection_INTERFACE_DEFINED__
#define __IPLMLineCollection_INTERFACE_DEFINED__

/* interface IPLMLineCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IPLMLineCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1887B706-1CAE-11D3-9CCE-00105A68BAF2")
    IPLMLineCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long pos,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Item( 
            /* [in] */ long pos,
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ long pos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveToFile( 
            /* [in] */ BSTR filename) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProcessPlmLines( 
            /* [in] */ IUnknown *piGraphmatFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AttachLemmatizer( 
            ILemmatizer *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProcessHyphenWords( 
            IUnknown *piGraphan) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CopyItems( 
            /* [in] */ IUnknown *piPlmLinesFrom) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPLMLineCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPLMLineCollection * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPLMLineCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPLMLineCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPLMLineCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPLMLineCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPLMLineCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPLMLineCollection * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IPLMLineCollection * This,
            /* [in] */ long pos,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Item )( 
            IPLMLineCollection * This,
            /* [in] */ long pos,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IPLMLineCollection * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IPLMLineCollection * This,
            /* [in] */ long pos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveToFile )( 
            IPLMLineCollection * This,
            /* [in] */ BSTR filename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProcessPlmLines )( 
            IPLMLineCollection * This,
            /* [in] */ IUnknown *piGraphmatFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AttachLemmatizer )( 
            IPLMLineCollection * This,
            ILemmatizer *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IPLMLineCollection * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProcessHyphenWords )( 
            IPLMLineCollection * This,
            IUnknown *piGraphan);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CopyItems )( 
            IPLMLineCollection * This,
            /* [in] */ IUnknown *piPlmLinesFrom);
        
        END_INTERFACE
    } IPLMLineCollectionVtbl;

    interface IPLMLineCollection
    {
        CONST_VTBL struct IPLMLineCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPLMLineCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPLMLineCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPLMLineCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPLMLineCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPLMLineCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPLMLineCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPLMLineCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IPLMLineCollection_get_Item(This,pos,pVal)	\
    (This)->lpVtbl -> get_Item(This,pos,pVal)

#define IPLMLineCollection_put_Item(This,pos,newVal)	\
    (This)->lpVtbl -> put_Item(This,pos,newVal)

#define IPLMLineCollection_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IPLMLineCollection_Remove(This,pos)	\
    (This)->lpVtbl -> Remove(This,pos)

#define IPLMLineCollection_SaveToFile(This,filename)	\
    (This)->lpVtbl -> SaveToFile(This,filename)

#define IPLMLineCollection_ProcessPlmLines(This,piGraphmatFile)	\
    (This)->lpVtbl -> ProcessPlmLines(This,piGraphmatFile)

#define IPLMLineCollection_AttachLemmatizer(This,pVal)	\
    (This)->lpVtbl -> AttachLemmatizer(This,pVal)

#define IPLMLineCollection_Clear(This)	\
    (This)->lpVtbl -> Clear(This)

#define IPLMLineCollection_ProcessHyphenWords(This,piGraphan)	\
    (This)->lpVtbl -> ProcessHyphenWords(This,piGraphan)

#define IPLMLineCollection_CopyItems(This,piPlmLinesFrom)	\
    (This)->lpVtbl -> CopyItems(This,piPlmLinesFrom)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IPLMLineCollection_get_Item_Proxy( 
    IPLMLineCollection * This,
    /* [in] */ long pos,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IPLMLineCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IPLMLineCollection_put_Item_Proxy( 
    IPLMLineCollection * This,
    /* [in] */ long pos,
    /* [in] */ BSTR newVal);


void __RPC_STUB IPLMLineCollection_put_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IPLMLineCollection_get_Count_Proxy( 
    IPLMLineCollection * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IPLMLineCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IPLMLineCollection_Remove_Proxy( 
    IPLMLineCollection * This,
    /* [in] */ long pos);


void __RPC_STUB IPLMLineCollection_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IPLMLineCollection_SaveToFile_Proxy( 
    IPLMLineCollection * This,
    /* [in] */ BSTR filename);


void __RPC_STUB IPLMLineCollection_SaveToFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IPLMLineCollection_ProcessPlmLines_Proxy( 
    IPLMLineCollection * This,
    /* [in] */ IUnknown *piGraphmatFile);


void __RPC_STUB IPLMLineCollection_ProcessPlmLines_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IPLMLineCollection_AttachLemmatizer_Proxy( 
    IPLMLineCollection * This,
    ILemmatizer *pVal);


void __RPC_STUB IPLMLineCollection_AttachLemmatizer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IPLMLineCollection_Clear_Proxy( 
    IPLMLineCollection * This);


void __RPC_STUB IPLMLineCollection_Clear_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IPLMLineCollection_ProcessHyphenWords_Proxy( 
    IPLMLineCollection * This,
    IUnknown *piGraphan);


void __RPC_STUB IPLMLineCollection_ProcessHyphenWords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IPLMLineCollection_CopyItems_Proxy( 
    IPLMLineCollection * This,
    /* [in] */ IUnknown *piPlmLinesFrom);


void __RPC_STUB IPLMLineCollection_CopyItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPLMLineCollection_INTERFACE_DEFINED__ */



#ifndef __LEMMATIZERLib_LIBRARY_DEFINED__
#define __LEMMATIZERLib_LIBRARY_DEFINED__

/* library LEMMATIZERLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_LEMMATIZERLib;

EXTERN_C const CLSID CLSID_Paradigm;

#ifdef __cplusplus

class DECLSPEC_UUID("6B232504-1816-11D3-9CC6-00105A68BAF2")
Paradigm;
#endif

EXTERN_C const CLSID CLSID_ParadigmCollection;

#ifdef __cplusplus

class DECLSPEC_UUID("6B232506-1816-11D3-9CC6-00105A68BAF2")
ParadigmCollection;
#endif

EXTERN_C const CLSID CLSID_LemmatizerRussian;

#ifdef __cplusplus

class DECLSPEC_UUID("6B23250F-1816-11D3-9CC6-00105A68BAF2")
LemmatizerRussian;
#endif

EXTERN_C const CLSID CLSID_LemmatizerEnglish;

#ifdef __cplusplus

class DECLSPEC_UUID("6B232511-1816-11D3-9CC6-00105A68BAF2")
LemmatizerEnglish;
#endif

EXTERN_C const CLSID CLSID_LemmatizerGerman;

#ifdef __cplusplus

class DECLSPEC_UUID("D26721AC-5BA9-4A1E-BBD0-A74CF86437E7")
LemmatizerGerman;
#endif

EXTERN_C const CLSID CLSID_PLMLineCollection;

#ifdef __cplusplus

class DECLSPEC_UUID("34E94E25-6C26-11D3-9D4A-00105A68BAF2")
PLMLineCollection;
#endif
#endif /* __LEMMATIZERLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


