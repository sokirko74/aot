
#include "StdGraph.h"
#include "GraphmatFile.h"
#include "GraphanDicts.h"

static size_t  FindSentEndAfterParagraph (const CGraphmatFile &F, size_t StartPos, size_t EndPos)
{
	 // pass  paragraph indention, if it exists
     size_t k =  F.BSoft (StartPos);

	 // if we are still on the soft delimiter, then we should exit
	 // since the very beginning of the text was already processed
	 if ( F.GetUnits()[k].IsSoft() ) 
	 {
		 assert(k == 0);
		 return EndPos;
	 };

	 // ignore all "spaces" between paragraphs
     for (; (k > 0) && F.GetUnits()[k].IsSpace(); k--);


     size_t PrevParagraphEnd  = k;

	 // going to the left, searching for the end of sentence, passing all spaces
	for (; (k > 0) && (F.HasDescr(k, OPun) || F.GetUnits()[k].IsSpace()); k--)
	  if (   F.IsSentenceEndMark(k) 
		  || F.HasDescr(k, OSentEnd)
		 )
	  {
		  // we have found "end of sentence" sequence
		  return k;
	  };
     
	// return the first not soft g-word of the paragraph
	return PrevParagraphEnd;
};



/*
	this function checks the absence of comma between sentences
*/


static bool CheckComma (CGraphmatFile& F, size_t Sent1No, size_t Sent2No)
{
	// no check if the pointers are invalid
	if  (Sent1No >= F.GetUnits().size()) return true;
	if  (Sent2No >= F.GetUnits().size()) return true;
	for (size_t j=Sent1No; j < Sent2No;  j++)
		if	(		F.IsOneChar(j, (unsigned char)',') 
				||	F.HasDescr(j, OSentEnd)
			) 
	return false;

	return true;
};
/*
	this function sets graphematical desriptors for the end of the sentence.
	PreviousSentEnd is the end of the previous sentence
	NewSentStart is the start of a new sentence.
	There are the following output arguments:
		SentenceOpenBracket  contains the code of the open bracket, which is between sentences.
		StartSentenceOffset is the offset of the sentence start in bytes, if Sent1No was valid.
	The function return 

*/

static bool SetSentMarkers (CGraphmatFile& F, const size_t PreviousSentEnd, const size_t NewSentStart , BYTE& SentenceOpenBracket, bool& bSentenceQuotationMarks, DWORD& StartSentenceOffset)
{	
	if (PreviousSentEnd >= NewSentStart)
	{
		assert (PreviousSentEnd < NewSentStart);
		return false;
	};

	size_t EndPos = F.GetUnits().size();
	bSentenceQuotationMarks = false;

	if (   (NewSentStart < EndPos) 
		&& (PreviousSentEnd < EndPos)) 
	{
		//check for brackets
		SentenceOpenBracket = 0;


		for (size_t j=PreviousSentEnd; j < NewSentStart;  j++)
		{
			if (F.HasDescr(j, OOpn))
				SentenceOpenBracket = F.GetUnits()[j].GetToken()[0];
			if ( F.IsOneOpenQuotationMark(j) )
				bSentenceQuotationMarks = true;

		};
	};      

	if	(		(PreviousSentEnd < EndPos) 
			&&	(PreviousSentEnd > 0)
		)
	{
		F.SetDes(PreviousSentEnd, OSentEnd);
		//  we calcultaing sentence offset from the previous sentence end
		StartSentenceOffset = F.GetTokenInputOffset(PreviousSentEnd);
	};
	return true;
};


bool CheckGermanSentenceBreak(CGraphmatFile& G, size_t WordNo)
{

	const size_t ArticlesCount = 6;
	const string Articles[ArticlesCount] =  {"DAS", "DEM","DEN","DER","DES","DIE" };

	if	(		(G.IsOneFullStop(WordNo))
			&&	(WordNo> 2)
			&&	G.HasDescr(WordNo-1, ODigits)
		)
		{
			// There should be a full stop after ordinal German numerals, for example:
			//den 8. Mai
			//Das 5.  Corps
			// This full stop is not considered a sentence break.
			int i  = G.BSoft(WordNo-2);
			if (i > 0) 
				if (binary_search(Articles,Articles+ArticlesCount, string(G.GetUppercaseToken(i))) )
					return false;
		};

	return true;
};



bool IsLastInGroupOrFree(const CGraphmatFile& F, size_t i) 
{
	if (F.GetUnits()[i].IsAbbreviation() && !F.HasDescr (i, OAbbr2)) return false;

	return 
			   F.HasDescr (i, OEXPR2)
			|| F.HasDescr (i, OFAM2)
			|| F.HasDescr (i, ORef2)
			|| F.HasDescr (i, OFile2)
			|| !F.GetUnits()[i].IsGrouped();
};


