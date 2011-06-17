
proc hierarchy_create_drawing_canvas { editor } {

    global GT GT_options

    frame $editor.drawing

	

    #
    # Create the canvas
    #
    set scrReg [list $GT_options(canvas_minx) $GT_options(canvas_miny) 10000  10000]

   #tk_messageBox -message  $scrReg

    canvas $editor.drawing.canvas \
	   -bg $GT_options(canvas_bg) \
	   -scrollregion $scrReg \
	   -xscrollcommand "$editor.drawing.hscrollbar set" \
	   -yscrollcommand "$editor.drawing.vscrollbar set" 


    #
    # individual look for windows & unix
    #
    
    global tcl_platform
    if { $tcl_platform(platform) == "windows" } {
	
	$editor.drawing configure \
	    -relief sunken -bd 2

	$editor.drawing.canvas configure \
	    -highlightthickness 0
	
    }    
    # The variable GT($editor,canvas) is the official access to
    # the canvas

    set GT($editor,canvas) $editor.drawing.canvas

    #
    # Create the scrollbars
    #

    scrollbar $editor.drawing.vscrollbar \
	-orient vertical \
	-command "$editor.drawing.canvas yview" 
	
	set GT($editor,vscroll) $editor.drawing.vscrollbar


    scrollbar $editor.drawing.hscrollbar \
	-orient horizontal \
	-command "$editor.drawing.canvas xview" 
	
	
    set GT($editor,hscroll) $editor.drawing.hscrollbar

    
    grid $editor.drawing.canvas -row 0 -column 0 -sticky news
    grid $editor.drawing.vscrollbar -row 0 -column 1 -sticky ns
    grid $editor.drawing.hscrollbar -row 20 -column 0 -sticky ew
    #grid $editor.drawing.pad -row 1 -column 1 -sticky {}

    # Dot know why, but the next two lines are needed to reize
    # the window properly ...
    grid rowconfigure $editor.drawing 0 -weight 1
    grid columnconfigure $editor.drawing 0 -weight 1



    $editor.drawing.canvas xview moveto 0
    $editor.drawing.canvas yview moveto 0
    # focus $editor.drawing.canvas

    return $editor.drawing
}



hierarchy_create_drawing_canvas $main
place $main.drawing -x 1 -y 1 -width 500 -height 600 
place $main.drawing.canvas -x 1 -y 1 -width 300 -height 400
set GT($main,graph) [graph]
$GT($main,graph) canvas $GT($main,canvas)
GT::init_selection $main
set GT($main,Menu) [menu $GT($main,canvas).menu -tearoff 0]
bind $GT($main,canvas) <Button-1> "ProcAction press %x %y 0"
bind $GT($main,canvas) <Control-Button-1> "ProcAction press %x %y 1"
bind $GT($main,canvas) <Button1-Motion> "ProcAction motion %x %y 0"
bind $GT($main,canvas) <Control-Button1-Motion> "ProcAction motion %x %y 1"
bind $GT($main,canvas) <Button1-ButtonRelease> "ProcAction release %x %y 0"
bind $GT($main,canvas) <Control-Button1-ButtonRelease> "ProcAction release %x %y 1"
bind $GT($main,canvas) <Button-3> "ShowContextMenu %X %Y %x %y"



proc SaveGraph {} {
    global GT  main
    set n 0
    set Buffer ""
    set graph $GT($main,graph)

    foreach node [$graph nodes] {
        set Label [$graph get $node -label]
        set x  [$graph get $node -x]
        set y  [$graph get $node -y]
        set x_rnd  [expr round($x)]
        set y_rnd  [expr round($y)]
        set s "сгек = $Label $y_rnd $x_rnd"
        set Buffer [concat $Buffer $s "|"]
    }

   foreach edge [$graph edges] {
        set s [$graph get [$graph get $edge -source] -label]
        set t [$graph get [$graph get $edge -target] -label]
        set q "бшье = $s $t"
        set Buffer [concat $Buffer $q "|"]
   }
  
   SaveToRossdev  $Buffer  $main

}

proc AddNode {} {

 global GT main GT_event
 #set graph 
 set graph $GT($main,graph)
 set Nodes ""

 foreach node [$graph nodes] {
        set Nodes [concat $Nodes [$graph get $node -label]]    
 }

 set NewNode [GetNewNode $Nodes $main]

 if {$NewNode != ""} {

   set n [$GT($main,graph) create node]

   $graph set $n -x $GT_event(canvasx) -y $GT_event(canvasy) -label $NewNode
  
   $GT($main,graph) draw
 }

}

