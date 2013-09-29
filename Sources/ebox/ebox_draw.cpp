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

void ebox_draw_background(t_ebox* x, t_glist* glist)
{
    t_egraphics* g = ebox_start_layer((t_object *)x, (t_object *)glist, gensym("eboxbg"), x->e_rect.width, x->e_rect.height);
    
    if(g)
    {
        egraphics_set_source_jrgba(g, &x->e_boxparameters.d_boxfillcolor);
        egraphics_rectangle(g, 0, 0, x->e_rect.width, x->e_rect.height);
        egraphics_fill(g);
        
        ebox_end_layer((t_object *)x, (t_object *)glist, gensym("eboxbg"));
    }
    ebox_paint_layer((t_object *)x, (t_object *)glist, gensym("eboxbg"), 0, 0);
}

void ebox_draw_text(t_ebox* x, t_glist* glist)
{
    /*
    t_egraphics* g = ebox_start_layer((t_object *)x, (t_object *)glist, gensym("cicmboxext"), x->e_rect.width, x->e_rect.height);
    
    if(g)
    {
        t_ergba black = {0., 0., 0., 1.};
        egraphics_set_line_width(g, 1.);
        t_etextlayout *jtl = cicm_text_layout_create();
        cicm_text_layout_settextcolor(jtl, &black);
        cicm_text_layout_set(jtl, x->e_box_text->s_name, &x->e_font, 2., 9, 0., 0., ETEXT_LEFT, ETEXT_NOWRAP);
        cicm_text_layout_draw(jtl, g);
        cicm_text_layout_destroy(jtl);
 
        ebox_end_layer((t_object *)x, (t_object *)glist, gensym("cicmboxext"));
    }
    ebox_paint_layer((t_object *)x, (t_object *)glist, gensym("cicmboxext"), 0., 0.);
     */
}

void ebox_draw_border(t_ebox* x, t_glist* glist)
{
    t_egraphics* g = ebox_start_layer((t_object *)x, (t_object *)glist, gensym("eboxbd"), x->e_rect.width, x->e_rect.height);
    
    if(g)
    {
        t_ergba grey    = {0.5, 0.5, 0.5, 1.};
        t_ergba white   = {1., 1., 1., 1.};

        egraphics_set_line_width(g, x->e_boxparameters.d_borderthickness);
        egraphics_rectangle(g, 0, 0, x->e_rect.width, x->e_rect.height);
        egraphics_stroke(g);
        
        egraphics_set_source_jrgba(g, &x->e_boxparameters.d_bordercolor);
        for(int i = 0; i < obj_ninlets((t_object *)x); i++)
        {
            int pos_x_inlet = 0;
            if(obj_ninlets((t_object *)x) != 1)
                pos_x_inlet = (int)(i / (float)(obj_ninlets((t_object *)x) - 1) * (x->e_rect.width - 8));
            egraphics_rectangle(g, pos_x_inlet, 0, 7, 3);
            if (obj_issignalinlet((t_object *)x, i))
            {
                egraphics_set_source_jrgba(g, &grey);
                egraphics_fill(g);
            }
            else
            {
                egraphics_set_source_jrgba(g, &white);
                egraphics_fill(g);
                egraphics_set_source_jrgba(g, &grey);
                egraphics_stroke(g);
            }
        }
        
        for(int i = 0; i < obj_noutlets((t_object *)x); i++)
        {
            int pos_x_outlet = 0;
            if(obj_noutlets((t_object *)x) != 1)
                pos_x_outlet = (int)(i / (float)(obj_noutlets((t_object *)x) - 1) * (x->e_rect.width - 8));
            
            egraphics_rectangle(g, pos_x_outlet, x->e_rect.height - 2, 7, 2);
            if (obj_issignaloutlet((t_object *)x, i))
            {
                egraphics_set_source_jrgba(g, &grey);
                egraphics_fill(g);
            }
            else
            {
                egraphics_set_source_jrgba(g, &white);
                egraphics_fill(g);
                egraphics_set_source_jrgba(g, &grey);
                egraphics_stroke(g);
            }
        }
        
        ebox_end_layer((t_object *)x, (t_object *)glist, gensym("eboxbd"));
    }
    ebox_paint_layer((t_object *)x, (t_object *)glist, gensym("eboxbd"), 0, 0);
}

