#
# canvas.tcl
#
# The description of filename goes HERE.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/editor/canvas.tcl,v $
# $Author: himsolt $
# $Revision: 1.20 $
# $Date: 1999/07/27 18:06:28 $
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
	find_object \
	create_drawing_canvas \
	ev_scroll_drawing_area_left \
    ev_scroll_drawing_area_right \
	ev_scroll_drawing_area_up \
	ev_scroll_drawing_area_down 

}




##########################################
#
# Create the canvas
#
##########################################






proc GT::find_object { editor cmd { tags selected } } {

    global GT

    set tags_of_object [$GT($editor,canvas) gettags $tags]

    if { $tags_of_object != "" } {

	#
	# Graphlet stores the type in the first tag
	#

	set type [lindex $tags_of_object 0]
	set object [lindex $tags_of_object 1]

	if { $object != "" && [regexp $cmd $type] } {
	    return $object
	} else {
	    return {}
	}
    } else {
	return {}
    }
}


proc GT::ev_scroll_drawing_area_left { } {

    global GT_event
    set canvas $GT_event(W)

    $canvas xview scroll -1 page
}


proc GT::ev_scroll_drawing_area_right { } {

    global GT_event
    set canvas $GT_event(W)

    $canvas xview scroll 1 page
}


proc GT::ev_scroll_drawing_area_up { } {

    global GT_event
    set canvas $GT_event(W)

    $canvas yview scroll -1 page
}


proc GT::ev_scroll_drawing_area_down { } {

    global GT_event
    set canvas $GT_event(W)

    $canvas yview scroll 1 page
}



##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
