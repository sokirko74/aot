#ifndef LessDomItem_h
#define LessDomItem_h

class IsLessByItemStrNew : public std::binary_function<const TDomItem, const TDomItem, bool>
{
 public:
  const TItemContainer* m_pRoss;

  IsLessByItemStrNew(const TItemContainer* pRoss)
	{m_pRoss = pRoss;}

  bool operator()(const TDomItem& Item1, const TDomItem& Item2)
  {
	  if (Item1.GetDomNo() != Item2.GetDomNo())
		  return (Item1.GetDomNo() < Item2.GetDomNo());
	  else
		return	 (strcmp (m_pRoss->GetDomItemStr(Item1), m_pRoss->GetDomItemStr(Item2)) < 0);
  }
};



class IsLessByNotStableItemStrNew : public std::binary_function<const TDomItem&, const TDomNoItemStr&, bool>
{
 public:
  const TItemContainer* m_pRoss;

  IsLessByNotStableItemStrNew(const TItemContainer* pRoss)
	{m_pRoss = pRoss;}

  bool operator()(const TDomItem& Item1, const TDomNoItemStr& Item2) const 
  {
	  if (Item1.GetDomNo() != Item2.DomNo)
		return (Item1.GetDomNo() < Item2.DomNo);
	  else
		return	 (strcmp (m_pRoss->GetDomItemStr(Item1), Item2.ItemStr) < 0);
  }
  bool operator()(const TDomNoItemStr& Item1,  const TDomItem& Item2) const
  {
	  if (Item1.DomNo != Item2.GetDomNo())
		return (Item1.DomNo < Item2.GetDomNo());
	  else
		return	 (strcmp (Item1.ItemStr, m_pRoss->GetDomItemStr(Item2)) < 0);
  }
  bool operator()(const TDomItem& Item1, const TDomItem& Item2) const 
  {
	  if (Item1.GetDomNo() != Item2.GetDomNo())
		return (Item1.GetDomNo() < Item2.GetDomNo());
	  else
		return	 (strcmp (m_pRoss->GetDomItemStr(Item1), m_pRoss->GetDomItemStr(Item2)) < 0);
  }

};

#endif
