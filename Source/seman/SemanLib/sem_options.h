#pragma once 

#include "DictUnitInterp.h"

const std::string CommonDomain = "общ";
const std::string FinanceDomain = "фин";
const std::string ComputerDomain = "информ";
const std::string GeoDomain = "геогр";


struct CSemOptions {
	// номер варианта дерева, который заказал пользователь
	long m_TreeVariantIndex;
	// максимально разрешенное кол-во вариантов деревьев
	long m_PanicTreeVariantCount;
	// запрещенные  лексические варианты (устанавливаются пользователем)
	// баня1, папа2
	std::vector<std::string> m_ProhibitedLexVars;
	// morph clause variant index
	int m_ClauseVariantsCombinationIndex;
	// предметная область
	std::string m_Domain;

    CSemOptions();
};
