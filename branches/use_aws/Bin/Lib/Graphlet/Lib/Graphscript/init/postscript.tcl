#
# init/postscript.tcl
#
#------------------------------------------
#
# $Source: /home/br/CVS/graphlet/lib/graphscript/init/postscript.tcl,v $
# $Author: himsolt $
# $Revision: 1.3 $
# $Date: 1999/03/05 20:41:50 $
# $Locker:  $
# $State: Exp $
#
#------------------------------------------
#
# (C) University of Passau 1995-1999, graphlet project
#

# package require Graphlet
# package provide Graphscript [gt_version]


proc GT_init_postscript_options { } {

    global GT_options GT_default_options

    set GT_default_options(postscript_colormode) color
    set GT_default_options(postscript_area) all
    set GT_default_options(postscript_anchor) c
    set GT_default_options(postscript_select) None
    set GT_default_options(postscript_select_name) None
    set GT_default_options(postscript_rotate) false

    set GT_default_options(postscript_crop_marks) 0
    set GT_default_options(postscript_signature) 0
    set GT_default_options(postscript_scale_to_fit) 1

    set GT_default_options(postscript_x) {}
    set GT_default_options(postscript_y) {}
    set GT_default_options(postscript_width) {}
    set GT_default_options(postscript_height) {}

    set GT_default_options(postscript_pagex) {}
    set GT_default_options(postscript_pagey) {}
    set GT_default_options(postscript_pagewidth) {}
    set GT_default_options(postscript_pageheight) {}

    set GT_default_options(postscript_outer_frameleft) 2.5c
    set GT_default_options(postscript_outer_frameright) 2.5c
    set GT_default_options(postscript_outer_frametop) 2c
    set GT_default_options(postscript_outer_framebottom) 2c

    set GT_default_options(postscript_inner_frameleft) 5m
    set GT_default_options(postscript_inner_frameright) 5m
    set GT_default_options(postscript_inner_frametop) 5m
    set GT_default_options(postscript_inner_framebottom) 5m
}


##########################################
#
# Set emacs variables
#
##########################################

# ;;; Local Variables: ***
# ;;; mode: tcl ***
# ;;; End: ***
