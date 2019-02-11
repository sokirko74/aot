#
# menu/node.tcl
#
# This module implements several tools for working with graphs.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/node.tcl,v $
# $Author: himsolt $
# $Revision: 1.14 $
# $Date: 1999/03/05 20:42:11 $
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
	create_Node_menu \
	duplicate_nodes \
	action_duplicate_nodes \
	action_clear_node_labels \
	action_adjust_size_to_label \
	clean_up_graph \
	action_clean_up_graph_size \
	action_clean_up_graph_color \
	action_clean_up_graph_random \
	snap_to_grid \
	action_snap_to_grid \
	remove_isolated_nodes \
	action_remove_isolated_nodes
}


##########################################
#
# Initialize menu
#
##########################################


proc GT::create_Node_menu { editor menu } {
    
    global GT GT_options GT_menu

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Node

    GT::add_menu_command $editor $menu {
	"^Duplicate" duplicate_nodes
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"Adjust Size To ^Label" adjust_size_to_label ""
    }

    GT::add_menu_command $editor $menu {
	"^Clear Node Labels" clear_node_labels "" 6
    }

    $menu add separator

#     GT::add_menu_command $editor $menu {
# 	"Set all Nodes to Default ^Size" clean_up_graph_size "" 8
#     }
#     GT::add_menu_command $editor $menu {
# 	"Set all Nodes and Edges to Default ^Color" clean_up_graph_color "" 8
#     }

#     $menu add separator

    GT::add_menu_command $editor $menu {
	"Remove all ^Isolated Nodes" remove_isolated_nodes "" 7
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"Number Nodes" number_nodes
    }

    GT::add_menu_command $editor $menu {
	"Set Node Labels to Node Degree" set_node_labels_to_degree "" 18
    }

    if {$GT_options(developer) == 1} {
	GT::add_menu_command $editor $menu {
	    "Set Node Labels to Internal Id" set_node_labels_to_internal_id
	}
    }

    $menu add separator

    GT::add_submenu $editor $menu {
	"^Options" Node_options
    }
}


##########################################
#
# Actions
#
# GT::duplicate_nodes
# GT::action_duplicate_nodes
#
##########################################


