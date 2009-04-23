#ifndef LexFuncts_H
#define LexFuncts_H


extern bool	IsOper1 (const string& LexFunct);
extern bool	IsOper2 (const string& LexFunct);
extern bool	IsOper3 (const string& LexFunct);
extern bool	IsOper (const string& LexFunct);
extern bool	IsFunc0 (const string& LexFunct);
extern bool	IsFunc1 (const string& LexFunct);
extern bool	IsFunc2 (const string& LexFunct);
extern bool	IsFunc3 (const string& LexFunct);
extern bool	IsFunc (const string& LexFunct);
extern bool IsOper(const string& lexFun, int& OperNum);
extern bool IsFunc(const string& lexFun, int& FuncNum);
extern bool	IsParameterOfVerb (const string& LexFunct);
extern bool	IsParameterOfAdjOrAdv (const string& LexFunct);


#endif
