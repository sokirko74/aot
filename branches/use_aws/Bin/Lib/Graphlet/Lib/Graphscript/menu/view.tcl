#
# menu/view.tcl
#
# This module implements the "View" menu.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/view.tcl,v $
# $Author: himsolt $
# $Revision: 1.10 $
# $Date: 1999/03/05 20:42:18 $
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
	create_View_menu \
	action_view_ \
	action_zoom \
	action_zoom_in \
	action_zoom_out
}


proc GT::create_View_menu { editor menu } {

    global GT_options GT

    $menu configure \
	-tearoff $GT_options(expert) \
	-title View

    $menu add cascade \
	-label "Zoom" \
	-menu $menu.zoom \
	-underline 0

    menu $menu.zoom \
	-tearoff $GT_options(expert) \
	-title Zoom

    foreach size $GT_options(zoom_sizes) {

	set size_percent [expr int($size*100)]
	$menu.zoom add radiobutton \
	    -label "$size_percent %" \
	    -variable GT($editor,zoomx) \
	    -value $size \
	    -command [list GT::menu_action $editor zoom [expr $size*100]]
    }

    GT::add_menu_command $editor $menu {
	"Zoom ^In" zoom_in
    }

    GT::add_menu_command $editor $menu {
	"Zoom ^Out" zoom_out
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"^Find Graph" find_graph
    }

    $menu add separator

    GT::add_submenu $editor $menu { "^Pagesize" }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"^Refresh" refresh "" 0
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
