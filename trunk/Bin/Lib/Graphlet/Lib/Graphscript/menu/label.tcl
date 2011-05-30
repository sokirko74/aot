#
# tools.tcl
#
# This module implements several tools for working with graphs.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/label.tcl,v $
# $Author: himsolt $
# $Revision: 1.8 $
# $Date: 1999/03/05 20:42:07 $
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
	create_Label_menu \
	action_number_nodes \
	action_set_node_labels_to_internal_id \
	action_set_node_labels_to_degree \
	action_number_edges \
	action_set_edge_labels_to_internal_id \
	action_set_edge_labels_to_endnode_labels
}


##########################################
#
# Initialize menu
#
##########################################


proc GT::create_Label_menu { editor menu } {
    
    global GT GT_options

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Label

    GT::add_menu_command $editor $menu {
	"Number Nodes" number_nodes
    }

    GT::add_menu_command $editor $menu {
	"Set Node Labels to Internal Id" set_node_labels_to_internal_id
    }

    GT::add_menu_command $editor $menu {
	"Set Node Labels to Node Degree" set_node_labels_to_degree "" 18
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"Number Edges" number_edges
    }

    GT::add_menu_command $editor $menu {
	"Set Edge Labels to Internal Id" set_edge_labels_to_internal_id
    }

    GT::add_menu_command $editor $menu {
	"Set Edge Labels to Endnode Labels" set_edge_labels_to_endnode_labels
    }
}


##########################################
#
# Actions
#
##########################################



proc GT::action_number_nodes editor {

    global GT GT_selection
    set graph $GT($editor,graph)

    if { $GT_selection($editor,selected,node) == {} } {
	set nodes [$graph nodes]
    } else {
	set nodes $GT_selection($editor,selected,node)
    }

    GT::undo $editor attributes $nodes -label

    set i 0
    foreach node $nodes {
	$graph set $node -label $i
	incr i
    }

    $graph draw
}


proc GT::action_set_node_labels_to_internal_id editor {

    global GT GT_selection
    set graph $GT($editor,graph)

    if { $GT_selection($editor,selected,node) == {} } {
	set nodes [$graph nodes]
    } else {
	set nodes $GT_selection($editor,selected,node)
    }

    GT::undo $editor attributes $nodes -label

    foreach node $nodes {
	$graph set $node -label [$graph get $node -id]
    }

    $graph draw
}


proc GT::action_set_node_labels_to_degree editor {

    global GT GT_selection
    set graph $GT($editor,graph)

    if { $GT_selection($editor,selected,node) == {} } {
	set nodes [$graph nodes]
    } else {
	set nodes $GT_selection($editor,selected,node)
    }

    GT::undo $editor attributes $nodes -label

    foreach node $nodes {
	$graph configure $node \
		-label [llength [$graph edges -node $node]]
    }

    $graph draw
}


proc GT::action_number_edges editor {

    global GT GT_selection
    set graph $GT($editor,graph)

    if { $GT_selection($editor,selected,edge) == {} } {
	set edges [$graph edges]
    } else {
	set edges $GT_selection($editor,selected,edge)
    }

    GT::undo $editor attributes $edges -label

    set i 0
    foreach edge $edges {
	$graph set $edge -label $i
	incr i
    }

    $graph draw
}


proc GT::action_set_edge_labels_to_internal_id editor {

    global GT GT_selection
    set graph $GT($editor,graph)

    if { $GT_selection($editor,selected,edge) == {} } {
	set edges [$graph edges]
    } else {
	set edges $GT_selection($editor,selected,edge)
    }

    GT::undo $editor attributes $edges -label

    foreach edge $edges {
	$graph set $edge -label [$graph get $edge -id]
    }

    $graph draw
}



proc GT::action_set_edge_labels_to_endnode_labels editor {

    global GT GT_selection
    set graph $GT($editor,graph)

    if { $GT_selection($editor,selected,edge) == {} } {
	set edges [$graph edges]
    } else {
	set edges $GT_selection($editor,selected,edge)
    }

    GT::undo $editor attributes $edges -label

    foreach edge $edges {
	set source [$graph get $edge -source]
	set target [$graph get $edge -target]
	$graph set $edge -label \
	    "[$graph get $source -label]-[$graph get $target -label]"
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