void ebox_select(t_ebox* x, t_glist* glist)
{
    t_canvas* canvas = glist_getcanvas(glist);
    if(x->e_selected)
    {
        t_ergba blue = {0., 0., 1., 1.};
        sys_vgui(".x%lx.c itemconfigure %lxeboxbd0 -fill %s\n", canvas, x, cicm_rgba_to_hex(blue));
    }
    else
        sys_vgui(".x%lx.c itemconfigure %lxeboxbd0 -fill %s\n", canvas, x, cicm_rgba_to_hex(x->e_boxparameters.d_bordercolor));
}

void ebox_move(t_ebox* x, t_glist* glist)
{
    t_egraphics* g;
    t_canvas *canvas = glist_getcanvas(glist);
    int pos_x = text_xpix(&x->e_obj, glist);
    int pos_y = text_ypix(&x->e_obj, glist);    
    
    char temp[246];
    char coordinates[2048] = "";
    for(long i = 0; i < x->e_number_of_graphics; i++)
    {
        g = &x->e_graphics[i];
        for(long j = 0; j < g->e_number_objects; j++)
        {
            sprintf(coordinates, "");
            for(int k = 0; k < g->e_objects[j].e_npoints; k ++)
            {
                sprintf(temp, "%d %d ", (int)((int)g->e_objects[j].e_points[k].x + (int)g->e_rect.x + pos_x), (int)((int)g->e_objects[j].e_points[k].y + (int)g->e_rect.y + pos_y));
                strcat(coordinates, temp);
            }
            sys_vgui(".x%lx.c coords %s %s\n",canvas, g->e_objects[j].e_name->s_name, coordinates);
        }
    }
}

void ebox_invalidate_all(t_ebox *x, t_glist *glist)
{   
    for(long i = 0; i < x->e_number_of_graphics; i++)
    {
        x->e_graphics[i].e_state = EGRAPHICS_INVALID;
    }
}

void ebox_update(t_ebox *x, t_glist *glist)
{
    t_egraphics* g;
    t_canvas *canvas = glist_getcanvas(glist);
    
    for(long i = 0; i < x->e_number_of_graphics; i++)
    {
        g = &x->e_graphics[i];
        if(g->e_state == EGRAPHICS_INVALID)
        {
            for(long j = 0; j < g->e_number_objects; j++)
            {
                sys_vgui(".x%lx.c delete %s\n", canvas, g->e_objects[j].e_name->s_name);
            }
        }
    }
}

void ebox_erase(t_ebox* x, t_glist* glist)
{
    t_egraphics* g;
    t_canvas *canvas = glist_getcanvas(glist);
    
    for(long i = 0; i < x->e_number_of_graphics; i++)
    {
        g = &x->e_graphics[i];
        for(long j = 0; j < g->e_number_objects; j++)
        {
            sys_vgui(".x%lx.c delete %s\n", canvas, g->e_objects[j].e_name->s_name);
        }
        free(x->e_graphics);
        x->e_number_of_graphics = 0;
    }
}

