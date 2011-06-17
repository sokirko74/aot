source $env(GRAPHLET_DIR)/lib/graphscript/ross/options.tcl
source $env(GRAPHLET_DIR)/lib/graphscript/ross/canvas.tcl
source $env(GRAPHLET_DIR)/lib/graphscript/ross/init_gra.tcl
source $env(GRAPHLET_DIR)/lib/graphscript/ross/dialog.tcl


set GT_options(spring_fr_weighted) {10}
set GT_options(spring_fr_vibration) {10}
set GT_options(spring_fr_max_iter) {100}
set GT_options(spring_fr_edgelen) {400}
set GT_options(spring_fr_max_force) {200}

set GT_options(tree_tr_edge_connection_for_bend) {0}
set GT_options(tree_tr_orientation) {0}
#set GT_options(tree_tr_edge_connection_for_bend) {90}
set GT_options(tree_tr_vert_node_node) {60}
set GT_options(tree_tr_permutation) {0}
set GT_options(tree_tr_father_place) {200}
set GT_options(tree_tr_hor_node_node) {100}
set GT_options(tree_tr_routing) {0}
set GT_options(tree_tr_bend_reduction) {1}
set GT_options(tree_tr_leveling) {0}
set GT_options(tree_tr_channel_width) {3}
set GT_options(tree_tr_edge_connection) {70}
set GT_options(tree_tr_marked_root) {2}
set GT_options(tree_tr_node_edge) {150}

set GT_options(walker_Vertical) {64}
set GT_options(walker_Sibling) {100}
set GT_options(walker_Subtree) {32}

set GT($main,tree_variant_props) "no properties"
set globalAllLexVars {{test "РОСС:zzz" 1 }}
set  CurrentNumber 0 
set po  общ
set globalLastWasSeman 0
set LastSentence ""
set clauseVarNo -1
set LayoutType ""

