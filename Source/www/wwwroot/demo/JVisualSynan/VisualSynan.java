//package JVisualSynAn;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.PopupMenu;
import java.applet.Applet;
import java.lang.Integer;
import java.lang.Float;
import java.io.*;
//import org.w3c.dom;
import java.awt.TextComponent;
//import javax.xml.parsers.*;

/*
 * VisualSynAn.java
 *
 * Created on 20 Ноябрь 2001 г., 21:42
 */

//package VisualSynAn;

/**
 *
 * @author  pankrat
 */
public class VisualSynAn extends java.applet.Applet
{

    private VisualSynAnPanel m_VisualSynanPanel;    
    public Scrollbar ranger;
    /** Initializes the applet JVisualSynAn */
    public void init() 
    {
            //String struct = "#words$дом@homonyms@дом^сущ$стоял@homonyms@стоить^гл@стоять^гл$на@homonyms@на^предлог$пригорке@homonyms@пригорок^сущ#groups$0,0,0,0 @ 0,1^подл^gr$0,0,0,0 @ 0,3^гл_личн^cl @ 0,1^подл^gr ";//getParameter("struct");
            String struct = getParameter("struct");
            m_VisualSynanPanel = new VisualSynAnPanel(this, struct);
            m_VisualSynanPanel.setBackground(Color.lightGray);
            initComponents();        
            add(m_VisualSynanPanel, java.awt.BorderLayout.CENTER);

            ranger = new Scrollbar(Scrollbar.VERTICAL);
            ranger.addAdjustmentListener(m_VisualSynanPanel);
            add(ranger, java.awt.BorderLayout.EAST); 
            Font oldFondt = GramInfoLabel.getFont();
            
            Font newFont = new Font(oldFondt.getName(), oldFondt.getStyle() + Font.BOLD, oldFondt.getSize());
            
            GramInfoLabel.setForeground(Color.blue);
            GramInfoLabel.setFont(newFont);
            GramInfoLabel.setText("");
    }

    public void updateLabel(String text) 
    {
        GramInfoLabel.setText(text);
    }
    
    
    public synchronized void paint(Graphics g) 
    {
    }
  
    private void initComponents() {//GEN-BEGIN:initComponents
        GramInfoLabel = new java.awt.Label();
        
        setLayout(new java.awt.BorderLayout());
        
        setBackground(java.awt.Color.white);
        GramInfoLabel.setBackground(java.awt.Color.lightGray);
        GramInfoLabel.setText("label2");
        add(GramInfoLabel, java.awt.BorderLayout.SOUTH);
        
    }//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private java.awt.Label GramInfoLabel;
    // End of variables declaration//GEN-END:variables

}

class Homonym
{
    public Homonym(String _Lemma)
    {
        m_Lemma = _Lemma;
    }
    
    public String m_Lemma;
    public String m_strGram;
}

