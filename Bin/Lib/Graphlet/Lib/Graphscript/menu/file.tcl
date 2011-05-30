#
# file.tcl
#
# This module implements the "File" menu.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/menu/file.tcl,v $
# $Author: sabine $
# $Revision: 1.16 $
# $Date: 1999/05/11 08:00:53 $
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
	create_File_menu \
	create_Export_menu \
	save_in_graphed_format \
	xfig_write_label \
	save_in_xfig_format \
	action_file_new \
	action_file_view \
	action_file_open \
	action_file_reload \
	action_file_save \
	action_file_save_as \
	action_file_save_in_graphed_format \
	action_file_save_in_xfig_format \
	action_file_postscript \
	action_file_postscript_options \
	action_file_close \
	action_file_exit
}


##########################################
#
# Initialize menu
#
##########################################


proc GT::create_File_menu { editor menu } {

    global GT_menu
    global GT_options

    $menu configure \
	-tearoff $GT_options(expert) \
	-title File

    GT::add_menu_command $editor $menu {
	"^New" file_new "Ctrl+N"
    }

    GT::add_menu_command $editor $menu {
	"New ^View" file_new_view ""
    }

    GT::add_menu_command $editor $menu {
	"^Open ..." file_open "Ctrl+O"
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"^Save" file_save "Ctrl+S"
    }

    GT::add_menu_command $editor $menu {
	"Save ^As ..." file_save_as
    }

    GT::add_submenu $editor $menu {
	"^Export"
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"^Reload" file_reload ""
    }

    $menu add separator

    GT::add_menu_command $editor $menu {
	"^Close" file_close ""
    }

    GT::add_menu_command $editor $menu {
	"E^xit" file_exit "Alt+X" 1
    }
}


proc GT::create_Export_menu {editor menu} {

    GT::add_menu_command $editor $menu {
	"^Encapsulated Postscript ..." file_postscript ""
    }

    GT::add_menu_command $editor $menu {
	"^Postscript Options ..." file_postscript_options ""
    }

    GT::add_menu_command $editor $menu {
	"^GraphEd ..." file_save_in_graphed_format ""
    }

    GT::add_menu_command $editor $menu {
	"^Xfig ..." file_save_in_xfig_format ""
    }
}

##########################################
#
# GT::save_in_graphed_format
#
##########################################


proc GT::save_in_graphed_format { editor filename } {

    global GT
    set graph $GT($editor,graph)

    if [catch { open $filename w } file] {

	GT::message $editor "Cannot open $filename: $file" error
	return 0

    } else {

	# 
	# Header of the graph
	#
	set label [$graph get -label]
	puts $file "GRAPH \"$label\" ="

	#
	# List of nodes (latest GraphEd version)
	#
	foreach node [$graph nodes] {

	    set label [$graph get $node -label]
	    set id [$graph get $node -id]
	    set x [$graph get $node -x]
	    set y [$graph get $node -y]
	    set w [$graph get $node -w]
	    set h [$graph get $node -h]

	    puts $file "$id {\$ NS [expr round($w)] [expr round($h)] NP [expr round($x)] [expr round($y)] \$} \"$label\";"

	}

	#
	# List of edges (latest GraphEd version)
	#
	foreach edge [$graph edges] {

	    set label [$graph get $edge -label]
	    set source [$graph get [$graph get $edge -source] -id]
	    set target [$graph get [$graph get $edge -target] -id]
	    set line [$graph get $edge -line]

	    set l {}
	    foreach i $line {
		lappend l [expr round($i)]
	    }
	    puts $file "$source $target {\$ EL $l \$} \"$label\";"

	}

	puts $file "END"

	close $file

    }
    return 1
}


##########################################
#
# GT::save_in_xfig_format
#
# Currently out of service. Needs a major overhaul.
#
##########################################


# proc GT::xfig_write_label { file label bbox scale }  {

#     if { $label != {} } {

