//******************************************************************************
// Lake.java:	Applet
//
// (c) David Griffiths, 1997
// This source code may not be reproduced without the express permission of the 
// author.
//******************************************************************************
import java.applet.*;
import java.net.URL;
import java.net.MalformedURLException;
import java.awt.*;

//==============================================================================
// Main Class for applet Lake
//
//==============================================================================
public class Lake extends Applet implements Runnable
{
	// THREAD SUPPORT:
	//		m_Lake	is the Thread object for the applet
	//--------------------------------------------------------------------------
	Thread	 m_Lake = null;

	// ANIMATION SUPPORT:
	//		m_Graphics		used for storing the applet's Graphics context
	//		m_WaveGraphics	used for storing the animation's Graphics context
	//		m_Image   		the original image
	//		m_WaveImage   	the image containing the wave animations
	//		m_nCurrImage	the index of the next image to be displayed
	//		m_ImgWidth		width of each image
	//		m_ImgHeight		height of each image
	//		m_OvlWidth		width of each overlay
	//		m_OvlHeight		height of each overlay
	//		m_fAllLoaded	indicates whether all images have been loaded
	//		m_tAnimate		indicates that OK to do animation (changed by mouse 
	//						click)
	//		NUM_FRAMES      number of NUM_FRAMES used in the animation
	//--------------------------------------------------------------------------
	private Graphics m_Graphics, m_WaveGraphics;
	private Image	 m_Image, m_Overlay, m_WaveImage;
	private int 	 m_nCurrImage;
	private int 	 m_nImgWidth  = 0;
	private int 	 m_nImgHeight = 0;
	private int 	 m_nOvlWidth  = 0;
	private int 	 m_nOvlHeight = 0;
	private boolean  m_fAllLoaded = false, m_tAnimate = true;
	private final int NUM_FRAMES = 12;

	// PARAMETER SUPPORT:
	//		Parameters allow an HTML author to pass information to the applet;
	// the HTML author specifies them using the <PARAM> tag within the <APPLET>
	// tag.  The following variables are used to store the values of the
	// parameters.
    //--------------------------------------------------------------------------

    // Members for applet parameters
    // <type>       <MemberVar>    = <Default Value>
    //--------------------------------------------------------------------------
	private String m_ImageName = "";
	private String m_OverlayName = "";
	private URL m_HRef;
	private String m_Frame = "_self";

    // Parameter names.  To change a name of a parameter, you need only make
	// a single change.  Simply modify the value of the parameter string below.
    //--------------------------------------------------------------------------
	private final String PARAM_image = "image";
	private final String PARAM_overlay = "overlay";
	private final String PARAM_href = "href";
	private final String PARAM_target = "target";

	// Lake Class Constructor
	//--------------------------------------------------------------------------
	public Lake()
	{
		// TODO: Add constructor code here
	}

	// APPLET INFO SUPPORT:
	//		The getAppletInfo() method returns a string describing the applet's
	// author, copyright date, or miscellaneous information.
    //--------------------------------------------------------------------------
	public String getAppletInfo()
	{
		return "Name: Lake v3.0\r\n" +
		       "Author: David Griffiths\r\n" +
		       "Created with Microsoft Visual J++ Version 1.0";
	}

	// PARAMETER SUPPORT
	//		The getParameterInfo() method returns an array of strings describing
	// the parameters understood by this applet.
	//
    // Lake Parameter Information:
    //  { "Name", "Type", "Description" },
    //--------------------------------------------------------------------------
	public String[][] getParameterInfo()
	{
		String[][] info =
		{
			{ PARAM_image, "String", "JPG or GIF file to reflect" },
			{ PARAM_overlay, "String", "JPG or GIF file to use as overlay" },
			{ PARAM_href, "URL", "URL to link to" },
			{ PARAM_target, "String", "Target frame" },
		};
		return info;		
	}

	// The init() method is called by the AWT when an applet is first loaded or
	// reloaded.  Override this method to perform whatever initialization your
	// applet needs, such as initializing data structures, loading images or
	// fonts, creating frame windows, setting the layout manager, or adding UI
	// components.
    //--------------------------------------------------------------------------
	public void init()
	{
		// PARAMETER SUPPORT
		//		The following code retrieves the value of each parameter
		// specified with the <PARAM> tag and stores it in a member
		// variable.
		//----------------------------------------------------------------------
		String param;

		// image: JPG of GIF file to reflect
		//----------------------------------------------------------------------
		param = getParameter(PARAM_image);
		if (param != null)
			m_ImageName = param;

		// overlay: JPG of GIF file to use as overlay
		//----------------------------------------------------------------------
		param = getParameter(PARAM_overlay);
		if (param != null) 
			m_OverlayName = param;

		// href: URL to link to
		//----------------------------------------------------------------------
		param = getParameter(PARAM_href);
		if (param != null) 
                try
                {
                    m_HRef = new URL(getDocumentBase(), param);
                }
                catch (MalformedURLException e)
                {
                    getAppletContext().showStatus("Bad URL: " + param);
                    return;
                }

		// target: Target frame
		//----------------------------------------------------------------------
		param = getParameter(PARAM_target);
		if (param != null) 
			m_Frame = param;
	}