proc sem_create_drawing_canvas { editor } {

    global GT GT_options

    frame $editor.drawing

	

    #
    # Create the canvas
    #
    set scrReg [list $GT_options(canvas_minx) $GT_options(canvas_miny) 4000  4000]


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

proc initialize_graph { main  } {
	global GT
	set GT($main,graph) [graph]
	set GT($main,seman_graph) [graph]
	set GT($main,clause_graph) [graph]
}


proc initialize_graphic { main } {
	global GT GT_options po clauseVarNo

    sem_create_drawing_canvas $main
	frame $main.controls
	place $main.controls -x 0 -y 0 -width 850 -height 50 

	frame $main.switches 
	place $main.switches -x 620 -y 50 -width 400 -height 400 

	initialize_graph $main
	$GT($main,graph) canvas $GT($main,canvas)

	GT::init_selection $main

	set  synanBtn [button $main.controls.synanBtn -text Synan -command onSynan]
	place $synanBtn -x 2 -y 4 -height 20

	set  buildBtn [button $main.controls.buildBtn -text Seman -command onBuild]
	place $buildBtn -x 50 -y 4 -height 20

	set  englishBtn [button $main.controls.englishBtn -text Transfer -command onEnglish -state normal]
	place $englishBtn -x 101 -y 4 -height 20

	set  SynthesisBtn [button $main.controls.synthesisBtn -text EngSynt -command onSynthesis -state normal]
	place $SynthesisBtn -x 157 -y 4 -height 20

	set  SaveSentenceBtn [button $main.controls.saveSentenceBtn -text Memo -command onSaveSentence -state normal]
	place $SaveSentenceBtn -x 2 -y 30 -height 20

	set  ClearMemoBtn [button $main.controls.clearMemoBtn -text Clear -command onClearMemo -state normal]
	place $ClearMemoBtn -x 55 -y 30 -height 20

	set  AnswerBtn [button $main.controls.answerBtn -text Answer -command onAnswer -state normal]
	place $AnswerBtn -x 101 -y 30 -height 20

	set  RusSynthesisBtn [button $main.controls.rusSynthesisBtn -text RusSynt -command onRusSynthesis -state normal]
	place $RusSynthesisBtn -x 157 -y 30 -height 20
	
	set  mainEntry [text $main.controls.mainEntry -relief sunken -bd 2 -setgrid 1 -height 3 -width 100]
	place $mainEntry -x 220 -y 4



	place [message $main.switches.panicTreeVariantCount -text "Panic Var.Count" -width 80 ] -x 0 -y 0
	set  panic_var_countEntry [entry $main.switches.panic_var_countEntry -width 5 ]
	$main.switches.panic_var_countEntry  insert 0 "2000" 
	place $panic_var_countEntry -x 90 -y 0

	place [message $main.switches.positionEntryText -text "Variant Number" -width 80 ] -x 0 -y 17

	set  positionEntry [entry $main.switches.positionEntry -width 3 ]
	place $positionEntry -x 90 -y 20

	place [message $main.switches.titleclauseVarNo -text "ClauseVariant Number" -width 120 ] -x 0 -y 40
	set clauseVarNo [entry $main.switches.clauseVarNo -width 3]
	$main.switches.clauseVarNo  insert 0 "-1" 
	place $clauseVarNo -x 120 -y 40


	place [label $main.switches.resultText -relief sunken -width 31 -height 17  -justify left  ] -x 7 -y 165

	# предметная область
	label $main.switches.po -text "ПO"
	place $main.switches.po -x 10 -y 60
	
	radiobutton $main.switches.po_common -text "общ" -variable po   -relief flat -value общ
	place $main.switches.po_common -x 10 -y 75

	radiobutton $main.switches.po_finance -text "фин" -variable po  -relief flat -value фин
	place $main.switches.po_finance -x 10 -y 95 

	radiobutton $main.switches.po_computer -text "комп" -variable po  -relief flat -value информ
	place $main.switches.po_computer -x 10 -y 115 


	# тип показываемого графа
	label $main.switches.graph_type -text "Граф"
	place $main.switches.graph_type -x 70 -y 60
	set graph_type main_graph_type
	radiobutton $main.switches.main_graph_type -text "основной" -variable graph_type   -relief flat \
		-value main_graph_type \
		-command {SetMainGraphType}
	place $main.switches.main_graph_type -x 70 -y 75

	radiobutton $main.switches.clause_graph_type -text "клаузы" -variable graph_type  -relief flat \
		-value clause_graph_type \
		-command {SetClauseGraphType}
	place $main.switches.clause_graph_type -x 70 -y 95 

	radiobutton $main.switches.lex_var_graph_type -text "лекс.вар." -variable graph_type  -relief flat \
		-value lex_var_graph_type \
		-command {SetLexVarType}
	place $main.switches.lex_var_graph_type -x 70 -y 115 


	set GT($main,Menu) [menu $GT($main,canvas).menu -tearoff 0]
	set GT($main,ControlsMenu) [menu $main.controls.menu -tearoff 0]

	bind $main.controls.mainEntry    <Button-3> "ShowContextMenuForControls %X %Y %x %y"
	bind $GT($main,canvas) <Button-1> "ProcActionForSemSemAn press %x %y 0"
	bind $GT($main,canvas) <Button-3> "ShowContextMenuForSemStruct %X %Y %x %y"
	bind $GT($main,canvas) <Button1-Motion> "ProcActionForSemSemAn motion %x %y 0"
	bind $GT($main,canvas) <Double-Button-1> "ShowArticleTrigger"
	bind $GT($main,canvas) <Button1-ButtonRelease> "ProcActionForSemSemAn release %x %y 0"
}

proc ResizeChildControls {} {

 global GT GT_options main 
 set main_width [winfo width $main]
 set main_height [winfo height $main]

 #switches
 set switches_x [expr $main_width-200]
 set switches_height [expr $main_height-60]
 set res_text_height [expr ($main_height)/(2)]
 place $main.switches -x $switches_x -y 50 -width 400 -height $switches_height 
 place $main.switches.resultText  -height $res_text_height 


 #canvas
 set canvas_width [expr $main_width-220]
 set canvas_height [expr $main_height-60]
 
 
 place $main.drawing -x 20 -y 55 -width $canvas_width -height $canvas_height

 #set canv_width  [expr $canvas_width+2900]
 #set canv_height [expr $canvas_height+2900]
 set canv_width  [expr $canvas_width]
 set canv_height [expr $canvas_height]


 place $GT($main,canvas) -x 0 -y 0 -width $canv_width -height $canv_height
};



proc GT::LSD_action_layout_tree { editor } {

global GT GT_options
set graph $GT($editor,graph)

  set new_graph [graph]
  $graph copynode [$graph nodes] $new_graph
  $new_graph configure -directed 0

  layout_extended_tr_tree $new_graph \
		    -leveling $GT_options(tree_tr_leveling) \
		    -orientation $GT_options(tree_tr_orientation) \
		    -direction $GT_options(tree_tr_direction) \
		    -routing $GT_options(tree_tr_routing) \
		    -father_place $GT_options(tree_tr_father_place) \
		    -permutation $GT_options(tree_tr_permutation) \
		    -father_son_dist $GT_options(tree_tr_vert_node_node) \
		    -son_son_dist $GT_options(tree_tr_hor_node_node) \
		    -node_edge_dist $GT_options(tree_tr_node_edge) \
		    -channel_width $GT_options(tree_tr_channel_width) \
		    -edge_connection $GT_options(tree_tr_edge_connection) \
		    -bend_reduction $GT_options(tree_tr_bend_reduction) \
		    -edge_connection_for_bend $GT_options(tree_tr_edge_connection_for_bend) \
		    -marked_root $GT_options(tree_tr_marked_root) 

    set i 0
	set new_graph_nodes [$new_graph nodes]

    foreach node [$graph nodes] {
	 set new_node [lindex  $new_graph_nodes $i]
	 set x [$new_graph get $new_node -x]
	 set y [$new_graph get $new_node -y]
	 $graph set $node -x $x -y $y
	 incr i 1
	} 
}




proc GT::LSD_action_layout_spring_fr { editor } {
    
    global GT GT_options
    set graph $GT($editor,graph)

    layout_spring_fr $graph \
		-weighted $GT_options(spring_fr_weighted) \
		-maximal_force $GT_options(spring_fr_max_force) \
		-vibration $GT_options(spring_fr_vibration) \
		-maximal_iterations $GT_options(spring_fr_max_iter) \
		-edgelength $GT_options(spring_fr_edgelen)
}


proc GT::LSD_action_layout_walker_tree { editor } {

 global GT GT_options

 layout_tree_walker $GT($editor,graph) \
		-vertical_separation $GT_options(walker_Vertical) \
		-sibling_separation $GT_options(walker_Sibling) \
		-subtree_separation $GT_options(walker_Subtree) 


 layout_tree_walker $GT($editor,graph) \
		-vertical_separation $GT_options(walker_Vertical) \
		-sibling_separation $GT_options(walker_Sibling) \
		-subtree_separation $GT_options(walker_Subtree) 

 
}


proc SetLayout {} {
 global main GT  LayoutType

 set graph $GT($main,graph) 

 set edges [$graph edges]
 set nodes [$graph nodes]

 set edges_count [llength $edges]
 set nodes_count [llength $nodes]

 

 if {$edges_count > 0} {
        set val [expr $nodes_count - 1]
		set res 0
        if {$edges_count == $val } {

		  catch {
		     GT::LSD_action_layout_walker_tree $main
		     set res 1
		     set LayoutType "LSD_action_layout_walker_tree"
		  }
		          
          if {$res == 0} {
		    catch {
		       GT::LSD_action_layout_tree $main     
		       set res 1 
  		       set LayoutType "GT::LSD_action_layout_tree"
		    }
		  }

                  
          
		  foreach node [$graph nodes] {
                      set x  [$graph get $node -x]
			incr x 100
			$graph set $node -x $x
		  }
		
        
		}
		
	 
		if {$edges_count != $val || $res == 0} {


		  GT::LSD_action_layout_spring_fr $main
 	      

		}
		 
		 
 }
 		 
 if {$edges_count == 0} {
 
    set i 100
 
    foreach node [$graph nodes] {
	 $graph set $node -x 100 -y $i
	 incr i 25
	}
 }

}




proc change {type delta} {
  global GT GT_options main

  set graph $GT($main,graph)
  foreach node [$graph nodes] {
	set val [$graph get $node -$type]
	set val [expr $val + $delta]
    $graph set $node -$type $val
  }
} 


proc GT::LSD_action_layout_spring_fr { editor } {
    
    global GT GT_options
    set graph $GT($editor,graph)


	 layout_dag $graph \
		-vertical_distance $GT_options(sugiyama_vert_dist) \
		-horizontal_distance 100 \
		-it1 $GT_options(sugiyama_it1) \
		-it2 $GT_options(sugiyama_it2) \
		-arrange $GT_options(sugiyama_level_arrange) \
		-resolve_cycles $GT_options(sugiyama_resolve_cycles) \
		-reduce_crossings $GT_options(sugiyama_reduce_crossings) 

 
   
   
   set minx 200
   set miny 200
   set maxx  -200
   set maxy  -200
   
   

   foreach node [$graph nodes] {
        set Label [$graph get $node -label]
        set x  [$graph get $node -x]
        set y  [$graph get $node -y]
		if {$minx > $x} {
		  set minx $x
		}
		if {$miny > $y} {
		  set minx $y
		}
		if {$maxx < $x} {
		  set maxx $x
		}

		if {$maxy < $y} {
		  set maxy $y
		}
   }

   
   if {$minx < 100} {
    change  x [expr 100 - $minx]
   }

  if {$miny < 100} {
    change  y [expr 100 - $miny]
   }

  if {$maxy > 400} {
    change  y [expr $maxy - 400]
   }

  if {$maxx > 400} {
    change  x [expr $maxx - 400]
   }

}



proc onSynan {} {
 global main GT 

 set text [$main.controls.mainEntry get 0.0 1000.0]
 set clauseVarNo [$main.switches.clauseVarNo get]
 if {$clauseVarNo != -1} {
  set clauseVarNo [expr $clauseVarNo - 1]
 }

 CreateSynStr $main $text $clauseVarNo

 SetLayout 

 $GT($main,graph) draw 
  
}


proc onBuild {} {
 global main GT CurrentNumber  PositionVar po globalLastWasSeman graph_type globalAllLexVars LastSentence


 SetMainGraphType
 
 $GT($main,graph) delete nodes 

 

 set t $main.controls.mainEntry
  
 set text [$main.controls.mainEntry get 0.0 1000.0]


 set position [$main.switches.positionEntry get]
 set panic_var_count [$main.switches.panic_var_countEntry get]
 set clauseVarNo [$main.switches.clauseVarNo get]
 
 if {$clauseVarNo != -1} {
  set clauseVarNo [expr $clauseVarNo - 1]
 }
 
 if {$LastSentence != $text}	{
   set globalAllLexVars {}

 }
 FindSituations $main $text $CurrentNumber  0 $position $po $panic_var_count  $clauseVarNo $clauseVarNo $globalAllLexVars

 set graph_type main_graph_type
 $GT($main,canvas) configure -background  "white"

 set GT($main,main_graph) [graph]
 $GT($main,graph) copynode [$GT($main,graph) nodes] $GT($main,main_graph) 


 $GT($main,graph) draw  


 $main.controls.synthesisBtn configure  -state disabled
 $main.controls.englishBtn configure  -state normal
 
 
 set globalLastWasSeman 1

 
 GT::remove_bends $main

}

proc onEnglish {} {
 global main GT globalLastWasSeman

 SetClauseGraphType

 TranslateToEnglish $main  0
 $GT($main,canvas) configure -background  "white"



 $main.controls.englishBtn configure  -state normal
 $main.controls.synthesisBtn configure  -state normal

 $GT($main,graph) draw  

 set globalLastWasSeman 0
   
}




proc onSynthesis {} {
 global GT main globalLastWasSeman
 SetClauseGraphType
 BuildSentence $main
 $main.controls.englishBtn configure  -state normal
 $main.controls.synthesisBtn configure  -state normal
 set globalLastWasSeman 0
}

proc onRusSynthesis {} {
 global GT main
 SyntRusSentence $main
}


proc onSaveSentence {} {
 global GT main
 SaveSentence $main
}

proc onClearMemo {} {
 global GT main
 ClearMemo $main
}

proc onAnswer {} {
 global GT main
 AnswerBySavedSentences $main
}


proc ShowAttr {attr} { 

 global GT_selection
 global GT main

 set node [lindex $GT_selection($main,selected,node) 0]
 
 
 set graph $GT($main,graph)
 
 set Label [$graph get $node .$attr]

 tk_messageBox -message $Label
}


proc ShowAttrEdge {attr} { 

 global GT_selection
 global GT main


 set edge [lindex $GT_selection($main,selected,edge) 0]

 set graph $GT($main,graph)

  
 set label [$graph get $edge .$attr]


 tk_messageBox -message $label
}





proc ShowContextMenuForSemStruct {x y canv_x canv_y} {
 global GT_event
 global GT_selection
 global GT main globalCurrX globalCurrY 
 global graph_type 
 global tree_variant_props
 set graph $GT($main,graph)
 $GT($main,Menu) delete 0 30
 set GT_event(canvasx) $canv_x
 set GT_event(canvasy) $canv_y
 
 set selected_count_node  [llength $GT_selection($main,selected,node)]
 set selected_count_edge  [llength $GT_selection($main,selected,edge)]

 

 if {$graph_type == "main_graph_type"} {
		 if  {$selected_count_node > 0}  {
   
		   $GT($main,Menu) add command -label "Morphology" -command {ShowAttr morphology}
		   $GT($main,Menu) add command -label "Dictionary interpretation" -command {ShowAttr dict_interp}
		   $GT($main,Menu) add command -label "Valencies" -command {ShowAttr vals}
		   $GT($main,Menu) add command -label "Prepositions" -command {ShowAttr preps}
		   $GT($main,Menu) add command -label "Semantic Features" -command {ShowAttr sem_fets}
		   $GT($main,Menu) add command -label "Higher Concepts" -command {ShowAttr higher_concepts}
		   $GT($main,Menu) add command -label "Other properties" -command {ShowAttr props}
		 }


		 if  {$selected_count_edge > 0}  {
		   $GT($main,Menu) add command -label "Properties" -command {ShowAttrEdge props}
		   $GT($main,Menu) add command -label "Semantic Features" -command {ShowAttrEdge sem_fets}
		   $GT($main,Menu) add command -label "Lexical Features" -command {ShowAttrEdge lex_fets}
		 }

 
		 $GT($main,Menu) add command -label "Count of Nodes" -command {tk_messageBox -message [llength [$GT($main,graph) nodes]]}

		 $GT($main,Menu) add command -label "Count of Edges" -command {tk_messageBox -message [llength [$GT($main,graph) edges]]}

		 $GT($main,Menu) add command -label "Save Graph" -command {GT::action_file_save_as $main}

		 $GT($main,Menu) add command -label "Load Graph" -command {GT::action_file_open $main}

		 $GT($main,Menu) add command -label "Properties of Tree Structure" -command {tk_messageBox -message $GT($main,tree_variant_props)}

		 $GT($main,Menu) add command -label "Statistics" -command {tk_messageBox -message $GT($main,statistics)}

		 $GT($main,Menu) add command -label "Variant Statistics" -command {tk_messageBox -message $GT($main,variants_statistics)}
 
		 $GT($main,Menu) add command -label "Remove bends" -command {GT::remove_bends $main}

  } else {
   
    $GT($main,Menu) add command -label "Count of Nodes" -command {tk_messageBox -message [llength [$GT($main,graph) nodes]]}
	 if  {$selected_count_node > 0}  {
		   $GT($main,Menu) add command -label "Properties" -command {ShowAttr props}
	 }
   

  }


 tk_popup $GT($main,Menu) $x $y
}

proc  PasteClipboard1 {} {
 global GT main
 set clip selection get -selection CLIPBOARD]  ;
                                   
 $main.controls.mainEntry insert 0.0 $clip

}


