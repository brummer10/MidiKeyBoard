/*
 *                           0BSD 
 * 
 *                    BSD Zero Clause License
 * 
 *  Copyright (c) 2020 Hermann Meyer
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include "xcustommap.h"
#include "xkeyboard.h"
#include <unistd.h>

static const char *notes[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
static const char *octaves[] = {"0","1","2","3","4","5","6","7","8","9"};

typedef struct {
    int active;
    long keys[128];
    Widget_t *keyboard;
} CustomKeymap;

void draw_custom_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    set_pattern(w,&w->app->color_scheme->selected,&w->app->color_scheme->normal,BACKGROUND_);
    cairo_paint (w->crb);
}

void draw_viewslider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    int v = (int)w->adj->max_value;
    if (!v) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    float sliderstate = adj_get_state(w->adj);
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, width-5,0,5,height);
    cairo_fill_preserve(w->crb);
    use_shadow_color_scheme(w, NORMAL_);
    cairo_fill(w->crb);
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, width-5,(height-10)*sliderstate,5,10);
    cairo_fill_preserve(w->crb);
    use_fg_color_scheme(w, NORMAL_);
    cairo_set_line_width(w->crb,1);
    cairo_stroke(w->crb);
}

void draw_custom_keymap(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    CustomKeymap *customkeys = (CustomKeymap*)w->parent_struct;
    int v = (int)w->adj->max_value;
    if (!v) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    int i = 4;
    int a = 0;
    int n = 0;
    int o = 0;
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb,0,0,width,height);
    cairo_fill(w->crb);
    use_fg_color_scheme(w, NORMAL_);
    cairo_text_extents_t extents;
    char s[64];
    for (;i<height/5;i++) {
        if (customkeys->active == a) {
            use_base_color_scheme(w, SELECTED_);
        } else {
            use_fg_color_scheme(w, NORMAL_);
        }
        snprintf(s, 63,"%d",  a);
        cairo_set_font_size (w->crb, w->app->big_font/w->scale.ascale);
        cairo_text_extents(w->crb, s, &extents);
        cairo_move_to (w->crb, 10, (i*5));
        cairo_show_text(w->crb, s);
        cairo_move_to (w->crb, 150, (i*5));
        cairo_show_text(w->crb, notes[n]);
        cairo_move_to (w->crb, 180, (i*5));
        cairo_show_text(w->crb, octaves[o]);
        if (customkeys->keys[a]) {
            cairo_move_to (w->crb, 50, (i*5));
            cairo_show_text(w->crb, XKeysymToString(customkeys->keys[a]));
        }
        cairo_rectangle(w->crb,40,(i*5)-20,100,25);
        cairo_stroke(w->crb);
        i +=5;
        a++;
        n++;
        if (n > 11) {
            n = 0;
            o++;
        }
    }
    use_base_color_scheme(w, SELECTED_);
    cairo_rectangle(w->crb,0,(i-5)*5,width,5);
    cairo_fill(w->crb);
    draw_viewslider(w, NULL);
}

void custom_motion(void *w_, void* xmotion_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    CustomKeymap *customkeys = (CustomKeymap*)w->parent_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    XMotionEvent *xmotion = (XMotionEvent*)xmotion_;
    //fprintf(stderr, "xmotion->x %i xmotion->y %i \n", xmotion->x, xmotion->y);
    if((xmotion->x < 180) && (xmotion->x > 0)) {
        int i = 4;
        int a = 0;
        for (;i<height/5;i++) {
            if (xmotion->y < (i*5)+5 && xmotion->y > (i*5)-20) {
                if (customkeys->active != a) {
                    customkeys->active = a;
                    expose_widget(w);
                }
                return;
            }
            i +=5;
            a++;
        }
    }
    if (customkeys->active > -2) {
        customkeys->active = -2;
        expose_widget(w);
    }
}

void key_press(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    CustomKeymap *customkeys = (CustomKeymap*)w->parent_struct;
    XKeyEvent *key = (XKeyEvent*)key_;
    if (!key) return;
    if (key->state & ControlMask) {
        return;
    } else {
        KeySym keysym  = XLookupKeysym (key, 0);
        if (keysym == XK_BackSpace) {
            customkeys->keys[customkeys->active] = 0;
            expose_widget(w);
            return;
        } else if (keysym == XK_Up) {
            return;
        } else if (keysym == XK_Down) {
            return;
        } else if (customkeys->active) {
            customkeys->keys[customkeys->active] = keysym;
            expose_widget(w);
        }
    }
}

void set_viewpoint(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    int v = (int)max(0,adj_get_value(w->adj));
    XMoveWindow(w->app->dpy,w->widget,0, -10*v);
}

static void customkeys_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    CustomKeymap *customkeys = (CustomKeymap*)w->parent_struct;
    free(customkeys);
}

Widget_t* add_viewport(Widget_t *parent, int width, int height) {
    Widget_t *wid = create_widget(parent->app, parent, 0, 0, width, height);
    XSelectInput(wid->app->dpy, wid->widget,StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask|KeyReleaseMask
                    |ButtonPressMask|Button1MotionMask|PointerMotionMask);
    Widget_t *p = (Widget_t*)parent->parent;
    wid->parent_struct = p->parent_struct;
    wid->scale.gravity = NONE;
    wid->flags &= ~USE_TRANSPARENCY;
    wid->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    XWindowAttributes attrs;
    XGetWindowAttributes(parent->app->dpy, (Window)parent->widget, &attrs);
    int height_t = attrs.height;
    float d = (float)height/(float)height_t;
    float max_value = (float)((float)height/((float)(height_t/(float)(height-height_t))*(d*10.0)));
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0,max_value ,3.0, CL_VIEWPORT);
    wid->adj = wid->adj_y;
    wid->func.adj_callback = set_viewpoint;
    wid->func.expose_callback = draw_custom_keymap;
    wid->func.motion_callback = custom_motion;
    wid->func.key_press_callback = key_press;
    adj_set_value(wid->adj,max_value/9.8);
    return wid;
}

void chancel_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *parent = (Widget_t*)w->parent;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        destroy_widget(parent,parent->app);
    }
}

void open_keymap(const char* keymapfile, long keys[128]) {
    if( access(keymapfile, F_OK ) != -1 ) {
        FILE *fp;
        if((fp=fopen(keymapfile, "rb"))==NULL) {
            fprintf(stderr,"Cannot open file %s\n", keymapfile);
        }

        if(fread(keys, sizeof(long), 128, fp) != 128) {
            if(feof(fp))
            fprintf(stderr,"Premature end of file %s\n", keymapfile);
            else
            fprintf(stderr,"File read error %s\n", keymapfile);
        }
        fclose(fp);
    }
}

void save_custom_keymap(Widget_t *w) {
    CustomKeymap *customkeys = (CustomKeymap*)w->parent_struct;
    MidiKeyboard *keys = (MidiKeyboard*)customkeys->keyboard->parent_struct;
    FILE *fp;
    if((fp=fopen(w->label, "wb"))==NULL) {
        fprintf(stderr,"Cannot open file %s\n", w->label);
    }

    if(fwrite(customkeys->keys, sizeof(long), 128, fp) != 128)
        fprintf(stderr,"File write error %s\n", w->label);
    fclose(fp);
    read_keymap(w->label,keys->custom_keys);
}

void save_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *parent = (Widget_t*)w->parent;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        save_custom_keymap(parent);
        destroy_widget(parent,parent->app);
    }
}

Widget_t *open_custom_keymap(Widget_t *keyboard, Widget_t *w, const char* keymapfile) {
    Widget_t *wid = create_window(w->app, DefaultRootWindow(w->app->dpy), 0, 0, 300, 400);
    XSelectInput(wid->app->dpy, wid->widget,StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask|KeyReleaseMask
                    |ButtonPressMask|Button1MotionMask|PointerMotionMask);

    XSizeHints* win_size_hints;
    win_size_hints = XAllocSizeHints();
    win_size_hints->flags =  PMinSize|PBaseSize|PMaxSize|PWinGravity;
    win_size_hints->min_width = 300;
    win_size_hints->min_height = 400;
    win_size_hints->base_width = 300;
    win_size_hints->base_height = 400;
    win_size_hints->max_width = 300;
    win_size_hints->max_height = 400;
    win_size_hints->win_gravity = CenterGravity;
    XSetWMNormalHints(wid->app->dpy, wid->widget, win_size_hints);
    XFree(win_size_hints);

    widget_set_title(wid, "Custom Keymap - Editor");
    wid->func.expose_callback = draw_custom_window;
    XSetTransientForHint(w->app->dpy, wid->widget, w->widget);
    CustomKeymap *customkeys = (CustomKeymap*)malloc(sizeof(CustomKeymap));
    wid->parent_struct = customkeys;
    customkeys->active = -2;
    memset(customkeys->keys, 0, 128*sizeof(long));
    customkeys->keyboard = keyboard;
    wid->label = keymapfile;
    open_keymap(wid->label,customkeys->keys);
    wid->flags &= ~USE_TRANSPARENCY;
    wid->flags |= HAS_MEM | NO_AUTOREPEAT | NO_PROPAGATE;
    wid->func.mem_free_callback = customkeys_mem_free;

    Widget_t *view = create_widget(wid->app, wid, 30, 30, 240, 300);
    add_viewport(view, 240, 5.5*700);
    Widget_t * button = add_button(wid, "Chancel", 50, 350, 75, 30);
    button->scale.gravity = ASPECT;
    button->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    button->func.value_changed_callback = chancel_callback;

    button = add_button(wid, "Save", 175, 350, 75, 30);
    button->scale.gravity = ASPECT;
    button->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    button->func.value_changed_callback = save_callback;

    widget_show_all(wid);
    return wid;
}