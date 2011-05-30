#
# edit_mode.tcl
#
# The description of filename goes HERE.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/modes/edit_mode.tcl,v $
# $Author: himsolt $
# $Revision: 1.11 $
# $Date: 1999/03/05 20:42:22 $
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
	action_edit_mode \
	action_bends_mode \
	ev_move_node_start \
	ev_move_node_motion \
	ev_move_node_end \
	leave_bends_mode \
	leave_edit_mode \
	find_bend_near_point
}


###########################################
#
# Edit mode action
#
###########################################


proc GT::action_edit_mode  { editor  } {
    GT::switch_to_mode $editor edit_mode
}

proc GT::action_bends_mode  { editor  } {
    GT::switch_to_mode $editor bends_mode
}




##########################################
#
# Event handlers for node move operations
#
##########################################


proc GT::ev_move_node_start { graph node } {

    global GT GT_selection GT_options GT_event
    global main

    set canvas GT($main,canvas)
    set editor $main
    set x $GT_event(canvasx)
    set y $GT_event(canvasy)

    set node_x [$graph get $node -x]
    set node_y [$graph get $node -y]

    set GT_event(start_x) $x
    set GT_event(start_y) $y
    set GT_event(last_x)  $GT_event(start_x)
    set GT_event(last_y)  $GT_event(start_y)
	
    set GT_event(move_node_start_object) $node
    set GT_event(did_move_node) 0

    set GT_event(move_node_vars) {
	    start_x
	    start_y
	    last_x
	    last_y
	    move_node_start_object
	    did_move_node
	}


}


proc GT::ev_move_node_motion {args} {
    
    global GT_event GT_selection GT_options

    set x $GT_event(canvasx)
    set y $GT_event(canvasy)
    set canvas $GT_event(W)
    set editor $GT_event(editor)
    set graph $GT_event(graph)

    #
    # Sometimes we get a move event without a preceeding
    # start. In this case, abort.
    #

    if ![info exists GT_event(move_node_start_object)] {
	catch { GT::move_node_start $args dummy_return_value }
	return -code break
    }

    GT::adjust_coordinates_for_grid $editor x y


    set move_x [expr $x - $GT_event(last_x)]
    set move_y [expr $y - $GT_event(last_y)]

    if { $GT_event(did_move_node) == 0 } {

	#
	# This is the first time we move. Mark all items with the
	# tag GT::move_nodes to be faster.
	#

	$canvas dtag GT::move_nodes

	foreach node $GT_selection($editor,selected,node) {
	    $canvas addtag GT::move_nodes withtag $node
	}

	foreach node $GT_selection($editor,selected,node) {
	    foreach edge [$graph edges -node $node] {
		set opposite [$graph nodes -opposite $node $edge]
		if { [lsearch $GT_selection($editor,selected,node) \
			  $opposite] != -1 } {
		    $canvas addtag GT::move_nodes withtag $edge
		}
	    }
	}

# 	$GT_event(graph) mark delete $canvas selected \
# 	    $GT_selection($editor,selected,edge)

    }

    set surrounding_edges \
	[$graph edges -embedding $GT_selection($editor,selected,node)]
    if { [llength $surrounding_edges] <
	 $GT_options(small_selection_treshold) } {
	$graph nodemove \
	    -fast $canvas $GT_selection($editor,selected,node) \
	    $move_x $move_y
    }

    $canvas move GT::move_nodes \
	[$graph translate $canvas -x $move_x] \
	[$graph translate $canvas -y $move_y]
    
    set GT_event(last_x) $x
    set GT_event(last_y) $y
    set GT_event(did_move_node) 1

}



proc GT::ev_move_node_end { {modifier {}} } {

    global GT_event GT_selection GT_options
    set graph $GT_event(graph)
    set canvas $GT_event(W)
    set editor $GT_event(editor)

    set x $GT_event(canvasx)
    set y $GT_event(canvasy)


    if [info exists GT_event(move_node_start_object)] {

	set x_from_start [expr $x - $GT_event(start_x)]
	set y_from_start [expr $y - $GT_event(start_y)]

	if { $GT_event(did_move_node) == 0 } {

	    #
	    # We did not move since mouse down, so IT'S A CLICK !
	    #

	    set node $GT_event(move_node_start_object)
	    if { $modifier == "add" } {
		GT::select $GT_event(editor) toggle $node
	    }

	} else {

	    #
	    # We did move since mouse down, so IT'S A MOVE !
	    #
            set GT_event(did_move_node) 0

	    set surrounding_edges \
		[$graph edges -embedding $GT_selection($editor,selected,node)]
	    if { [llength $surrounding_edges] <
		 $GT_options(small_selection_treshold) } {
	    
		set move_x [expr $x - $GT_event(last_x)]
		set move_y [expr $y - $GT_event(last_y)]

	    } else {
		
		set move_x $x_from_start
		set move_y $y_from_start
		# We need to undo the fake movement
		$canvas move GT::move_nodes \
		    [$graph translate $canvas -x [expr $GT_event(start_x) - $GT_event(last_x)]] \
		    [$graph translate $canvas -y [expr $GT_event(start_y) - $GT_event(last_y)]]
	    }

	    if { $move_x != 0 } {
		set move_x [expr round($move_x)]
	    }
	    if { $move_y != 0 } {
		set move_y [expr round($move_y)]
	    }

	    if { [llength $surrounding_edges] <
		 $GT_options(small_selection_treshold) } {
 		$graph nodemove \
		    -fast $canvas $GT_selection($editor,selected,node) \
 		    $move_x $move_y
	    } else {
		$graph nodemove $GT_selection($editor,selected,node) \
 		    $move_x $move_y
	    }

# 	    $GT_event(graph) mark create $canvas selected \
# 		$GT_selection($editor,selected,edge)

	    foreach i $GT_event(move_node_vars) {
		if [info exists GT_event($i)] {
		    unset GT_event($i)
		}
	    }
	}
    }

}







##########################################
#
# GT::leave_edit_mode
#
##########################################

proc GT::leave_edit_mode { editor mode } {
    return {}
}

proc GT::leave_bends_mode { editor mode } {
    return {}
}



##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