proc ShowContextMenuForControls {x y canv_x canv_y} {
 global GT main
 $GT($main,ControlsMenu) delete 0 1
 $GT($main,ControlsMenu) add command -label "Paste" -command {PasteClipboard $main}
 #$GT($main,ControlsMenu) add command -label "Paste1" -command {PasteClipboard1  }

 tk_popup $GT($main,ControlsMenu) $x $y
}

proc ProcActionForSemSemAn {action x y ctrl} {
    global GT_options GT_event GT_selection GT
    global main MoveTick

    set canvas $GT($main,canvas)
    set editor $main
    set graph $GT($main,graph)
    set GT_event(canvasx) [$canvas canvasx $x]
    set GT_event(canvasy) [$canvas canvasy $y]
	
    set GT_event(editor) $main
    set GT_event(W) $GT($main,canvas)
    set GT_event(graph)  $GT($main,graph)

    $canvas addtag GT::closest overlapping \
	[expr [$graph translate $canvas -x $GT_event(canvasx)] - $GT_options(overlap_gap)] \
	[expr [$graph translate $canvas -y $GT_event(canvasy)] - $GT_options(overlap_gap)] \
	[expr [$graph translate $canvas -x $GT_event(canvasx)] + $GT_options(overlap_gap)] \
	[expr [$graph translate $canvas -y $GT_event(canvasy)] + $GT_options(overlap_gap)]
      set node [GT::find_object $editor node GT::closest]
      $canvas dtag GT::closest

    if { $node != "" }  {

     
        if {$action == "press"} {

            GT::select $main select $node
            GT::ev_move_node_start  $GT($main,graph)  node
          }

        if {$action == "motion"} {
            GT::ev_move_node_motion 
        }

        if {$action == "release"} {
           if {[info exists $GT_event(did_move_node)] == 1 && $GT_event(did_move_node) == 1} {
             GT::ev_move_node_end 
           }
        }

    }  else {

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

       
    }


}