# 	puts -nonewline $file "4 0 0 0 0 0 12 0.0000 0"
# 	set x0 [lindex $bbox 0]
# 	set y0 [lindex $bbox 3]
# 	set w [expr [lindex $bbox 2] - [lindex $bbox 0]]
# 	set h [expr [lindex $bbox 3] - [lindex $bbox 1]]
# 	puts -nonewline $file " [expr $scale * $w]"
# 	puts -nonewline $file " [expr $scale * $h]"
# 	puts -nonewline $file " [expr round($scale * $x0)]"
# 	puts -nonewline $file " [expr round($scale * $y0)]"
# 	puts $file " $label\\001"
#     }
# }

proc GT::xfig_label_box { graph node }  {
    canvas .xfigcanvas
    set bbox [.xfigcanvas bbox [.xfigcanvas create text 0 0 -text [$graph get $node -label]]]
    destroy .xfigcanvas
    return $bbox
}


proc GT::save_in_xfig_format { editor filename } {

    global GT
    set graph $GT($editor,graph)
    set canvas $GT($editor,canvas)

    if [catch { open $filename w } file] {

	GT::message $editor "Cannot open $filename: $file" error
	return 0

    } else {

	set scale [expr 1200.0 / 72.0]

	# 
	# Header of the graph
	#
	set label [$graph get -label]
	puts $file "#FIG 3.2"
	puts $file "Landscape"
	puts $file "Center"
	puts $file "Metric"
	puts $file "Letter"
	puts $file "100.00"
	puts $file "Single"
	puts $file "-2"
	puts $file "1200 2"

	#
	# List of nodes (latest Xfig version)
	#
	foreach node [$graph nodes] {

	    set typ [$graph get $node -type]
	    set x [expr round([$graph get $node -x] * $scale)]
	    set y [expr round([$graph get $node -y] * $scale)]
	    set w [expr round([$graph get $node -w] * $scale)]
	    set h [expr round([$graph get $node -h] * $scale)]
	    # if node is oval: oval in xfig; otherwise: box in xfig
	    if {$typ == "oval"} {
		set last_x [expr $x + $w/2]
		set last_y [expr $y - $h/2]
		set coords [list 1 1 0 1 0 7 0 0 -1 0.000 1 0.0000 $x $y [expr $w/2] [expr $h/2] $x $y $last_x $last_y]
		puts $file $coords 
	    } else {
		puts $file "2 2 0 1 0 7 0 0 -1 0.000 0 0 -1 0 0 5"


		set x_left [expr $x - $w/2]
		set x_right [expr $x + $w/2]
		set y_top [expr $y - $h/2]
		set y_bottom [expr $y + $h/2]

		# List of box point
		set coords [list $x_left $y_top $x_right $y_top $x_right $y_bottom $x_left $y_bottom $x_left $y_top]
		puts $file $coords
	    }

	    # Set label
	    set label [$graph get $node -label]
	    if { $label != {} } {
		set zeichen [string range $label 0 0]
		if { $zeichen == "\{" } {
		    set length [string length $label]
		    set length [expr $length - 2]
		    set label [string range $label 1 $length]
		} 
		# Position of the label
		set xcoord [expr round([$graph get $node label_graphics -x] *$scale)]
		set ycoord [expr round([$graph get $node label_graphics -y] *$scale)]
# 		set bbox [xfig_label_box $graph $node]
# 		puts "coords"
# 		puts $xcoord
# 		puts $ycoord
# 		puts "Rueckgabewert:"
# 		puts [lindex $bbox 0]
# 		puts [lindex $bbox 1]
# 		puts [lindex $bbox 2]
# 		puts [lindex $bbox 3]
# 		puts "Verschiebebetrag"
# 		puts [expr ([lindex $bbox 2] - [lindex $bbox 0])/2]
# 		puts [expr ([lindex $bbox 3] - [lindex $bbox 1])/2]
# 		set xleft [expr $xcoord - [expr round([expr ([lindex $bbox 2] - [lindex $bbox 0])/2*$scale])]]
# 		set ybottom [expr $ycoord + [expr round([expr ([lindex $bbox 3] - [lindex $bbox 1])/2*$scale])]]
# 		puts "verschoben:"
# 		puts $xleft
# 		puts $ybottom
		# Which font
		if {[$graph get $node label_graphics -font_size] != "0"} {
		    set size [$graph get $node label_graphics -font_size]
		} else {
		    set size 12
		}
		set myfontstyle [$graph get $node label_graphics -font_style]
		switch $myfontstyle {
		    roman {set style 1}
		    bold {set style 2}
		    italic {set style 3}
		    default {set style 0}
		}
		set justify [$graph get $node label_graphics -justify]
		switch  $justify {
		    center {set just 1}
		    right {set just 2}
		    default {set just 0}
		}

		#Sabine: 'just' verwenden (2. Variable), falls bbox funktioniert und die y-Verschiebung rausnehmen
		set coords [concat 4 1 0 0 0 $style $size 0.0000 0 0 0 $xcoord [expr $ycoord + $size/2*10] $label\\001]
#		set coords [concat 4 0 0 0 0 $style $size 0.0000 0 0 0 $xleft $ybottom $label\\001]
		puts $file $coords
	    }
	}
	# Determine if  the graph is directed
	set directed [$graph get -directed]
	foreach edge [$graph edges] {
	    set arrow [$graph get $edge -arrow]
	    set line [$graph get $edge -line]
	    if {$arrow == ""} {
		puts $file "2 1 0 1 -1 7 0 0 -1 0.000 0 0 -1 0 0 [expr [llength $line]/2]"		
	    } else {
		switch $arrow {
		    "first" {
			puts $file "2 1 0 1 -1 7 0 0 -1 0.000 0 0 -1 0 1 [expr [llength $line]/2]"
			puts $file "1 1 1.00 60.00 120.00"
		    }
		    "last" {
			puts $file "2 1 0 1 -1 7 0 0 -1 0.000 0 0 -1 1 0 [expr [llength $line]/2]"
			puts $file "1 1 1.00 60.00 120.00"
		    }
		    "both" {
			puts $file "2 1 0 1 -1 7 0 0 -1 0.000 0 0 -1 1 1 [expr [llength $line]/2]"
			puts $file "1 1 1.00 60.00 120.00"
			puts $file "1 1 1.00 60.00 120.00"
		    }
		    "none" {
			puts $file "2 1 0 1 -1 7 0 0 -1 0.000 0 0 -1 0 0 [expr [llength $line]/2]"
		    }
		}
	    }

	    foreach coord $line {
		puts -nonewline $file "[expr round($coord * $scale)] "
	    }
	    puts $file ""

	    set label [$graph get $edge -label]
	    if { $label != {} } {
		set zeichen [string range $label 0 0]
		if { $zeichen == "\{" } {
		    set length [string length $label]
		    set length [expr $length - 2]
		    set label [string range $label 1 $length]
		} 
		# Position of the label
		set xcoord [expr round([$graph get $edge label_graphics -x] *$scale)]
		set ycoord [expr round([$graph get $edge label_graphics -y] *$scale)]
			# Which font
		set size [$graph get $edge label_graphics -font_size]
		set myfontstyle [$graph get $edge label_graphics -font_style]
		switch $myfontstyle {
		    roman {set style 1}
		    bold {set style 2}
		    italic {set style 3}
		    default {set style 0}
		}
		set justify [$graph get $edge label_graphics -justify]
		switch  $justify {
		    center {set just 1}
		    right {set just 2}
		    default {set just 0}
		}

		#Sabine: 'just' verwenden (2. Variable), falls bbox funktioniert und die y-Verschiebung rausnehmen
		set coords [concat 4 1 0 0 0 $style $size 0.0000 0 0 0 $xcoord [expr $ycoord + $size/2*10] $label\\001]
		puts $file $coords
	    }
	}

	close $file

    }
    return 1
}


