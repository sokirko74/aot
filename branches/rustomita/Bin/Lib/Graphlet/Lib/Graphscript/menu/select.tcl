#
# select.tcl
#
# This module implements the "Select" menu
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/select.tcl,v $
# $Author: himsolt $
# $Revision: 1.8 $
# $Date: 1999/03/05 20:42:14 $
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
	create_Select_menu \
	action_select_all \
	action_select_all_nodes \
	action_select_all_edges \
	action_deselect_all \
	action_deselect_all_nodes \
	action_deselect_all_edges \
	action_select_neighbours \
	action_select_in_neighbours \
	action_select_out_neighbours \
	action_select_adjacent_edges \
	action_select_in_edges \
	action_select_out_edges \
	action_select_inner_edges \
	action_select_embedding_edges \
	action_select_inner_edges \
	action_select_multiple_edges \
	action_select_isolated_nodes
}


proc GT::create_Select_menu { editor menu } {

    global GT_options

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Select

    GT::add_menu_command $editor $menu {
	"Select ^All" edit_select_all "Ctrl+slash"
    }

    GT::add_menu_command $editor $menu {
	"^Deselect All" edit_deselect_all "Ctrl+backslash"
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"Select All ^Nodes" edit_select_all_nodes "Ctrl+A"
    }

    GT::add_menu_command $editor $menu {
	"Select All ^Edges" edit_select_all_edges "Ctrl+E"
    }

    GT::add_menu_command $editor $menu {
	"Deselect All Nodes" edit_deselect_all_nodes
    }

    GT::add_menu_command $editor $menu {
	"Deselect All Edges" edit_deselect_all_edges
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"Neigh^bours" select_neighbours
    }

    GT::add_menu_command $editor $menu {
	"In Neighbours" select_in_neighbours
    }

    GT::add_menu_command $editor $menu {
	"Out Neighbours" select_out_neighbours
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"^Adjacent edges" select_adjacent_edges
    }

    GT::add_menu_command $editor $menu {
	"In Edges" select_in_edges
    }

    GT::add_menu_command $editor $menu {
	"Out Edges" select_out_edges
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"E^mbedding Edges" select_embedding_edges
    }

    GT::add_menu_command $editor $menu {
	"^Inner Edges" select_inner_edges
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"^Multiple Edges" select_multiple_edges
    }

    GT::add_menu_command $editor $menu {
	"Is^olated Nodes" select_isolated_nodes
    }
}


###########################################
#
# Actions
#
###########################################



proc GT::action_select_all  { editor  } {

    global GT
    set graph $GT($editor,graph)

    GT::select $editor select all
}


proc GT::action_select_all_nodes  { editor  } {

    global GT
    set graph $GT($editor,graph)

    GT::select $editor select nodes
}


proc GT::action_select_all_edges  { editor  } {

    global GT

    GT::select $editor select edges
}


proc GT::action_deselect_all  { editor  } {

    GT::select $editor remove all
}


proc GT::action_deselect_all_nodes  { editor  } {

    GT::select $editor remove nodes
}


proc GT::action_deselect_all_edges  { editor  } {

    GT::select $editor remove edges

}


proc GT::action_select_neighbours  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::select $editor remove edges
    GT::select $editor select \
	[$graph nodes -adj $GT_selection($editor,selected,node)]
}


proc GT::action_select_in_neighbours  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::select $editor remove edges
    GT::select $editor select \
	[$graph nodes -in $GT_selection($editor,selected,node)]
}


proc GT::action_select_out_neighbours  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::select $editor remove edges
    GT::select $editor select \
	[$graph nodes -out $GT_selection($editor,selected,node)]
}


proc GT::action_select_adjacent_edges  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::select $editor remove edges
    GT::select $editor add \
	[$graph edges -adj $GT_selection($editor,selected,node)]
}


proc GT::action_select_in_edges  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::select $editor remove edges
    if [$graph get -directed] {
	GT::select $editor add \
	    [$graph edges -in $GT_selection($editor,selected,node)]
    } else {
	GT::select $editor add \
	    [$graph edges -adj $GT_selection($editor,selected,node)]
    }
}


proc GT::action_select_out_edges  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::select $editor remove edges
    if [$graph get -directed] {
	GT::select $editor add \
	    [$graph edges -out $GT_selection($editor,selected,node)]
    } else {
	GT::select $editor add \
	    [$graph edges -adj $GT_selection($editor,selected,node)]	
    }
}


proc GT::action_select_inner_edges  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::select $editor remove edges
    GT::select $editor add \
	[$graph edges -inner $GT_selection($editor,selected,node)]
}


proc GT::action_select_embedding_edges  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::select $editor remove edges
    GT::select $editor add \
	[$graph edges -embedding $GT_selection($editor,selected,node)]
}


proc GT::action_select_inner_edges  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::select $editor remove edges
    GT::select $editor add \
	[$graph edges -inner $GT_selection($editor,selected,node)]
}


proc GT::action_select_multiple_edges  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::select $editor remove edges
    GT::select $editor add \
	[$graph edges -multi]
}

proc GT::action_select_isolated_nodes  { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    GT::select $editor remove edges
    GT::select $editor add \
	[$graph nodes -isolated]
}


##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