proc GT::save_graph { editor filename} {

    global GT GT_status
    set graph $GT($editor,graph)


    set cmd [list $graph print -file $filename]
    if { [catch $cmd error_message] == 0 } {
	 set GT_status($graph,dirty) 0
	 set GT($editor,filename) $filename
    } else {
	GT::message $editor $error_message error
	return 1
    }
}




proc GT::remove_bends { editor } {

    global GT
    set graph $GT($editor,graph)

    set count 0
    foreach edge [$graph edges] {

	set line [$graph get $edge -line]
	if { [llength $line] > 4 } {

	    set source [$graph get $edge -source]
	    set x0 [$graph get $source -x]
	    set y0 [$graph get $source -y]

	    set target [$graph get $edge -target]
	    set x1 [$graph get $target -x]
	    set y1 [$graph get $target -y]

	    $graph configure $edge graphics \
		-line [list $x0 $y0 $x1 $y1]

	    incr count [expr [llength $line]/2 -2]
	}
    }

    $graph draw 
    return $count
}


proc SetMainGraphType {} {
  global GT main globalLastWasSeman

  delete_lexical_variants $main

  if (!$globalLastWasSeman) return

  $GT($main,graph) delete nodes
  $GT($main,graph) draw 

  $GT($main,main_graph) copynode [$GT($main,main_graph) nodes] $GT($main,graph) 

  $GT($main,graph) canvas $GT($main,canvas)
  $GT($main,graph) draw 
  $GT($main,canvas) configure -background  "white"
};

