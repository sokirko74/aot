#
# menu/graph.tcl
#
# This module implements several tools for working with graphs.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/graph.tcl,v $
# $Author: himsolt $
# $Revision: 1.8 $
# $Date: 1999/03/05 20:42:04 $
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
	create_Graph_menu \
	create_complete_graph \
	action_create_complete_graph \
	create_circle_graph \
	action_create_circle_graph \
	create_graphsize_menu \
	action_select_graphsize \
	select_graphsize_ok_button \
	action_switch_directed
}


##########################################
#
# Initialize menu
#
##########################################


proc GT::create_Graph_menu { editor menu } {
    
    global GT GT_options GT_menu

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Graph

    $menu add checkbutton \
	-label "Directed" \
	-variable GT_options(directed) \
	-command "GT::menu_action $editor switch_directed" \
	-underline 0

    $menu add separator

    GT::create_graphsize_menu $editor $menu \
	"Generate ^Complete Graph" \
	create_complete_graph
    GT::create_graphsize_menu $editor $menu \
	"Generate C^ircle Graph" \
	create_circle_graph

    GT_Fibonacci_Tree::create_menu $editor $menu

    GT_Complete_Binary_Tree::create_menu $editor $menu
}


##########################################
#
# Actions
#
##########################################



proc GT::action_switch_directed editor {

    global GT
    set graph $GT($editor,graph)

    GT::excursion $editor {
	$graph set -directed [expr ![$graph get -directed]]
	$graph draw
    }

    return
}


##########################################
#
# GT::generate_graph
# GT::action_generate_graph
#
##########################################


proc GT::generate_graph {editor n args} {

    global GT

    for {set i 0} {$i < [llength $args]} {incr i} {
	switch -- [lindex $args $i] {
	    -drawing_area {
		set drawing_area [lindex $args [incr i]]
	    }
	    -proc {
		set proc [lindex $args [incr i]]
	    }
	    -with_nodes {
		set with_nodes [lindex $args [incr i]]
	    }
	    -with_edges {
		set with_edges [lindex $args [incr i]]
	    }
	    default {
		set proc [lindex $args $i]
	    }
	}
    }

    if ![info exists drawing_area] {
	set drawing_area [GT::visible_drawing_area $editor]	
    }

    set graph $GT($editor,graph)
    set canvas $GT($editor,canvas)

    foreach {minx miny maxx maxy} $drawing_area {}

    set width 0
    set height 0
    foreach node $graph {
	set w [$graph get $node -width]
	if {$width > $width} {
	    set width $w
	}
	set h [$graph get $node -height]
	if {$width > $height} {
	    set height $h
	}
    }

    #
    # compute center (x,y) and radius (r) for a circle in the
    # drawing area
    #

    set w [$graph translate $canvas -reverse 1 -x \
	       [expr $maxx - $minx - 2*$width]]
    set h [$graph translate $canvas -reverse 1 -y \
	       [expr $maxy - $miny - 2*$height]]
    set x [$graph translate $canvas -reverse 1 -x \
	       [expr ($maxx - $minx)/2 + $minx]]
    set y [$graph translate $canvas -reverse 1 -y \
	       [expr ($maxy - $miny)/2 + $miny]]

    return [$proc $editor $n $x $y $w $h $with_nodes $with_edges]
}


proc GT::action_generate_graph  { editor size args } {

    global GT GT_selection
    set graph $GT($editor,graph)

    if {$size <= 0} {
	GT::message $editor "$size is an invalid size for a graph." error
	return
    }

    foreach {proc dialog simple} $args {}

    GT::excursion $editor {

	if { [$graph nodes] != {} } {
	    if {$simple != {}} {
		set query [tk_dialog \
			       $editor.deletegraph_dialog \
			       $dialog \
			       "Delete the existing graph ?" \
			       warning  \
			       0 \
			       "Delete" "Cancel" \
			      ]
		if {$query == 1} {
		    set query 4
		}
	    } else {
		set query [tk_dialog \
			       $editor.deletegraph_dialog \
			       $dialog \
			       "Delete the existing graph ?" \
			       warning  \
			       0 \
			       "Delete" "Insert" "Embed" "Embed All" "Cancel" \
			      ]
	    }
	} else {
	    set query 0
	}

	switch -regexp $query {
	    0 {
		$graph delete [$graph nodes]
		set with_nodes {}
		set with_edges {}
	    }
	    1 {
		set with_nodes {}
		set with_edges {}
	    }
	    2 {
		set with_nodes $GT_selection($editor,selected,node)
		set with_edges $GT_selection($editor,selected,edge)
	    }
	    3 {
		set with_nodes [$graph nodes]
		set with_edges [$graph edges]
	    }
	}
	
	if { $query != 4 } {
	    set inserted_nodes [GT::generate_graph $editor $size \
				    -with_nodes $with_nodes \
				    -with_edges $with_edges \
				    -proc $proc]
	    $graph draw
	}

	switch -regexp $query {
	    1|2|3 {
		GT::select $editor $inserted_nodes
	    }
	}
    }
}


##########################################
#
# GT::create_complete_graph
# GT::action_create_complete_graph
#
##########################################


