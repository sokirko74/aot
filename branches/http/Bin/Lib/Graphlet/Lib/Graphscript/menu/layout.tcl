#
# layout.tcl
#
# This module implements the "Layout" menu.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/layout.tcl,v $
# $Author: boerncke $
# $Revision: 1.12 $
# $Date: 1999/03/08 12:06:21 $
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
	create_Layout_menu \
	create_Layout_options_menu
}


proc GT::create_Layout_menu { editor menu } {

    global GT GT_options

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Layout

    #
    # Add "normal" layout algorithms
    #

    if [has_package Gt_cfr_layout] {
        GT::add_menu_command $editor $menu {
            "Spring Embedder with ^Constraints" \
		layout_constraint_fr ""
        }
    }

    if [has_package Gt_icse_layout] {
        GT::add_menu_command $editor $menu {
            "^Iterative Constraint Spring Embedder" \
		layout_iterative_constraint_se ""
        }
    }

    if [has_package Gt_lsd_gem] {
	GT::add_menu_command $editor $menu {
	    "Spring Embedder (^GEM)" layout_spring_gem ""
	}
    }

    if [has_package Gt_lsd_springembedder_kamada] {
	GT::add_menu_command $editor $menu {
	    "Spring Embedder (^Kamada)" layout_spring_kk ""
	}
    }

    if [has_package Gt_lsd_springembedder_rf] {
	GT::add_menu_command $editor $menu {
	    "Spring Embedder (^FR)" layout_spring_fr ""
	}
    }

    if [has_package Gt_lsd_tunkelang] {
	$menu add separator
	GT::add_menu_command $editor $menu {
	    "General Graphs (^Tunkelang)" layout_tunkelang ""
	}
    }

    if [has_package {Gt_lsd_sugiyama Gt_xdag}] {
	$menu add separator
    }
    
    if [has_package Gt_lsd_sugiyama] {
	GT::add_menu_command $editor $menu {
	    "^DAG" layout_dag ""
	}
    }

    if [has_package Gt_xdag] {
  	GT_xdag::add_layout_menu_entries $editor $menu
    }

    if [has_package {
	Gt_tree_layout Gt_radial_tree_layout Gt_lsd_tree_layout_walker
    }] {
	$menu add separator
    }

    if [has_package Gt_tree_layout] {
	GT::add_menu_command $editor $menu {
	    "^Tree" layout_tree ""
	}
    }

    if [has_package Gt_lsd_tree_layout_walker] {
	GT::add_menu_command $editor $menu {
	    "Tree (^Walker)" layout_tree_walker ""
	}
    }

    if [has_package Gt_radial_tree_layout] {
  	GT_layout_radial_tree::add_layout_menu_entries $editor $menu
    }

    if {[$menu index end] > 0} {
	$menu add separator
    }

    #
    # First entry is a menu of randomizers
    #

    GT::add_menu_command $editor $menu {
	"R^andom Coordinates" layout_random "" 0
    }

    $menu add separator

    GT::add_submenu $editor $menu {
	"^Options" Layout_options
    }

}



proc GT::create_Layout_options_menu {editor menu} {

    global GT
    global GT_options

    $menu configure \
	-tearoff $GT_options(expert) \
	-title "Layout Options"

    #
    # Add "normal" layout algorithms
    #

    if [has_package Gt_cfr_layout] {
        GT::add_menu_command $editor $menu {
            "Spring Embedder with ^Constraints ..." \
		layout_constraint_fr_options ""
        }
    }

    if [has_package Gt_icse_layout] {
        GT::add_menu_command $editor $menu {
            "^Iterative Constraint Spring Embedder ..." \
		layout_iterative_constraint_se_options ""
        }
    }

    if [has_package Gt_lsd_gem] {
	GT::add_menu_command $editor $menu {
	    "Spring Embedder (^GEM) ..." layout_spring_gem_options ""
	}
    }

    if [has_package Gt_lsd_springembedder_kamada] {
	GT::add_menu_command $editor $menu {
	    "Spring Embedder (^Kamada) ..." layout_spring_kk_options ""
	}
    }

    if [has_package Gt_lsd_springembedder_rf] {
	GT::add_menu_command $editor $menu {
	    "Spring Embedder (^FR) ..." layout_spring_fr_options ""
	}
    }

    if [has_package Gt_lsd_tunkelang] {
	$menu add separator
	GT::add_menu_command $editor $menu {
	    "General Graphs (^Tunkelang) ..." layout_tunkelang_options ""
	}
    }

    if [has_package {Gt_lsd_sugiyama Gt_xdag}] {
	$menu add separator
    }
    
    if [has_package Gt_lsd_sugiyama] {
	GT::add_menu_command $editor $menu {
	    "^Dag ..." layout_dag_options ""
	}
    }

    if [has_package Gt_xdag] {
 	GT_xdag::add_layout_options_menu_entries $editor $menu
    }

    if [has_package {
	Gt_tree_layout Gt_radial_tree_layout Gt_lsd_tree_layout_walker
    }] {
	$menu add separator
    }

    if [has_package Gt_tree_layout] {
	GT::add_menu_command $editor $menu {
	    "^Tree ..." layout_tree_options ""
	}
    }

    if [has_package Gt_lsd_tree_layout_walker] {
	GT::add_menu_command $editor $menu {
	    "Tree (^Walker) ..." layout_tree_walker_options ""
	}
    }

    # the following modified by fb
    if [has_package Gt_radial_tree_layout] {
	GT::add_menu_command $editor $menu {
	    "Tree (^Radial) ..." layout_radial_options ""
	}
	GT::add_menu_command $editor $menu {
	    "Tree (Co^ncentric) ..." layout_concentric_options ""
	}
    }
}


##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