proc SetClauseGraphType {} {
  global GT globalLastWasSeman
  global main

  delete_lexical_variants $main

  if (!$globalLastWasSeman) return

  $GT($main,graph) delete nodes
  $GT($main,graph) draw 

  $GT($main,clause_graph) copynode [$GT($main,clause_graph) nodes] $GT($main,graph) 
   
  $GT($main,graph) canvas $GT($main,canvas)
  if { [catch SetLayout] != 0 } {   }
  $GT($main,graph) draw 
  $GT($main,canvas) configure -background  "CadetBlue"
  
};


proc  ShowArticleTrigger {}  {
 global GT_selection
 global GT main 

 
 set selected_count_node  [llength $GT_selection($main,selected,node)]
 if  {$selected_count_node > 0}  {
   set node [lindex $GT_selection($main,selected,node) 0]
   set graph $GT($main,graph)
   set type_dict [$graph get $node .type_dict]
   set unit_no [$graph get $node .unit_no]
   if {$type_dict != ""}   {
     ShowArticle $main $type_dict $unit_no
   }
 }

};





proc update_globalAllLexVars { editor } {

    global GT globalAllLexVars LastSentence 
	
    set graph $GT($editor,graph)
	set text [$editor.controls.mainEntry get 0.0 1000.0]
    
	if { $LastSentence != $text || $globalAllLexVars == {} }	{
		
		
		set LastSentence $text
		set AllLexVars  {}
		#получаем все интерпретации
		foreach node [$graph nodes] {
			set CurrLexVar  [$graph get $node .dict_interp]
			set CurrList  [split $CurrLexVar  \n]
			#берем только омонимы (последний элемент списка всегда пуст)
			
			if {[llength $CurrList] > 2} {
			
				set lemma [$graph get $node .lemma]
				if {$lemma != ""} {
					set m {}
					lappend m $lemma
					lappend m $CurrList
					lappend AllLexVars $m
				}
			}
	   }
	   
	   #не включаем  пустые списки
	   set globalAllLexVars {}
	   foreach x_pos $AllLexVars {
			
			set lemma [lindex $x_pos 0]
			set interp_list [lindex $x_pos 1]

			# формируем тройку 
			foreach interp $interp_list {
				if {$interp != ""} {
					set m {}
					lappend m $lemma
					# в начале интепретации стоит плюс или минус, который надо стереть
					set interp [lreplace $interp 0 0]
					lappend m $interp
					lappend m 1
					lappend globalAllLexVars $m
				}
			}
	   }
	   
	}



};