	// Place additional applet clean up code here.  destroy() is called when
	// when you applet is terminating and being unloaded.
	//-------------------------------------------------------------------------
	public void destroy()
	{
		// TODO: Place applet cleanup code here
	}

    // ANIMATION SUPPORT:
    //		Draws the next image, if all images are currently loaded
    //--------------------------------------------------------------------------
	private void displayImage(Graphics g)
	{
		if (!m_fAllLoaded)
			return;

		// Draw frame of rippled lower half
		//----------------------------------------------------------------------
		if (m_WaveImage != null) {
			g.drawImage (m_WaveImage, (-m_nCurrImage * m_nImgWidth), m_nImgHeight, this);
			g.drawImage (m_WaveImage, ((NUM_FRAMES-m_nCurrImage) * m_nImgWidth), 
				m_nImgHeight, this);
		}
		// Draw the original in the tophalf.
		//----------------------------------------------------------------------
		g.drawImage (m_Image, 0, -1, this);
	}

	// Lake Paint Handler
	//--------------------------------------------------------------------------
	public void paint(Graphics g)
	{
		// ANIMATION SUPPORT:
		//		The following code displays a status message until all the
		// images are loaded. Then it calls displayImage to display the current
		// image.
		//----------------------------------------------------------------------
		if (m_fAllLoaded)
			displayImage(g);
		else
			g.drawString("Loading images...", 10, 20);

		// TODO: Place additional applet Paint code here
	}

	//		The start() method is called when the page containing the applet
	// first appears on the screen. The AppletWizard's initial implementation
	// of this method starts execution of the applet's thread.
	//--------------------------------------------------------------------------
	public void start()
	{
		if (m_Lake == null)
		{
			m_Lake = new Thread(this);
			m_Lake.start();
		}
	}
	
	// The stop() method is called when the page containing the applet is
	// no longer on the screen. The AppletWizard's initial implementation of
	// this method stops execution of the applet's thread.
	//--------------------------------------------------------------------------
	public void stop()
	{
		if (m_Lake != null)
		{
			m_Lake.stop();
			m_Lake = null;
		}

	}

	// THREAD SUPPORT
	//		The run() method is called when the applet's thread is started. If
	// your applet performs any ongoing activities without waiting for user
	// input, the code for implementing that behavior typically goes here. For
	// example, for an applet that performs animation, the run() method controls
	// the display of images.
	//--------------------------------------------------------------------------
	public void run()
	{
		m_nCurrImage = 0;
		
		// If re-entering the page, then the images have already been loaded.
		// m_fAllLoaded == TRUE.
		//----------------------------------------------------------------------
        if (!m_fAllLoaded)
		{
    		repaint();
    		m_Graphics = getGraphics();

    		// Now load up the image to be used in the animation. Rather than do
			// this asynchronously with imageUpdate (which is a pain in the bum to
			// use) we'll do it synchronously with a MediaTracker. This hangs
			// around until the image is loaded. Using the waitForAll method, just
			// in case we ever add other images to the applet.
    		//------------------------------------------------------------------
    		MediaTracker tracker = new MediaTracker(this);
    		String strImage;

		    m_Image = getImage(getDocumentBase(), m_ImageName);
            if (!"".equals(m_OverlayName))
			    m_Overlay = getImage(getDocumentBase(), m_OverlayName);

            tracker.addImage(m_Image, 0);
            if (!"".equals(m_OverlayName))
				tracker.addImage(m_Overlay, 1);

    		// Wait until all images are fully loaded
    		//------------------------------------------------------------------
			try
			{
				tracker.waitForAll();
				m_fAllLoaded = !tracker.isErrorAny();
			}
			catch (InterruptedException e) {}
			
			if (!m_fAllLoaded)
			{
			    stop();
			    m_Graphics.drawString("Error loading images!", 10, 40);
			    return;
			}
			

			// Can now set width and height straight away because the 
			// MediaTracker object has ensured this information is now available.
			//--------------------------------------------------------------
		    m_nImgWidth  = m_Image.getWidth(this);
		    m_nImgHeight = m_Image.getHeight(this);
            if (!"".equals(m_OverlayName)) {
			    m_nOvlWidth  = m_Overlay.getWidth(this);
			    m_nOvlHeight = m_Overlay.getHeight(this);
			}

			// Now create the animation of the rippling waves.
			//--------------------------------------------------------------
			createAnimation();
        }		
		repaint();

		while (true)
		{
			try {
				// Draw next image in animation
				//--------------------------------------------------------------
				if (m_tAnimate) 
				{
					displayImage(m_Graphics);
					if (++m_nCurrImage == NUM_FRAMES)
						m_nCurrImage = 0;

					Thread.sleep(50);
				}
				else
					Thread.sleep(500);
			}
			catch (InterruptedException e)
			{
				stop();
			}
		}
	}

