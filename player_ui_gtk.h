#ifndef __PLAYER_UI_GTK__H
#define __PLAYER_UI_GTK__H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gi18n.h>

#define BORDER_WIDTH       5
#define CTRL_WINDOW_WIDTH  700
#define CTRL_WINDOW_HEIGHT 25

GtkWidget *g_ctrl_window;
GtkWidget *g_window;

GtkWidget *full_screen_pause_button, *ctrl_process_scale, *process_scale;

gboolean g_is_fullscreen, g_is_moving;

GtkWidget *player_widget, *hbox, *vbox, *menubar;
GtkAdjustment  *process_adjust;

#if defined (GDK_WINDOWING_WIN32)
HWND wid  = -1;
#elif defined (GDK_WINDOWING_X11)
uint64_t wid = -1;
#elif defined (GDK_WINDOWING_QUARTZ)
uint64_t wid = -1;
#endif


GtkWidget *pause_button;
GtkWidget *stop_button;

void on_open(GtkWidget *widget, gpointer data);
void on_full_screen(GtkWidget *widget, gpointer data);
void on_quit_full_screen(GtkWidget *widget, gpointer data);
void on_play_pause(GtkWidget *widget, gpointer data);
void on_stop(GtkWidget *widget, gpointer data);
void on_scale_value_change(GtkWidget *widget, gpointer data);

void open_media(const char *uri);
void play();

gboolean _update_scale(gpointer data);
gboolean _hide_ctrl_window(gpointer data);

void on_play_widget_button_press(GtkWidget *widget, GdkEvent *event, gpointer data);


void on_mouse_motion(GtkWidget *widget, gpointer data);

void main_window_init();

void control_window_init();

#endif //__PLAYER_UI_GTK__H


