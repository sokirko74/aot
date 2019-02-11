#
# help.tcl
#
# This module implements the "Help" menu.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/help.tcl,v $
# $Author: himsolt $
# $Revision: 1.9 $
# $Date: 1999/03/05 20:42:06 $
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
	create_Help_menu \
	action_help_about \
	action_help_www \
	action_help_copyright \
	action_help_acknowledgements \
	action_show_configuration
}

proc GT::create_Help_menu { editor menu } {

    global GT_options

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Help

    GT::add_menu_command $editor $menu {
	"^WWW Page" help_www {}
    }

#     GT::add_menu_command $editor $menu {
# 	"Acknowledgements" help_acknowledgements {} 0
#     }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"^About Graphlet" help_about {}
    }

    GT::add_menu_command $editor $menu {
	"^Copyright" help_copyright {}
    }

    GT::add_menu_command $editor $menu {
	"^Show Configuration" show_configuration {}
    }

    global tcl_platform
    if {$tcl_platform(platform) == "windows"} {
	$menu add cascade -label "Emergency Exit" \
	    -menu $menu.emergency_exit
	menu $menu.emergency_exit \
	    -tearoff 0

	GT::add_menu_command $editor $menu.emergency_exit {
	    "^Emergency Exit" emergency_exit
	}
    }
}


###########################################
#
# Actions
#
###########################################


proc GT::action_help_about { editor  } {

    GT::show_about_graphlet_dialog $editor
}


proc GT::action_help_www { editor  } {

    global GT_options
    GT::browse_url "$GT_options(www_address)" new-window
}


proc GT::action_help_copyright { editor } {

    global GT
    GT::display_file_dialog $editor \
	$GT(graphlet_dir)/lib/graphlet/etc/COPYRIGHT
}


proc GT::action_help_acknowledgements { editor } {

    global GT
    GT::display_file_dialog $editor \
	$GT(graphlet_dir)/lib/graphlet/etc/ACKNOWLEDGEMENTS
}


proc GT::action_show_configuration { editor } {

    global GT
    GT::display_configuration_dialog
}



##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
