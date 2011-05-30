#
# menu/edge.tcl
#
# This module implements the "Edge" menu.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/edge.tcl,v $
# $Author: himsolt $
# $Revision: 1.12 $
# $Date: 1999/03/05 20:41:57 $
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
	create_Edge_menu \
	split_edges \
	action_split_edges \
	action_clear_edge_labels \
	remove_multiple_edges \
	action_remove_multiple_edges \
	remove_self_loops \
	action_remove_self_loops \
	remove_bends \
	action_remove_bends \
	clear_bends \
	action_clear_bends
}


##########################################
#
# Initialize menu
#
##########################################


proc GT::create_Edge_menu { editor menu } {
    
    global GT GT_options GT_menu

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Edge

    GT::add_menu_command $editor $menu {
	"^Split" split_edges
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"^Clear Edge Labels" clear_edge_labels "" 6
    }

    GT::add_menu_command $editor $menu {
	"^Clear Bends" clear_bends "" 6
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"Remove all ^Bends" remove_bends "" 7
    }
    GT::add_menu_command $editor $menu {
	"Remove all ^Self Loops" remove_self_loops "" 7
    }
    GT::add_menu_command $editor $menu {
	"Remove all ^Multiple Edges" remove_multiple_edges "" 7
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"Number Edges" number_edges
    }

    GT::add_menu_command $editor $menu {
	"Set Edge Labels to Endnode Labels" set_edge_labels_to_endnode_labels
    }

    if {$GT_options(developer) == 1} {
	GT::add_menu_command $editor $menu {
	    "Set Edge Labels to Internal Id" set_edge_labels_to_internal_id
	}
    }    

    $menu add separator

    GT::add_submenu $editor $menu {
	"^Options" Edge_options
    }
}


##########################################
#
# Actions
#
# GT::split_nodes
# GT::split_edges
#
# GT::action_split_nodes
# GT::action_split_edges
#
##########################################


proc GT::split_nodes { editor nodes } {

    global GT
    set graph $GT($editor,graph)

    foreach n $nodes {
	set in($n) [$graph edges -in $n]
	set out($n) [$graph edges -out $n]
    }

    foreach n $nodes {
	    
	set x [$graph get $n -x]
	set w [$graph get $n -w]

	set new_node [lindex [$graph copynode $n] 0]
	lappend new_nodes $new_node
	$graph configure $n -x [expr $x - $w/2]
	$graph configure $new_node -x [expr $x + $w/2]

	foreach e $in($n) {
	    set source [$graph get $e -source]
	    set target [$graph get $e -target]
	    lappend new_edges [$graph copyedge $e $source $new_node]
	}
	foreach e $out($n) {
	    set source [$graph get $e -source]
	    set target [$graph get $e -target]
	    lappend new_edges [$graph copyedge $e $new_node $target]
	}
    }

    if { [info exists new_nodes] && [info exists new_edges] } {
	return [list $new_nodes $new_edges]
    } elseif [info exists new_nodes] {
	return [list $new_nodes {}]
    } elseif [info exists new_edges] {
	return [list $new_edges {}]
    } else {
	return { {} {} }
    }
}

proc GT::split_edges { editor edges } {

    global GT
    set graph $GT($editor,graph)

    foreach e $edges {

	# Create new node
	set n [$graph new_node]
	lappend new_nodes $n

	# Compute coordinates for the new node
	set line [$graph get $e -line]
	set line_length [llength $line]
	if {$line_length % 4 == 2} {
	    # Uneven number of bends. Coordinates for new node
	    # are the central bend.
	    set x [lindex $line [expr $line_length/2-1]]
	    set y [lindex $line [expr $line_length/2]  ]
	} else {
	    # Uneven number of bends. Coordinates for new node
	    # are middle of the central line.
	    set x0 [lindex $line [expr $line_length/2-2]]
	    set y0 [lindex $line [expr $line_length/2-1]]
	    set x1 [lindex $line [expr $line_length/2  ]]
	    set y1 [lindex $line [expr $line_length/2+1]]
	    set x [expr ($x0+$x1)/2]
	    set y [expr ($y0+$y1)/2]
	}

	$graph configure $n \
	    -x $x \
	    -y $y	  

	# Create new edges
	set source [$graph get $e -source]
	set target [$graph get $e -target]
	set new_edge1 [$graph copyedge $e $source $n]
	set new_edge2 [$graph copyedge $e $n $target]
	lappend new_edges $new_edge1 $new_edge2
	
	# Assign lines for the new edges
	if {$line_length % 4 == 2} {
	    $graph set $new_edge1 \
		-line [lrange $line 0 [expr $line_length/2]]
	    $graph set $new_edge2 \
		-line [lrange $line [expr $line_length/2-1] end]
	} else {
	    $graph set $new_edge1 \
		-line [concat [lrange $line 0 [expr $line_length/2-1]] $x $y]
	    $graph set $new_edge2 \
		-line [concat $x $y [lrange $line [expr $line_length/2] end]]
	}

	# Delete old edge
	$graph delete $e
    }

    if { [info exists new_nodes] && [info exists new_edges] } {
	return [list $new_nodes $new_edges]
    } elseif [info exists new_nodes] {
	return [list $new_nodes {}]
    } elseif [info exists new_edges] {
	return [list $new_edges {}]
    } else {
	return { {} {} }
    }
}


