#
# frame.tcl
#
# The description of filename goes HERE.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/editor/frame.tcl,v $
# $Author: himsolt $
# $Revision: 1.11 $
# $Date: 1999/03/05 20:41:30 $
# $Locker:  $
# $State: Exp $
#
#------------------------------------------
#
# (C) University of Passau 1995-1999, graphlet Project
#

package require Graphlet
package provide Graphscript [gt_version]

namespace eval GT {
    namespace export \
	create_frame \
	change_title \
	close_frame \
	new_frame_name
}


proc GT::create_frame { editor } {

    global GT GT_options

    #
    # Insert into the list of toplevel windows. We are only
    # allowed to quit if this list is empty.
    #

    lappend GT(toplevel_windows) $editor

    #
    # Build the program.
    #

    GT::create_menubar $editor [concat $GT(menubar) Help]
    pack [GT::create_message_label $editor] \
	-side bottom \
	-fill x
    pack [GT::create_toolbar $editor] \
	-side top \
	-fill x
    pack [GT::create_modebar $editor] \
	-pady 3 \
	-side left \
	-fill y
#     pack [GT::create_graphicbar $editor] \
# 	-pady 3 \
# 	-side right \
# 	-fill y
    pack [GT::create_drawing_canvas $editor] \
	-padx 4 \
	-pady 3 \
	-fill both \
	-expand true

    #
    # Check if we have an explicit size request
    #

    set explicit_geometry 0

    switch -regexp -- $GT_options(custom_graphlet_window) {
	explicit {
	    if { $GT_options(graphlet_window_width) != {} } {
		set width $GT_options(graphlet_window_width)
		set explicit_geometry 1
	    } else {
		set width [winfo reqwidth $editor]
	    }
	    if { $GT_options(graphlet_window_height) != {} } {
		set height $GT_options(graphlet_window_height)
		set explicit_geometry 1
	    } else {
		set height [winfo reqheight $editor]
	    }
	    if { $GT_options(graphlet_window_x) != {} } {
		set x $GT_options(graphlet_window_x)
	    } else {
		set x {}
	    }
	    if { $GT_options(graphlet_window_y) != {} } {
		set y $GT_options(graphlet_window_y)
	    } else {
		set y {}
	    }	    
	}
	fullscreen {
	    set width [winfo screenwidth $editor]
	    set height [winfo screenheight $editor]
	    set x 0
	    set y 0
	    set explicit_geometry 1
	}
	none|default {
	    set explicit_geometry 0
	}
    }
    if $explicit_geometry {
	if { $x != {} && $y != {} } {
	    set spec "${width}x${height}+$x+$y"
	} else {
	    set spec "${width}x${height}"
	}
	wm geometry $editor $spec
    }

    trace variable GT($editor,filename) w GT::change_frame_title

    proc change_frame_title { varname index op } {
	upvar $varname var
	set editor [lindex [split $index ","] 0]
	switch $op {
	    w {
		set filename $var($index)
		if { $filename != "" } {
		    wm title $editor "Graphlet: [file tail $filename]"
		    wm iconname $editor "[file tail $filename]"
		} else {
		    wm title $editor "Graphlet: Untitled"
		    wm iconname $editor "Untitled"
		}
	    }
	}
    }

    if {$editor == {}} {
	wm protocol . WM_DELETE_WINDOW "GT::close_frame $editor"
    } else {
	wm protocol $editor WM_DELETE_WINDOW "GT::close_frame $editor"
    }

    return $editor
}


proc GT::close_frame {editor {mode close}} {
    
    global GT GT_status GT_options
    set graph $GT($editor,graph)

    if {[llength $GT(toplevel_windows)] == 1} {

	if {$GT_status($graph,dirty) == 1 &&
	    [GT::show_exitdialog $editor] == "cancel"} {
	    return
	}

    } else {

	if {$GT_status($graph,dirty) == 1 &&
	    [GT::show_closedialog $editor] == "cancel"} {
	    return
	}

    }
    
    # Remove this editor's canvas from the list of
    # canvases associated with $graph

    set canvases [$graph canvases]
    foreach c $canvases {
	if { [lindex $c 0] != $GT($editor,canvas) } {
	    lappend new_canvases $c
	}
    }
    if [info exists new_canvases] {
	$graph canvases $new_canvases
    } else {
	$graph canvases {}
    }

    # Remove this editor from the list of editors
    # associated with $graph

    set editors [$graph editors]
    ldelete editors $editor
    $graph editors $editors

    # Remove this editor from Graphlet's list of toplevels.

    set index [lsearch $GT(toplevel_windows) $editor]
    set GT(toplevel_windows) \
	[lreplace $GT(toplevel_windows) $index $index]

    # If this was the last window, take the whole thing down

    if {$mode == "exit" || [llength $GT(toplevel_windows)] == 0} {
	# Remember frame geometry
	set GT_options(graphlet_window_x) [winfo x $editor]
	set GT_options(graphlet_window_y) [winfo y $editor]
	set GT_options(graphlet_window_height) [winfo height $editor]
	set GT_options(graphlet_window_width) [winfo width $editor]
	GT::action $editor options_save
	destroy $editor
	exit
    } else {	
	destroy $editor
    }
}


##########################################
#
# GT::new_frame_name basename
#
##########################################


proc GT::new_frame_name { {basename ".graphlet"} } {

    global GT GT_options

    set nr [incr GT(frame_nr)]
    return "${basename}_$nr"
}



##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
