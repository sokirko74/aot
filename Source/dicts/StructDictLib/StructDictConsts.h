// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)
#pragma once 

const	uint32_t	MaxDomensUnionSize = 20;
	
const	uint32_t	AuthorNameSize = 20;
// максимальное число актантов 
const	BYTE	MaxNumAct = 7;

// ошибочный номер словарного входа 
const	uint16_t	ErrUnitNo  = 65000;
const	uint32_t	EntryStrSize = 40;

// Исходный материал доменов
const	BYTE	dsUnion = 'O';
const	BYTE	dsExpres = 'C';
const	BYTE	dsSystem = 'S';
const	BYTE	dsText = 'T';
const	BYTE	dsMetaText = 'M';

const	BYTE	frOne = 'O';
const	BYTE	frMany = 'M';
const	BYTE	frFormula = 'F';
