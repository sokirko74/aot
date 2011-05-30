#
# tools.tcl
#
# This module implements several tools for working with graphs.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/tool.tcl,v $
# $Author: himsolt $
# $Revision: 1.17 $
# $Date: 1999/03/05 20:42:16 $
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
	create_Tool_menu \
	action_global_set_font \
	show_font_menu \
	action_tool_inspect \
	action_find_graph \
	action_scale_up \
	action_scale_down \
	action_scale_dialog \
	read_directory \
	action_read_directory \
	action_read_directory_options
}


##########################################
#
# Initialize menu
#
##########################################


proc GT::create_Tool_menu { editor menu } {
    
    global GT GT_options GT_menu

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Tool

    GT::IS::create_menu_entry $editor $menu {
	"^Inspector" "Alt+I"
    }

#     $menu add separator

#     GT::add_menu_command $editor $menu {
# 	"^Find Graph" find_graph
#     }

    #
    # Scale
    #

#     GT::add_menu_command $editor $menu {
# 	"Scale ^Down" scale_down "Alt+comma" 6
#     }

#     GT::add_menu_command $editor $menu {
# 	"Scale ^Up" scale_up "Alt+period" 6
#     }

#     GT::add_submenu $editor $menu { "^Pagesize" }

    $menu add separator

    GT::add_submenu $editor $menu {
	"^Arrange"
    }

    GT::add_submenu $editor $menu {
	"Gri^d" Grid_options
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"Make ^Invisible" make_invisible
    }

    GT::add_menu_command $editor $menu {
	"Show all In^visibles" make_visible
    }

#     $menu add separator

#     GT::add_menu_command $editor $menu {
# 	"^Refresh" refresh "" 0
#     }

#     $menu add separator

#     GT::add_submenu $editor $menu { "^Label" }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"Read Directory ..." read_directory_options "" 5
    }

    $menu add separator

#     set GT(action,revert_options_to_factory_settings) \
# 	GT::action_revert_options_to_factory_settings
#     GT::add_menu_command $editor $menu {
# 	"Revert Options to Factory Settings" revert_options_to_factory_settings
#     }

#     set GT(action,revert_options_to_defaults) \
# 	GT::action_revert_options_to_defaults
#     GT::add_menu_command $editor $menu {
# 	"Revert Options to Defaults" revert_options_to_defaults
#     }

    GT::add_menu_command $editor $menu {
	"^Options ..." global_options_dialog "" 0
    }

#     GT::add_menu_command $editor $menu {
# 	"^Save Options" options_save
#     }

}


proc GT::create_Pagesize_menu { editor menu } {

    global GT_options

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Pagesize

    GT::add_menu_command $editor $menu {
	"^Shrink Drawing Area" shrink_drawing_area
    }

    GT::add_menu_command $editor $menu {
	"^Expand Drawing Area" expand_drawing_area
    }

    GT::add_menu_command $editor $menu {
	"^Adapt Drawing Area" adapt_drawing_area
    }

}


##########################################
#
# Actions
#
##########################################


proc GT::action_find_graph  { editor {complain_if_empty yes}} {

    global GT GT_selection

    if {$GT_selection($editor,selected,node) != {} ||
	$GT_selection($editor,selected,edge) != {} } {
	
	GT::guarantee_visible $editor selection $complain_if_empty
    } else {
	GT::guarantee_visible $editor graph $complain_if_empty
    }
}



proc GT::action_scale_up { editor { scale 2.0 } } {

    global GT

    GT::scale $editor $scale

    set canvas $GT($editor,canvas)
    set visible \
	[eval $canvas find overlapping [GT::visible_drawing_area $editor]]
    if { $visible == {} } {
	GT::action_find_graph $editor
    }
}