proc create_lexvars_checkboxs { editor} {

    global GT globalAllLexVars 

    set i 1
	set line_no 1
	set column_no 1
	set last_lemma ""
    foreach x_pos $globalAllLexVars {
		 set name  "chk$i"
		 set text_name  "text$i"
		 set lemma_name  "lemma$i"
		 set GT($name) [lindex $x_pos 2] 
 		 set label  [lindex $x_pos 1]
		 set lemma  [lindex $x_pos 0]
		 set x [expr $column_no * 190 - 160]
		 if {$lemma != $last_lemma} {
 			 message $editor.$lemma_name  -text $lemma -width 180 -background grey
			 set y [expr 65 + $line_no * 20]
			 place $editor.$lemma_name -x $x -y $y  
			 incr line_no 1
			 set last_lemma $lemma
		 };


		 checkbutton $editor.$name -text " "  -variable GT($name)  -background grey
		 set y [expr 60 + $line_no * 20]
		 place $editor.$name -x $x -y $y -width 60 
		 
 		 message $editor.$text_name  -text $label -width 180 -background grey
		 set y [expr 65 + $line_no * 20]
		 set x_mess [expr $x + 40] 
		 place $editor.$text_name -x $x_mess -y $y  
		 

		 incr i 1
		 incr line_no 1
		 if {$line_no == 15} {
		    set line_no 1
			incr column_no 1
			
		 }
		 
	}
}