class WordPannel    extends Panel
                    implements MouseListener, ActionListener
{
    public int m_Y = 0;
    private String m_Word;        
    public  int m_iActiveHomonym = 0;        
    private boolean m_bSizeCalculated = false;
    VisualSynAnPanel m_visualSynAnPanel;
    Vector m_Homonyms;
    
    class VisualSynAnMouseMoveListener extends MouseAdapter 
                                       implements MouseMotionListener 
    {
        private VisualSynAn m_VisualSynAnApplet;
        
        public VisualSynAnMouseMoveListener(VisualSynAn VisualSynAnApplet)
        {
            m_VisualSynAnApplet = VisualSynAnApplet;
        }      
        
        
        public void mouseMoved(MouseEvent e) 
        {
            if( (m_iActiveHomonym >= 0) && (m_iActiveHomonym < m_Homonyms.size() ) )
            {
                Homonym hom = (Homonym)m_Homonyms.elementAt(m_iActiveHomonym);
                m_VisualSynAnApplet.updateLabel(hom.m_Lemma + " " + hom.m_strGram);
            }
            
        }   

        public void mouseDragged(java.awt.event.MouseEvent mouseEvent) {
        }
    }    

    public void addHomonym(String str)
    {
        StringTokenizer strTok = new  StringTokenizer(str, "^");
        if( !strTok.hasMoreTokens() )
            return;
        Homonym homonym = new Homonym("");
        homonym.m_Lemma = strTok.nextToken();
        if( !strTok.hasMoreTokens() )
            return;
        homonym.m_strGram = strTok.nextToken();        
        m_Homonyms.addElement(homonym);
    }

    public String Decode(String _Word, String mask, String Symbol)
    {
		boolean flog = true;
		String res = new String("");
		int prevIndex = 0;
		int index = _Word.indexOf(mask);
		while( index != -1 )
		{	
			res = res.concat(_Word.substring(prevIndex,index));		
			res = res.concat(Symbol);			
			prevIndex = index + mask.length();
			index = _Word.indexOf(mask, prevIndex);
		}
		res = res.concat(_Word.substring(prevIndex,_Word.length()));		
		return res;
    }


    public String Decode(String _Word)
    {
		String res;
		res = Decode(_Word, ";lattice" , "#" );
		res = Decode(res, ";at" , "@" );
		res = Decode(res, ";buks" , "$" );
		res = Decode(res, ";cover" , "^" );
		res = Decode(res, ";semicolon" , ";" );		
		return res;
    }
    
    public WordPannel(String _Word, VisualSynAnPanel visualSynAnPanel)
    {
        super();             
        m_Word = Decode(_Word);
        m_visualSynAnPanel = visualSynAnPanel;
        m_Homonyms = new Vector();
        /*
        m_Homonyms.addElement(new Homonym(m_Word));
        if( m_Word == "$$")
            m_Homonyms.addElement(new Homonym(m_Word));            */
        addMouseMotionListener(new VisualSynAnMouseMoveListener( m_visualSynAnPanel.m_VisualSynAnApplet) );
        addMouseListener(this);
    }
    
    public int getActiveHomonym()
    {
        return m_iActiveHomonym;
    }
    public Dimension getPreferredSize()
    {
        Dimension size = getSize();
        if( (size.height == 0) || (size.width == 0) )
        {
            setSize(new Dimension(10, 10));
            size = getSize();
        }
        return size;
    }

    public Dimension getMinimumSize()
    {        
        return getSize();
    }
       
    public void calcSize(Graphics g)
    {
        Font font = g.getFont();
        FontMetrics font_metrics = getFontMetrics(font);
        int width = font_metrics.stringWidth(m_Word);
        int height = font_metrics.getHeight();
        setSize(new Dimension(width, height));
        m_visualSynAnPanel.setValidate(true);                
    }

    private void drawCanvas(Graphics g)
    {
        Color bg = getBackground(); 
        g.setColor(bg);        
        Dimension size = getSize();              
        g.draw3DRect(0, 0, size.width - 1, size.height - 1, true);
        g.draw3DRect(3, 3, size.width - 7, size.height - 7, false);          
        g.setColor(Color.black);     
    }
    
    
    public synchronized void paint(Graphics g) 
    {  

        Font ff = new Font("Arial", Font.PLAIN, 12);
        g.setFont(ff);

        Font font = g.getFont();
        Font oldFont = g.getFont();
        if( m_Homonyms.size() > 1)
            font = new Font(oldFont.getName(), oldFont.getStyle() + Font.BOLD, oldFont.getSize());            
        
        FontMetrics font_metrics = getFontMetrics(font);
        int width = font_metrics.stringWidth(m_Word);
        int height = font_metrics.getHeight();
        
        if(!((width == getSize().width) && (height == getSize().height)))
        {
            setSize(new Dimension(width, height));            
            m_visualSynAnPanel.setValidate(true);
        }
        else
        {       
            FontMetrics font_metrics1 = getFontMetrics(g.getFont());
            g.setColor(Color.black);             
            int ii = font_metrics1.getDescent();
            Dimension dim = getSize();
            int h = (int)dim.height;
            int y = h - font_metrics1.getDescent();                  
            
            if( m_Homonyms.size() > 1)
            {
                Font newFont = new Font(oldFont.getName(), oldFont.getStyle() + Font.BOLD, oldFont.getSize());
                g.setFont(newFont);            
            }
            
            g.drawString(m_Word, 0, y);            
            g.setFont(oldFont);            
        } 
    }
    
    public void mouseExited(MouseEvent mouseEvent) {
    }
    
    public void mouseReleased(MouseEvent mouseEvent) {
    }
    
    public void mousePressed(MouseEvent mouseEvent) {
    }
    
    public void mouseClicked(MouseEvent mouseEvent) 
    {
        if( (mouseEvent.getModifiers() & InputEvent.BUTTON3_MASK) == InputEvent.BUTTON3_MASK)
        {
            PopupMenu P = new PopupMenu();            
            for(int i = 0 ; i < m_Homonyms.size() ; i++ )
            {
                MenuItem item = new MenuItem(((Homonym)m_Homonyms.elementAt(i)).m_Lemma);
                item.setActionCommand(Integer.toString(i));
                P.add(item);
            }
            
            add(P);
            P.show (this, 0, getBounds().height);       
            P.addActionListener(this);
        }
    }
    
   
    public void mouseEntered(MouseEvent mouseEvent) 
    {
    }

    
    
    public void actionPerformed(ActionEvent actionEvent) 
    {
        
        if (actionEvent.getSource() instanceof MenuItem) 
        {
            String command = actionEvent.getActionCommand();            
            int iHom = Integer.decode(command).intValue();            
            if( (iHom != -1) && (iHom != m_iActiveHomonym) )
            {
                m_iActiveHomonym = iHom;
                m_visualSynAnPanel.setValidate(true);
                m_visualSynAnPanel.invalidate();
                m_visualSynAnPanel.repaint();
            }                
        }        
    }
    
}


