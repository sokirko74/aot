#include "stdafx.h"
#include "SemanticRusStructure.h"


void CRusSemStructure::CreateVerbAnalyticalForm(long AuxVerbNodeNo)
{
	CRusSemNode* AuxVerb = &m_Nodes[AuxVerbNodeNo];
	if (AuxVerb->m_Words.size() != 1) return;
	CRusSemWord* Word = &AuxVerb->m_Words[0];
	if (Word->m_SynWordNo == -1) return;
	const CSynWord& AuxVerbHom = m_piSent->m_Words[Word->m_SynWordNo];
	if (AuxVerbHom.m_MainVerbs.empty()) return;

	// пока обрабатываем только один главный глагол, надо будет расширить  эту процедуру для 
	// множественного случая
	assert(AuxVerbHom.m_MainVerbs.size() == 1);

	size_t MainVerbNodeNo = 0;
	{


		for (;MainVerbNodeNo < m_Nodes.size(); MainVerbNodeNo++)
			if ((m_Nodes[MainVerbNodeNo].m_Words.size() == 1)
				&& (m_Nodes[MainVerbNodeNo].m_Words[0].m_SynWordNo == AuxVerbHom.m_MainVerbs[0])
				)
				break;

		if (MainVerbNodeNo == m_Nodes.size())
		{
			assert(false);
			// the main verb is not found, why?
			return;
		};
	};


	std::string GramCodes;
	uint64_t Grammems = 0;
	long Poses = 0;


	const CRusGramTab* piRusGramTab = m_pData->GetRusGramTab();

	for (int j = 0; j < m_Nodes[MainVerbNodeNo].m_GramCodes.length(); j += 2)
		for (int i = 0; i < AuxVerb->m_GramCodes.length(); i += 2)
		{
			//  Граммемы вспомогательного глагола
			grammems_mask_t VerbGrammems;
			piRusGramTab->GetGrammems(AuxVerb->m_GramCodes.c_str() + i, VerbGrammems);;
			//  Граммемы смысловой части
			grammems_mask_t MainGrammems;
			piRusGramTab->GetGrammems(m_Nodes[MainVerbNodeNo].m_GramCodes.c_str() + j, MainGrammems);

			BYTE POS = piRusGramTab->GetPartOfSpeech(m_Nodes[MainVerbNodeNo].m_GramCodes.c_str() + j);

			//  Полученные общие граммемы и части речи
			grammems_mask_t CurrentGrammems = 0;
			part_of_speech_mask_t CurrentPoses = 0;



			if (POS == INFINITIVE)
			{
				CurrentGrammems = MainGrammems;
				if (VerbGrammems & _QM(rImperative))
				{
					//  анал. форма  повелительного наклонения ед. числа не существует
					if (VerbGrammems & _QM(rSingular)) continue;

					CurrentGrammems |= _QM(rImperative); // "будем же жить!"
				}
				else
					CurrentGrammems |= (VerbGrammems & rAllTimes); // "я стал пилить" или "стану писать"

				if ((MainGrammems & _QM(rImpersonal)) == 0)
				{
					CurrentGrammems |= (VerbGrammems & rAllPersons);
					CurrentGrammems |= (VerbGrammems & rAllNumbers);
					CurrentGrammems |= (VerbGrammems & rAllGenders);
				}
				CurrentPoses = Word->m_Poses;
			}
			else
				if (POS == PARTICIPLE_SHORT)
				{
					// "*будем же взяты!" 
					if (VerbGrammems & _QM(rImperative))
						continue;

					CurrentGrammems = MainGrammems;
					// убиваем время причастия
					CurrentGrammems &= ~rAllTimes;

					CurrentGrammems |= (VerbGrammems & rAllPersons);
					CurrentGrammems |= (VerbGrammems & rAllTimes);

					if ((VerbGrammems & _QM(rImperative)) != 0)
						CurrentGrammems |= _QM(rImperative);

					CurrentPoses |= (1 << PARTICIPLE_SHORT);
				}
				else
					if (POS == ADJ_SHORT)
					{
						if (VerbGrammems & (1 << rImperative))
							continue; // будем cчитать, что "будем же красивы!" - это плохо!
						// на самом деле, просто не хочется вводить  ее кучу кодов.

						CurrentGrammems = VerbGrammems | (MainGrammems & (rAllNumbers | rAllGenders | rAllAnimative));

						if ((CurrentGrammems & _QM(rActiveVoice)) != 0)
							CurrentGrammems &= ~_QM(rActiveVoice);

						CurrentPoses |= (1 << ADJ_SHORT);
					}
					else
						//  "мне было больно"
						if (POS == PREDK)
						{
							CurrentGrammems = VerbGrammems;

							if ((CurrentGrammems & (1 << rActiveVoice)) != 0)
								CurrentGrammems &= ~(1 << rActiveVoice);

							CurrentPoses |= (1 << POS);
						}
						else
							// вам не о чем было беспокоиться
							if (POS == PRONOUN_PREDK)
							{
								// приравниваем глагольным граммемам, сохраняя падежи
								CurrentGrammems = VerbGrammems | (rAllCases & MainGrammems);

								if ((CurrentGrammems & (1 << rActiveVoice)) != 0)
									CurrentGrammems &= ~(1 << rActiveVoice);

								CurrentPoses |= (1 << POS);
							}
							else
								// он был больше тебя
								if ((MainGrammems & _QM(rComparative)) != 0)
								{
									if ((VerbGrammems & _QM(rActiveVoice)) != 0)
										VerbGrammems &= ~_QM(rActiveVoice);

									CurrentGrammems = (VerbGrammems | _QM(rComparative));
									CurrentPoses |= (1 << POS);
								}
								else
									if (POS == ADJ_FULL)  // учнитожаем омонимию, поскольку анал. формы  с полным прилагат. не строятся.
									{
										continue;
									};

			// если вспомогательный  глагол был инфинитивом, тогда  все вместе - инфинитив
			//  "быть  лучше" или "должно быть принесено"
			if (AuxVerb->HasPOS(INFINITIVE))
			{
				CurrentPoses |= (1 << INFINITIVE);
			}
			else
				//  "будучи лишней"
				if (AuxVerb->HasPOS(ADVERB_PARTICIPLE))
				{
					CurrentPoses |= (1 << ADVERB_PARTICIPLE);
				}

			if (CurrentPoses == 0) continue;
		
			Grammems |= CurrentGrammems;
			Poses |= CurrentPoses;

		};

	assert((Grammems != 0) && (Poses != 0));

	if ((Grammems != 0) && (Poses != 0) && CanBeDeleted(MainVerbNodeNo))
	{
		// "стал писать" "стану писать" "стать писать" - совершенный вид
		if (AuxVerb->IsLemma("СТАТЬ"))
		{
			Grammems |= _QM(rPerfective);
		}

		Word->m_Lemma = m_Nodes[MainVerbNodeNo].m_Words[0].m_Lemma;
		Word->SetWord (Word->GetWord() + "-" + m_Nodes[MainVerbNodeNo].m_Words[0].GetWord());
		Word->SetFormGrammems(Grammems);
		Word->m_Poses = Poses;
		Word->m_ParadigmId = m_Nodes[MainVerbNodeNo].m_Words[0].m_ParadigmId;

		AuxVerb->SetGrammems(Grammems);
		AuxVerb->m_RichPoses = Poses;
		AuxVerb->CopyInterps(m_Nodes[MainVerbNodeNo]);
		AuxVerb->m_TypeAncode = m_Nodes[MainVerbNodeNo].m_TypeAncode;
		AuxVerb->m_Colloc = m_Nodes[MainVerbNodeNo].m_Colloc;
		MoveSynRelations(MainVerbNodeNo, AuxVerbNodeNo);
		DelNode(MainVerbNodeNo);

	};
}
