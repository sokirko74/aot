#
# init.tcl
#
# The description of filename goes HERE.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/init.tcl,v $
# $Author: himsolt $
# $Revision: 1.9 $
# $Date: 1999/03/05 20:40:12 $
# $Locker:  $
# $State: Exp $
#
#------------------------------------------
#
# (C) University of Passau 1995-1999, graphlet Project
#

proc test_rebuild_pkgIndex_if_neccessary { {dir .} } {

    set tclfiles [glob -nocomplain [file join $dir *.tcl]]
    set self [lsearch $tclfiles init.tcl]
    set tclfiles_without_self [concat \
				   [lrange $tclfiles 0 [expr $self-1]] \
				   [lrange $tclfiles [expr $self+1] end]]

    set pkgIndex_needs_update 0
    if [file exists [file join $dir pkgIndex.tcl]] {
	set mtime_index [file mtime [file join $dir pkgIndex.tcl]]
	foreach file $tclfiles_without_self {
	    if {[file mtime [file join $dir $file]] > $mtime_index} {
		return 1
	    }
	}
    } else {
	return 1
    }

    return 0
}

# if {[test_rebuild_pkgIndex_if_neccessary $GT(graphscript_dir)] == 1} {
#     source [file join $GT(graphscript_dir) make_index]
# }

set GT(version) $GT(major_version).$GT(minor_version).$GT(mini_version)

global GT GT_options

set GT(copyright) {
    {Graphlet Copyright (c) Universität Passau 1995-1999.}
    {GTL Copyright (c) Universität Passau 1998.}
    {GraphEd Copyright (c) Universität Passau 1986-1995.}
    {Tcl/Tk Copyright (c) 1991-1994 The Regents of the University of California.}
    {Tcl/Tk Copyright (c) 1994-1997 Sun Microsystems, Inc.}
    {Tcl/Tk Copyright (c) 1998-1999 by Scriptics Corporation.}
}

#
# initialize variables which need to be set
#

set GT(initialized) 0
set GT(graphlet_initialized) 0

set GT(image_dir) [file join $GT(graphscript_dir) images]
set GT(bitmap_dir) [file join $GT(graphscript_dir) bitmaps]

set GT(canvas_types) {
    arc bitmap image line oval polygon rectangle text
}

global GT_selection
set GT_selection(types) {node edge}

source [file join $GT(graphscript_dir) init options.tcl]
GT_init_options
source [file join $GT(graphscript_dir) init postscript.tcl]
GT_init_postscript_options
source [file join $GT(graphscript_dir) init action.tcl]
GT_init_actions

global env
if [info exists env(HOME)] {
    if [file readable [file join $env(HOME) .graphscript]]  {
	namespace eval :: [source $env(HOME)/.graphscript]
    }
} else {
    if [file readable graphscript.ini] {
	namespace eval :: [source graphscript.ini]
    }	
}


if { [info command tk] != {} } {
    set GT(no_window_system) 1
    tk appname Graphlet
} else {
    set GT(no_window_system) 0
}

#
# Load Initialization files for the graphlet editor
#

#
# If ~/.graphlet is not a directory, make it one.
#

global errorInfo
set init_dir $GT_options(init_dir)
if { [file readable $init_dir] && ![file isdirectory $init_dir] } {

    # create ~/.graphlet_tmp
    # move ~/.graphlet ~/.graphlet_tmp/init.tcl
    # move ~/.graphlet_tmp ~/.graphlet
    # move ~/.graphlet_options ~/.graphlet/options.tcl
    
    set try [catch {

	file mkdir ${init_dir}_tmp

	if [info exists env(HOME)] {
	    if [file exists [file join $env(HOME) .graphlet]] {
		file rename \
		    [file join $env(HOME) .graphlet] \
		    [file join ${init_dir}_tmp init.tcl]
	    }
	} else {
	    if [info file exists graphlet.ini] {
		file rename \
		    graphlet.ini \
		    [file join ${init_dir}_tmp init.tcl]
	    }
	}

	file rename ${init_dir}_tmp $init_dir

	if [file exists [file join $env(HOME) .graphlet_options]] {
	    file rename [file join $env(HOME) .graphlet_options]
	}

    } errormsg]
    if { $try != 0 } {
	error $errormsg $errorInfo
    }

    # puts stdout "Created $init_dir and converted old files."

} elseif ![file exists $init_dir] {
    
    # create ~/.graphlet if it does not exist
    
    set try [catch {file mkdir $init_dir } error_msg]
    if { $try != 0 } {
	error $errormsg $errorInfo
    }
}


#
# End of initialization
#

set GT(initialized) 1

##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