###########################################
#
# Actions
#
###########################################


proc GT::action_file_new  { editor  } {

    graphlet
}


proc GT::action_file_new_view  { editor  } {

    global GT
    graphlet -graph $GT($editor,graph)
}


proc GT::action_file_open  { editor {filename ""} } {

    global GT GT_options

    if { $filename == "" } {

	set filename [tk_getOpenFile \
			  -filetypes $GT_options(fileselector_gml_graphtypes) \
			  -parent $editor \
			  -title "Open File (GML)" \
			  -defaultextension $GT_options(fileselector_defaultextension)]

	if { $filename == "" } {
	    return
	}
    }

    GT::load_graph $editor $filename
}


proc GT::action_file_reload  { editor  } {

    global GT

    if { [info exists GT($editor,filename)] && $GT($editor,filename) != {} } {
	set query [tk_dialog \
		       .reloas_dialog \
		       "Reload Graph" \
		       "Throw away all changes and reload the graph ?" \
		       warning  \
		       0 \
		       "Reload" "Cancel" \
		      ]
	if { $query == 0 } {
	    GT::load_graph $editor $GT($editor,filename)
	}
    } else {
	GT::message $editor "No file loaded." warning
    }

    return
}


proc GT::action_file_save  { editor  } {

    global GT

    if {![info exists GT($editor,filename)] ||
	$GT($editor,filename) == "" } {
	return [GT::action_file_save_as $editor]
    } else {
	GT::save_graph $editor $GT($editor,filename)
	return $GT($editor,filename)
    }
}