proc GT::create_complete_graph {editor n x y w h with_nodes with_edges args} {

    global GT
    set graph $GT($editor,graph)
    set directed [$graph get -directed]

    set r [expr $w > $h ? $h/2 : $w/2]
    set pi 3.1415926
    set slice [expr 2*$pi/$n]

    for {set i 0} {$i < $n} {incr i} {
	if {[lindex $with_nodes $i] != {}} {
	    set nodes($i) [lindex $with_nodes $i]
	} else {
	    set nodes($i) [$graph create node]
	    $graph configure $nodes($i) graphics \
		-x [expr sin($slice*$i) * $r + $x ] \
		-y [expr cos($slice*$i) * $r + $y ]
	}
	lappend inserted_nodes $nodes($i)
    }
    
    for {set i 0} {$i < $n} {incr i} {
	for {set j 0} {$j < $n} {incr j} {
	    if { $directed == 0 } {
		if {$i < $j} {
		    if {[$graph edges -multi $nodes($i) $nodes($j)] == {}} {
			$graph create edge $nodes($i) $nodes($j)
		    }
		}
	    } else {
		if {$i != $j} {
		    if {[$graph edges -multi $nodes($i) $nodes($j)] == {}} {
			$graph create edge $nodes($i) $nodes($j)
		    }
		}
	    }
	}
    }

    return $inserted_nodes
}


proc GT::action_create_complete_graph {editor n args} {

    return [eval GT::action_generate_graph $editor $n \
		GT::create_complete_graph \
		{{Complete Graph}} \
		$args]
}



##########################################
#
# GT::action_create_complete_graph
#
##########################################


proc GT::create_circle_graph {editor n x y w h with_nodes with_edges args} {

    global GT
    set graph $GT($editor,graph)

    set r [expr $w > $h ? $h/2 : $w/2]
    set pi 3.1415926
    set slice [expr 2*$pi/$n]

    for {set i 0} {$i < $n} {incr i} {
	if {[lindex $with_nodes $i] != {}} {
	    set nodes($i) [lindex $with_nodes $i]
	} else {
	    set nodes($i) [$graph create node]
	}
	$graph configure $nodes($i) graphics \
	    -x [expr sin($slice*$i) * $r + $x ] \
	    -y [expr cos($slice*$i) * $r + $y ]
	lappend inserted_nodes $nodes($i)
    }

    for {set i 0} {$i < $n} {incr i} {
	set j [expr ($i+1) % $n]
	if { [$graph edges -multi $nodes($i) $nodes($j)] == {} } {
	    $graph create edge $nodes($i) $nodes([expr ($i+1) % $n])
	}
    }

    return $inserted_nodes
}


proc GT::action_create_circle_graph {editor n args} {

    return [eval GT::action_generate_graph $editor $n \
		GT::create_circle_graph \
		{{Create Circle Graph}} \
		$args]
}



##########################################
#
# GT::create_graphsize_menu editor menu label action size
#
##########################################

proc GT::create_graphsize_menu { editor menu label action
    { size { 1 2 3 4 5 6 7 8 9 10 15 20 25 42 {Other ...} } }
} {

    set underline [string first ^ $label]
    if {$underline != -1} {
	regsub \\^ $label "" label
	$menu add cascade \
	    -label $label \
	    -menu $menu.$action \
	    -underline $underline
    } else {
	$menu add cascade \
	    -label $label \
	    -menu $menu.$action
    }

    menu $menu.$action \
	-tearoff 0

    foreach s $size {
	if {[string match Other* $s] == 0} {
	    $menu.$action add command \
		-label $s \
		-command "GT::menu_action $editor $action $s"
	} else {
	    $menu.$action add command \
		-label $s \
		-command "GT::menu_action $editor select_graphsize $action"
	}
    }
}


proc GT::action_select_graphsize { editor action } {

    global GT

    toplevel $editor.select_graphsize
    
    #
    # Upper row: enter the size
    #

    frame $editor.select_graphsize.upper \
	-borderwidth 1 \
	-relief raised
    label $editor.select_graphsize.upper.label \
	-text "Size:"
    pack $editor.select_graphsize.upper.label \
	-side left
    entry $editor.select_graphsize.upper.size \
	-width 4 \
	-textvariable GT($editor.select_graphsize)
    bind $editor.select_graphsize.upper.size <Return> \
	"GT::select_graphsize_ok_button $editor $action"
    pack $editor.select_graphsize.upper.size \
	-side left
    pack $editor.select_graphsize.upper \
	-ipadx 10 \
	-ipady 10 \
	-expand true \
	-fill x

    #
    # "Cancel" and "Ok" buttons
    #

    proc ::GT::select_graphsize_ok_button { editor action } {
	global GT
	destroy $editor.select_graphsize;	
	GT::action $editor $action $GT($editor.select_graphsize)
    }

    frame $editor.select_graphsize.buttons \
	-borderwidth 1 \
	-relief raised
    button $editor.select_graphsize.buttons.ok \
	-text "Ok" \
	-command "GT::select_graphsize_ok_button $editor $action"
    pack $editor.select_graphsize.buttons.ok \
	-side left \
	-padx 5    

    button $editor.select_graphsize.buttons.cancel \
	-text "Cancel" \
	-command "destroy $editor.select_graphsize"
    pack $editor.select_graphsize.buttons.cancel \
	-side left \
	-padx 5
    pack $editor.select_graphsize.buttons \
	-ipadx 10 \
	-ipady 10 \
	-expand true \
	-fill x

    GT::position_window_near_graph $editor $editor.select_graphsize default
}



##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
