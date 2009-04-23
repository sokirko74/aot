%{



#include "ChunkNode.h"
#include "ChunkParser.h"


#define YYERROR_VERBOSE
#define YYPARSE_PARAM _parser
#define YYLEX_PARAM _parser
#define _prs ((CChunkParser*) _parser)


void yyerror (const char *error);
int yylex (void*,  void*);
extern int CurrentSourceLineNo;
extern string CurrentSourceFileName;



%} 
%pure-parser
%debug
%union 
{
const string*		m_LabelPtr;  
CChunkNode*			m_pNode;  
CNodeAttributes*	m_pAttributes;  
CNodeAttribute*		m_pAttribute;
CChunkRule*			m_pRule;  
CChunkGrammar*		m_pGrammar;  
CRuleFeature*		m_pRuleFeature;  
}


%token	<m_LabelPtr>CHUNK_ATTRIB_VALUE
%token	<m_LabelPtr>CHUNK_WORD_FORM
%token	<m_LabelPtr>CHUNK_NAME
%token	<m_LabelPtr>CHUNK_RULE_PART_DELIM
%token	<m_LabelPtr>CHUNK_RULE_DELIM
%token	<m_LabelPtr>CHUNK_ATTR_ASSIGNMENT
%token	<m_LabelPtr>CHUNK_ATTR_EQU_CHAR
%token	<m_LabelPtr>CHUNK_ATTR_VARIABLE
%token	<m_LabelPtr>CHUNK_ATTR
%token	<m_LabelPtr>CHUNK_INCLUDE





%type	<m_pGrammar>chunker
%type	<m_pRule>chunk_rule
%type	<m_pRule>seq_descr


%type	<m_pAttribute>attribute
%type	<m_pAttributes>attributes
%type	<m_pNode>chunk_descr
%type	<m_pRuleFeature>feature_bundle
%type	<m_pRuleFeature>func_expr
%type	<m_pRuleFeature>feat_list


%left	CHUNK_RULE_DELIM
%left	CHUNK_RULE_PART_DELIM 
%left	CHUNK_OR 
%right  CHUNK_ATTR_ASSIGNMENT
%right  CHUNK_FACULTATIVE




%%
//-----------------------------------------------------------------------------------------------------------------
//				RULES PART
//-----------------------------------------------------------------------------------------------------------------

chunker : chunk_rule CHUNK_RULE_DELIM
		{
			CChunkGrammar* pObj = &_prs->m_ChunkGrammar;
			assert (pObj != 0);

			pObj->m_Rules.push_back($1);
			
			$$ = pObj;
		}
		| chunker  chunk_rule CHUNK_RULE_DELIM
		{
			CChunkGrammar* pObj = $1;
			assert (pObj == &_prs->m_ChunkGrammar);
			pObj->m_Rules.push_back($2);
			$$ = pObj;
		}
		| chunker CHUNK_INCLUDE CHUNK_ATTRIB_VALUE
		{
			CChunkGrammar* pObj = $1;
			assert (pObj == &_prs->m_ChunkGrammar);
			CChunkParser P;
			if (!P.ParseGrammarInFile(*$3))
				YYABORT;
			pObj->AddRules(P.m_ChunkGrammar);		
			P.m_ChunkGrammar.m_Rules.clear(); // prevent destructor for rules
			$$ = pObj;
		} 
		| CHUNK_INCLUDE CHUNK_ATTRIB_VALUE
		{
			CChunkGrammar* pObj = &_prs->m_ChunkGrammar;
			CChunkParser P;
			if (!P.ParseGrammarInFile(*$2))
				YYABORT;
			pObj->AddRules(P.m_ChunkGrammar);		
			P.m_ChunkGrammar.m_Rules.clear(); // prevent destructor for rules
			$$ = pObj;
		} 
		;

chunk_rule : chunk_rule ':' feature_bundle
		{
			CChunkRule* pObj = $1;
			pObj->AddRuleFeatute($3);
		}

chunk_rule : chunk_descr CHUNK_RULE_PART_DELIM  seq_descr
		{
			CChunkRule* pObj = $3;
			pObj->SetLeftHandNode($1);
			pObj->m_SourceLineNo = CurrentSourceLineNo;
			
			pObj->m_SourceFileName = CurrentSourceFileName;
			$$ = pObj;
		};



seq_descr		:		
			seq_descr CHUNK_OR seq_descr
			{
				CChunkRule* pObj = $1;
				if (!pObj->AddRightHandMultipleNode($3, stOr))
					YYABORT;
				$$ = pObj;
			}
		|	seq_descr CHUNK_FACULTATIVE
			{
				CChunkRule* pObj = $1;
				if (!pObj->MakeLastNodeFacultative())
					YYABORT;
				$$ = pObj;
			}
		|	seq_descr seq_descr
			{
				CChunkRule* pObj = $1;
				if (!pObj->AddRightHandMultipleNode($2, stSequence))
					YYABORT;
				$$ = pObj;
			}
		|	'(' seq_descr ')'
			{
				CChunkRule* pObj = $2;
				pObj->EncloseNodes();
				$$ = pObj;	
			}
		|	chunk_descr
			{
				CChunkRule* pObj = new CChunkRule;
				pObj->m_SourceFileName = "";
				if (!pObj) YYABORT;
				if (!pObj->CreateRule($1))	YYABORT;
				$$ = pObj;	
			};


						