proc GT::duplicate_nodes { editor nodes } {

    global GT GT_options
    set graph $GT($editor,graph)

    foreach n $nodes {
	foreach e [$graph edges -in $n] {
	    if {[lsearch -exact $nodes [$graph get $e -source]] >= 0} {
		lappend inner_in($n) $e
	    } else {
		lappend embed_in($n) $e
	    }
	}
	foreach e [$graph edges -out $n] {
	    if {[lsearch -exact $nodes [$graph get $e -target]] >= 0} {
		lappend inner_out($n) $e
	    } else {
		lappend embed_out($n) $e
	    }
	}
    }

    foreach n $nodes {
	set copy($n) [lindex [$graph copynode $n] 0]
	lappend new_nodes $copy($n)
    }

    foreach n $nodes {
	if [info exists inner_in($n)] {
	    foreach e $inner_in($n) {
		set source [$graph get $e -source]
		set target [$graph get $e -target]
		lappend new_edges \
		    [$graph copyedge $e $copy($source) $copy($target)]
	    }
	}
	if [info exists inner_out($n)] {
	    foreach e $inner_out($n) {
		set source [$graph get $e -source]
		set target [$graph get $e -target]
		lappend new_edges \
		    [$graph copyedge $e $copy($source) $copy($target)]
	    }
	}
	if [info exists embed_in($n)] {
	    foreach e $embed_in($n) {
		set source [$graph get $e -source]
		set target [$graph get $e -target]
		lappend new_edges [$graph copyedge $e $source $copy($target)]
	    }
	}
	if [info exists embed_out($n)] {
	    foreach e $embed_out($n) {
		set source [$graph get $e -source]
		set target [$graph get $e -target]
		lappend new_edges [$graph copyedge $e $copy($source) $target]
	    }
	}
    }

    $graph draw

    if [info exists new_nodes] {
	$graph nodemove $new_nodes \
	    $GT_options(duplicate_offset_x) \
	    $GT_options(duplicate_offset_y)
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


proc GT::action_duplicate_nodes editor {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::excursion $editor {
	set nodes $GT_selection($editor,selected,node)
	set duplicated_nodes [GT::duplicate_nodes $editor $nodes]
	GT::undo $editor delete [lindex $duplicated_nodes 0]
    }

    $graph draw


    GT::select $editor [join $duplicated_nodes]
}


proc GT::action_clear_node_labels editor {

    global GT GT_selection
    set graph $GT($editor,graph)

    if { $GT_selection($editor,selected,node) == {} } {
	set nodes [$graph nodes]
    } else {
	set nodes $GT_selection($editor,selected,node)
    }

    GT::undo $editor attributes $nodes -label

    $graph configure $nodes -label {}
    $graph draw
}



##########################################
#
# Adjust Size To Label
#
##########################################


proc GT::action_adjust_size_to_label { editor } {

    global GT GT_selection GT_options
    set graph $GT($editor,graph)

    set save_cursor [$editor cget -cursor]
    $editor configure -cursor watch

    set nodes $GT_selection($editor,selected,node)
    if { $nodes == {} } {
	set nodes [$graph nodes]
    }

    GT::undo $editor attributes $nodes -w -h

    set save_option_adjust_to_label $GT_options(adjust_size_to_label)
    set GT_options(adjust_size_to_label) 1

    foreach node $nodes {
	$graph set $node -label [$graph get $node -label]
    }
    $GT($editor,graph) draw

    set GT_options(adjust_size_to_label) $save_option_adjust_to_label
    $editor configure -cursor $save_cursor
}



##########################################
#
# GT::clean_up_graph
# GT::action_clean_up_graph_*
#
##########################################


proc GT::clean_up_graph { editor args } {

    global GT
    set graph $GT($editor,graph)

    #
    # options
    #
    set random 0
    set blackwhite 0
    set resize 0

    set argc [llength args]
    for {set i 0} {$i < $argc} {incr i} {
	switch -regexp -- [lindex $args $i] {
	    -random {
		set random 1
	    }
	    -bw|-blackwhite {
		set blackwhite 1
	    }
	    -resize {
		set resize 1
		set w [lindex $args [incr i]]
		set h [lindex $args [incr i]]
	    }
	}
    }

    if $resize {
	$graph configure [$graph nodes] graphics \
		-w $w \
		-h $h
    }
    if $blackwhite {
	$graph configure [$graph nodes] graphics \
		-fill "#FFFFFF" \
		-outline "#000000"
	$graph configure [$graph edges] graphics \
		-fill "#000000"
    }    

    if $random {
	GT::action_layout_random $editor
    }
}


proc GT::action_clean_up_graph_size { editor  } {

    global GT
    set graph $GT($editor,graph)

    GT::excursion $editor -selection_is_save {

	foreach node [$graph nodes] { 

	    GT::undo $editor attributes $node -w -h

	    set style [$graph style $node]
 	    set w [$graph get $style -w]
	    set h [$graph get $style -h]
	    $graph set $node graphics \
		-w $w \
		-h $h
	}

	$graph draw
    }
}


proc GT::action_clean_up_graph_color { editor  } {

    global GT
    set graph $GT($editor,graph)

    GT::excursion $editor -selection_is_save {

	foreach node [$graph nodes] {

 	    set style [$graph style $node]
 	    set fill [$graph get $style -fill]
	    if {$fill != {}} {
		GT::undo $editor attributes $node -fill
		$graph set $node graphics \
		    -fill $fill
	    }

	    set outline [$graph get $style -outline]
	    if {$outline != {}} {
		GT::undo $editor attributes $node -outline
		$graph set $node graphics \
		    -outline $outline
	    }
	}
	$graph draw
    }
    $graph draw
}


proc GT::action_clean_up_graph_random { editor  } {

    global GT
    set graph $GT($editor,graph)

    GT::excursion $editor {
	GT::clean_up_graph $editor -random
	$graph draw
    }
}



##########################################
#
# Adjust To Grid
#
##########################################


proc GT::snap_to_grid { editor {nodes {}} } {

    global GT

    set graph $GT($editor,graph)
    if { $nodes == {} } {
	set nodes [$graph nodes]
    }

    set gw [GT::get_option $editor grid 0]

    if { $gw != {} &&  $gw > 0 } {
	foreach node $nodes {
	    $graph configure $node graphics \
		    -x [expr round([$graph get $node -x] / $gw) * $gw] \
		    -y [expr round([$graph get $node -y] / $gw) * $gw]
	}
    }
}


proc GT::action_snap_to_grid { editor } {

    global GT GT_selection

    GT::undo $editor attributes \
	$GT_selection($editor,selected,node) -x -y

    GT::snap_to_grid $editor $GT_selection($editor,selected,node)
    $GT($editor,graph) draw
}


##########################################
#
# GT::remove_isolated_nodes
# GT::action_remove_isolated_nodes
#
##########################################


proc GT::action_remove_isolated_nodes { editor  } {

    global GT
    set graph $GT($editor,graph)

    set isolated_nodes [$graph nodes -isolated]
    GT::undo $editor push $isolated_nodes
    GT::excursion $editor {
	$graph delete $isolated_nodes
    }

    if { [llength $isolated_nodes] == 0 } {
	GT::message $editor "No isolated nodes found." message
    } elseif { [llength $isolated_nodes] == 1 } {
	GT::message $editor "One isolated node deleted" message
    } else {
	GT::message $editor \
	    "[llength $isolated_nodes] isolated nodes deleted." \
	    message
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
