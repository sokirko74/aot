#
# arrange.tcl
#
# This module implements several tools and a menu for arranging
# graphs.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/arrange.tcl,v $
# $Author: himsolt $
# $Revision: 1.8 $
# $Date: 1999/03/05 20:41:52 $
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
	create_Arrange_menu \
	arrange \
	action_arrange_vertical \
	action_arrange_horizontal \
	action_arrange_circular \
	action_arrange_elliptic
}


##########################################
#
# Initialize menu
#
##########################################


proc GT::create_Arrange_menu { top menu } {

    global GT_options

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Arrange

    GT::add_menu_command $top $menu {
	"^Vertical" arrange_vertical ""
    }

    GT::add_menu_command $top $menu {
	"^Horizontal" arrange_horizontal ""
    }

    GT::add_menu_command $top $menu {
	"^Circular" arrange_circular ""
    }

    GT::add_menu_command $top $menu {
	"^Elliptic" arrange_elliptic ""
    }
}


##########################################
#
# GT::arrange_vertical
# GT::action_arrange_vertical
#
##########################################


proc GT::arrange { editor cmd {nodes {}} {edges {}} } {

    global GT
    set graph $GT($editor,graph)

    set xcoords [$graph get $nodes -x]
    set minx [lindex [lsort -real $xcoords] 0]
    set maxx [lindex [lsort -real $xcoords] end]
    set ycoords [$graph get $nodes -y]
    set miny [lindex [lsort -real $ycoords] 0]
    set maxy [lindex [lsort -real $ycoords] end]
    set pi 3.1415926
    set pi2 [expr $pi*2]

    GT::excursion $editor {
	switch -- $cmd {
	    -vertical {
		if {$nodes != {}} {
		    $graph configure $nodes graphics -x $minx
		}
	    }
	    -horizontal {
		if { $nodes != {} } {
		    $graph configure $nodes graphics -y $miny
		}
	    }
	    -circular {
		if { $nodes != {} } {
		    set center_x [expr ($minx + $maxx) / 2]
		    set center_y [expr ($miny + $maxy) / 2]
		    set radius_x [expr ($maxx - $minx) / 2]
		    set radius_y [expr ($maxy - $miny) / 2]
		    if {$radius_x > $radius_y} {
			set radius $radius_x
		    } else {
			set radius $radius_y
		    }
		    set i 0
		    set n [llength $nodes]
		    set nodes [sort_nodes $graph -circular \
				   $nodes $center_x $center_y]
		    foreach node $nodes {
			set angle [expr double($i)/double($n)*2*$pi]
			$graph configure $node graphics \
			    -x [expr $center_x + cos($angle) * $radius] \
			    -y [expr $center_y + sin($angle) * $radius]
			incr i
		    }
		}
	    }
	    -elliptic {
		if { $nodes != {} } {
		    set center_x [expr ($minx + $maxx) / 2]
		    set center_y [expr ($miny + $maxy) / 2]
		    set radius_x [expr ($maxx - $minx) / 2]
		    set radius_y [expr ($maxy - $miny) / 2]
		    set i 0
		    set n [llength $nodes]
		    set nodes [sort_nodes $graph -circular \
				   $nodes $center_x $center_y]
		    foreach node $nodes {
			set angle [expr double($i)/double($n)*2*$pi]
			$graph configure $node graphics \
			    -x [expr $center_x + cos($angle) * $radius_x] \
			    -y [expr $center_y + sin($angle) * $radius_y]
			incr i
		    }
		}
	    }
	}
    }
}


proc GT::action_arrange_vertical { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    set nodes $GT_selection($editor,selected,node)

    if {[llength $nodes] > 1} {
	GT::excursion $editor {
	    GT::undo $editor attributes $nodes -x -y
	    GT::arrange $editor \
		-vertical $nodes
	    $graph draw
	}
    }
}


proc GT::action_arrange_horizontal { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    set nodes $GT_selection($editor,selected,node)

    if {[llength $nodes] > 1} {
	GT::excursion $editor {
	    GT::undo $editor attributes $nodes -x -y
	    GT::arrange $editor -horizontal $nodes
	    $graph draw
	}
    }
}


proc GT::action_arrange_circular { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    set nodes $GT_selection($editor,selected,node)

    if {[llength $nodes] > 1} {
	GT::excursion $editor {
	    GT::undo $editor attributes $nodes -x -y
	    GT::arrange $editor -circular $nodes
	    $graph draw
	}
    }
}


proc GT::action_arrange_elliptic { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    set nodes $GT_selection($editor,selected,node)

    if {[llength $nodes] > 1} {
	GT::excursion $editor {
	    GT::undo $editor attributes $nodes -x -y
	    GT::arrange $editor -elliptic $nodes
	    $graph draw
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
