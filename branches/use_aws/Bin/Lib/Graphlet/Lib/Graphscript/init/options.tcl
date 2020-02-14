#
# The description of the file goes HERE.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/init/options.tcl,v $
# $Author: himsolt $
# $Revision: 1.22 $
# $Date: 1999/03/05 20:41:48 $
# $Locker:  $
# $State: Exp $
#
#------------------------------------------
#
# (C) University of Passau 1995-1999, graphlet project
#

# package require Graphlet
# package provide Graphscript [gt_version]

##########################################
#
# GT::init_options
#
##########################################


proc GT_trace_default_options { name index op } {
    global GT_options GT_default_options
    if ![info exists GT_options($index)] {
	set GT_options($index) $GT_default_options($index)
    }
}


proc GT_trace_tk_strictMotif { name index op } {
    global tk_strictMotif GT_options
    if { $name == "tk_strictMotif" } {
	set GT_options(tk_strictMotif) $tk_strictMotif
    } else {
	set tk_strictMotif $GT_options(tk_strictMotif)
    }
}


proc GT_init_options { } {
    
    global GT GT_options GT_default_options env
    
    #
    # Experimental: add a trace to initialize GT_options whenever
    # an entry in GT_default_options is set
    #

    trace variable GT_default_options w GT_trace_default_options

    #
    # Graphlet options version
    #

    set GT_default_options(options_version) 1

    # The name of the initialization directory is not really an option as it
    # cannot changed in the startup file 8-)

    if [info exists env(HOME)] {
	set GT_options(init_dir) [file join $env(HOME) .graphlet]
    } else {
	set GT_options(init_dir) [file dirname [info nameofexecutable]]
    }

    # Customize files files within $GT_options(init_dir)

    set GT_options(initfile) [file join $GT_options(init_dir) init.tcl]
    set GT_options(optionsfile) [file join $GT_options(init_dir) options.tcl]
    set GT_options(greetingsfile) [file join $GT_options(init_dir) cookies]

    # use native file selector at startop

    global tcl_platform
    if { $tcl_platform(platform) == "unix" } {
	set GT_default_options(use_native_fileselector) 0
    } else {
	set GT_default_options(use_native_fileselector) 1
    }

    #
    # Graph related options
    #

    array set GT_default_options {
	directed 1
	allow_multiple_edges 0
	allow_self_loops 0
	allow_bends 1
	autonumber_nodes 0
	autonumber_nodes_by_id 0
	autonumber_nodes_by_degree 0
	autonumber_edges 0
	autonumber_edges_by_id 0
    }
    
    #
    # Window Size at Startup
    #

    array set GT_default_options {
	graphlet_window_width 600
	graphlet_window_height 400
	graphlet_window_x 0
	graphlet_window_y 0
	custom_graphlet_window explicit
    }

    #
    # Canvas options
    # 

    array set GT_default_options {
	grid 0
	canvas_minx 0
	canvas_miny 0
	canvas_maxx 4000
	canvas_maxy 4000
	canvas_bg white
    }

    #
    # Size model: small/medium/large (unused !)
    #

    array set GT_default_options {
	size,min_nodes,small 0
	size,max_nodes,small 16
	size,min_nodes,medium 0
	size,max_nodes,medium 100
	size,min_nodes,large 0
	size,max_nodes,large 1000
	size medium	
    }

    # GUI Options 

    array set GT_default_options {
	bell 1
	default_mode Create
	edit_mode_on_extra_mouse_button 0
	select_after_create 1
	expert 0
	double_click_for_inspector 1
	text_mode_uses_label_editor 0
    }

    # Marker Options
    array set GT_default_options {
	marker_width 2
	marker_color "#ff0000"
	minimum_selection_rect_width 2
    }

    #
    # Selection options
    #
    # Minimum gap allowed when checking for overlapping objects
    set GT_default_options(overlap_gap) 4
    # Minimum gap to check wether a bend of an edge is clicked
    set GT_default_options(bend_overlap_gap) 4
    # Select enclosed or overlapping ?
    set GT_default_options(select_in_rectangle) enclosed
    set GT_default_options(select_with_rubberbox) all

    # Selections smaller than small_selection_treshold are
    # considered small and are treated with less efficient routines    
    set GT_default_options(small_selection_treshold) 15

    # Gap between a pop-up window and the selection
    set GT_default_options(position_window_gap_x) 10
    set GT_default_options(position_window_gap_y) 10

    # Color for endnode indicator
    set GT_default_options(temporary_endnode_indicator_color) blue

    # Adjust node size to label
    array set GT_default_options {
	adjust_size_to_label 1
	adjust_size_to_label_gap_x 8.0
	adjust_size_to_label_gap_y 2.0
	adjust_size_to_label_minimum default
    }

    # Quickhelp
    array set GT_default_options {
	tooltips 1
	tooltips_interval 400
    }

    # Duplicate

    array set GT_default_options {
	duplicate_offset_x 10
	duplicate_offset_y 0
    }
    # Misc

    array set GT_default_options {
	save_options ask
	zoom_sizes { 0.25 0.33 0.5 0.66 0.8 1 2 4 8 }
	developer 0
	confirm_close_empty_window 0
    }

    # Fonts
    global tcl_platform
    if { $tcl_platform(platform) == "unix" } {
	array set GT_default_options {
	    system_default_font "Helvetica"
	    system_default_font_size 12
	    system_default_font_style "roman"
	}
    } else {
	array set GT_default_options {
	    system_default_font "MS Sans Serif"
	    system_default_font_size 8
	    system_default_font_style "roman"
	}
    }

    # Construct menus on demand: faster startup, but a small
    # delay when popping up the menu
    set GT_default_options(create_menus_dynamically) 0

    # use pre-8.0 method to create the menubar or attatch it to
    # its toplevel with -menu
    set GT_default_options(create_menus_oldstyle) 0

    # Font menu: show only on Windows
    if { $tcl_platform(platform) == "unix" } {
	set GT_default_options(show_font_menu) 0
    } else {
	set GT_default_options(show_font_menu) 0
    }

    # Fileselector Options (only center and directory are used)
    array set GT_default_options {
	fileselector_directory .
	fileselector_center parent
	fileselector_historylength 10
	fileselector_patternhistlength 10
	fileselector_defaultextension .gml
	fileselector_gml_graphtypes {
	    {{GML Graphs} {*.gml}}
	    {{Graphs} {*.g*}}
	    {{All Files} {*}}
	}
	fileselector_graphed_graphtypes {
	    {{Graphs} {*.g*}}
	    {{All Files} {*}}
	}
	fileselector_postscript_types {
	    {{Postscript Files} {*.ps}}
	    {{All Files} {*}}
	}
	fileselector_fig_types {
	    {{FIG Files} {*.fig}}
	    {{All Files} {*}}
	}
    }

    # Version of GML file format
    set GT_default_options(gml_version) 2

    # Cut & paste
    set GT_default_options(paste_at_cursor_position) 1

    #
    # Label Editor Prefs (size = Characters !)
    #

    array set GT_default_options {
	label_edit_dialog_minwidth 40
	label_edit_dialog_minheight 5
    }


    # Options for search & replace
    array set GT_default_options {
	search_type regexp
	search_direction forward
	search_domain all
    }


    # WWW and Net options
    array set GT_default_options {
	www_browser "/usr/local/bin/netscape"
	www_address "http://www.fmi.uni-passau.de/Graphlet"
	graphlet_bug_address "graphlet@fmi.uni-passau.de"
	graphlet_bug_mail_identification 1
	graphlet_bug_mail_stack 1
    }

    # Startup options

    set GT_default_options(display_version_at_startup) 0

    # Debug options

    set GT_default_options(debug) 0
    set GT_default_options(debug_graphics) 0

    #
    # Layout Algorithms (temporary)
    #

    array set GT_default_options {
	tree_tr_direction 1
	tree_tr_routing 0
	tree_tr_leveling 1
	tree_tr_vert_node_node 20
	tree_tr_hor_node_node 16
	tree_tr_permutation 0
	tree_tr_orientation 0
	tree_tr_father_place 2
	tree_tr_node_edge 50
	tree_tr_channel_width 3
	tree_tr_edge_connection 70
	tree_tr_bend_reduction 0
	tree_tr_edge_connection_for_bend 90
	tree_tr_marked_root 1
    }

    array set GT_default_options {
	constraint_fr_optimal_distance 0
	constraint_fr_minimal_distance 0
	constraint_fr_animation 0
	constraint_fr_delimiter ","
	constraint_fr_random 1
	constraint_fr_respect_sizes 1
	constraint_fr_colour_nodes 0
	constraint_fr_constraint_forces 0
	constraint_fr_new_bends 1
	constraint_fr_phase1_iteration 20
	constraint_fr_phase1_damping 2.0
	constraint_fr_phase2_iteration 20
	constraint_fr_phase2_damping 4.0
	constraint_fr_phase3_iteration 20
	constraint_fr_phase3_damping 15.0
	constraint_fr_vibration_ratio 0.00001
	constraint_fr_minimal_force 0.5
    }

    array set GT_default_options {
	tunkelang_quality 4
	tunkelang_depth 100
	tunkelang_randomize 1
	tunkelang_cut 0
	tunkelang_edgelength 64
	tunkelang_scan_corner 0
    }

    array set GT_default_options {
	walker_Vertical 64
	walker_Sibling 16
	walker_Subtree 32
    }

    array set GT_default_options {
	spring_kk_edgelength 64
    }

    array set GT_default_options {
	hispring_unite_edges 0
	hispring_minimize_nodes 1
	hispring_variable_edgelen 0
	hispring_force_GEM 0
	hispring_what_hierarchie 0
	hispring_show_hie 0
	hispring_max_nodes_per_set 8
	hispring_external_scaling 3
    }

    array set GT_default_options {
	spring_fr_weighted 1
	spring_fr_max_iter 1000
	spring_fr_max_force 3
	spring_fr_vibration 0
	spring_fr_max_iter 1000
	spring_fr_edgelen 64
    }

    array set GT_default_options {
	gem_insert_max_temp 1
	gem_insert_start_temp 0.3
	gem_insert_final_temp 0.05
	gem_insert_max_iter 10
	gem_insert_gravity 0.05
	gem_insert_oscilation 0.4
	gem_insert_rotation 0.5
	gem_insert_shake 0.2
	gem_insert_skip 0
	gem_arrange_max_temp 1.5
	gem_arrange_start_temp 1
	gem_arrange_final_temp 0.02
	gem_arrange_max_iter 3
	gem_arrange_gravity 0.1
	gem_arrange_oscilation 0.4
	gem_arrange_rotation 0.9
	gem_arrange_shake 0.3
	gem_arrange_skip 0
	gem_optimize_max_temp 1.5
	gem_optimize_start_temp 1.0
	gem_optimize_final_temp 0.02
	gem_optimize_max_iter 3
	gem_optimize_gravity 0.1
	gem_optimize_oscilation 0.4
	gem_optimize_rotation 0.9
	gem_optimize_shake 0.3
	gem_optimize_skip 0
	gem_random 1
	gem_quality 0
	gem_default_edgelength 64
    }

    array set GT_default_options {
	sugiyama_vert_dist 64
	sugiyama_horiz_dist 64
	sugiyama_it1 1
	sugiyama_it2 1
	sugiyama_level_arrange 0
	sugiyama_resolve_cycles 0
	sugiyama_reduce_crossings 0
    }

    #
    # Read Directory options
    #

    array set GT_default_options {
	read_directory_dir_background #000000
	read_directory_dir_foreground #FFFFFF
	read_directory_file_background #000080
	read_directory_file_foreground #FFFFFF
	read_directory_depth 1
    }

    #
    # Fonts
    #

    set GT_default_options(default_font) \
	"-adobe-helvetica-medium-r-*-*-*-120-*-*-*-*-*-*"
    set GT_default_options(default_bold_font) \
	"-adobe-helvetica-bold-r-*-*-*-120-*-*-*-*-*-*"
    set GT_default_options(small_font) \
	"-adobe-helvetica-medium-r-*-*-*-100-*-*-*-*-*-*"
    set GT_default_options(windows_default_font) \
	"-adobe-helvetica-bold-r-*-*-*-120-*-*-*-*-*-*"
    set GT_default_options(windows_small_font) \
	"-*-MS Sans Serif-*-*-*-*-10-*-*-*-*-*-*-*"

    #
    # Inspector
    #
    
    array set GT_default_options {
	inspector_dock_into_editor 1
    }
    
    #
    # Load saved options
    #

    global GT GT_options GT_default_options tk_strictMotif

    if [info exists tk_strictMotif] {
	global tk_strictMotif
	set GT_default_options(tk_strictMotif) $tk_strictMotif
	trace variable tk_strictMotif w GT_trace_tk_strictMotif
	trace variable GT_options(tk_strictMotif) w GT_trace_tk_strictMotif
    }
    
    if {[file readable $GT_options(optionsfile)]} {
	
	set GT_options(options_version) unknown

	namespace eval :: source $GT_options(optionsfile)
	if {[info commands GT::user_preferences] != {}} {
	    GT::user_preferences
	} elseif {[info commands GT_user_preferences] != {}} {
	    GT_user_preferences
	}

	if {$GT_options(options_version) == "unknown"} {
	    # Old versions - reset to factory settings
	    if {[info command  tk] == "tk"} {
		wm withdraw .
		tk_dialog .info \
		    "Graphlet" \
		    "Your options are saved in old format. Reverting to factory settings." \
		    warning 0 "Ok"
		wm deiconify .
	    } else {
		puts "Your options are saved in old format. Reverting to factory settings."
	    }
	    foreach i [array names GT_default_options] {
		set GT_options($i) $GT_default_options($i)
	    }
	    catch {file rename \
		       $GT_options(optionsfile) \
		       [file rootname $GT_options(optionsfile)].bak}
	}

	set GT_options(options_version) $GT_default_options(options_version)
    }

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
