#
# utility.tcl
#
# This module implements several utilities for the editor.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/utility.tcl,v $
# $Author: himsolt $
# $Revision: 1.6 $
# $Date: 1999/03/05 20:40:28 $
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
	max_string_length_in_array \
	pop \
	push \
	node_array \
	has_package \
	load_package \
	pset \
	lempty \
	luniq \
	lcontains \
	streq \
	sort_nodes
}


##########################################
#
# Parallel set
#
##########################################

proc GT::pset { a b } {
    uplevel 1 "foreach \{ $a \} \{ $b \} {}"
}

##########################################
#
# List utilities
#
##########################################

proc GT::ldelete { var_list args } {

    upvar $var_list list

    foreach pattern $args {
	set index [lsearch $list $pattern]
	if { $index != -1 } {
	    set list [lreplace $list $index $index]
	}
    }
}


proc GT::lempty { l } {
    return [expr [llength $l] == 0]
}

proc GT::luniq { list } {
    foreach elem $list {
	set a($elem) 1
    }
    
    return [array names a]
}

proc GT::lcontains { list item } {
    return [expr [lsearch $list $item] != -1 ]
}

##########################################
#
# String utilities
#
##########################################

proc GT::streq { s1 s2 } {
    return [expr [string compare $s1 $s2] == 0]
}

##########################################
#
# GT::max_string_length_in_array
#
##########################################

proc GT::max_string_length_in_array { arrayName } {

    upvar $arrayName array

    set max 0
    foreach name [array names array] {
	set length [string length $array($name)]
	if { $length > $max } {
	    set max $length
	}
    }

    return $max
}


##########################################
#
# Utility procs for lists/stacks
#
##########################################


proc GT::pop { var_list } {

    upvar $var_list list
    set first [lindex $list 0]
    set list [lreplace $list 0 0]
    return $first
}


proc GT::push { var_list element } {
    upvar $var_list list
    set list [linsert $list 0 $element]
}



##########################################
#
# Simulate a LEDA type node_array
#
##########################################


proc GT::node_array { name graph {init 0} } {
    upvar $name a
    foreach node [$graph nodes] {
	set a($node) $init
    }
}


##########################################
#
# GT::has_package
#
##########################################


proc GT::has_package {names} {

    set packages [package names]
    foreach package $names {
	if {[lsearch $packages $package] >= 0} {
	    return 1
	}
    }

    return 0
}


proc GT::load_package {names} {

    global GT

    set packages [package names]
    foreach package $names {

	set already_loaded 0
	foreach loaded [info loaded] {
	    if {[lindex $loaded 1] == $package} {
		set already_loaded 1
		break
	    }
	}
	
	if {$already_loaded == 0} {
	    
	    set shared_lib \
		lib[string tolower $package][info sharedlibextension]
	    set returncode [catch {
		load [file join $GT(graphscript_dir) $shared_lib]
	    } message]

	    if {$returncode != 0} {
		return 1
	    }
	}
    }

    return 0
}


##########################################
#
# GT::sort_nodes -horizonal|-vertical|-circular nodes 
#
##########################################


proc GT::sort_nodes {graph command nodes {center_x 0} {center_y 0}} {

    if {$nodes == {}} {
	return {}
    }

    switch -- $command {
	-horizontal {
	    foreach node $nodes {
		lappend to_sort [list $node [$graph get $node -x]]
	    }
	}
	-vertical {
	    foreach node $nodes {
		lappend to_sort [list $node [$graph get $node -y]]
	    }
	}
	-circular {
	    foreach node $nodes {
		set x [$graph get $node -x]
		set y [$graph get $node -y]
		set angle [expr atan2 ($x-$center_x, $y-$center_y)]
		lappend to_sort [list $node $angle]
	    }
	}
	default {
	    error "Unknown switch $command"
	}
    }

    set sorted [lsort -real -index 1 $to_sort]
    
    foreach element $sorted {
	lappend sorted_nodes [lindex $element 0]
    }

    return $sorted_nodes

}

##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