class WordArc
{

    public WordArc(int FirstWord, int LastWord, boolean val)
    {
        m_FirstWord = FirstWord;
        m_LastWord = LastWord;
        m_ChildArcs = new Vector();
        m_ParentGroupLeg = new Point();
        m_bGroupArc = val;
        m_ChildArcs = new Vector();
        m_ParentGroupLeg = new Point();
    }
    
    protected void Init(String strArc)
    {
        StringTokenizer strTok = new StringTokenizer(strArc, "^");
        if( !strTok.hasMoreTokens() )
            return;
        String ss;
        ss = strTok.nextToken();
        ss = ss.trim();
        //StringBuffer ssBuf = new StringBuffer(ss);        
        m_FirstWord = Integer.parseInt(ss);//.intValue();
        if( !strTok.hasMoreTokens() )
            return;
        ss = strTok.nextToken();
        ss = ss.trim();
        m_LastWord = Integer.decode(ss).intValue();
        if( !strTok.hasMoreTokens() )
            return;
        m_strName = strTok.nextToken();
        if( !strTok.hasMoreTokens() )
            return;
        ss = strTok.nextToken();
        ss = ss.trim();
        if( ss.compareTo("gr") == 0 )
        {
            m_bGroupArc = true;
            m_bIsSubj = false;
        }
        else if(ss.compareTo("cl") == 0 )
        {
            m_bGroupArc = false;
            m_bIsSubj = false;
        }
        else
        {
            m_bIsSubj = true;
        }
    }
    
    public WordArc(String strArc)
    {          
          m_ChildArcs = new Vector();
          m_ParentGroupLeg = new Point();
          Init(strArc);
    }
    
    public WordArc(int FirstWord, int LastWord)
    {
        m_FirstWord = FirstWord;
        m_LastWord = LastWord;
        m_ChildArcs = new Vector();
        m_ParentGroupLeg = new Point();
        m_bGroupArc = true;
        m_ChildArcs = new Vector();
        m_ParentGroupLeg = new Point();
    }
    
    public void addWordArc(WordArc arc)
    {
        m_ChildArcs.addElement(arc);
    }
    
    public void addWordArc(int FirstWord, int LastWord)
    {
        m_ChildArcs.addElement(new WordArc(FirstWord, LastWord));
    }
    
    private Point getLeftLegPoint(Graphics g, VisualSynAnPanel SynAnPanel)
    {
        WordPannel WordPannelLeft = SynAnPanel.getWordPannel(m_FirstWord);
        WordPannel WordPannelRight = SynAnPanel.getWordPannel(m_LastWord);

        Point leftPoint = new Point();
        boolean bSet = false;
        if( m_ChildArcs.size() > 0 )
        {
            WordArc wordArcLeft  = (WordArc)m_ChildArcs.elementAt(0);            
            if( (wordArcLeft.m_FirstWord == m_FirstWord) && 
                    (   (wordArcLeft.m_bGroupArc && m_bGroupArc) ||
                        (!wordArcLeft.m_bGroupArc && !m_bGroupArc) ||
                        ( !wordArcLeft.m_bGroupArc && m_bGroupArc)))
            {
                leftPoint = wordArcLeft.getParentGroupLeg();
                bSet = true;
            }
        }          
        
        if(!bSet)
        {
            if( m_bGroupArc )
            {
                leftPoint.x = WordPannelLeft.getBounds().x + WordPannelLeft.getBounds().width/2;
                leftPoint.y = WordPannelLeft.getBounds().y;
            }
            else
            {
                leftPoint.x = WordPannelLeft.getBounds().x;
                leftPoint.y = WordPannelLeft.getBounds().y;
            }
        }
        
        return leftPoint;
    }