proc GT::action_split_edges editor {

    global GT GT_selection
    set graph $GT($editor,graph)

    if { $GT_selection($editor,selected,edge) == {}} {
	return
    }

    GT::excursion $editor {
	set edges $GT_selection($editor,selected,edge)
	GT::undo $editor push $edges
	set splitted [GT::split_edges $editor $edges]
	GT::undo $editor delete [join $splitted]
    }

    $graph draw

    GT::select $editor $splitted
}



proc GT::action_clear_edge_labels editor {

    global GT GT_selection
    set graph $GT($editor,graph)

    if { $GT_selection($editor,selected,edge) == {} } {
	set edges [$graph edges]
    } else {
	set edges $GT_selection($editor,selected,edge)
    }

    GT::undo $editor attributes $edges -label

    $graph configure $edges -label {}
    $graph draw
}


proc GT::remove_multiple_edges { editor } {

    global GT
    set graph $GT($editor,graph)

    set edges_deleted {}

    foreach edge [$graph edges] {

	set source [$graph get $edge -source]
	set target [$graph get $edge -target]
	set edges [$graph edges -between $source $target]

	if { [llength $edges] > 1 } {
	    set to_delete [lrange $edges 1 end]
	    foreach e $to_delete {
		if { [lsearch $edges_deleted $e] == -1 } {
		    lappend edges_deleted $e
		}
	    }
	}
    }
    
    $graph delete $edges_deleted

    return $edges_deleted
}


proc GT::action_remove_multiple_edges { editor  } {

    global GT
    set graph $GT($editor,graph)

    GT::excursion $editor {
	set edges_deleted [GT::remove_multiple_edges $editor]
    }

    if { [llength $edges_deleted] == 0 } {
	GT::message $editor "No multiple edges found." message
    } elseif { [llength $edges_deleted] == 1 } {
	GT::message $editor "One multiple edge deleted" message
    } else {
	GT::message $editor \
	    "[llength $edges_deleted] multiple edges deleted." \
	    message
    }

    $graph draw
}


proc GT::remove_self_loops { editor } {

    global GT
    set graph $GT($editor,graph)

    set edges_deleted {}

    foreach edge [$graph edges] {
	set source [$graph get $edge -source]
	set target [$graph get $edge -target]
	if { $source == $target } {
	    lappend edges_deleted $edge
	}
    }
    
    $graph delete $edges_deleted

    return $edges_deleted
}


proc GT::action_remove_self_loops { editor  } {

    global GT
    set graph $GT($editor,graph)

    GT::excursion $editor {
	set edges_deleted [GT::remove_self_loops $editor]
    }

    if { [llength $edges_deleted] == 0 } {
	GT::message $editor "No self loops found." message
    } elseif { [llength $edges_deleted] == 1 } {
	GT::message $editor "One self loop deleted" message
    } else {
	GT::message $editor \
	    "[llength $edges_deleted] self loops deleted." \
	    message
    }

    $graph draw
}




##########################################
#
# GT::remove_bends graph
# GT::action_remove_bends
#
##########################################

proc GT::remove_bends { editor } {

    global GT
    set graph $GT($editor,graph)

    set count 0
    foreach edge [$graph edges] {

	set line [$graph get $edge -line]
	if { [llength $line] > 4 } {

	    set source [$graph get $edge -source]
	    set x0 [$graph get $source -x]
	    set y0 [$graph get $source -y]

	    set target [$graph get $edge -target]
	    set x1 [$graph get $target -x]
	    set y1 [$graph get $target -y]

	    $graph configure $edge graphics \
		-line [list $x0 $y0 $x1 $y1]

	    incr count [expr [llength $line]/2 -2]
	}
    }

    return $count
}


proc GT::action_remove_bends { editor } {

    global GT
    set graph $GT($editor,graph)

    GT::excursion $editor {
	set count [GT::remove_bends $editor]
    }

    if {$count == 0} {
	GT::message $editor "No bends found."
    } else {
	GT::message $editor "$count bends removed."
    }

    $graph draw
}



##########################################
#
# GT::clear_bends graph
# GT::action_clear_bends
#
##########################################


proc GT::clear_bends {editor edges} {

    global GT
    set graph $GT($editor,graph)

    set count 0
    foreach edge $edges {

	set line [$graph get $edge -line]
	if { [llength $line] > 4 } {

	    set source [$graph get $edge -source]
	    set x0 [$graph get $source -x]
	    set y0 [$graph get $source -y]

	    set target [$graph get $edge -target]
	    set x1 [$graph get $target -x]
	    set y1 [$graph get $target -y]

	    $graph configure $edge graphics \
		-line [list $x0 $y0 $x1 $y1]

	    incr count [expr [llength $line]/2 -2]
	}
    }

    return $count
}


proc GT::action_clear_bends { editor } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::excursion $editor {
	set count \
	    [GT::clear_bends $editor $GT_selection($editor,selected,edge)]
    }

    if {$count == 0} {
	GT::message $editor "No bends found."
    } else {
	GT::message $editor "$count bends removed."
    }

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