proc GT::action_scale_down { editor { scale 0.5 } } {

    global GT

    GT::scale $editor $scale

    set canvas $GT($editor,canvas)
    set visible \
	[eval $canvas find overlapping [GT::visible_drawing_area $editor]]
    if { $visible == {} } {
	GT::action_find_graph $editor
    }
}


proc GT::action_scale_dialog { editor  } {

    GT::show_scale_dialog $editor
}


proc GT::action_make_invisible { editor } {

    global GT GT_selection
    set graph $GT($editor,graph)

    $graph set $GT_selection($editor,selected,node) -visible 0
    $graph set $GT_selection($editor,selected,edge) -visible 0
    GT::select $editor {}
    $graph draw
}


proc GT::action_make_visible { editor } {

    global GT GT_selection
    set graph $GT($editor,graph)

    $graph set [$graph nodes] -visible 1
    $graph set [$graph edges] -visible 1
    $graph draw
}


proc GT::action_refresh { editor  } {

    global GT GT_selection
    set graph $GT($editor,graph)

    set old_selection [concat \
			   $GT_selection($editor,selected,node) \
			   $GT_selection($editor,selected,edge)]
    GT::select $editor remove all

    $graph draw -force true

    GT::select $editor $old_selection
}


##########################################
#
# GT::read_directory
# GT::action_read_directory
#
##########################################


proc GT::read_directory { editor rootfile rootnode maxdepth { depth 0 } } {

    global GT GT_options
    set graph $GT($editor,graph)

    if { $depth > $maxdepth } {
	return
    }

    set hasfiles 0
    foreach file [glob $rootfile/*] {

	if { [string match *CVS* $file] || [string match *~ $file] } {
	    continue
	}

	if [file isdirectory $file] {

	    set node [$graph create node]
	    $graph configure $node \
		    -label "[file tail $file]"
	    $graph configure $node graphics \
		-fill $GT_options(read_directory_file_background)
	    $graph configure $node label_graphics \
		-fill $GT_options(read_directory_file_foreground)
	    set edge [$graph create edge $rootnode $node]

	    GT::read_directory $editor \
		$file $node \
		$maxdepth [expr $depth+1]

	} else {

	    set hasfiles 1
	    set node [$graph create node]
	    $graph configure $node -label "[file tail $file]"
	    set edge [$graph create edge $rootnode $node]
	}
    }

}



proc GT::action_read_directory { editor args } {

    global GT GT_options
    set graph $GT($editor,graph)

    if { [$graph nodes] != {} } {
	set query [tk_dialog \
		       .deletegraph_dialog \
		       "Create Complete Graph" \
		       "This will delete the graph in the window." \
		       warning  \
		       0 \
		       "Delete" "Cancel" \
		      ]
    } else {
	set query 0
    }
    
    if { $query == 0 } {

	$graph delete [$graph nodes]
	GT::select $editor remove selection

	set root [$graph create node]
	$graph configure $root \
		-label "DIRECTORY"
	$graph configure $root graphics \
	    -fill $GT_options(read_directory_dir_background)
	$graph configure $root label_graphics \
	    -fill $GT_options(read_directory_dir_foreground)
	
	GT::read_directory $editor . $root $GT_options(read_directory_depth)
	$graph draw
	
	GT::action $editor adjust_size_to_label
	$graph draw

	GT::action $editor find_graph
	GT::action $editor layout_tree
    }

    return
}



proc GT::action_read_directory_options { editor args } {

    lappend options { color
	"Directory Background Color"
	read_directory_dir_background
    }
    lappend options { color
	"Directory Foreground Color"
	read_directory_dir_foreground
    }
    lappend options { color
	"File Background Color"
	read_directory_file_background
    }
    lappend options { color
	"File Foreground Color"
	read_directory_file_foreground
    }

    lappend options { integer
	"Include Subdirectories"
	read_directory_depth
    }

    set dialog(title) "Read Directory Options"
    set dialog(name) read_directory_options

    GT::simple_dialog $editor dialog $options read_directory

    return
}




##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