    private Point getRightLegPoint(Graphics g, VisualSynAnPanel SynAnPanel)
    {
        Point rightPoint = new Point();        
        
        WordPannel WordPannelLeft = SynAnPanel.getWordPannel(m_FirstWord);
        WordPannel WordPannelRight = SynAnPanel.getWordPannel(m_LastWord);
        
        boolean bSet = false;
        if( m_ChildArcs.size() >= 1 )
        {
            WordArc wordArcRight  = (WordArc)m_ChildArcs.elementAt(m_ChildArcs.size() - 1);
            if( (wordArcRight.m_LastWord == m_LastWord) && 
                    (   (wordArcRight.m_bGroupArc && m_bGroupArc) ||
                        (!wordArcRight.m_bGroupArc && !m_bGroupArc) ||
                        ( !wordArcRight.m_bGroupArc && m_bGroupArc)))
            {
                rightPoint = wordArcRight.getParentGroupLeg();
                bSet = true;
            }
        }
        
        if( !bSet )
        {
            if( m_bGroupArc )
            {
                rightPoint.x = WordPannelRight.getBounds().x + WordPannelRight.getBounds().width/2;
                rightPoint.y = WordPannelRight.getBounds().y;
            }
            else
            {                
                rightPoint.x = WordPannelRight.getBounds().x + WordPannelRight.getBounds().width;
                rightPoint.y = WordPannelRight.getBounds().y;            
            }
        }                
        
        return rightPoint;
    }
    
    
    private void drawOneLineArc(Graphics g, VisualSynAnPanel SynAnPanel, Point leftPoint, Point rightPoint)
    {
        WordPannel WordPannelLeft = SynAnPanel.getWordPannel(m_FirstWord);
        WordPannel WordPannelRight = SynAnPanel.getWordPannel(m_LastWord);
        
        int[] xs = new int[4];
        int[] ys = new int[4];
        
        xs[0] = leftPoint.x;
        xs[1] = leftPoint.x;
        xs[2] = rightPoint.x;
        xs[3] = rightPoint.x;
        
        ys[0] = leftPoint.y;
        ys[1] = WordPannelLeft.getBounds().y - m_Height;
        ys[2] = WordPannelLeft.getBounds().y - m_Height;
        ys[3] = rightPoint.y;
        
        m_ParentGroupLeg.x = leftPoint.x + (rightPoint.x - leftPoint.x)/2;
        m_ParentGroupLeg.y = WordPannelLeft.getBounds().y - m_Height;        
        g.drawPolyline(xs, ys, 4);
        g.setColor(Color.black);    
    }
    
    
    private void drawMultyLineArc(Graphics g, VisualSynAnPanel SynAnPanel, Point leftPoint, Point rightPoint)
    {        
        WordPannel WordPannelLeft = SynAnPanel.getWordPannel(m_FirstWord);
        WordPannel WordPannelRight = SynAnPanel.getWordPannel(m_LastWord);
        
        int[] xs = new int[3];
        int[] ys = new int[3];
        
        xs[0] = leftPoint.x;
        xs[1] = leftPoint.x;
        xs[2] = SynAnPanel.getBounds().width - 5;
        
        ys[0] = leftPoint.y;
        ys[1] = WordPannelLeft.getBounds().y - m_Height;
        ys[2] = WordPannelLeft.getBounds().y - m_Height;
        
        m_ParentGroupLeg.x = xs[1] + (xs[2] - xs[1])/2;
        m_ParentGroupLeg.y = ys[1];
        
        
        g.drawPolyline(xs, ys, 3);
        
        int vGap = ((FlowLayout)SynAnPanel.getLayout()).getVgap();
        int wordPanelHeight = WordPannelLeft.getSize().height;
        
        int curY = WordPannelLeft.getBounds().y + vGap + wordPanelHeight;
        while ( rightPoint.y - curY  > 0 )
        {
            g.drawLine(5,curY - m_Height, SynAnPanel.getBounds().width-5, curY - m_Height);
            curY += vGap;
            curY += wordPanelHeight;
        }
            
        xs[0] = 5;
        xs[1] = rightPoint.x;
        xs[2] = rightPoint.x;
        
        ys[0] = curY - m_Height;
        ys[1] = curY - m_Height;
        ys[2] = rightPoint.y;
        
        g.drawPolyline(xs, ys, 3);
        g.setColor(Color.black);    
    }
    