t_egraphics* ebox_start_layer(t_object *b, t_object *view, t_symbol *name, double width, double height)
{
    t_ebox* x = (t_ebox*)b;   
    for(int i = 0; i < x->e_number_of_graphics; i++)
    {
        t_egraphics* graphic = &x->e_graphics[i];
        if(graphic->e_name == name)
        {
            if(graphic->e_state == EGRAPHICS_INVALID)
            {
                graphic->e_owner        = b;
                
                graphic->e_matrix       = NULL;
                graphic->e_rotation     = 0.;

                graphic->e_width    = 1.f;
                graphic->e_color        = gensym("#000000");
                graphic->e_rect.x       = 0.f;
                graphic->e_rect.y       = 0.f;
                graphic->e_rect.height  = pd_clip_min(height, 0.);
                graphic->e_rect.width   = pd_clip_min(width, 0.);
                
                
                graphic->e_number_objects  = 0;
                if(graphic->e_new_objects.e_points)
                    free(graphic->e_new_objects.e_points);
                graphic->e_new_objects.e_points = NULL;
                graphic->e_new_objects.e_npoints = 0;
                graphic->e_objects      = NULL;
                
                graphic->e_state        = EGRAPHICS_OPEN;
                return &x->e_graphics[i];
            }
            else
            {
                return NULL;
            }
        }
    }
    if(x->e_graphics == NULL)
        x->e_graphics = (t_egraphics*)calloc(1, sizeof(t_egraphics));
    else
        x->e_graphics = (t_egraphics*)realloc(x->e_graphics, (x->e_number_of_graphics + 1) * sizeof(t_egraphics));
    if(x->e_graphics)
    {
        t_egraphics* graphic = x->e_graphics+x->e_number_of_graphics;
        x->e_number_of_graphics++;
        
        graphic->e_owner        = b;
        graphic->e_matrix       = NULL;
        graphic->e_rotation     = 0.;
        
        graphic->e_width    = 1.f;
        graphic->e_color        = gensym("#000000");
        graphic->e_rect.x       = 0.f;
        graphic->e_rect.y       = 0.f;
        graphic->e_rect.height  = pd_clip_min(height, 0.);
        graphic->e_rect.width   = pd_clip_min(width, 0.);
        
        
        graphic->e_number_objects  = 0;
        graphic->e_new_objects.e_points = NULL;
        graphic->e_new_objects.e_npoints = 0;
        graphic->e_objects      = NULL;
        
        graphic->e_state        = EGRAPHICS_OPEN;
        graphic->e_name         = name;
        return graphic;
    }
    else
    {
        return NULL;
    }
}

t_pd_err ebox_end_layer(t_object *b, t_object *view, t_symbol *name)
{
    t_ebox* x = (t_ebox*)b;
    for(long i = 0; i < x->e_number_of_graphics; i++)
    {
        if(x->e_graphics[i].e_name == name)
        {
            x->e_graphics[i].e_state = EGRAPHICS_TODRAW;
            return 0;
        }
    }
    
    return 0;
}

t_pd_err ebox_invalidate_layer(t_object *b, t_object *view, t_symbol *name)
{
    t_ebox* x = (t_ebox*)b;
    for(long i = 0; i < x->e_number_of_graphics; i++)
    {
        if(x->e_graphics[i].e_name == name)
        {
            x->e_graphics[i].e_state = EGRAPHICS_INVALID;
            return 0;
        }
    }
    return -1;
}

t_pd_err ebox_paint_layer(t_object *b, t_object *view, t_symbol *name, double x, double y)
{
    t_ebox* obj = (t_ebox *)b;
    t_canvas* canvas = glist_getcanvas((t_glist *)view);
    t_egraphics* g = NULL;
    for(int i = 0; i < obj->e_number_of_graphics; i++)
    {
        if(obj->e_graphics[i].e_name == name && obj->e_graphics[i].e_state == EGRAPHICS_TODRAW)
        {
            g = &obj->e_graphics[i];
            g->e_rect.x = x;
            g->e_rect.y = y;
        }
    }
    if(g)
    {
        char temp[128];
        char script[1024];
        
        for(int i = 0; i < g->e_number_objects; i++)
        {
            t_egraphics_obj* gobj = g->e_objects+i;
            if(gobj->e_type != E_GOBJ_INVALID)
            {
                if(gobj->e_filled)
                {
                    sprintf(script, ".x%lx.c create polygon ", (unsigned long)canvas);
                }
                else
                {
                    sprintf(script, ".x%lx.c create line ", (unsigned long)canvas);
                }
                
                for(int j = 0; j < gobj->e_npoints; j ++)
                {
                    sprintf(temp, "%d %d ", (int)(gobj->e_points[j].x + g->e_rect.x + obj->e_obj.te_xpix), (int)(gobj->e_points[j].y + g->e_rect.y + obj->e_obj.te_ypix));
                    strncat(script, temp, 128);
                }
                if(gobj->e_filled)
                {
                    sprintf(temp, "-fill %s -width 0 -tags %s\n", gobj->e_color->s_name, gobj->e_name->s_name);
                }
                else
                {
                    sprintf(temp, "-fill %s -width %d -tags %s\n", gobj->e_color->s_name, (int)gobj->e_width, gobj->e_name->s_name);
                }
                strncat(script, temp, 128);
                sys_gui(script);
                //post(script);
                g->e_state = EGRAPHICS_CLOSE;
            }
            else
            {
                error("Invalid layer object %s", gobj->e_name->s_name);
            }
        }
    }
    else
        return -1;
    
    return 0;
}


