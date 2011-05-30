#
# developer.tcl
#
# This module implements the "Dev" (Developer) menu.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/developer.tcl,v $
# $Author: himsolt $
# $Revision: 1.10 $
# $Date: 1999/07/25 10:48:21 $
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
	create_Developer_menu \
	action_rebuild_index \
	action_show_console \
	action_developer_mail_developers \
	action_emergency_exit \
	action_remove_developer_menu
}


proc GT::create_Developer_menu { editor menu } {
    
    global GT GT_options
    global tcl_platform

    $menu configure \
	-tearoff $GT_options(expert) \
	-title Developer

    GT::add_menu_command $editor $menu {
	"Show ^Console" show_console "" 5
    }

    $menu add separator

#     GT::add_menu_command $editor $menu {
# 	"Rebuild ^Index (does not work)" rebuild_index "" disabled
#     }

    $menu add command \
	-label "Show System Errors" \
	-command [list GT::display_messages_dialog $editor \
		      errorInfo \
		      "Error Messages"] \
	-underline 5

    $menu add separator
    
    $menu add checkbutton \
	-label "Debug Graphics" \
	-variable GT_options(debug_graphics)
    $menu add command \
	-label "Show Graphics Commands" \
	-command [list GT::display_messages_dialog $editor \
		      GT(debug_graphics) \
		      "Graphics Commands"] \
	-underline 5


    $menu add separator

    $menu add checkbutton \
	-label "Debug" \
	-variable GT_options(debug)
    $menu add command \
	-label "Show Graphlet Error Messages" \
	-command [list GT::display_messages_dialog $editor \
		      GT(error_messages) \
		      "Graphlet Error Messages"]

    if {$tcl_platform(platform) == "windows"} {
	$menu add separator
	GT::add_menu_command $editor $menu {
	    "Mail Developers" mail_developers {} 0
	}
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"Remove this menu" remove_developer_menu
    }

    $menu add cascade -label "Emergency Exit" \
	-menu $menu.emergency_exit
    menu $menu.emergency_exit \
	-tearoff 0

    GT::add_menu_command $editor $menu.emergency_exit {
	"Emergency Exit" emergency_exit
    }

}


###########################################
#
# Actions
#
###########################################


proc GT::action_show_console "Console" {
    global GT
    set ::TKCON(font) {Arial 24}
    uplevel #0 eval {source [file join $GT(graphscript_dir) contrib tkcon.tcl]}
    tkcon attach Main
}


proc GT::action_rebuild_index {editor} {

    global GT

    set saved_cursor [$editor cget -cursor]
    $editor configure -cursor watch
    update

    file delete [file join $GT(graphscript_dir) pkgIndex.tcl]
    source [file join $GT(graphscript_dir) make_index]
    source [file join $GT(graphscript_dir) pkgIndex.tcl]

    $editor configure -cursor $saved_cursor
}

 

proc GT::action_mail_developers { editor } {

    global GT
    GT::display_mail_developers_dialog {}
}


proc GT::action_emergency_exit editor {
    exit
}

proc GT::action_remove_developer_menu editor {

    global GT_options

    set GT_options(show_developer_menu) 0
    for {set index 0} {$index < [$editor.menubar index end]} {incr index} {
	if {[$editor.menubar type $index] == "cascade"} {
	    if {[$editor.menubar entrycget $index -label] == "Dev"} {
		$editor.menubar delete $index
	    }
	}
    }
    destroy $editor.menubar.developer
}


##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
