#
# edit.tcl
#
# This module implements the "Edit" menu
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/edit.tcl,v $
# $Author: himsolt $
# $Revision: 1.16 $
# $Date: 1999/03/05 20:41:59 $
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
	create_Edit_menu \
	action_edit_undo \
	action_edit_cut \
	action_edit_copy \
	action_edit_paste \
	action_edit_paste_here \
	action_edit_delete \
	action_node_style \
	action_edge_style \
	action_label_style \
	action_style
}


proc GT::create_Edit_menu { editor menu } {

    global GT
    global GT_options

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Edit

    GT::add_menu_command $editor $menu {
	"^Undo" undo "Ctrl+Z"
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"Cu^t" cut "Ctrl+X" 2
    }

    GT::add_menu_command $editor $menu {
	"^Copy" copy "Ctrl+C" 0 
    }

    if $GT_options(paste_at_cursor_position) {
	GT::add_menu_command $editor $menu {
	    "^Paste" paste_here "Ctrl+V" 0 
	}
    } else {
	GT::add_menu_command $editor $menu {
	    "^Paste" paste "Ctrl+V" 0 
	}
    }

    $menu add separator    

    GT::add_menu_command $editor $menu {
  	"Cle^ar" delete "Delete" 0 
    }

#     $menu add separator

#     GT::add_menu_command $editor $menu {
# 	"Select All" edit_select_all "Ctrl+slash"
#     }

#     GT::add_menu_command $editor $menu {
# 	"Deselect All" edit_deselect_all "Ctrl+backslash"
#     }

#     GT::add_menu_command $editor $menu {
# 	"Select All ^Nodes" edit_select_all_nodes "Ctrl+A"
#     }

#     GT::add_menu_command $editor $menu {
# 	"Select All ^Edges" edit_select_all_edges "Ctrl+E"
#     }

#     GT::add_submenu $editor $menu { "Select ^More ..." Select }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"^Find & Replace ..." search_and_replace_dialog "Ctrl+F" 0
    }

    GT::add_menu_command $editor $menu {
	"Find Ne^xt" search_next "Ctrl+G" {}
    }

    GT::add_menu_command $editor $menu {
	"Replace & Find Next" replace_next "Ctrl+H" {}
    }

    $menu add separator
 
    GT::add_menu_command $editor $menu {
	"Edit ^Label ..." edit_label_dialog "Ctrl+L" 4
    }
 

#     $menu add separator

#     GT::add_menu_command $editor $menu {
# 	"Node Style ..." node_style "" 0 disabled
#     }

#     GT::add_menu_command $editor $menu {
# 	"Edge Style ..." edge_style "" 0 disabled
#     }

#     GT::add_menu_command $editor $menu {
# 	"Label Style ..." label_style "" 0  disabled
#     }

#     GT::add_menu_command $editor $menu {
# 	"Style ..." style "" 0 disabled
#     }

}


###########################################
#
# Actions
#
###########################################


proc GT::action_edit_undo  { editor  } {
    
    global GT
    set graph $GT($editor,graph)

    variable undo
    if {[info exists undo($editor,stack)] &&
	$undo($editor,stack) != {} &&
	[lindex [lindex $undo($editor,stack) end] 0] != "frame"
    } {
	GT::undo $editor undo
    } else {
	GT::message $editor \
	    "No undo information available." \
	    error
    }
    
    $graph draw
}


proc GT::action_edit_cut  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::undo $editor push [concat \
			       $GT_selection($editor,selected,node) \
			       $GT_selection($editor,selected,edge)]

    GT::cutpaste $editor cut clipboard
    $graph draw

    return
}


proc GT::action_edit_copy  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::cutpaste $editor copy clipboard
    $graph draw

    return
}


proc GT::action_edit_paste  { editor {x {}} {y {}} } {

    global GT GT_selection
    set graph $GT($editor,graph)
    set canvas $GT($editor,canvas)

    set pasted [GT::cutpaste $editor paste $graph $x $y]
    $graph draw

    GT::undo $editor delete [join $pasted]

    if { $pasted != {} } {
	eval GT::select $editor select [lindex $pasted 0]
    }

    return
}


proc GT::action_edit_paste_here  { editor } {

    global GT GT_options GT_event
    set graph $GT($editor,graph)
    set canvas $GT($editor,canvas)

    if $GT_options(paste_at_cursor_position) {
	set x [$graph translate $canvas -reverse 1 -x $GT_event(canvasx)]
	set y [$graph translate $canvas -reverse 1 -y $GT_event(canvasy)]
	return [GT::action_edit_paste $editor $x $y]
    } else {
	set visible [GT::visible_drawing_area $editor]
	set x [expr ([lindex $visible 0] + [lindex $visible 2]) / 2.0]
	set y [expr ([lindex $visible 1] + [lindex $visible 3]) / 2.0]
	set x [$graph translate $canvas -reverse 1 -x $x]
	set y [$graph translate $canvas -reverse 1 -y $y]
	return [GT::action_edit_paste $editor $x $y]
    }
}


proc GT::action_edit_delete  { editor  } {

    global GT GT_selection

    set canvas $GT($editor,canvas)
    set graph $GT($editor,graph)

    if {[focus] == $canvas} {
	GT::undo $editor push [concat \
				   $GT_selection($editor,selected,node) \
				   $GT_selection($editor,selected,edge)]
	
	$graph delete $GT_selection($editor,selected,node)
	$graph delete $GT_selection($editor,selected,edge)
	GT::select $editor {}
    }

    $graph draw
}


proc GT::action_node_style  { editor  } {
}


proc GT::action_edge_style   { editor  } {
}


proc GT::action_label_style   { editor  } {
}


proc GT::action_style  { editor  } {
}



##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
