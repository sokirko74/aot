#pragma once

enum EngVerbTenseEnum
{
	zero_tn,
	present_smp_tn,		 // he works
	past_smp_tn,		 // he worked
	future_smp_tn,		 // he will work
	present_prf_tn,		 // he has worked
	past_prf_tn,		 // he had worked
	future_prf_tn,		 // he will have worked
	present_cnt_tn,		 // he is working
	past_cnt_tn,		 // he was working
	future_cnt_tn,		 // he will be working
	present_prf_cnt_tn,  // he has been working
	past_prf_cnt_tn,     // he had been working
	future_prf_cnt_tn,   // he will have been working
	would_smp_tn,        // he would work
	would_prf_tn,        // he would have worked
	would_cnt_tn,        // he would be working
	would_prf_cnt_tn,    // he would have been working    
	gerund_tn,			 // working
	gerund_prf_tn,		 // having  worked
	pp_tn,		 // worked	
	present_inf_tn,		 // to work
	present_cont_inf_tn, // to be working
	prf_inf_tn,			 // to have worked
	prf_cont_inf_tn,	 // to have been working 
	pp_cont_tn,			 // been working
	ing_tn				 // working (морфологически совпадает с gerund_tn)
};


struct CTenseHistory {
	EngVerbTenseEnum	m_Tense;
	std::string				m_Maker;
	CTenseHistory(EngVerbTenseEnum	Tense, std::string Maker)
	{
		m_Tense = Tense;
		m_Maker = Maker;
	};
	CTenseHistory()
	{
		m_Tense = zero_tn;
		m_Maker = "";
	};
};
