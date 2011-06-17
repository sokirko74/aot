#
# action.tcl
#
# This module implements the actions.
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/init/action.tcl,v $
# $Author: himsolt $
# $Revision: 1.12 $
# $Date: 1999/03/05 20:41:42 $
# $Locker:  $
# $State: Exp $
#
#------------------------------------------
#
# (C) University of Passau 1995-1999, graphlet Project
#

# package require Graphlet
# package provide Graphscript [gt_version]

proc GT_init_actions { } {

    global GT

    set GT(action,no_action) GT::no_action

    #
    # Menu related actions
    #

    set GT(action,file_new) GT::action_file_new
    set GT(action,file_new_view) GT::action_file_new_view
    set GT(action,file_open) GT::action_file_open
    set GT(action,file_reload) GT::action_file_reload
    set GT(action,file_save) GT::action_file_save
    set GT(action,file_save_as) GT::action_file_save_as
    set GT(action,file_postscript) GT::action_file_postscript
    set GT(action,file_postscript_options) GT::action_file_postscript_options
    set GT(action,file_save_in_graphed_format) \
	GT::action_file_save_in_graphed_format
    set GT(action,file_save_in_xfig_format) \
	GT::action_file_save_in_xfig_format
    set GT(action,file_close) GT::action_file_close
    set GT(action,file_exit) GT::action_file_exit

    set GT(action,undo) GT::action_edit_undo
    set GT(action,cut) GT::action_edit_cut
    set GT(action,copy) GT::action_edit_copy
    set GT(action,paste) GT::action_edit_paste
    set GT(action,paste_here) GT::action_edit_paste_here
    set GT(action,clear) GT::action_edit_clear
    set GT(action,delete) GT::action_edit_delete
    set GT(action,edit_select_all) GT::action_select_all
    set GT(action,edit_select_all_nodes) GT::action_select_all_nodes
    set GT(action,edit_select_all_edges) GT::action_select_all_edges
    set GT(action,edit_deselect_all) GT::action_deselect_all
    set GT(action,edit_select_all) GT::action_select_all
    set GT(action,edit_deselect_all_nodes) GT::action_deselect_all_nodes
    set GT(action,edit_deselect_all_edges) GT::action_deselect_all_edges
    set GT(action,edit_label_dialog) GT::action_edit_label_dialog
    set GT(action,select_neighbours) GT::action_select_neighbours
    set GT(action,select_in_neighbours) GT::action_select_in_neighbours
    set GT(action,select_out_neighbours) GT::action_select_out_neighbours
    set GT(action,select_adjacent_edges) GT::action_select_adjacent_edges
    set GT(action,select_in_edges) GT::action_select_in_edges
    set GT(action,select_out_edges) GT::action_select_out_edges
    set GT(action,select_embedding_edges) GT::action_select_embedding_edges
    set GT(action,select_inner_edges) GT::action_select_inner_edges
    set GT(action,select_multiple_edges) GT::action_select_multiple_edges
    set GT(action,select_isolated_nodes) GT::action_select_isolated_nodes
    set GT(action,switch_directed) GT::action_switch_directed

    set GT(action,node_style) GT::action_node_style
    set GT(action,edge_style) GT::action_edge_style
    set GT(action,label_style) GT::action_label_style
    set GT(action,style) GT::action_style

    set GT(action,create_mode) GT::action_create_mode
    set GT(action,edit_mode) GT::action_edit_mode
    set GT(action,bends_mode) GT::action_bends_mode
    set GT(action,text_mode) GT::action_text_mode
    set GT(action,view_mode) GT::action_view_mode
    set GT(action,zoom_mode) GT::action_zoom_mode
    set GT(action,rotate_mode) GT::rotate_mode::action
    set GT(action,scale_mode) GT::scale_mode::action

    set GT(action,zoom) GT::action_zoom
    set GT(action,zoom_in) GT::action_zoom_in
    set GT(action,zoom_out) GT::action_zoom_out
    set GT(action,refresh) GT::action_refresh

    set GT(action,adapt_drawing_area) GT::action_adapt_drawing_area
    set GT(action,shrink_drawing_area) GT::action_shrink_drawing_area
    set GT(action,expand_drawing_area) GT::action_expand_drawing_area

    #
    # Tool menu
    #

    set GT(action,tool_inspect) GT::action_tool_inspect
    set GT(action,find_graph) GT::action_find_graph
    set GT(action,search_and_replace_dialog) \
	GT::action_search_and_replace_dialog
    set GT(action,arrange_vertical) GT::action_arrange_vertical
    set GT(action,arrange_horizontal) GT::action_arrange_horizontal
    set GT(action,arrange_circular) GT::action_arrange_circular
    set GT(action,clean_up_graph_size) GT::action_clean_up_graph_size
    set GT(action,clean_up_graph_color) GT::action_clean_up_graph_color
    set GT(action,clean_up_graph_random) GT::action_clean_up_graph_random
    set GT(action,adjust_size_to_label) GT::action_adjust_size_to_label
    set GT(action,snap_to_grid) GT::action_snap_to_grid
    set GT(action,select_graphsize) GT::action_select_graphsize 
    set GT(action,remove_bends) GT::action_remove_bends
    set GT(action,remove_multiple_edges) GT::action_remove_multiple_edges
    set GT(action,remove_self_loops) GT::action_remove_self_loops
    set GT(action,remove_isolated_nodes) GT::action_remove_isolated_nodes
    set GT(action,clear_bends) GT::action_clear_bends

    set GT(action,create_complete_graph) GT::action_create_complete_graph
    set GT(action,create_soirko_graph) GT::action_create_sokirko_graph
    set GT(action,create_circle_graph) GT::action_create_circle_graph
    set GT(action,split_edges) GT::action_split_edges
    set GT(action,duplicate_nodes) GT::action_duplicate_nodes

    set GT(action,scale_down) GT::action_scale_down
    set GT(action,scale_up) GT::action_scale_up
    set GT(action,scale) GT::action_scale
    set GT(action,scale_dialog) GT::action_scale_dialog

    set GT(action,make_invisible) GT::action_make_invisible
    set GT(action,make_visible) GT::action_make_visible

    set GT(action,read_directory) GT::action_read_directory
    set GT(action,read_directory_options) GT::action_read_directory_options

    # Tool/Label
    set GT(action,clear_node_labels) GT::action_clear_node_labels
    set GT(action,clear_edge_labels) GT::action_clear_edge_labels
    set GT(action,number_nodes) \
	GT::action_number_nodes
    set GT(action,set_node_labels_to_degree) \
	GT::action_set_node_labels_to_degree
    set GT(action,set_node_labels_to_internal_id) \
	GT::action_set_node_labels_to_internal_id
    set GT(action,number_edges) \
	GT::action_number_edges
    set GT(action,set_edge_labels_to_internal_id) \
	GT::action_set_edge_labels_to_internal_id
    set GT(action,set_edge_labels_to_endnode_labels) \
	GT::action_set_edge_labels_to_endnode_labels

    #
    # Search/Replace Dialog
    #

    set GT(action,search_start) GT::action_search_start
    set GT(action,search_next) GT::action_search_next
    set GT(action,search_all) GT::action_search_all

    set GT(action,replace_next) GT::action_replace_next
    set GT(action,replace_all) GT::action_replace_all

    set GT(action,search_dialog_cancel) \
	GT::action_search_dialog_cancel

    #
    # Layout menu
    #

    set GT(action,layout_random) GT::action_layout_random
    set GT(action,layout_constraint_fr_options) \
	GT::action_layout_constraint_fr_options
    set GT(action,layout_iterative_constraint_se_options) \
	GT::action_layout_iterative_constraint_se_options
    set GT(action,layout_spring_gem_options) \
	GT::LSD_action_layout_spring_gem_options
    set GT(action,layout_spring_kk_options) \
	GT::LSD_action_layout_spring_kk_options
    set GT(action,layout_spring_fr_options) \
	GT::LSD_action_layout_spring_fr_options
    set GT(action,layout_tunkelang_options) \
	GT::LSD_action_layout_tunkelang_options
    set GT(action,layout_tree_walker_options) \
	GT::LSD_action_layout_tree_walker_options
    set GT(action,layout_hispring_options) \
	GT::action_layout_hispring_options
    set GT(action,layout_tree_options) \
	GT::action_layout_tree_options
    set GT(action,layout_dag_options) \
	GT::action_layout_sugiyama_options

    set GT(action,layout_constraint_fr) \
	GT::action_layout_constraint_fr
    set GT(action,layout_iterative_constraint_se) \
	GT::action_layout_iterative_constraint_se
    set GT(action,layout_spring_gem)    GT::LSD_action_layout_spring_gem
    set GT(action,layout_spring_fr)     GT::LSD_action_layout_spring_fr
    set GT(action,layout_spring_kk)     GT::LSD_action_layout_spring_kk
    set GT(action,layout_tunkelang)     GT::LSD_action_layout_tunkelang
    set GT(action,layout_dag)           GT::action_layout_sugiyama
    set GT(action,layout_tree)          GT::action_layout_tree
    set GT(action,layout_tree_walker)   GT::LSD_action_layout_tree_walker
    set GT(action,layout_hispring)      GT::action_layout_hispring

    # Options menu

    set GT(action,global_options_dialog) GT::action_global_options_dialog
    set GT(action,options_save) GT::action_options_save

    # Help menu

    set GT(action,help_about) GT::action_help_about
    set GT(action,help_www) GT::action_help_www
    set GT(action,help_copyright) GT::action_help_copyright
    set GT(action,help_acknowledgements) GT::action_help_acknowledgements

    # Developer menu
    set GT(action,show_configuration) GT::action_show_configuration
    set GT(action,mail_developers) GT::action_mail_developers
    set GT(action,emergency_exit) GT::action_emergency_exit

    #Walter temporary
    set GT(action,GT::action_layout_sugiyama) GT::action_layout_sugiyama

    # Canvas Popup Menu
    set GT(action,insert_bend) GT::action_insert_bend
    set GT(action,remove_bend) GT::action_remove_bend
}



##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
