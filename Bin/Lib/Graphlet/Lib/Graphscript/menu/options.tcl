#
# menu/options.tcl
#
# This file generates the "Options" menu.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/options.tcl,v $
# $Author: himsolt $
# $Revision: 1.11 $
# $Date: 1999/03/05 20:42:13 $
# $Locker:  $
# $State: Exp $
#
#------------------------------------------
#
# (C) University of Passau 1995-1999, graphlet project
#

package require Graphlet
package provide Graphscript [gt_version]

namespace eval GT {
    namespace export \
	create_Options_menu \
	create_Graph_options_menu \
	create_Node_options_menu \
	create_Edge_options_menu \
	create_Grid_options_menu \
	action_global_options_dialog
}


proc GT::create_Options_menu { editor menu } {

    global GT GT_options GT_menu

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Options

    #
    # Graph submenu
    #

    GT::add_submenu $editor $menu {
	"^Graph" Graph_options
    }
    GT::add_submenu $editor $menu {
	"^Node" Node_options
    }
    GT::add_submenu $editor $menu {
	"^Edge" Edge_options
    }
    GT::add_submenu $editor $menu {
	"Gri^d" Grid_options
    }

    #
    # Layout Options Submenu
    #

    $menu add separator

    GT::add_submenu $editor $menu {
	"^Layout" Layout_options 0 enabled
    }

    GT::add_menu_command $editor $menu {
	"^Postscript ..." file_postscript_options "" 0
    }

    GT::add_menu_command $editor $menu {
	"Gl^obal ..." global_options_dialog "" 0
    }

    $menu add separator

    set GT(action,revert_options_to_factory_settings) \
	GT::action_revert_options_to_factory_settings
    GT::add_menu_command $editor $menu {
	"Revert to Factory Settings" revert_options_to_factory_settings
    }

    set GT(action,revert_options_to_defaults) \
	GT::action_revert_options_to_defaults
    GT::add_menu_command $editor $menu {
	"Revert to Defaults" revert_options_to_defaults
    }

    GT::add_menu_command $editor $menu {
	"^Save Options" options_save
    }

}


proc GT::create_Graph_options_menu { editor menu } {

    global GT_options

    $menu add checkbutton \
	-label "Directed graph" \
	-variable GT_options(directed) \
	-command "GT::menu_action $editor switch_directed" \
	-underline 0
}


proc GT::create_Node_options_menu { editor menu } {

    global GT_options

    $menu add checkbutton \
	-label "Adjust Size To Label" \
	-variable GT_options(adjust_size_to_label)

    $menu add separator

    $menu add checkbutton \
	-label "Autonumber nodes" \
	-variable GT_options(autonumber_nodes) \
	-command {
	    set GT_options(autonumber_nodes_by_id) 0
	    set GT_options(autonumber_nodes_by_degree) 0
	}

    $menu add checkbutton \
	-label "Autonumber nodes by id" \
	-variable GT_options(autonumber_nodes_by_id) \
	-command {
	    set GT_options(autonumber_nodes) 0
	    set GT_options(autonumber_nodes_by_degree) 0
	}

    $menu add checkbutton \
	-label "Autonumber nodes by degree" \
	-variable GT_options(autonumber_nodes_by_degree) \
	-command {
	    set GT_options(autonumber_nodes) 0
	    set GT_options(autonumber_nodes_by_id) 0
	}
}


proc GT::create_Edge_options_menu { editor menu } {

    global GT_options

    $menu add checkbutton \
	-label "Allow multiple edges" \
	-variable GT_options(allow_multiple_edges)

    $menu add checkbutton \
	-label "Allow self loops" \
	-variable GT_options(allow_self_loops)

    $menu add checkbutton \
	-label "Allow bends" \
	-variable GT_options(allow_bends)

    $menu add separator

    $menu add checkbutton \
	-label "Autonumber edges" \
	-variable GT_options(autonumber_edges) \
	-command {
	    set GT_options(autonumber_edges_by_id) 0
	}

    $menu add checkbutton \
	-label "Autonumber edges by id" \
	-variable GT_options(autonumber_edges_by_id) \
	-command {set GT_options(autonumber_edges) 0}
}


proc GT::create_Grid_options_menu {editor menu} {
    $menu add radio \
	-variable GT_options($editor,grid) \
	-value 0 \
	-label None
    foreach i { 1 2 3 4 5 8 10 12 15 16 20 24 25 32 42 50 64 } {
	$menu add radio -variable GT_options($editor,grid) -label $i
    }
}


###########################################
#
# Actions
#
###########################################


proc GT::action_global_options_dialog  { editor } {
    GT::global_options_dialog $editor
}



##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
