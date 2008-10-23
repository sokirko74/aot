import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.PopupMenu;
import java.applet.Applet;
import java.lang.Integer;
import java.lang.Float;
import java.io.*;
import java.awt.TextComponent;
/*
   класс MessageBox, чтобы показывать сообщени€
*/
class MessageBox extends Dialog
{
  public MessageBox(String sMsg,Frame  parent, String  sTitle, int x, int y, Container Owner)
  {
   super(parent, sTitle, true);
   setTitle (sTitle);
   setModal(true);
   
   // пишем содержимое сообщени€ 
   Point point = Owner.getLocationOnScreen();
   
   setBounds(point.x+x,point.y+y,200,200);
   TextArea  T = new TextArea (sMsg);
   T.setEditable (false);
   add (T);
   
   // создаем кнопку OK
   Panel P = new Panel();
   FlowLayout layout = new FlowLayout();
   P.setLayout(layout);
   add("South", P);
   Button button = new Button("   ok   ");
   P.add(button);   
  }
  
  public boolean handleEvent(Event evt)
  {
    if(evt.id == Event.WINDOW_DESTROY)
    {
      dispose();
      return true;
    }
    else
      return super.handleEvent(evt);
 }
  
 public boolean action(Event evt, Object arg) 
 {
		if ("   ok   ".equals(arg)) {
		     dispose();
		    return true;
		}
		return false;
 }

}
/*
  класс  узел графа
*/

class Node {
	// координаты узла
    double m_x;
    double m_y;
	double m_width;
	double m_height;
    // €вл€етс€ ли узел фокусным
    boolean m_bFixed;
	// метка узла
    String m_Label;
	// морфологи€ узла 
	String m_Morphology;
}
/*
  класс  отношени€
*/

class Edge {
    int m_SourceNodeNo;
    int m_TargetNodeNo;
    String m_Label;
}

/*
  класс  панели, котора€ показывает граф
*/