proc GT::action_file_save_as  { editor  } {

    global GT GT_options
#     set filename [GT::fileselector \
# 		      -parent $editor \
# 		      -directory $GT_options(fileselector_directory) \
# 		      -center $GT_options(fileselector_center) \
# 		      -mode save \
# 		      -title "Save File (GML)" \
# 		      -types [list \
# 				  "Graphs (*.g*)" \
# 				  "Graphlet Graphs (*.gml)"\
# 				  "All Files (*)" \
# 				 ]]

    if [info exists GT($editor,filename)] {
	set initialfile $GT($editor,filename)
    } else {
	set initialfile {}
    }

    set filename [tk_getSaveFile \
		      -filetypes $GT_options(fileselector_gml_graphtypes) \
		      -parent $editor \
		      -title "Save File (GML)" \
		      -defaultextension $GT_options(fileselector_defaultextension) \
		      -initialfile $initialfile]
		      
    if { $filename != "" } {
	GT::save_graph $editor $filename
    }

    return $filename
}


proc GT::action_file_save_in_graphed_format  { editor  } {

    global GT GT_options

    if {[info exists GT($editor,filename)] && $GT($editor,filename) != {}} {
	set initialfile [file rootname $GT($editor,filename)].g
    } else {
	set initialfile {}
    }

    set filename [tk_getSaveFile \
		      -filetypes $GT_options(fileselector_graphed_graphtypes) \
		      -parent $editor \
		      -title "Save File (Graphlet)" \
		      -defaultextension .g \
		      -initialfile $initialfile]

    if { $filename != {} } {
	GT::save_in_graphed_format $editor $filename
    }

    return $filename
}


proc GT::action_file_save_in_xfig_format  { editor  } {

    global GT GT_options

    if {[info exists GT($editor,filename)] && $GT($editor,filename) != {}} {
	set initialfile [file rootname $GT($editor,filename)].fig
    } else {
	set initialfile {}
    }

    set filename [tk_getSaveFile \
		      -filetypes $GT_options(fileselector_fig_types) \
		      -parent $editor \
		      -title "Save File (XFIG)" \
		      -defaultextension .fig \
		      -initialfile $initialfile]
    
    if { $filename != {} } {
	GT::save_in_xfig_format $editor $filename
    }

    return $filename
}


proc GT::action_file_postscript  { editor  } {
    
    global GT GT_options

    if {[info exists GT($editor,filename)] && $GT($editor,filename) != {}} {
	set initialfile [file rootname $GT($editor,filename)].ps
    } else {
	set initialfile {}
    }

    set filename [tk_getSaveFile \
		      -filetypes $GT_options(fileselector_postscript_types) \
		      -parent $editor \
		      -title "Save File (Postscript)" \
		      -defaultextension .ps \
		      -initialfile $initialfile]

    if { $filename != {} } {
	GT::save_as_postscript $editor $filename
    }

    return $filename
}


proc GT::action_file_postscript_options { editor } {
    GT::show_postscript_options_dialog  $editor
}


proc GT::action_file_close  { editor  } {

    GT::close_frame $editor
}


proc GT::action_file_exit  { editor  } {

    GT::close_frame $editor exit
}




##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