    public void Draw(Graphics g, VisualSynAnPanel SynAnPanel)
    {
        m_Height = getHeight();
        for(int i = 0 ; i < m_ChildArcs.size() ; i++ )
        {         
            WordArc wordArc  = (WordArc)m_ChildArcs.elementAt(i);
            wordArc.Draw(g, SynAnPanel);
        }   

        WordPannel WordPannelLeft = SynAnPanel.getWordPannel(m_FirstWord);
        WordPannel WordPannelRight = SynAnPanel.getWordPannel(m_LastWord);
        
        Point leftPoint; 
        Point rightPoint;
        
        leftPoint = getLeftLegPoint(g, SynAnPanel);
        rightPoint = getRightLegPoint(g, SynAnPanel);

        if( m_bGroupArc )
        {
            g.setColor(Color.blue);
        }
        else
        {
            g.setColor(Color.red);
        /*    leftPoint = new Point();
            rightPoint = new Point();
            
            leftPoint.x = WordPannelLeft.getBounds().x ;
            leftPoint.y = WordPannelLeft.getBounds().y;
            rightPoint.x = WordPannelRight.getBounds().x + WordPannelRight.getBounds().width;
            rightPoint.y = WordPannelRight.getBounds().y;*/
        }

        int vGap = ((FlowLayout)SynAnPanel.getLayout()).getVgap();
        
        if( rightPoint.y <= WordPannelLeft.getBounds().y  )        
            drawOneLineArc(g, SynAnPanel, leftPoint, rightPoint);
        else
            drawMultyLineArc(g, SynAnPanel, leftPoint, rightPoint);        
        
        Font ff = new Font("Arial", Font.PLAIN, 12);
        g.setFont(ff);

        Font oldFont = g.getFont();
        Font smallFont = VisualSynAnPanel.getSmallFont(g);
        g.setFont(smallFont);
        g.drawString(m_strName, m_ParentGroupLeg.x + 2, m_ParentGroupLeg.y - 2);
        g.setFont(oldFont);
    }
    
    public int getHeight()
    {
        if( m_Height == 0 )
            m_Height = calculateHeight();
        return m_Height;
    }
    
    
    private int calculateHeight()
    {
        int Height = 0;
        for(int i = 0 ; i < m_ChildArcs.size() ; i++ )
        {
            WordArc wordArc  = (WordArc)m_ChildArcs.elementAt(i);
            int ii = wordArc.calculateHeight();
            if( ii >  Height)
                Height = ii;
        }
        return Height + VisualSynAnPanel.ArcHeight;
    }
    
    private Point getParentGroupLeg()
    {
        return m_ParentGroupLeg;
    }
    
    private String m_strName;
    public int m_FirstWord = 0;
    private Point m_ParentGroupLeg;
    public int m_LastWord = 0;
    private int m_Height = 0;
    private boolean m_bGroupArc = true;
    public boolean m_bIsSubj = false;
    private Vector m_ChildArcs;
    private boolean m_IsGroup;
}