class GraphPanel extends Panel 
		implements AdjustmentListener
{
    SemGraph parent;
    int m_NodesCount;
    Node m_Nodes[] = new Node[100];

    int m_RelationsCount;
    Edge m_Relations[] = new Edge[200];
    int yScrolBarVal = 0;
	int xScrolBarVal = 0;


    GraphPanel(SemGraph _parent) 
	{
		super();                   
		this.parent = _parent;
    }

	// добал€ет узел в граф
    void addNode(String lbl, int x, int y, String Morphology) 
	{
		Node n = new Node();
		n.m_Label = lbl;
		n.m_x = x;
		n.m_y = y;
		n.m_Morphology = Morphology;
		m_Nodes[m_NodesCount] = n;
		m_NodesCount++;
    }
	// добал€ет отношение  в граф
    void addEdge(int  SourceNo, int TargetNo, String Label) 
	{
		Edge e = new Edge();
		e.m_SourceNodeNo = SourceNo;
		e.m_TargetNodeNo = TargetNo;
		e.m_Label = Label;
		m_Relations[m_RelationsCount++] = e;
    }
	
	// обнул€ет граф
	void Clear() 
	{
		m_RelationsCount = 0;
		m_NodesCount  = 0;
	};
		

    public void show() 
	{
	  repaint();
    }
	// текущий узел (фокусный узел)
    Node pick;
	
	// текуща€ графика
    Image image;
    int image_width;
	int image_height;
    Graphics image_graphics;
    
	// цветовые константы
    final Color fixedColor = Color.red;
    final Color selectColor = Color.pink;
    final Color edgeColor = Color.black;
    final Color nodeColor = new Color(250, 220, 100);
    final Color stressColor = Color.darkGray;
    final Color arcColor = Color.red;

	// рисует узел 
    public void paintNode(Graphics g, Node n, FontMetrics fm) 
	{
		int x = (int)n.m_x;
		int y = (int)n.m_y;
		g.setColor(nodeColor);
		if (n.m_bFixed) 
		{
			g.setColor(fixedColor);
		};

		// рисуем метку узла
		String Label = n.m_Label;
		Font font = new Font("Arial", Font.PLAIN, 12);
		g.setFont(font);

		int w = fm.stringWidth(Label) + 10;
		int h = fm.getHeight() + 4;
		g.fillRect(x - w/2, y - h / 2, w, h);
		g.setColor(Color.black);
		g.drawRect(x - w/2, y - h / 2, w-1, h-1);
		g.drawString(Label, x - (w-10)/2, (y - (h-4)/2) + fm.getAscent());
		
		n.m_width = w;
		n.m_height = h;
    }
	
	// рисуем стрелку и название отношени€
  public void draw_arrows() 
  {
	int arrowLength = 15;   
	int arrowAngle = 20;
    for(int i=0; i < m_RelationsCount; i++) 
	{
	    Edge e = m_Relations[i];
	    int x1 = (int)m_Nodes[e.m_SourceNodeNo].m_x;
	    int y1 = (int)m_Nodes[e.m_SourceNodeNo].m_y;
	    int x2 = (int)m_Nodes[e.m_TargetNodeNo].m_x;
	    int y2 = (int)m_Nodes[e.m_TargetNodeNo].m_y;
		
		// длина отношени€
		int len = (int)Math.abs(Math.sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
		// тангенс угла 
		double gradient = ((double)(x2 - x1))/(y2 - y1);
		
		// перевод из радиан в градусы
		double a_const = (double)(360/(2*Math.PI));
		double angle_in_radians = Math.atan(gradient);
		// angle - угол наклона в градусах
		int angle =  (int)(  a_const * angle_in_radians );
		
		// вычисление насколько нужно сместитьс€ по пр€мой,
		// чтобы конец стрелки указывал на пр€моугольник метки узла 
		double arrowx;
		double arrowy;
		double Offset = m_Nodes[e.m_TargetNodeNo].m_height/ (Math.cos(angle_in_radians)*2);
		double label_gradient =  m_Nodes[e.m_TargetNodeNo].m_width/m_Nodes[e.m_TargetNodeNo].m_height;
		double angle_label = Math.atan(label_gradient);
		if (    (angle_label <  angle_in_radians)
		   )
			Offset = m_Nodes[e.m_TargetNodeNo].m_width/ (Math.sin(angle_in_radians)*2);
		
		if (   (angle < 0)
			&& (angle_in_radians <  -angle_label)
		   )
			Offset = -m_Nodes[e.m_TargetNodeNo].m_width/ (Math.sin(angle_in_radians)*2);
		
		// рисуем стрелку
		double label_x;
		double label_y;
		image_graphics.setColor(arcColor); 
		if (y1 < y2) {
			 
			 arrowx = x2 - Math.sin(angle_in_radians)*Offset - arrowLength;
			 arrowy = y2 - Math.cos(angle_in_radians)*Offset - arrowLength;
			 image_graphics.fillArc((int)arrowx,(int)arrowy, arrowLength*2,arrowLength*2, 
								 (angle-arrowAngle + 90), arrowAngle*2);
			 label_x = x2 - Math.sin(angle_in_radians)*len/2;
		     label_y = y2 - Math.cos(angle_in_radians)*len/2;
		}
		else
		{
			 arrowx = x2 + Math.sin(angle_in_radians)*Offset - arrowLength;
			 arrowy = y2 + Math.cos(angle_in_radians)*Offset - arrowLength;
			 image_graphics.fillArc((int)arrowx,(int)arrowy, arrowLength*2, arrowLength*2,
								 (angle-arrowAngle + 270), arrowAngle*2);
 			 label_x = x2 + Math.sin(angle_in_radians)*len/2;
		     label_y = y2 + Math.cos(angle_in_radians)*len/2;
		};
			
		//рисуем метку отношени€		
		image_graphics.setColor(Color.black);
		image_graphics.drawString(e.m_Label, (int)label_x, (int)label_y);
    }
  }
  
  public synchronized boolean calc_image_size() 
  {
		// узнаем размеры текущего окна	  
		Dimension d = size();
		// узнаем максимальное значение х и у дл€ данного рисунка
		int new_image_width = d.width;
		int new_image_height = d.height;
		for (int i = 0 ; i < m_NodesCount ; i++) 
		{
			Node n = m_Nodes[i];
			if (new_image_width <  (int)n.m_x + 60) // 30 - максимальна€ ширина узла (примерно)
				new_image_width = (int)n.m_x + 60;
			if (new_image_height <  (int)n.m_y + 30) // 30 - максимальна€ высота узла (примерно)
				new_image_height = (int)n.m_y + 30;
		}
		if (    (new_image_width == image_width)
			 && (new_image_height == image_height)
		  )
		{
			return false;
		}
		else
		{
			image_width = new_image_width;
			image_height = new_image_height;
			return true;
		};
  };
  
  public synchronized void update(Graphics g) 
  {
		if (   calc_image_size()
			|| (image == null) 
		   )
		{
		    image = createImage(image_width, image_height);
		    image_graphics = image.getGraphics();
		    image_graphics.setFont(getFont());
		}
		
		
		

		// очищаем экран 
		image_graphics.setColor(getBackground());
		image_graphics.fillRect(0, 0, image_width, image_height);
		// рисуем отношени€
		for (int i = 0 ; i < m_RelationsCount ; i++) 
		{
		    Edge e = m_Relations[i];
		    int x1 = (int)m_Nodes[e.m_SourceNodeNo].m_x;
		    int y1 = (int)m_Nodes[e.m_SourceNodeNo].m_y;
		    int x2 = (int)m_Nodes[e.m_TargetNodeNo].m_x;
		    int y2 = (int)m_Nodes[e.m_TargetNodeNo].m_y;
		    int len = (int)Math.abs(Math.sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)));
		    image_graphics.setColor(arcColor) ;
		    image_graphics.drawLine(x1, y1, x2, y2);
		}
		// рисуем стрелки  и названи€
		draw_arrows();
	
		// рисуем узлы
		FontMetrics fm = image_graphics.getFontMetrics();
		for (int i = 0 ; i < m_NodesCount ; i++) 
		{
		    paintNode(image_graphics, m_Nodes[i], fm);
		}
		
		
		// узнаем размеры текущего окна	  
		Dimension d = size();
		// устанавливаем вертикальный scrollbar
		if(image_height == d.height)
			yScrolBarVal = 0;
		else 
			if (yScrolBarVal > image_height)
				yScrolBarVal = image_height;
		parent.vert_ranger.setValues(yScrolBarVal, d.height, 0,  image_height);
			
		
		
		// устанавливаем горизонтальный scrollbar
		if(image_width == d.height)
			xScrolBarVal = 0;
		else 
			if (xScrolBarVal > image_width)
				xScrolBarVal = image_width;
		parent.hor_ranger.setValues(xScrolBarVal, d.width, 0,  image_width);

		
		
		
		 
		// копируем  картинку в текущее окно			
		image_graphics.copyArea(xScrolBarVal,yScrolBarVal, d.width, d.height, -xScrolBarVal, -yScrolBarVal);
				
		//======= debug
		/*
		 String trace ;
		 Integer q = new Integer(yScrolBarVal);
		 trace = q.toString();
 		 image_graphics.setColor(Color.black);
		 image_graphics.drawString(trace, 5, 15);
		*/
		//===============
		
		// выкидываем картинку			
		g.drawImage(image, 0, 0,  null);
   }
   /*
     экранные координаты курсора мыши, где последний раз была нажата кака€-та
     кнопка мыши
   */
   int last_x;
   int last_y;
   
   public boolean action(Event evt, Object arg) 
   {
	   // выводим сообщение на экран 
		if ("Morphology".equals(arg)) {
			Frame frm = new Frame ("Properties");
			MessageBox mbox;
			mbox = new MessageBox(
			  pick.m_Morphology, 
			  frm, "Morphology", (int)last_x, (int)last_y, this);
			mbox.show();
		    return true;
		}
		return false;
  }


    public synchronized boolean mouseDown(Event evt, int x, int y) 
	{
		// сохран€ем последние координаты
		last_x = x;
		last_y = y;
		// если была нажата права€ кнопка мыши
		if (evt.modifiers == 4) {
				PopupMenu P = new PopupMenu();
				P.add ("Morphology");
				P.add ("Dictionary Interpretations");
				add(P);
				P.show (this, x,y);
		  	    return true;
		};

		// нужно учесть сколько мы уже прокрутили по scrollbar 
		yScrolBarVal = parent.vert_ranger.getValue();
		y += yScrolBarVal;
		
		xScrolBarVal = parent.hor_ranger.getValue();
		x += xScrolBarVal;

		// вычисление ближайшего узла
		double bestdist = Double.MAX_VALUE;
		for (int i = 0 ; i < m_NodesCount ; i++) 
		{
		    Node n = m_Nodes[i];
			n.m_bFixed = false;
		    double dist = (n.m_x - x) * (n.m_x - x) + (n.m_y - y) * (n.m_y - y);
		    if (dist < bestdist) 
			{
				pick = n;
				bestdist = dist;
		    }
		}
		// запись фокусного узла
		pick.m_bFixed = true;
		pick.m_x = x;
		pick.m_y = y;
		repaint();
		return true;
    }

    public synchronized boolean mouseDrag(Event evt, int x, int y) 
	{
        yScrolBarVal = parent.vert_ranger.getValue();
		xScrolBarVal = parent.hor_ranger.getValue();

		pick.m_x = x + xScrolBarVal;
		pick.m_y = y + yScrolBarVal;
		repaint();
		return true;
    }
	
    public synchronized void paint(Graphics g) {
	  update(g);
    }


    public void start() 
	{
		show();
    }
    public void adjustmentValueChanged(AdjustmentEvent adjustmentEvent) 
    {
		if (adjustmentEvent.getSource() == parent.vert_ranger)
			yScrolBarVal = adjustmentEvent.getValue();
		else
			if (adjustmentEvent.getSource() == parent.hor_ranger)
				xScrolBarVal = adjustmentEvent.getValue();
		//!!!!
        repaint();
   }

}