proc ShowContextMenu {x y canv_x canv_y} {
 global GT_event
 global GT_selection
 global GT main
 set graph $GT($main,graph)
 $GT($main,Menu) delete 0 5
 set GT_event(canvasx) $canv_x
 set GT_event(canvasy) $canv_y

 
 set selected_count_node  [llength $GT_selection($main,selected,node)]
 set selected_count_edge  [llength $GT_selection($main,selected,edge)]

 if  {$selected_count_node > 0}  {
   $GT($main,Menu) add command -label "Delete selected node" -command "DelSelected node"
 }

 if  {$selected_count_edge > 0}  {
   $GT($main,Menu) add command -label "Delete selected edge" -command "DelSelected edge"
 }

 $GT($main,Menu) add command -label "Save" -command SaveGraph
 #$Menu add command -label "View" -command "tk_messageBox -message $main"
 $GT($main,Menu) add command -label "Add node" -command "AddNode"
 $GT($main,Menu) add command -label "Count of Nodes" -command {tk_messageBox -message [llength [$GT($main,graph) nodes]]}
 $GT($main,Menu) add command -label "Count of Edges" -command {tk_messageBox -message [llength [$GT($main,graph) edges]]}
 
 tk_popup $GT($main,Menu) $x $y
}


proc ProcAction {action x y ctrl} {
    global GT_options GT_event GT_selection GT
    global main MoveTick

    set canvas $GT($main,canvas)
    set editor $main
    set graph $GT($main,graph)
    set GT_event(canvasx) [$canvas canvasx $x]
	set GT_event(canvasy) [$canvas canvasy $y]
	#tk_messageBox -message $GT_event(canvasy)
    set GT_event(editor) $main
    set GT_event(W) $GT($main,canvas)
    set GT_event(graph)  $GT($main,graph)

    if {$action == "motion" && [info exists $GT_event(did_move_node)] == 1 && $GT_event(did_move_node) == 1 } {
        set node $GT_event(move_node_start_object) 
    } else {
      $canvas addtag GT::closest overlapping \
	[expr [$graph translate $canvas -x $GT_event(canvasx)] - $GT_options(overlap_gap)] \
	[expr [$graph translate $canvas -y $GT_event(canvasy)] - $GT_options(overlap_gap)] \
	[expr [$graph translate $canvas -x $GT_event(canvasx)] + $GT_options(overlap_gap)] \
	[expr [$graph translate $canvas -y $GT_event(canvasy)] + $GT_options(overlap_gap)]
      set node [GT::find_object $editor node GT::closest]
      $canvas dtag GT::closest
    }

    if { $node != "" }  {

     
        if {$action == "press"} {

          if {$ctrl == 1} {

            GT::ev_create_edge_start

          } else {
            GT::select $main select $node
            GT::ev_move_node_start  $GT($main,graph)  node
          }
        } 

        if {$action == "motion"} {
           if {$ctrl == 0} { 
            GT::ev_move_node_motion 
           } else {
            GT::ev_create_edge_motion
          }
        }


        if {$action == "release"} {
           if {[info exists $GT_event(did_move_node)] == 1 && $GT_event(did_move_node) == 1} {
             GT::ev_move_node_end 
           }
           if {$ctrl == 1} {
            GT::ev_create_edge_end
           }
        }
    } else {

         $canvas addtag GT::closest overlapping \
 	     [expr [$graph translate $canvas -x $GT_event(canvasx)] - $GT_options(overlap_gap)] \
	     [expr [$graph translate $canvas -y $GT_event(canvasy)] - $GT_options(overlap_gap)] \
	     [expr [$graph translate $canvas -x $GT_event(canvasx)] + $GT_options(overlap_gap)] \
	     [expr [$graph translate $canvas -y $GT_event(canvasy)] + $GT_options(overlap_gap)]


         set edge [GT::find_object $editor edge GT::closest]

         if { $edge != "" }  {

             if {$action == "press"} {
              GT::select $main select $edge
             } 

             $canvas dtag GT::closest
          }

	      if {$action == "motion"} {

             if [info exists GT_event(create_edge_source_node)] {
             
                GT::ev_create_edge_motion 
             }
  
          }

       
    }

}

