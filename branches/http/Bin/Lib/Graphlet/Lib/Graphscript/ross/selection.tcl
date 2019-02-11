#
# selection.tcl
#
# The description of filename goes HERE.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/selection.tcl,v $
# $Author: himsolt $
# $Revision: 1.8 $
# $Date: 1999/03/05 20:40:23 $
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
	init_selection \
	select \
	excursion
}


proc GT::init_selection editor {
    
    global GT_selection
    
    set GT_selection($editor,selected,node) {}
    set GT_selection($editor,selected,edge) {}
}




##########################################
#
# Selection management - preliminary
#
##########################################


proc GT::select { editor args } {

    global GT GT_options GT_selection
    
    if [info exists GT($editor,canvas)] {
	set canvas $GT($editor,canvas)
    }
    set graph $GT($editor,graph)

    set operation select
    set selection selected

    set arglength [llength $args]
    for { set argc 0 } { $argc < $arglength } { incr argc } {
	set arg [lindex $args $argc]
	switch -- $arg {
	    select {
		set operation select
	    }
	    add {
		set operation add
	    }
	    remove {
		set operation remove
	    }
	    delete {
		set operation remove
	    }
	    toggle {
		set operation toggle
	    }
	    selection {
		foreach type $GT_selection(types) {
		    if { $GT_selection($editor,$selection,$type) != {} } {
			if [info exists objects($type)] {
			    set objects($type) \
				[concat \
				     $objects($type) \
				     $GT_selection($editor,$selection,$type)]
			} else {
			    set objects($type) \
				$GT_selection($editor,$selection,$type)
			}
		    }
		}
	    }
	    all {
		set objects(node) [$graph nodes]
		set objects(edge) [$graph edges]
	    }
	    nodes {
		set objects(node) [$graph nodes]
	    }
	    edges {
		set objects(edge) [$graph edges]
	    }
	    default {
		foreach list_of_objects [lindex $args $argc] {
		    foreach object $list_of_objects {
			switch [$graph typeof -short $object] {
			    node { lappend objects(node) $object }
			    edge { lappend objects(edge) $object }
			}
		    }
		}
	    }
	}
    }
    
    #
    # Look at each individial object
    #


    switch -regexp $operation {
	select {
	    foreach type $GT_selection(types) {
		if [info exists objects($type)] {
		    foreach object $GT_selection($editor,$selection,$type) {
			if { [lsearch $objects($type) $object] == -1 } {
			    lappend to_remove($type) $object
			}
		    }
		} else {
		    set to_remove($type) \
			$GT_selection($editor,$selection,$type)
		}
	    }
	    foreach type [array names objects] {
		foreach object $objects($type) {
		    if { [lsearch \
			      $GT_selection($editor,$selection,$type) \
			      $object] == -1 } {
			lappend to_add($type) $object
		    }
		}
	    }
	}
	add {
	    foreach type [array names objects] {
		foreach object $objects($type) {
		    if { [lsearch \
			      $GT_selection($editor,$selection,$type) \
			      $object] == -1 } {
			lappend to_add($type) $object
		    }
		}
	    }
	}
	remove {
	    foreach type [array names objects] {
		foreach object $objects($type) {
		    if { [lsearch \
			      $GT_selection($editor,$selection,$type) \
			      $object] != -1 } {
			lappend to_remove($type) $object
		    }
		}
	    }
	}
	toggle {
	    foreach type [array names objects] {
		foreach object $objects($type) {
		    if { [lsearch \
			      $GT_selection($editor,$selection,$type) \
			      $object] == -1 } {
			lappend to_add($type) $object
		    } else {
			lappend to_remove($type) $object
		    }
		}
	    }
	}
    }

    
    foreach type [array names to_remove] {
	
	if [info exists canvas] {
	    $graph mark delete $canvas $selection $to_remove($type)
	}

	eval ldelete GT_selection($editor,$selection,$type) $to_remove($type)

	if [info exists canvas] {
	    if {[llength $GT_selection($editor,$selection,$type)] == 1 &&
		![info exists to_add($type)]
	    } {
		$graph mark delete $canvas \
		    $selection $GT_selection($editor,$selection,$type)
		$graph mark create -type blocks \
		    $canvas $selection $GT_selection($editor,$selection,$type)
	    }
	}
    }

    foreach type [array names to_add] {

	if [info exists canvas] {
	    if {[llength $to_add($type)] +
		[llength $GT_selection($editor,$selection,$type)] > 1
	    } {
		if {[llength $GT_selection($editor,$selection,$type)] == 1} {
		    $graph mark delete \
			$canvas $selection \
			$GT_selection($editor,$selection,$type)
		}
		$graph mark create -type frame \
		    $canvas $selection \
		    [concat \
			 $to_add($type) \
			 $GT_selection($editor,$selection,$type)\
			]
	    } else {
		$graph mark create -type blocks \
		    $canvas $selection $to_add($type)
	    }
	}

	eval lappend GT_selection($editor,$selection,$type) $to_add($type)
    }
}


proc GT::excursion { editor args } {

    global GT GT_selection
    set graph $GT($editor,graph)
    
    set short 0
    set find 1
    set check_selection 1
    set name ""
    set adapt 1

    set argc [llength $args]
    for {set i 0} {$i < $argc} {incr i} {
	set arg [lindex $args $i]
	switch -- $arg {
	    -short {
		set short [lindex $args [incr i]]
	    }
	    -long {
		set short [expr ![lindex $args [incr i]]]
	    }
	    -name {
		set name [lindex $args [incr i]]
	    }
	    -check_selection {
		set check_selection [lindex $args [incr i]]
	    }
	    -selection_is_save {
		set check_selection 0
	    }
	    -find {
		set find [lindex $args [incr i]]
	    }
	    -command {
		set command [lindex $args [incr i]]
	    }
	    -adapt {
		set adapt [lindex $args [incr i]]
	    }
	    default {
		set command $arg
	    }
	}
    }

    
    set error [catch {
	uplevel $command
    } error_message]

    if $error {
	tk_dialog \
	    $editor.error $name $error_message error 0 "Ok"
    }

    if {$check_selection} {
	foreach type $GT_selection(types) {
	    if [info exists GT_selection($editor,selected,$type)] {
		foreach object $GT_selection($editor,selected,$type) {
		    if { [$graph typeof $object] == {} } {
			ldelete GT_selection($editor,selected,$type) $object
		    }
		}
	    }
	}
    }

    
	$editor configure -cursor $saved_cursor
}



##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
