// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)
#pragma once 

const	DWORD	MaxDomensUnionSize = 20;
	
const	DWORD	AuthorNameSize = 20;
// максимальное число актантов 
const	BYTE	MaxNumAct = 7;

// ошибочный номер словарного входа 
const	WORD	ErrUnitNo  = 65000;
const	DWORD	EntryStrSize = 40;

// Исходный материал доменов
const	BYTE	dsUnion = 'O';
const	BYTE	dsExpres = 'C';
const	BYTE	dsSystem = 'S';
const	BYTE	dsText = 'T';
const	BYTE	dsMetaText = 'M';

const	BYTE	frOne = 'O';
const	BYTE	frMany = 'M';
const	BYTE	frFormula = 'F';