bool CGraphmatFile::DealSentBreaker ()
{
	size_t EndPos   = GetUnits().size();
	
	/*
		if the whole  sentence is in brackets, then 
		SentenceOpenBracket contains the code  of the this bracket,
		otherwize it is 0.
		
	*/
	BYTE SentenceOpenBracket = 0;

	/*
		if  InnerOpenBracket!=0, then we have found an open  bracket inside the sentence,
		InnerOpenBracket contains  the code of this  bracket.
		As soon we have found an open bracket inside the sentence  we should find a matching close bracket for it.
		We pass all possible  "end of sentence" marks until we find a match. Only if we find 
		a paragraph break then we set an end of sentence.
		According to this  rule, in the example "Test (test. test)  Мама." the procedure finds only  two sentences.
	*/
	BYTE InnerOpenBracket = 0;

	/*
	true, if before sentence was a quotation mark.
	*/
	bool bSentenceQuotationMarks = false;

	/*
		the byte offset of the start sentence. It used to limit the size of sentences.
	*/
	DWORD StartSentenceOffset = 0;

	/*
	 count of sentences (= end of sentences)
	*/
	long CountOfSentence = 0;

	long FirstSentStart = PPunctOrSoft (1, EndPos);
	{
		if (!SetSentMarkers (*this, 0, FirstSentStart, SentenceOpenBracket, bSentenceQuotationMarks, StartSentenceOffset))
			return false;
	};

	// for empty texts
	if (EndPos == 1) return true;



	
	// going through the input text, 
	// we should start from FirstSentStart, since SentenceOpenBracket, bSentenceQuotationMarks
	// were calculated 
	for (size_t WordNo = FirstSentStart; WordNo < EndPos; WordNo++)
	{
		// WordNo cannot be between sentences, after we have set a sentence break we should
		// shift  WordNo to the beginning of a new sentence if it is neccessary
		assert (WordNo+1 >= EndPos || !HasDescr(WordNo+1, OSentEnd));
		if (HasDescr(WordNo, OOpn))
		{
			//  we have found an inner open bracket
			InnerOpenBracket = GetUnits()[WordNo].GetToken()[0];
		};

		if (InnerOpenBracket != 0)
			if (GetUnits()[WordNo].IsChar(dual_bracket(InnerOpenBracket)  )  )
			{
				//  we have found a match for the inner open bracket
				InnerOpenBracket = 0;
			}

		if (IsOneCloseQuotationMark(WordNo))
		{
			bSentenceQuotationMarks = !bSentenceQuotationMarks;
		};

		// =========   Paragraph  delimiter ===============
		if (  HasDescr (WordNo, OPar) && (CountOfSentence>0))
		{
			size_t End = FindSentEndAfterParagraph(*this, WordNo - 1, EndPos);
			if (!SetSentMarkers (*this, End, WordNo, SentenceOpenBracket, bSentenceQuotationMarks, StartSentenceOffset))
				return false;

			// no more search for  a close bracket!
			InnerOpenBracket = 0;
			CountOfSentence++;
		}
		else 

		// =========   Macrosyntax delimiter ===============
		if (
				GetUnits()[WordNo].HasMacroSyntaxDelimiter()
			// and no fixed graphematical group will be divided by this sentence break
			// for example, we have an oborot in the text, und user has inserted accidentally a line break 
			// inside the oborot, so we have  MacroSyntaxDelimiter
			&&	IsLastInGroupOrFree(*this, WordNo) 
			)
		{
			size_t StartSent = PSoft (WordNo+1, EndPos);
			if ( StartSent == EndPos) continue;
			size_t End = FindSentEndAfterParagraph(*this, StartSent-1, EndPos);
			if (!SetSentMarkers (*this, End, StartSent, SentenceOpenBracket, bSentenceQuotationMarks, StartSentenceOffset))
				return false;
			InnerOpenBracket = 0;
			CountOfSentence++;
			WordNo = StartSent - 1;
		} 
		else
		if (m_bSentenceAsParagraph)
		{
			//  no more sentence  breaks if user wants that sentences will as long as paragraphs
		}
		else
		// ============================================
		// =========   Sentence bracket ===============

		if  (      (SentenceOpenBracket != 0)
				&& GetUnits()[WordNo].IsChar(dual_bracket(SentenceOpenBracket) )
			)
		{
			/*
				In the sentence "[Reuter] We were informed..."
				"[Reuter]"  will be a separate sentence, though there is no end of sentence after the
				close bracket
			*/
			long StartSent = WordNo+1;
			for (; StartSent<EndPos; StartSent++)
				if (GetUnits()[StartSent].IsWordOrNumberOrAbbr())
					break;

			if (CheckComma(*this, WordNo, StartSent))
			{
				if ( !SetSentMarkers (*this, WordNo, StartSent, SentenceOpenBracket, bSentenceQuotationMarks, StartSentenceOffset) )
					return false;

				WordNo = StartSent-1;
			};

			CountOfSentence++;
			InnerOpenBracket = 0;
		}
		else
		// ============================================
		// =========   Normal end of sentence =========
		// ============================================
		if	(		 IsSentenceEndMark(WordNo)    
				&&	IsLastInGroupOrFree(*this, WordNo)
				&&	!HasDescr (WordNo, OBullet)
			)
		{
			size_t StartSent;
			if (!GetUnits()[WordNo].IsSpace())
				StartSent = PPunct(WordNo, EndPos);
			else
				StartSent = PSoft(WordNo, EndPos); // assert ( GetUnits()[WordNo].IsTextAreaEnd() );
			

			if (   StartSent == EndPos ) continue; // end of file has a special processing

			StartSent--;
						
			// the end of sentence  cannot be at the very beginning of the line
			if  (GetUnits()[StartSent-1].IsEOLN ()) continue;

			// search for the next sentence start
			for (;StartSent < EndPos;	StartSent++)
			{
				if (GetUnits()[StartSent].IsWordOrNumberOrAbbr()) break; 
			};

			
		
			// if the next sentence  starts with a word then its first char should be uppercase
			if	(       StartSent<EndPos 
					&&   (HasDescr (StartSent, ORLE) || HasDescr (StartSent, OLLE))
					&&  !(HasDescr (StartSent, OUp) || HasDescr (StartSent, OUpLw))
				)
				continue;

			
			{
				// the sentence should contain at least one word
				bool bWordFound = false;
				for (size_t l = WordNo; l>0; l--)
				{
					if (GetUnits()[l].IsWordOrNumberOrAbbr()) 
					{
						bWordFound = true;
						break; 
					};
					if (HasDescr (l, OSentEnd)) break;
				};

				if (!bWordFound)
					continue;
			}


			// commas  are prohibited between sentences
			if (!CheckComma(*this, WordNo, StartSent)) continue;

			size_t  SentEnd = WordNo;
			if (		InnerOpenBracket != 0 )
			{
				/*
				if there was an open bracket before the sentence  and  there is a matching close bracket after it,
				then the end of sentence  will be the close bracket.
				For example:
				 "I (go.)" here the close bracket is end of sentence.
				*/
				size_t i = PassSpace(WordNo+1, EndPos);
				if	(	( i<EndPos)
					&&	GetUnits()[i].IsChar(dual_bracket(InnerOpenBracket) )
					)
				SentEnd = i;
			};
			if (bSentenceQuotationMarks)
			{
				// if the sentence was started with a quotation mark and terminates with a quotation mark
				//  then the close quotation mark should be the end of the sentence
				size_t i = PassSpace(SentEnd+1, EndPos);
				if	(		( i<EndPos)
						&&	IsOneCloseQuotationMark(i)
					)
				SentEnd = i;

			};

			if (m_Language == morphGerman)
				if (!CheckGermanSentenceBreak(*this, WordNo))
					continue;
			
			
			if (!SetSentMarkers (*this, SentEnd, StartSent, SentenceOpenBracket, bSentenceQuotationMarks, StartSentenceOffset))
				return false;
			
			WordNo = StartSent-1;

			CountOfSentence++;
			InnerOpenBracket = 0;
	
		}
		else
		// ============================================
		// ==  Breaking if the sentence is too long ===
		// ============================================
		if	(		!GetUnits()[WordNo].IsSoft()
				&&	(GetTokenInputOffset(WordNo) > StartSentenceOffset + m_MaxSentenceLength)
			)
		{
			// SentEnd  cannot be at the same graphematical line 
			size_t StartSent = PSoft(WordNo+1, EndPos);
			if (   StartSent == EndPos ) continue; //  the end of file  has a special processing
			if (!SetSentMarkers (*this, WordNo, StartSent, SentenceOpenBracket, bSentenceQuotationMarks, StartSentenceOffset))
				return false;
			
			CountOfSentence++;
			InnerOpenBracket = 0;
			WordNo = StartSent-1;
		};

	}; // main for  cycle

	/*
	we set OSentEnd at the the last word or delimiter of the text,
	for example:
	I go to the forest (!)
	Here the close bracket will be marked with OSentEnd, (not "!")
	*/
	if ( !SetSentMarkers (*this, BSoft (EndPos-1), EndPos,  SentenceOpenBracket, bSentenceQuotationMarks, StartSentenceOffset) )
			return false;
	

	return true;
};

