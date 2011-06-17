#
# generate.tcl
#
# This module implements the menu and actions for generating graphs.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/generate.tcl,v $
# $Author: himsolt $
# $Revision: 1.7 $
# $Date: 1999/03/05 20:42:02 $
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
	create_Generate_menu
}


##########################################
#
# Initialize menu
#
##########################################


proc GT::create_Generate_menu { editor menu } {
    
    global GT_options

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Generate

    GT::create_graphsize_menu $editor $menu \
	"Complete Graph" \
	create_complete_graph

    GT::create_graphsize_menu $editor $menu \
	"Circle Graph" \
	create_circle_graph

    GT_Fibonacci_Tree::create_menu $editor $menu

    GT_Complete_Binary_Tree::create_menu $editor $menu

    $menu add separator

    GT::add_menu_command $editor $menu {
	"Split" split
    }

    GT::add_menu_command $editor $menu {
	"Duplicate" duplicate
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

	set line [$graph get $e -line]
	set x0 [lindex $line 0]
	set y0 [lindex $line 1]
	set x1 [lindex $line [expr [llength $line]-2]]
	set y1 [lindex $line [expr [llength $line]-1]]

	set n [$graph new_node]
	$graph configure $n \
	    -x [expr ($x0+$x1)/2] \
	    -y [expr ($y0+$y1)/2]	  
	lappend new_nodes $n

	set source [$graph get $e -source]
	set target [$graph get $e -target]
	lappend new_edges [$graph copyedge $e $source $n]
	lappend new_edges [$graph copyedge $e $n $target]
	
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
	set splitted_edges [GT::split_edges $editor $edges]
    }

    $graph draw

    GT::select $editor $splitted_edges
}


##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
