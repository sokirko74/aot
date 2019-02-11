
load   $env(GRAPHLET_DIR)/lib/graphlet.dll

package require Gt_lsd
package require Gt_lsd_springembedder_rf

set libpath $env(GRAPHLET_DIR)/lib/graphscript

source $libpath/ross/selection.tcl
source $libpath/ross/utility.tcl
source $libpath/ross/edit_mode.tcl
source $libpath/ross/create_mode.tcl
source $libpath/ross/canvas.tcl
source $libpath/menu/arrange.tcl
set   GT_options(canvas_maxy) 1600
set   GT_options(canvas_maxx) 1600


set GT_options(overlap_gap)  5
set GT_event(canvasx) 0
set GT_event(canvasy) 0
set GT_event(start_x) 0
set GT_event(start_y) 0
set GT_event(last_x)  $GT_event(start_x)
set GT_event(last_y)  $GT_event(start_y)
set GT_event(move_node_start_object) {}
set GT_event(did_move_node) 0









proc DelSelected {type} {
    global GT main GT_selection
    set graph $GT($main,graph)
    $graph delete $GT_selection($main,selected,$type)
    set GT_selection($main,selected,$type) {}
    $graph draw
}













proc GT::action_file_save_as  { editor  } {

    global GT GT_options

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


proc GT::load_graph { editor filename} {

    global GT GT_options
    set graph $GT($editor,graph)
	#tk_messageBox -message  load_graph

    set code [catch {$graph load -file $filename} error_message]
    if { $code != 0 } {
	tk_messageBox -message "Error loading $filename: $error_message"
	return 1
    }

    return 0
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

    #tk_messageBox  -message drawing...
	$GT($main,graph) draw 
}

