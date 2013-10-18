/*
 * PdEnhanced - Pure Data Enhanced 
 *
 * An add-on for Pure Data
 *
 * Copyright (C) 2013 Pierre Guillot, CICM - Université Paris 8
 * All rights reserved.
 *
 * Website  : http://www.mshparisnord.fr/HoaLibrary/
 * Contacts : cicm.mshparisnord@gmail.com
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "ebox.h"

static t_pt mouse_global_pos;

static char *cursorlist[] = {
    "$cursor_runmode_nothing",
    "$cursor_runmode_clickme",
    "$cursor_runmode_thicken",
    "$cursor_runmode_addpoint",
    "$cursor_editmode_nothing",
    "$cursor_editmode_connect",
    "$cursor_editmode_disconnect"
};


void ebox_set_mouse_global_position(t_ebox* x, float x_p, float y_p)
{
    mouse_global_pos.x = x_p;
    mouse_global_pos.y = y_p;
}

t_pt ebox_get_mouse_global_position(t_ebox* x)
{
    sys_vgui("global_mousepos %s\n", x->e_name_rcv->s_name);
    sys_vgui("global_mousepos %s\n", x->e_name_rcv->s_name);
    return mouse_global_pos;
}

t_pt ebox_get_mouse_canvas_position(t_ebox* x)
{
    t_pt point;
    sys_vgui("global_mousepos %s\n", x->e_name_rcv->s_name);
    point.x = mouse_global_pos.x - x->e_canvas->gl_screenx1;
    point.y = mouse_global_pos.y - x->e_canvas->gl_screeny1 - 22; // The top of canvas
    
    return point;
}

void ebox_set_cursor(t_ebox* x, int mode)
{
    mode = pd_clip_minmax(mode, 0, 6);
    sys_vgui("%s configure -cursor %s\n", x->e_drawing_id->s_name, cursorlist[mode]);
}

void ebox_mouse_enter(t_ebox* x)
{
    t_eclass *c = (t_eclass *)x->e_obj.te_g.g_pd;

    if(!x->e_canvas->gl_edit)
    {
        ebox_set_cursor(x, 1);
        if(c->c_widget.w_mouseenter && !x->e_mouse_down)
        {
            c->c_widget.w_mouseenter(x);
        }
    }
    else
    {
        ebox_set_cursor(x, 4);
    }
}

void ebox_mouse_leave(t_ebox* x)
{
    t_eclass *c = (t_eclass *)x->e_obj.te_g.g_pd;
    
    if(!x->e_canvas->gl_edit)
    {
        ebox_set_cursor(x, 0);
        if(c->c_widget.w_mouseleave && !x->e_mouse_down)
        {
            c->c_widget.w_mouseleave(x);
            clock_delay(x->e_deserted_clock, x->e_deserted_time);
        }
    }
    else
    {
        ebox_set_cursor(x, 4);
    }
}

void ebox_mouse_move(t_ebox* x, float x_p, float y_p, float key)
{
    t_eclass *c = (t_eclass *)x->e_obj.te_g.g_pd;
    ebox_get_mouse_global_position(x);
    if(!x->e_canvas->gl_edit)
    {
        ebox_set_cursor(x, 1);
        if(!x->e_mouse_down)
        {
            x->e_mouse.x = x_p;
            x->e_mouse.y = y_p;
            
            if(c->c_widget.w_mousemove)
                c->c_widget.w_mousemove(x, x->e_canvas, x->e_mouse, (long)key);
        }
        else
        {
            ebox_mouse_drag(x, x_p, y_p, key);
        }
    }
    else
    {
        ebox_set_cursor(x, 4);
        if(!x->e_mouse_down)
        {
            ebox_mouse_move_editmode(x, x_p, y_p, key);
        }
        else
        {
            x->e_move_box = ebox_get_mouse_canvas_position(x);
            sys_vgui("pdtk_canvas_motion %s %i %i 0\n", x->e_canvas_id->s_name, (int)x->e_move_box.x, (int)x->e_move_box.y);
        }
    }
}

void ebox_mouse_drag(t_ebox* x, float x_p, float y_p, float key)
{
    t_eclass *c = (t_eclass *)x->e_obj.te_g.g_pd;
    
    if(!x->e_canvas->gl_edit)
    {
        x->e_mouse.x = x_p;
        x->e_mouse.y = y_p;
        if(c->c_widget.w_mousedrag && x->e_mouse_down)
            c->c_widget.w_mousedrag(x, x->e_canvas, x->e_mouse, (long)key);
    }
}

void ebox_mouse_down(t_ebox* x, float x_p, float y_p, float key)
{
    t_eclass *c = (t_eclass *)x->e_obj.te_g.g_pd;
    if(!x->e_canvas->gl_edit)
    {
        x->e_mouse.x = x_p;
        x->e_mouse.y = y_p;
        if(c->c_widget.w_mousedown)
            c->c_widget.w_mousedown(x, x->e_canvas, x->e_mouse, (long)key);
    }
    else
    {
        x->e_move_box = ebox_get_mouse_canvas_position(x);
        if(key == EMOD_CMD)
            sys_vgui("pdtk_canvas_rightclick %s %i %i 0\n", x->e_canvas_id->s_name, (int)x->e_move_box.x, (int)x->e_move_box.y);
        else
            sys_vgui("pdtk_canvas_mouse %s %i %i 0 0\n", x->e_canvas_id->s_name, (int)(x->e_move_box.x), (int)(x->e_move_box.y));
    }
    x->e_mouse_down = 1;
}

void ebox_mouse_up(t_ebox* x, float x_p, float y_p, float key)
{
    t_pt point;
    t_eclass *c = (t_eclass *)x->e_obj.te_g.g_pd;
    if(!x->e_canvas->gl_edit)
    {
        point = ebox_get_mouse_canvas_position(x);
        x->e_mouse.x = point.x;
        x->e_mouse.y = point.y;
        if(c->c_widget.w_mouseup)
            c->c_widget.w_mouseup(x, x->e_canvas, x->e_mouse, (long)key);
    }
    else
    {
        x->e_move_box = ebox_get_mouse_canvas_position(x);
        sys_vgui("pdtk_canvas_mouseup %s %i %i 0\n", x->e_canvas_id->s_name, (int)x->e_move_box.x, (int)x->e_move_box.y);
    }
    x->e_mouse_down = 0;
}

void ebox_mouse_dblclick(t_ebox* x, float x_p, float y_p)
{
    t_eclass *c = (t_eclass *)x->e_obj.te_g.g_pd;
    
    if(!x->e_canvas->gl_edit)
    {
        x->e_mouse.x = x_p;
        x->e_mouse.y = y_p;
        if(c->c_widget.w_dblclick)
            c->c_widget.w_dblclick(x, x->e_canvas, x->e_mouse);
    }
}

void ebox_mouse_rightclick(t_ebox* x, float x_p, float y_p)
{
    if(!x->e_canvas->gl_edit)
    {
        ;
    }
    else
    {
        ;
    }
}

void ebox_deserted(t_ebox *x)
{
    t_eclass* c = (t_eclass *)x->e_obj.te_g.g_pd;
    if(c->c_widget.w_deserted)
        c->c_widget.w_deserted(x);
    clock_unset(x->e_deserted_clock);
}

void ebox_mouse_move_editmode(t_ebox* x, float x_p, float y_p, float key)
{
    int i;
    //t_gobj* g = (t_gobj*)x;
    x->e_selected_outlet    = -1;
    x->e_selected_inlet     = -1;
    
    x->e_move_box = ebox_get_mouse_canvas_position(x);
    sys_vgui("pdtk_canvas_motion %s %i %i 0\n", x->e_canvas_id->s_name, (int)x->e_move_box.x, (int)x->e_move_box.y);
    
     // Check inlets //
    if(y_p > 0 && y_p < 3)
    {
        for(i = 0; i < obj_noutlets((t_object *)x); i++)
        {
            int pos_x_inlet = 0;
            if(obj_ninlets((t_object *)x) != 1)
                pos_x_inlet = (int)(i / (float)(obj_ninlets((t_object *)x) - 1) * (x->e_rect.width - 8));
            
            if(x_p >= pos_x_inlet && x_p <= pos_x_inlet +7)
            {
                //sys_vgui("pdtk_gettip .x%lx.c inlet %d [list %s] [list %s] [list %s]\n", x->e_canvas, i, g->g_pd->c_name->s_name, g->g_pd->c_helpname->s_name, g->g_pd->c_externdir);
                x->e_selected_inlet = i;
                ebox_set_cursor(x, 4);
                break;
            }
            
        }
    }
    if(x->e_selected_inlet == -1)
    {
        // Check outlets //
        if(y_p > x->e_rect.width + x->e_boxparameters.d_borderthickness * 2. - 3 && y_p < x->e_rect.width + x->e_boxparameters.d_borderthickness * 2.)
        {
            for(i = 0; i < obj_noutlets((t_object *)x); i++)
            {
                int pos_x_outlet = 0;
                if(obj_noutlets((t_object *)x) != 1)
                    pos_x_outlet = (int)(i / (float)(obj_noutlets((t_object *)x) - 1) * (x->e_rect.width - 8));
                
                if(x_p >= pos_x_outlet && x_p <= pos_x_outlet +7)
                {
                    //sys_vgui("pdtk_gettip .x%lx.c outlet %d [list %s] [list %s] [list %s]\n", x->e_canvas, i, g->g_pd->c_name->s_name, g->g_pd->c_helpname->s_name, g->g_pd->c_externdir);
                    x->e_selected_outlet = i;
                    ebox_set_cursor(x, 5);
                    break;
                }
                
            }
        }
        // my box //
        if(x->e_selected_outlet == -1)
        {
            //sys_vgui("pdtk_gettip .x%lx.c text %d [list %s] [list %s] [list %s]\n", x->e_canvas, i, g->g_pd->c_name->s_name, g->g_pd->c_helpname->s_name, g->g_pd->c_externdir);
            ebox_set_cursor(x, 4);
        }
    }
    ebox_invalidate_layer((t_object *)x, NULL, gensym("eboxio"));
    ebox_redraw(x);
}

void ebox_mouse_wheel(t_ebox* x, float delta, float key)
{
    t_eclass* c = (t_eclass *)x->e_obj.te_g.g_pd;
    
    if(!x->e_canvas->gl_edit)
    {
        if(c->c_widget.w_mousewheel)
            c->c_widget.w_mousewheel(x, x->e_canvas, x->e_mouse, (long)key, delta, delta);
    }
}

void ebox_popup(t_ebox* x, t_symbol* s, float itemid)
{
    t_eclass* c = (t_eclass *)x->e_obj.te_g.g_pd;

    if(c->c_widget.w_popup)
        c->c_widget.w_popup(x, s, (long)itemid);
}

void ebox_dosave_box(t_gobj* z, t_binbuf *b)
{
    t_ebox*   x = (t_ebox *)z;
    t_eclass* c = (t_eclass *)x->e_obj.te_g.g_pd;
    
    binbuf_addv(b, "ssii", gensym("#X"), gensym("obj"), (t_int)x->e_obj.te_xpix, (t_int)x->e_obj.te_ypix);
    binbuf_add(b, binbuf_getnatom(x->e_obj.te_binbuf), binbuf_getvec(x->e_obj.te_binbuf));
    
    if(c->c_widget.w_save != NULL)
        c->c_widget.w_save(x, b);
    
    binbuf_addv(b, ";");
}

void ebox_dosave_nobox(t_gobj* z, t_binbuf *b)
{
    int         i;
    char        attr_name[MAXPDSTRING];
    long        argc    = 0;
    t_atom*     argv    = NULL;
    t_ebox*   x = (t_ebox *)z;
    t_eclass* c = (t_eclass *)x->e_obj.te_g.g_pd;
    
    binbuf_addv(b, "ssiis", gensym("#X"), gensym("obj"), (int)x->e_obj.te_xpix, (int)x->e_obj.te_ypix, gensym(class_getname(x->e_obj.te_g.g_pd)));
    
    for(i = 0; i < c->c_nattr; i++)
    {
        if(c->c_attr[i].save)
        {
            sprintf(attr_name, "@%s", c->c_attr[i].name->s_name);
            object_attr_getvalueof((t_object *)x, c->c_attr[i].name, &argc, &argv);
            if(argc && argv)
            {
                binbuf_append_atoms(b, gensym(attr_name), argc, argv);
                argc = 0;
                free(argv);
                argv = NULL;
            }
        }
    }

    if(c->c_widget.w_save != NULL)
        c->c_widget.w_save(x, b);
    
    binbuf_addv(b, ";");
}

void ebox_save(t_gobj* z, t_binbuf *b)
{
    t_ebox*   x = (t_ebox *)z;
    t_eclass* c = (t_eclass *)x->e_obj.te_g.g_pd;
    
    if(c->c_box)
    {
        ebox_dosave_box(z, b);
    }
    else if(x->e_ready_to_draw)
    {
       ebox_dosave_nobox(z, b);
    }
}