	// MOUSE SUPPORT:
	// Clicking on the applet starts/stops it.
	// Doesn't call 'stop()' directly because otherwise an InterruptedException 
	// is thrown. ..
	//--------------------------------------------------------------------------
    public boolean mouseUp(Event event, int i, int j)
    {
        boolean flag = super.mouseUp(event, i, j);
		if (m_HRef == null)
			m_tAnimate = !m_tAnimate; // Toggle m_tAnimate to start/stop animation.
		else
		{
			showStatus("" + m_HRef);
			getAppletContext().showDocument(m_HRef, m_Frame);
		}
        return true;
    }

	// ANIMATION
	// Create the animation within a single background image. We use a single
	// image rather than the default multiple images because it's quicker.
	//---------------------------------------------------------------------------
    public void createAnimation () 
	{
		// Create inverted image of original loaded image.
		// We create a background image (backImg) 1 pixel higher
		// than the original because we'll need an extra line of
		// pixels to play with when we flip the image upside down.
		//--------------------------------------------------------        
		Image backImg = createImage (m_nImgWidth, m_nImgHeight + 1);
        Graphics backG = backImg.getGraphics();
		// Copy the original image (m_Image) onto the background
		// version.
		//--------------------------------------------------------
        backG.drawImage (m_Image, 0, 1, this);
		// Now flip the image upside down.
		//--------------------------------------------------------
        for (int i = 0; i < (m_nImgHeight >> 1); i++) 
		{
            backG.copyArea (0, i, m_nImgWidth, 1, 0, m_nImgHeight - i);
            backG.copyArea (0, m_nImgHeight - 1 - i, m_nImgWidth, 1,
		        0, -m_nImgHeight + 1 + (i << 1));
            backG.copyArea (0, m_nImgHeight, m_nImgWidth, 1, 0, -1 - i);
        }
		// Now create the large (NUM_FRAMES + 1 times the width) image
		// that will store dithered copies of the inverted original.
		//--------------------------------------------------------
        m_WaveImage = createImage ((NUM_FRAMES + 1) * m_nImgWidth, m_nImgHeight);
        m_WaveGraphics = m_WaveImage.getGraphics();
        m_WaveGraphics.drawImage (backImg, NUM_FRAMES * m_nImgWidth, 0, this);
		// Now create dithered copies (sine displacement up or down) of the
		// inverted original.
		//--------------------------------------------------------
        for (int phase = 0; phase < NUM_FRAMES; phase++) 
            makeWaves (m_WaveGraphics, phase);
		// Now, if there is an overlay image, draw the top half of it
		// over the frame. (The bottom half of the overlay will be drawn over
		// the rippled image)
		//------------------------------------------------------------
		backG.drawImage (m_Image, 0, 1, this);
		if (!"".equals(m_OverlayName)) 
			backG.drawImage (m_Overlay, 
				(m_nImgWidth - m_nOvlWidth) >> 1,
				m_nImgHeight - (m_nOvlHeight >> 1), this);
		m_Image = backImg;
	}

	// ANIMATION
	// Take the initial (unwaved) image from the left-hand-side of the graphics
	// and make NUM_FRAMES copies of it - the pixels rows of each one dithered
	// up or down depending upon the dispy sine function.
	//---------------------------------------------------------------------------
	public void makeWaves (Graphics g, int phase) 
	{
		double p1;
		int     dispx, dispy;
		// Convert the phase into radians (by splitting 2*PI into 
		// NUM_FRAMES segments).
		//--------------------------------------------------------
		p1 = 2 * Math.PI * (double)phase / (double)NUM_FRAMES;
		// dispx defines how far across the image has to be copied 
		// from the original LHS frame.
		//--------------------------------------------------------
		dispx = (NUM_FRAMES - phase) * m_nImgWidth;
		// Now process each horizontal line of pixels. Copy across
		// from original frame on the left-had-side and displacing 
		// up or down WRT the dispy sine function.
		//--------------------------------------------------------
		for (int i = 0; i < m_nImgHeight; i++) 
		{
			// dispy defines the vertical sine displacement. It 
			// attenuates higher up the image, for perspective.
			//--------------------------------------------------------
			dispy = (int)((m_nImgHeight/14) * ((double) i + 28.0)
				* Math.sin ((double)((m_nImgHeight/14)*(m_nImgHeight - i))
				/(double)(i + 1)
				+ p1)
				/ (double) m_nImgHeight);
			// If no line dithers here then copy original.
			//--------------------------------------------------------
			if (i < -dispy)
				g.copyArea (NUM_FRAMES * m_nImgWidth, i, m_nImgWidth, 1,
					-dispx, 0);
			else
			// Else copy dithered line.
			//--------------------------------------------------------
				g.copyArea (NUM_FRAMES * m_nImgWidth, i + dispy,
					m_nImgWidth, 1, -dispx, -dispy);
		}

		// Now, if there is an overlay image, draw the bottom half of it
		// over the frame. (The top half of the overlay will be drawn over
		// the original image)
		//------------------------------------------------------------
		if (!"".equals(m_OverlayName))
			g.drawImage (m_Overlay, 
				(phase * m_nImgWidth) + ((m_nImgWidth - m_nOvlWidth) >> 1), 
				-m_nOvlHeight >> 1, this);
	}
}
