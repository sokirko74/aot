#
# canvas.tcl
#
# The description of filename goes HERE.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/canvas_popup_menu.tcl,v $
# $Author: himsolt $
# $Revision: 1.12 $
# $Date: 1999/07/27 18:06:29 $
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
	create_canvas_menu \
	action_insert_bend \
	action_remove_bend
}


proc GT::create_canvas_menu {editor {menu unused} {name unused}} {

    # parameters menu and name are for compatibility with the
    # rest of the menu system; GT::create_canvas_menu can be fed
    # into GT::post_menu with that.

    global GT GT_options GT_selection GT_event
    set graph $GT($editor,graph)
    set canvas $GT($editor,canvas)

    if {[winfo exists $GT($editor,canvas).menu]} {
	destroy $GT($editor,canvas).menu
    }

    set menu [menu $GT($editor,canvas).menu \
		 -tearoff $GT_options(expert) \
		 -title Canvas]
    
    GT::IS::create_menu_entry $editor $menu {
	"^Inspector" "Alt+I"
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"^Undo" undo "Ctrl+Z"
    }

    GT::add_menu_command $editor $menu {
	"Cu^t" cut "Ctrl+X"
    }

    GT::add_menu_command $editor $menu {
	"Co^py" copy "Ctrl+C"
    }

    GT::add_menu_command $editor $menu {
	"^Paste" paste_here "Ctrl+V"
    }

    GT::add_menu_command $editor $menu {
	"Cle^ar" delete "Delete"
    }

#     $menu add separator

#     global GT_modes
#     foreach mode $GT_modes(modes) {
# 	GT::add_menu_command $editor $menu \
# 	    "$GT_modes($mode,name) $mode"
#     }

    set node [GT::find_object $editor node current]
    set edge [GT::find_object $editor edge current]

    if {$node != {}} {
	$menu add separator
	GT::add_menu_command $editor $menu {
	    "^Duplicate" duplicate_nodes
	}
	$menu add separator
	GT::add_menu_command $editor $menu {
	    "Adjust Size To ^Label" adjust_size_to_label ""
	}	
	GT::add_menu_command $editor $menu {
	    "^Clear Label" clear_node_labels "" 6
	}
    }

    if {$edge != {}} {
	$menu add separator
	GT::add_menu_command $editor $menu {
	    "^Split" split_edges
	}
	$menu add separator	
	GT::add_menu_command $editor $menu {
	    "^Clear Label" clear_edge_labels
	}
	GT::add_menu_command $editor $menu {
	    "Clear ^Bends" clear_bends
	}

	set x $GT_event(canvasx)
	set y $GT_event(canvasy)
	set bend [find_bend_near_point $graph $canvas \
		      [$graph get $edge -line] $x $y]

	switch [llength $bend] {
	    1 {
		$menu add separator
		GT::add_menu_command $editor $menu \
		    [list "^Remove Bend" \
			 [list remove_bend $graph $edge \
			      [lindex $bend 0]]]
	    }
	    2 {
		$menu add separator
		GT::add_menu_command $editor $menu \
		    [list "^Insert Bend" \
			 [list insert_bend $graph $edge \
			      [lindex $bend 0] \
			      [$graph translate $canvas -reverse 1 -x $x] \
			      [$graph translate $canvas -reverse 1 -y $y]]]
	    }
	    default {
	    }
	}
    }

    if {($node != {} || $edge != {}) &&
	([llength $GT_selection($editor,selected,node)] == 1 ||
	 [llength $GT_selection($editor,selected,edge)] == 1)
    } {
	$menu add separator
	GT::add_menu_command $editor $menu {
	    "Edit ^Label" edit_label_dialog "Ctrl+L"
	}
    }

    if {$node == {} && $edge == {}} {
	$menu add separator
	GT::add_menu_command $editor $menu {
	    "^Find Graph" find_graph "" 0
	}
    }
}


proc GT::action_insert_bend { editor 
			      {graph {}} {edge {}}
			      {index {}} {x {}} {y {}} } {
    
    if {$graph == {} || $edge == {} || $index == {} || $x == {} || $y == {}} {
	return
    }
 
    set line [$graph get $edge -line]
    set line [concat \
		  [lrange $line 0 [expr $index+1]] \
		  $x $y \
		  [lrange $line [expr $index+2] end]]
    $graph set $edge -line $line
    $graph draw
}


proc GT::action_remove_bend { editor {graph {}} {edge {}} {index {}} } {
    
    if {$graph == {} || $edge == {} || $index == {}} {
	return
    }
 
    set line [$graph get $edge -line]
    set line [concat \
		  [lrange $line 0 [expr $index-1]] \
		  [lrange $line [expr $index+2] end]]
    $graph set $edge -line $line
    $graph draw
}


##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