chunk_descr	: '[' CHUNK_NAME attributes ']'
				{
					CChunkNode* pObj = new CChunkNode;
					pObj->CreateAtomic($2->c_str(), $3);
					pObj->m_RefCount--;
					$$ = pObj;
				}
			| '[' CHUNK_NAME ']'
				{
					CChunkNode* pObj = new CChunkNode;
					pObj->CreateAtomic($2->c_str(), NULL);
					pObj->m_RefCount--;
					$$ = pObj;
				}
			|	CHUNK_WORD_FORM
				{
					CChunkNode* pObj = new CChunkNode;
					CNodeAttributes* pAttrs = new CNodeAttributes;
					pAttrs->m_Items.push_back( new CNodeAttribute("form",*$1) );
					pObj->CreateAtomic("TOKEN", pAttrs);
					$$ = pObj;
				
				};
attributes :	attribute
				{
					CNodeAttributes* pObj = new CNodeAttributes;
					if (!pObj) 
						YYABORT;
					pObj->m_Items.push_back( $1 );	
					$$ = pObj
				}
			|	attributes attribute
				{
					CNodeAttributes* pObj = $1;
					pObj->m_Items.push_back( $2 );	
					$$ = pObj
				};
		

attribute :		CHUNK_ATTR
				{
					if ( (*$1 != "root") && (*$1 != "atomic"))
					{
						yyerror("Bad single attribute name");
						YYABORT;
					};
					CNodeAttribute* pObj = new CNodeAttribute(*$1, "");
					if (!pObj) 
						YYABORT;
					$$ = pObj
				}
			|	CHUNK_ATTR  CHUNK_ATTR_EQU_CHAR CHUNK_ATTRIB_VALUE
				{
					CNodeAttribute* pObj = new CNodeAttribute(*$1, *$3);
					if (!pObj) 
						YYABORT;
					$$ = pObj
				};
				
	feature_bundle :  CHUNK_ATTR_VARIABLE CHUNK_ATTR_ASSIGNMENT CHUNK_NAME func_expr
				{
					CRuleFeature* pFeat = $4;
					if (!pFeat) 
						YYABORT;
					string ErrorStr = pFeat->InitAssignement(*$1, *$3);
					if (!ErrorStr.empty())
					{
						yyerror(ErrorStr.c_str());
						YYABORT;
					};
					$$ = pFeat;
				}		
				| CHUNK_NAME func_expr
				{
					CRuleFeature* pFeat = $2;
					if (!pFeat) 
						YYABORT;
					string ErrorStr = pFeat->InitCheck(*$1);
					if (!ErrorStr.empty())
					{
						yyerror(ErrorStr.c_str());
						YYABORT;
					};
					$$ = pFeat;
				}
				| CHUNK_ATTR_VARIABLE CHUNK_ATTR_ASSIGNMENT CHUNK_NAME
				{
					CRuleFeature* pFeat = new CRuleFeature;
					if (!pFeat) 
						YYABORT;
					string ErrorStr = pFeat->InitAssignement(*$1, *$3);
					if (!ErrorStr.empty())
					{
						yyerror(ErrorStr.c_str());
						YYABORT;
					};
					$$ = pFeat;
					
				};

func_expr : '(' feat_list ')'
			{
				$$ = $2;
			};

				
feat_list :  CHUNK_ATTR_VARIABLE
			{
				CRuleFeature* pObj = new CRuleFeature;
				if (!pObj->AddFeatureArgument(*$1))
				{
					string ErrorStr = string("Bad item id:") + *$1;
					yyerror(ErrorStr.c_str());
					YYABORT;
				};
				$$ = pObj;
			}
			| feat_list ',' CHUNK_ATTR_VARIABLE
			{
				CRuleFeature* pObj = $1;
				if (!pObj->AddFeatureArgument(*$3))
				{
					string ErrorStr = string("Bad item id:") + *$3;
					yyerror(ErrorStr.c_str());
					YYABORT;
				};
				$$ = pObj;
			}	
			| CHUNK_ATTRIB_VALUE
			{
				CRuleFeature* pObj = new CRuleFeature;
				if (!pObj->AddFeatureValue(_prs->m_pGramTab, *$1))
				{
					string ErrorStr = string("Bad argument:") + *$1;
					yyerror(ErrorStr.c_str());
					YYABORT;
				};
				$$ = pObj;
			}
			| feat_list ',' CHUNK_ATTRIB_VALUE
			{
				CRuleFeature* pObj = $1;
				if (!pObj->AddFeatureValue(_prs->m_pGramTab, *$3))
				{
					string ErrorStr = string("Bad argument :") + *$3;
					yyerror(ErrorStr.c_str());
					YYABORT;
				};
				$$ = pObj;
			}
;
	