proc delete_lexical_variants { editor} {

  global GT globalAllLexVars 

  
 catch {

    set i 1
    #сохраняем значения, которые содержат checkboх, в	globalAllLexVars
    foreach x_pos $globalAllLexVars {
     
	 set name  "chk$i"
	 set text_name  "text$i"
	 set lemma_name  "lemma$i"

	 if {$GT($name) == 0} {
	    set x_pos [lreplace $x_pos 2 2 0]
	 } else {
		set x_pos [lreplace $x_pos 2 2 1]
	 };
	 
	 set q [expr $i - 1]
     set globalAllLexVars [lreplace $globalAllLexVars $q $q $x_pos]

	 incr i 1

	 destroy  $editor.$name
	 destroy  $editor.$text_name
	 destroy  $editor.$lemma_name
   }
	

   
  }
}






proc SetLexVarType {} {
  global GT  LastSentence 
  global main


  
  SetMainGraphType

    
  update_globalAllLexVars $main

  $GT($main,graph) delete nodes
  $GT($main,graph) draw 

  create_lexvars_checkboxs $main
  $GT($main,canvas) configure -background  grey
}


proc GetJavaGraph {} {
 global main GT  

 #tk_messageBox -message "GetJavaGraph started"
  set graph $GT($main,graph) 

 
 set Result "nodes"
 set i 0
 foreach node [$graph nodes] {
   set x [$graph get $node -x]
   set y [$graph get $node -y]
   set label [$graph get $node -label]
   set morph [$graph get $node .morphology]
   set Result "$Result#(#x $x#y $y#label $label#Morphology $morph#)"
   $graph set $node  .node_id $i
   incr i 1
 };

 set Result "$Result#edges"
 foreach edge [$graph edges] {
 	    set source [$graph get $edge -source]
	    set target [$graph get $edge -target]
	    set source_id [$graph get $source .node_id]
        set target_id [$graph get $target .node_id]
		set label [$graph get $edge -label]
		set Result "$Result#(#source_id $source_id#target_id $target_id#label $label#)"

 }
 set Result "$Result#"
 
 return $Result

};