

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Oct 23 10:48:41 2008
 */
/* Compiler settings for .\graphan.idl:
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

#ifndef __graphan_h__
#define __graphan_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IGraphmatFile_FWD_DEFINED__
#define __IGraphmatFile_FWD_DEFINED__
typedef interface IGraphmatFile IGraphmatFile;
#endif 	/* __IGraphmatFile_FWD_DEFINED__ */


#ifndef __GraphmatFile_FWD_DEFINED__
#define __GraphmatFile_FWD_DEFINED__

#ifdef __cplusplus
typedef class GraphmatFile GraphmatFile;
#else
typedef struct GraphmatFile GraphmatFile;
#endif /* __cplusplus */

#endif 	/* __GraphmatFile_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IGraphmatFile_INTERFACE_DEFINED__
#define __IGraphmatFile_INTERFACE_DEFINED__

/* interface IGraphmatFile */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IGraphmatFile;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ECDEABD9-0855-11D3-A75D-AA5D93000000")
    IGraphmatFile : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_bMacSynHierarchy( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_bMacSynHierarchy( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_XmlMacSynOutputFile( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_XmlMacSynOutputFile( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Language( 
            /* [retval][out] */ int *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Language( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_bEmptyLineIsSentenceDelim( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_bEmptyLineIsSentenceDelim( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_bUseParagraphTagToDivide( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_bUseParagraphTagToDivide( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_bUseIndention( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_bUseIndention( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_bSubdueWrongMacroSynUnitToMainRoot( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_bSubdueWrongMacroSynUnitToMainRoot( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GraOutputFile( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_GraOutputFile( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadDicts( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadFileToGraphan( 
            /* [in] */ BSTR CommandLine) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadStringToGraphan( 
            /* [in] */ BSTR Buffer) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsTableLoaded( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLineCount( 
            /* [out][retval] */ UINT *Count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWord( 
            /* [in] */ UINT LineNo,
            /* [out][retval] */ BSTR *s) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE HasDescr( 
            /* [in] */ UINT LineNo,
            /* [in] */ UINT D,
            /* [out][retval] */ BOOL *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDescriptorStr( 
            /* [in] */ UINT DescriptorNo,
            /* [out][retval] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRubiconTypeStr( 
            /* [in] */ UINT RubiconTypeNo,
            /* [out][retval] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLine( 
            /* [in] */ UINT LineNo,
            /* [out][retval] */ BSTR *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetOborotNo( 
            /* [in] */ UINT LineNo,
            /* [out][retval] */ LONG *result) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetOborDic( 
            /* [in] */ IUnknown *Dic) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUnitOffset( 
            /* [in] */ int LineNo,
            /* [retval][out] */ int *Offset) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUnitLength( 
            /* [in] */ int LineNo,
            /* [retval][out] */ BYTE *Length) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FreeTable( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FreeDicts( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTokenLanguage( 
            /* [in] */ int LineNo,
            /* [retval][out] */ int *Language) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGraphmatFileVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGraphmatFile * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGraphmatFile * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGraphmatFile * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IGraphmatFile * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IGraphmatFile * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IGraphmatFile * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IGraphmatFile * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_bMacSynHierarchy )( 
            IGraphmatFile * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_bMacSynHierarchy )( 
            IGraphmatFile * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_XmlMacSynOutputFile )( 
            IGraphmatFile * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_XmlMacSynOutputFile )( 
            IGraphmatFile * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Language )( 
            IGraphmatFile * This,
            /* [retval][out] */ int *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Language )( 
            IGraphmatFile * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_bEmptyLineIsSentenceDelim )( 
            IGraphmatFile * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_bEmptyLineIsSentenceDelim )( 
            IGraphmatFile * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_bUseParagraphTagToDivide )( 
            IGraphmatFile * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_bUseParagraphTagToDivide )( 
            IGraphmatFile * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_bUseIndention )( 
            IGraphmatFile * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_bUseIndention )( 
            IGraphmatFile * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_bSubdueWrongMacroSynUnitToMainRoot )( 
            IGraphmatFile * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_bSubdueWrongMacroSynUnitToMainRoot )( 
            IGraphmatFile * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GraOutputFile )( 
            IGraphmatFile * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_GraOutputFile )( 
            IGraphmatFile * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadDicts )( 
            IGraphmatFile * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadFileToGraphan )( 
            IGraphmatFile * This,
            /* [in] */ BSTR CommandLine);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadStringToGraphan )( 
            IGraphmatFile * This,
            /* [in] */ BSTR Buffer);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsTableLoaded )( 
            IGraphmatFile * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLineCount )( 
            IGraphmatFile * This,
            /* [out][retval] */ UINT *Count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetWord )( 
            IGraphmatFile * This,
            /* [in] */ UINT LineNo,
            /* [out][retval] */ BSTR *s);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *HasDescr )( 
            IGraphmatFile * This,
            /* [in] */ UINT LineNo,
            /* [in] */ UINT D,
            /* [out][retval] */ BOOL *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDescriptorStr )( 
            IGraphmatFile * This,
            /* [in] */ UINT DescriptorNo,
            /* [out][retval] */ BSTR *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRubiconTypeStr )( 
            IGraphmatFile * This,
            /* [in] */ UINT RubiconTypeNo,
            /* [out][retval] */ BSTR *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLine )( 
            IGraphmatFile * This,
            /* [in] */ UINT LineNo,
            /* [out][retval] */ BSTR *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetOborotNo )( 
            IGraphmatFile * This,
            /* [in] */ UINT LineNo,
            /* [out][retval] */ LONG *result);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetOborDic )( 
            IGraphmatFile * This,
            /* [in] */ IUnknown *Dic);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetUnitOffset )( 
            IGraphmatFile * This,
            /* [in] */ int LineNo,
            /* [retval][out] */ int *Offset);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetUnitLength )( 
            IGraphmatFile * This,
            /* [in] */ int LineNo,
            /* [retval][out] */ BYTE *Length);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FreeTable )( 
            IGraphmatFile * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FreeDicts )( 
            IGraphmatFile * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTokenLanguage )( 
            IGraphmatFile * This,
            /* [in] */ int LineNo,
            /* [retval][out] */ int *Language);
        
        END_INTERFACE
    } IGraphmatFileVtbl;

    interface IGraphmatFile
    {
        CONST_VTBL struct IGraphmatFileVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGraphmatFile_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGraphmatFile_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGraphmatFile_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGraphmatFile_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGraphmatFile_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGraphmatFile_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGraphmatFile_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGraphmatFile_get_bMacSynHierarchy(This,pVal)	\
    (This)->lpVtbl -> get_bMacSynHierarchy(This,pVal)

#define IGraphmatFile_put_bMacSynHierarchy(This,newVal)	\
    (This)->lpVtbl -> put_bMacSynHierarchy(This,newVal)

#define IGraphmatFile_get_XmlMacSynOutputFile(This,pVal)	\
    (This)->lpVtbl -> get_XmlMacSynOutputFile(This,pVal)

#define IGraphmatFile_put_XmlMacSynOutputFile(This,newVal)	\
    (This)->lpVtbl -> put_XmlMacSynOutputFile(This,newVal)

#define IGraphmatFile_get_Language(This,pVal)	\
    (This)->lpVtbl -> get_Language(This,pVal)

#define IGraphmatFile_put_Language(This,newVal)	\
    (This)->lpVtbl -> put_Language(This,newVal)

#define IGraphmatFile_get_bEmptyLineIsSentenceDelim(This,pVal)	\
    (This)->lpVtbl -> get_bEmptyLineIsSentenceDelim(This,pVal)

#define IGraphmatFile_put_bEmptyLineIsSentenceDelim(This,newVal)	\
    (This)->lpVtbl -> put_bEmptyLineIsSentenceDelim(This,newVal)

#define IGraphmatFile_get_bUseParagraphTagToDivide(This,pVal)	\
    (This)->lpVtbl -> get_bUseParagraphTagToDivide(This,pVal)

#define IGraphmatFile_put_bUseParagraphTagToDivide(This,newVal)	\
    (This)->lpVtbl -> put_bUseParagraphTagToDivide(This,newVal)

#define IGraphmatFile_get_bUseIndention(This,pVal)	\
    (This)->lpVtbl -> get_bUseIndention(This,pVal)

#define IGraphmatFile_put_bUseIndention(This,newVal)	\
    (This)->lpVtbl -> put_bUseIndention(This,newVal)

#define IGraphmatFile_get_bSubdueWrongMacroSynUnitToMainRoot(This,pVal)	\
    (This)->lpVtbl -> get_bSubdueWrongMacroSynUnitToMainRoot(This,pVal)

#define IGraphmatFile_put_bSubdueWrongMacroSynUnitToMainRoot(This,newVal)	\
    (This)->lpVtbl -> put_bSubdueWrongMacroSynUnitToMainRoot(This,newVal)

#define IGraphmatFile_get_GraOutputFile(This,pVal)	\
    (This)->lpVtbl -> get_GraOutputFile(This,pVal)

#define IGraphmatFile_put_GraOutputFile(This,newVal)	\
    (This)->lpVtbl -> put_GraOutputFile(This,newVal)

#define IGraphmatFile_LoadDicts(This)	\
    (This)->lpVtbl -> LoadDicts(This)

#define IGraphmatFile_LoadFileToGraphan(This,CommandLine)	\
    (This)->lpVtbl -> LoadFileToGraphan(This,CommandLine)

#define IGraphmatFile_LoadStringToGraphan(This,Buffer)	\
    (This)->lpVtbl -> LoadStringToGraphan(This,Buffer)

#define IGraphmatFile_get_IsTableLoaded(This,pVal)	\
    (This)->lpVtbl -> get_IsTableLoaded(This,pVal)

#define IGraphmatFile_GetLineCount(This,Count)	\
    (This)->lpVtbl -> GetLineCount(This,Count)

#define IGraphmatFile_GetWord(This,LineNo,s)	\
    (This)->lpVtbl -> GetWord(This,LineNo,s)

#define IGraphmatFile_HasDescr(This,LineNo,D,result)	\
    (This)->lpVtbl -> HasDescr(This,LineNo,D,result)

#define IGraphmatFile_GetDescriptorStr(This,DescriptorNo,result)	\
    (This)->lpVtbl -> GetDescriptorStr(This,DescriptorNo,result)

#define IGraphmatFile_GetRubiconTypeStr(This,RubiconTypeNo,result)	\
    (This)->lpVtbl -> GetRubiconTypeStr(This,RubiconTypeNo,result)

#define IGraphmatFile_GetLine(This,LineNo,result)	\
    (This)->lpVtbl -> GetLine(This,LineNo,result)

#define IGraphmatFile_GetOborotNo(This,LineNo,result)	\
    (This)->lpVtbl -> GetOborotNo(This,LineNo,result)

#define IGraphmatFile_SetOborDic(This,Dic)	\
    (This)->lpVtbl -> SetOborDic(This,Dic)

#define IGraphmatFile_GetUnitOffset(This,LineNo,Offset)	\
    (This)->lpVtbl -> GetUnitOffset(This,LineNo,Offset)

#define IGraphmatFile_GetUnitLength(This,LineNo,Length)	\
    (This)->lpVtbl -> GetUnitLength(This,LineNo,Length)

#define IGraphmatFile_FreeTable(This)	\
    (This)->lpVtbl -> FreeTable(This)

#define IGraphmatFile_FreeDicts(This)	\
    (This)->lpVtbl -> FreeDicts(This)

#define IGraphmatFile_GetTokenLanguage(This,LineNo,Language)	\
    (This)->lpVtbl -> GetTokenLanguage(This,LineNo,Language)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_get_bMacSynHierarchy_Proxy( 
    IGraphmatFile * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IGraphmatFile_get_bMacSynHierarchy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_put_bMacSynHierarchy_Proxy( 
    IGraphmatFile * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IGraphmatFile_put_bMacSynHierarchy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_get_XmlMacSynOutputFile_Proxy( 
    IGraphmatFile * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IGraphmatFile_get_XmlMacSynOutputFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_put_XmlMacSynOutputFile_Proxy( 
    IGraphmatFile * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IGraphmatFile_put_XmlMacSynOutputFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_get_Language_Proxy( 
    IGraphmatFile * This,
    /* [retval][out] */ int *pVal);


void __RPC_STUB IGraphmatFile_get_Language_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_put_Language_Proxy( 
    IGraphmatFile * This,
    /* [in] */ int newVal);


void __RPC_STUB IGraphmatFile_put_Language_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_get_bEmptyLineIsSentenceDelim_Proxy( 
    IGraphmatFile * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IGraphmatFile_get_bEmptyLineIsSentenceDelim_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_put_bEmptyLineIsSentenceDelim_Proxy( 
    IGraphmatFile * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IGraphmatFile_put_bEmptyLineIsSentenceDelim_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_get_bUseParagraphTagToDivide_Proxy( 
    IGraphmatFile * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IGraphmatFile_get_bUseParagraphTagToDivide_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_put_bUseParagraphTagToDivide_Proxy( 
    IGraphmatFile * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IGraphmatFile_put_bUseParagraphTagToDivide_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_get_bUseIndention_Proxy( 
    IGraphmatFile * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IGraphmatFile_get_bUseIndention_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_put_bUseIndention_Proxy( 
    IGraphmatFile * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IGraphmatFile_put_bUseIndention_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_get_bSubdueWrongMacroSynUnitToMainRoot_Proxy( 
    IGraphmatFile * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IGraphmatFile_get_bSubdueWrongMacroSynUnitToMainRoot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_put_bSubdueWrongMacroSynUnitToMainRoot_Proxy( 
    IGraphmatFile * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IGraphmatFile_put_bSubdueWrongMacroSynUnitToMainRoot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_get_GraOutputFile_Proxy( 
    IGraphmatFile * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IGraphmatFile_get_GraOutputFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_put_GraOutputFile_Proxy( 
    IGraphmatFile * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IGraphmatFile_put_GraOutputFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_LoadDicts_Proxy( 
    IGraphmatFile * This);


void __RPC_STUB IGraphmatFile_LoadDicts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_LoadFileToGraphan_Proxy( 
    IGraphmatFile * This,
    /* [in] */ BSTR CommandLine);


void __RPC_STUB IGraphmatFile_LoadFileToGraphan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_LoadStringToGraphan_Proxy( 
    IGraphmatFile * This,
    /* [in] */ BSTR Buffer);


void __RPC_STUB IGraphmatFile_LoadStringToGraphan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_get_IsTableLoaded_Proxy( 
    IGraphmatFile * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IGraphmatFile_get_IsTableLoaded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_GetLineCount_Proxy( 
    IGraphmatFile * This,
    /* [out][retval] */ UINT *Count);


void __RPC_STUB IGraphmatFile_GetLineCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_GetWord_Proxy( 
    IGraphmatFile * This,
    /* [in] */ UINT LineNo,
    /* [out][retval] */ BSTR *s);


void __RPC_STUB IGraphmatFile_GetWord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_HasDescr_Proxy( 
    IGraphmatFile * This,
    /* [in] */ UINT LineNo,
    /* [in] */ UINT D,
    /* [out][retval] */ BOOL *result);


void __RPC_STUB IGraphmatFile_HasDescr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_GetDescriptorStr_Proxy( 
    IGraphmatFile * This,
    /* [in] */ UINT DescriptorNo,
    /* [out][retval] */ BSTR *result);


void __RPC_STUB IGraphmatFile_GetDescriptorStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_GetRubiconTypeStr_Proxy( 
    IGraphmatFile * This,
    /* [in] */ UINT RubiconTypeNo,
    /* [out][retval] */ BSTR *result);


void __RPC_STUB IGraphmatFile_GetRubiconTypeStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_GetLine_Proxy( 
    IGraphmatFile * This,
    /* [in] */ UINT LineNo,
    /* [out][retval] */ BSTR *result);


void __RPC_STUB IGraphmatFile_GetLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_GetOborotNo_Proxy( 
    IGraphmatFile * This,
    /* [in] */ UINT LineNo,
    /* [out][retval] */ LONG *result);


void __RPC_STUB IGraphmatFile_GetOborotNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_SetOborDic_Proxy( 
    IGraphmatFile * This,
    /* [in] */ IUnknown *Dic);


void __RPC_STUB IGraphmatFile_SetOborDic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_GetUnitOffset_Proxy( 
    IGraphmatFile * This,
    /* [in] */ int LineNo,
    /* [retval][out] */ int *Offset);


void __RPC_STUB IGraphmatFile_GetUnitOffset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_GetUnitLength_Proxy( 
    IGraphmatFile * This,
    /* [in] */ int LineNo,
    /* [retval][out] */ BYTE *Length);


void __RPC_STUB IGraphmatFile_GetUnitLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_FreeTable_Proxy( 
    IGraphmatFile * This);


void __RPC_STUB IGraphmatFile_FreeTable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_FreeDicts_Proxy( 
    IGraphmatFile * This);


void __RPC_STUB IGraphmatFile_FreeDicts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IGraphmatFile_GetTokenLanguage_Proxy( 
    IGraphmatFile * This,
    /* [in] */ int LineNo,
    /* [retval][out] */ int *Language);


void __RPC_STUB IGraphmatFile_GetTokenLanguage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGraphmatFile_INTERFACE_DEFINED__ */



#ifndef __GRAPHANLib_LIBRARY_DEFINED__
#define __GRAPHANLib_LIBRARY_DEFINED__

/* library GRAPHANLib */
/* [helpstring][version][uuid] */ 


enum Descriptors
    {	OBeg	= 0,
	ORLE	= 1,
	OLLE	= 2,
	ODel	= 3,
	OPun	= 4,
	ODigits	= 5,
	ONumChar	= 6,
	OElectAddr	= 7,
	OUnk	= 8,
	OSpc	= 9,
	OHyp	= 10,
	OEOLN	= 11,
	OEOP	= 12,
	OPlu	= 13,
	OUp	= 14,
	OLw	= 15,
	OUpLw	= 16,
	ONam	= 17,
	OOpn	= 18,
	OCls	= 19,
	ONil	= 20,
	OBullet	= 21,
	OPar	= 22,
	ODUMMY_PLEASE_USE_ME	= 23,
	OQuoted	= 24,
	OParagraph	= 25,
	CS_Undef	= 26,
	CS_Simple	= 27,
	CS_Quasi	= 28,
	CS_Parent	= 29,
	CS_Heading	= 30,
	CS_Explan	= 31,
	CS_Doc	= 32,
	OSentEnd	= 33,
	ORoman	= 34,
	OEXPR1	= 35,
	OEXPR2	= 36,
	OFAM1	= 37,
	OFAM2	= 38,
	ORef1	= 39,
	ORef2	= 40,
	OFile1	= 41,
	OFile2	= 42,
	OAbbr1	= 43,
	OAbbr2	= 44,
	OKey1	= 45,
	OKey2	= 46,
	OGerDivComp1	= 47,
	OGerDivComp2	= 48,
	NumberOfGraphematicalDescriptors	= 49
    } ;

EXTERN_C const IID LIBID_GRAPHANLib;

EXTERN_C const CLSID CLSID_GraphmatFile;

#ifdef __cplusplus

class DECLSPEC_UUID("ECDEABDA-0855-11D3-A75D-AA5D93000000")
GraphmatFile;
#endif
#endif /* __GRAPHANLib_LIBRARY_DEFINED__ */

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