public class SemGraph extends Applet {
   	public GraphPanel panel;
	public Scrollbar vert_ranger;
	public Scrollbar hor_ranger;
	int test_y;
	int m_Width;
	int m_Height;

	/*
	  „итаем граф из строки
	  "nodes#(#x 31.5#y 65#label ћјћј#)#(#x 31.5#y 18#label ћџЋј#)#edges#(#source_id 1#target_id 0#label —”Ѕ #)#">
	  из этой строки строитс€ граф "мыла->мама"
	*/
    public void read_graph() 
	{
		String graph = getParameter("graph");
		
		//"$$$" - обозначает конец строки 
		for (int i =0;  i < graph.length()-3; i++)
			if (graph.substring(i, i+3).equals ("$$$"))
			{
				graph = graph.substring(0,i)+"\n"+graph.substring(i+3);
			}
				
					   
		boolean IsZoneNodes  = true;
		boolean InUnit  = false;
		int Curr_x = 0;
		int Curr_y = 0;
		int Curr_source = 0;
		int Curr_target = 0;
		m_Width = 0;
		m_Height = 0;
		panel.Clear();
			
		String  Curr_label = "";
		String  Curr_Morphology = "";
		for (StringTokenizer t = new StringTokenizer(graph, "#") ; t.hasMoreTokens() ; ) 
		{
			String str = t.nextToken();
			
			if (str.charAt(0) == 'x')
			{
				Float f  = new Float(str.substring(1));
				Curr_x = f.intValue();
				if (Curr_x > m_Width){
					 m_Width = Curr_x;
				};
				continue;
			};
			
			if (str.charAt(0) == 'y')
			{
				Float f  = new Float(str.substring(1));
				Curr_y = f.intValue();
				if (Curr_y > m_Height){
					 m_Height = Curr_y;
				};

				continue;
			};
			if (str.length() > 10)
				if (str.substring(0, 10).equals("Morphology"))
				{
				 Curr_Morphology = str.substring(11);
				 continue;
				};
			
			if (str.length() > 5)
				if (str.substring(0, 5).equals("label"))
				{
				 Curr_label = str.substring(6);
				 continue;
				};
			
			if (str.length() > 9) {
				String s = str.substring(10);
				if (str.substring(0, 9).equals("source_id"))
				{
				    
					Float f  = new Float(s);
					Curr_source = f.intValue();
					continue;
				};
				if (str.substring(0, 9).equals("target_id"))
				{
					Float f  = new Float(s);
					Curr_target = f.intValue();
					continue;
				};
				
			}
			if (str.equals("("))
				{
					InUnit = true;
					continue;
				};
			if (str.equals("edges"))
				{
					IsZoneNodes = false;
					continue;
				};
				 
			if (InUnit)	 
 			if (str.equals(")"))
				{
					
					InUnit = false;
					if (IsZoneNodes)
						panel.addNode(Curr_label, Curr_x, Curr_y, Curr_Morphology );
					else
						panel.addEdge(Curr_source, Curr_target, Curr_label);
					
					Curr_Morphology = "";
					continue;
				};
		}
	};
    public void init() 
	{
			setLayout(new BorderLayout());
			panel = new GraphPanel(this);
			add("Center", panel);
			vert_ranger = new Scrollbar(Scrollbar.VERTICAL);
			vert_ranger.addAdjustmentListener(panel);
			add("East", vert_ranger); 
			hor_ranger = new Scrollbar(Scrollbar.HORIZONTAL);
			hor_ranger.addAdjustmentListener(panel);
			add("South", hor_ranger); 

			read_graph();
    }
	public String getAppletInfo()
	{
		return "Sokirko's SemGraph Applet";
	}
    public void start() 
	{
		panel.start();
    }
	
	
}
