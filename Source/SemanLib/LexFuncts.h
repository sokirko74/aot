#ifndef LexFuncts_H
#define LexFuncts_H


extern bool	IsOper1 (const std::string& LexFunct);
extern bool	IsOper2 (const std::string& LexFunct);
extern bool	IsOper3 (const std::string& LexFunct);
extern bool	IsOper (const std::string& LexFunct);
extern bool	IsFunc0 (const std::string& LexFunct);
extern bool	IsFunc1 (const std::string& LexFunct);
extern bool	IsFunc2 (const std::string& LexFunct);
extern bool	IsFunc3 (const std::string& LexFunct);
extern bool	IsFunc (const std::string& LexFunct);
extern bool IsOper(const std::string& lexFun, int& OperNum);
extern bool IsFunc(const std::string& lexFun, int& FuncNum);
extern bool	IsParameterOfVerb (const std::string& LexFunct);
extern bool	IsParameterOfAdjOrAdv (const std::string& LexFunct);


#endif