class VisualSynAnPanel  extends Panel                         
                        implements AdjustmentListener, ComponentListener
{
   static int ArcHeight = 20;

   static Font getSmallFont(Graphics g)
   {
       Font font = g.getFont();
       //float size = font.getSize()*((float)3)/((float)7);
       int size = font.getSize()*5/6;
       //int size = font.getSize();
       Font newFont = new Font(font.getName(), font.getStyle(), size);
       return newFont;
   }
   
    private void addWordArcs()
    {
        {
            m_ChildArcs = new Vector();        
            WordArc arc = new WordArc(0, 4);
            arc.addWordArc(0,1);        
            WordArc arc1 = new WordArc(2, 4, false);
            arc1.addWordArc(2,3);
            arc.addWordArc(arc1);
            m_ChildArcs.addElement(arc);                
        }
        
        {
            m_ChildArcs1 = new Vector();
            WordArc arc = new WordArc(0, 4);
            m_ChildArcs1.addElement(arc);                        
        }
    }
    
    private void addWordPanels()
    {
        for(int i = 0 ; i < m_WordPanels.size() ; i++ )
            add((WordPannel)m_WordPanels.elementAt(i));
    }
    
    protected void parseOneWord(String strWord)
    {
        StringTokenizer strTok = new StringTokenizer(strWord,"@");
        if( !strTok.hasMoreTokens() )
            return;
        String str = strTok.nextToken();
        
        WordPannel word = new WordPannel(str,this);
        if( !strTok.hasMoreTokens() )
            return;
        
        str = strTok.nextToken();
        
        if( str.compareTo("homonyms") != 0 )
            return;       
        while( strTok.hasMoreTokens() )
            word.addHomonym(strTok.nextToken());
        
        m_WordPanels.addElement(word);
    }
    
    protected void parseWords(String strWords)
    {
        StringTokenizer strTok = new StringTokenizer(strWords,"$");
        if( !strTok.hasMoreTokens() )
            return;
        String str = strTok.nextToken();
        if( str.compareTo( "words") != 0 )
            return;
        while( strTok.hasMoreTokens() )
            parseOneWord(strTok.nextToken());
    }
    
    protected void fillHomonymNumbers(HomonymNumbers homs, String str, int wordsCount)
    {
        StringTokenizer strTok = new StringTokenizer(str,", ");
        int ii = 0;
        int[] arr = new int[wordsCount];
        while( strTok.hasMoreTokens() && (ii < wordsCount))
        {
            String strItem = strTok.nextToken();
            int homNum = Integer.decode(strItem).intValue();            
            arr[ii] = homNum;
            ii++;
        }
        
        homs.m_HomNums = arr;
    }
    
    protected void parseOneVariant(String strVar)
    {
        StringTokenizer strTok = new StringTokenizer(strVar,"@");
        HomonymNumbers homs = new HomonymNumbers();
        if( !strTok.hasMoreTokens() )
            return;
        
        fillHomonymNumbers(homs,strTok.nextToken(), m_WordPanels.size());
        Vector arcs = new Vector();
        HomonymNumbers2WorddArcs homonymNumbers2WorddArcs = new HomonymNumbers2WorddArcs();
        homonymNumbers2WorddArcs.m_SubjArcs = new Vector();
        
        while( strTok.hasMoreTokens() )
        {
            WordArc arc = new WordArc(strTok.nextToken());
            if( arc.m_bIsSubj )
                homonymNumbers2WorddArcs.m_SubjArcs.addElement(arc);
            else
                arcs.addElement(arc);
        }
        
        homonymNumbers2WorddArcs.m_HomonymNumbers = homs;
        homonymNumbers2WorddArcs.m_Arcs = new Vector();
        orderArcs(arcs, homonymNumbers2WorddArcs.m_Arcs);
        m_HomNums2ArcVariant.addElement(homonymNumbers2WorddArcs);
    }
    
    protected int orderArcsRec(Vector arcs, WordArc parentArc, int iCur)
    {        
        for(int i = iCur ; i < arcs.size() ;  )
        {
            WordArc arc = (WordArc)arcs.elementAt(i);
            if( arc.m_FirstWord > parentArc.m_LastWord )
                return i;
            i = orderArcsRec(arcs, arc, i + 1);            
            parentArc.addWordArc(arc);
        }
        return arcs.size();
    }
    
    protected void orderArcs(Vector arcs, Vector newVector)
    {
        for(int i = 0 ; i < arcs.size() ;  )
        {
            WordArc arc = (WordArc)arcs.elementAt(i);
            i = orderArcsRec(arcs, arc, i + 1 );
            newVector.addElement(arc);
        }
    }
    
    protected void parseVariants(String strVars)
    {
        StringTokenizer strTok = new StringTokenizer(strVars,"$");
        if( !strTok.hasMoreTokens() )
            return;
        String str = strTok.nextToken();
        if( str.compareTo("groups") != 0)
            return;
        while( strTok.hasMoreTokens() )
            parseOneVariant(strTok.nextToken());    
        setActiveHomonyms();
    }
    
    protected void setActiveHomonyms()
    {
        if( m_HomNums2ArcVariant.size() == 0 )
            return;
        HomonymNumbers2WorddArcs hom = (HomonymNumbers2WorddArcs)m_HomNums2ArcVariant.elementAt(0);
        
        for(int i = 0 ; i < hom.m_HomonymNumbers.m_HomNums.length ; i++ )
        {
            WordPannel panel = getWordPannel(i);            
            panel.m_iActiveHomonym = hom.m_HomonymNumbers.m_HomNums[i];
        }
    }
    
    protected void parseMainArgumnet(String mainStr)
    {
        StringTokenizer strTok = new StringTokenizer(mainStr,"#");
        if( !strTok.hasMoreTokens() )
            return;
        parseWords(strTok.nextToken());
        if( !strTok.hasMoreTokens() )
            return;
        parseVariants(strTok.nextToken());        
    }
    
    public VisualSynAnPanel(VisualSynAn VisualSynAnApplet, String strArg)
    {
        super();                   
        m_WordPanels = new Vector();        
        m_HomNums2ArcVariant = new Vector();
        m_VisualSynAnApplet = VisualSynAnApplet;                
        m_bValidate = false;
        setLayout(new java.awt.FlowLayout(FlowLayout.LEFT,10, 10));                                      
        parseMainArgumnet(strArg);
        
         /*
        addWordArcs();
        
        
      
        {
            int[] arr = {0,0,0,0,0};        
            HomonymNumbers homs = new HomonymNumbers(arr);
            HomonymNumbers2WorddArcs homonymNumbers2WorddArcs = new HomonymNumbers2WorddArcs();
            homonymNumbers2WorddArcs.m_HomonymNumbers = homs;
            homonymNumbers2WorddArcs.m_Arcs = m_ChildArcs;
            m_HomNums2ArcVariant.addElement(homonymNumbers2WorddArcs);
        }
        
        {
            int[] arr = {1,0,0,0,0};        
            HomonymNumbers homs = new HomonymNumbers(arr);
            HomonymNumbers2WorddArcs homonymNumbers2WorddArcs = new HomonymNumbers2WorddArcs();
            homonymNumbers2WorddArcs.m_HomonymNumbers = homs;
            homonymNumbers2WorddArcs.m_Arcs = m_ChildArcs1;            
            m_HomNums2ArcVariant.addElement(homonymNumbers2WorddArcs);
        }
        m_bValidate = false;
        setLayout(new java.awt.FlowLayout(FlowLayout.LEFT,10, 10));                              
        
        m_WordPanels = new Vector();        
        
        
        m_WordPanels.addElement(new WordPannel("$$",this));
        m_WordPanels.addElement(new WordPannel("the_word2",this));
        m_WordPanels.addElement(new WordPannel("the_word3qy",this));
        m_WordPanels.addElement(new WordPannel("the_word4",this));
        m_WordPanels.addElement(new WordPannel("the",this));                                        
        */
        addWordPanels();
        addComponentListener(this);
    }  
    
    private void setWordPannelsSize(Graphics g)
    {
        int size = m_WordPanels.size();
        for(int i = 0 ; i < size ; i++ )
        {            
            WordPannel wPanel = (WordPannel)m_WordPanels.elementAt(i);
            wPanel.calcSize(g);
        }
    }
    
    private void drawCanvas(Graphics g)
    {
        Color bg = getBackground(); 
        g.setColor(bg);        
        Dimension size = getSize();              
        g.draw3DRect(0, 0, size.width - 1, size.height - 1, true);
        g.draw3DRect(3, 3, size.width - 7, size.height - 7, false);          
        g.setColor(Color.black);     
    }
    
    private int calcMaxArcHeight(Vector arcs)
    {
        int height = 0;
        for(int i = 0 ; i < arcs.size() ; i++ )
        {
            int curHeight = ((WordArc)arcs.elementAt(i)).getHeight();
            if(  curHeight > height )
                height = curHeight;
        }
        return height;
    }
    
    public synchronized void validate()
    {
        reposeWordPanels();
        super.validate();
    }
    
    public synchronized void update(Graphics g) 
    {
        reposeWordPanels();
        super.update(g);
    }
    
    public synchronized void paintComponents(Graphics g)
    {
        //reposeWordPanels();
        super.paintComponents(g);
    }
    
    public synchronized void paint(Graphics g) 
    {        
        
        Vector arcs = getCurArcs();                
        
        if( m_bValidate )
        {   
            int height = calcMaxArcHeight(arcs);
            ((FlowLayout)getLayout()).setVgap(height + getSmallFont(g).getSize() + 10);
            yScrolBarSpan = 0;
            yScrolBarVal = 0;
            validate();                     
            m_bValidate = false;        
            Component comp = getComponent( getComponentCount() - 1 );
            m_VisualSynAnApplet.ranger.setValues(0, getBounds().height,0, comp.getBounds().y + comp.getBounds().height + 20);                        
            setWordPanelsInitialY();
        }         
        drawArcs(g, arcs);    
        drawCanvas(g);        
        drawSubjPredic(g);
    }    

    private void drawSubjPredic(Graphics g)
    {
        if( m_iCurHoms < 0 )
            return;
        HomonymNumbers2WorddArcs homs = (HomonymNumbers2WorddArcs)m_HomNums2ArcVariant.elementAt(m_iCurHoms);
        for(int i = 0 ; i < homs.m_SubjArcs.size() ; i++ )
        {
            WordArc arc = (WordArc)homs.m_SubjArcs.elementAt(i);
            WordPannel panel1 = (WordPannel)m_WordPanels.elementAt(arc.m_FirstWord);
            WordPannel panel2 = (WordPannel)m_WordPanels.elementAt(arc.m_LastWord);
            drawSubj(panel1, g);
            drawPredic(panel2, g);
            
        }
    }

     private void drawSubj(WordPannel panel, Graphics g)
     {
        Rectangle rect = panel.getBounds();
        g.drawLine(rect.x, rect.y + rect.height , rect.x + rect.width, rect.y + rect.height );
     }
     
     private void drawPredic(WordPannel panel, Graphics g)
     {
        Rectangle rect = panel.getBounds();
        g.drawLine(rect.x, rect.y + rect.height , rect.x + rect.width, rect.y + rect.height );
        g.drawLine(rect.x, rect.y + rect.height + 2, rect.x + rect.width, rect.y + rect.height + 2);
     }

    
    private Vector getCurArcs()
    {
        HomonymNumbers homs = getActiveHomonymNumbers();
        
        for(int i = 0 ; i < m_HomNums2ArcVariant.size() ; i++ )
        {
            HomonymNumbers2WorddArcs homonymNumbers2WorddArcs = (HomonymNumbers2WorddArcs)m_HomNums2ArcVariant.elementAt(i);            
            if( homonymNumbers2WorddArcs.equals(homs) )
            {
                m_iCurHoms = i;
                return homonymNumbers2WorddArcs.m_Arcs;
            }
            
        }
        
        m_iCurHoms = -1;
        return null;
    }

    private void drawArcs(Graphics g, Vector arcs)
    {
        for(int i = 0 ; i < arcs.size() ; i++ )                        
        {
            WordArc arc = (WordArc)arcs.elementAt(i);
            arc.Draw(g, this);
        }
    }

    
    public WordPannel getWordPannel(int i )
    {
        return  (WordPannel)(m_WordPanels.elementAt(i));
    }
    
    public void setValidate(boolean val)
    {
        m_bValidate = val;
    }
    
    private HomonymNumbers getActiveHomonymNumbers()
    {
        int[] homs = new int[m_WordPanels.size()];
        for(int i = 0 ; i < m_WordPanels.size() ; i++ )
            homs[i] = getWordPannel(i).getActiveHomonym();
        
        return new HomonymNumbers(homs);
    }
    
    public void reposeWordPanels()
    {
        for(int i = 0 ; i < m_WordPanels.size() ; i++ )
        {
            WordPannel word = getWordPannel(i);
            Rectangle oldRect = word.getBounds();            
            //getWordPannel(i).setLocation(oldRect.x, oldRect.y + yScrolBarSpan);                              
            word.setLocation(oldRect.x, word.m_Y - yScrolBarVal);                    
        }
    }
    
   
    public void setWordPanelsInitialY()
    {
        for(int i = 0 ; i < m_WordPanels.size() ; i++ )
        {
            Rectangle Rect = getWordPannel(i).getBounds();            
            getWordPannel(i).m_Y = Rect.y;
        }
    }
   
    public void adjustmentValueChanged(AdjustmentEvent adjustmentEvent) 
    {
        int val = adjustmentEvent.getValue();
        yScrolBarSpan =  yScrolBarVal - val;
        yScrolBarVal = val;
        repaint();
   }
    
    public void componentShown(java.awt.event.ComponentEvent componentEvent) {
    }
    
    public void componentMoved(java.awt.event.ComponentEvent componentEvent) {
    }
    
    public void componentResized(ComponentEvent componentEvent) 
    {
        Vector arcs = getCurArcs();
        Graphics g = getGraphics();
        
        yScrolBarSpan = 0;
        yScrolBarVal = 0;

        setWordPannelsSize(g);                
        int height = calcMaxArcHeight(arcs);
        ((FlowLayout)getLayout()).setVgap(height + getSmallFont(g).getSize() + 10);
        validate();            
        m_bValidate = false;        
        Component comp = getComponent( getComponentCount() - 1 );
        m_VisualSynAnApplet.ranger.setValues(0, getBounds().height,0, comp.getBounds().y + comp.getBounds().height + 20);        
        setWordPanelsInitialY();
    }
    
    public void componentHidden(java.awt.event.ComponentEvent componentEvent) {
    }
    
    public VisualSynAn m_VisualSynAnApplet;           
    private Vector m_ChildArcs;
    private Vector m_ChildArcs1;
    //private TreeMap m_HomNums2ArcVariant;
    private Vector m_HomNums2ArcVariant;
    private int m_iCurHoms = 0;
    private Vector m_WordPanels;
    private boolean m_bValidate;
    private Scrollbar ranger;
    private int yScrolBarVal = 0;
    private int yScrolBarSpan = 0;
}

class HomonymNumbers 
{
    public HomonymNumbers(int[] _HomNums)
    {
        m_HomNums = (int[])(_HomNums.clone());
    }
    
    public HomonymNumbers()
    {       
        
    }
    
    public int compareTo(Object _obj) 
    {
        HomonymNumbers HomNums2 = (HomonymNumbers)_obj;
        for(int i = 0 ; i < HomNums2.m_HomNums.length ; i++ )
        {
            if( m_HomNums[i] < HomNums2.m_HomNums[i] )
                return -1;
            if( m_HomNums[i] > HomNums2.m_HomNums[i] )
                return 1;
        }
        return 0;
    }
    
    public int[] m_HomNums;
}

class HomonymNumbers2WorddArcs 
{
   
    public HomonymNumbers m_HomonymNumbers;
    public Vector m_Arcs;
    public Vector m_SubjArcs;    
    public boolean equals(HomonymNumbers homonymNumbers)
    {
        return (m_HomonymNumbers.compareTo(homonymNumbers) == 0);
    }
}
