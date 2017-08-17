
#include "player_ui_gtk.h"

#if defined GDK_WINDOWING_WIN32
#include <gdk/gdkwin32.h>
#elif defined GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#elif defined GDK_WINDOWING_QUARTZ
#include <gdk/gdkquartz.h>
#endif

gint screen_width, screen_height;
GdkCursor *cur;

gboolean _hide_ctrl_window(gpointer data)
{
    if (g_is_moving || !g_is_fullscreen)
	    return FALSE;

	//gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(g_window)), gdk_cursor_new(GDK_BLANK_CURSOR));
	gtk_widget_hide(GTK_WIDGET(g_ctrl_window));

	return TRUE;
}

void on_full_screen(GtkWidget *widget, gpointer data)
{
    gtk_window_fullscreen(GTK_WINDOW(g_window));
    g_is_fullscreen = TRUE;
	gtk_widget_hide(hbox);
	gtk_widget_hide(menubar);
	gtk_widget_show_all(GTK_WIDGET(g_ctrl_window));

	g_signal_handlers_unblock_by_func(G_OBJECT(player_widget), on_mouse_motion, NULL);
	g_timeout_add(3000,(GSourceFunc) _hide_ctrl_window, NULL);
}

void on_play_widget_button_press(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    if (event->button.type == GDK_BUTTON_PRESS) {
	    on_play_pause(widget, data);
	} else if (event->button.type == GDK_2BUTTON_PRESS) {
        if (g_is_fullscreen)
		    on_quit_full_screen(widget, data);
		else
			on_full_screen(widget, data);
	}
}

void open_media(const char *uri)
{
#if defined (GDK_WINDOWING_WIN32)
    wid = GDK_WINDOW_HWND(gtk_widget_get_window(player_widget));
#elif defined (GDK_WINDOWING_X11)
    wid = GDK_WINDOW_XID(gtk_widget_get_window(player_widget));
#elif defined
	wid = gdk_quartz_window_get_nsview(gtk_widget_get_window(player_widget));
#endif

    printf("######################## open media %s.\n", uri);
	char play_str[128] = {'\0'};

	sprintf(play_str, "/usr/bin/zvision-show %s --wid %ld", uri, wid);
	if (system(play_str) < 0)
	    printf("call zvision show failed.\n");;
}

void on_open(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    dialog = gtk_file_chooser_dialog_new("open a media file", GTK_WINDOW(widget), action, _("Cancel"), GTK_RESPONSE_CANCEL, _("Open"), GTK_RESPONSE_ACCEPT, NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
	    char *uri;
		uri  = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog));
		open_media(uri);
		g_free(uri);
	}

    gtk_widget_destroy(dialog);	
}

void on_stop(GtkWidget *widget, gpointer data)
{
    GtkWidget *play_icon_image = gtk_image_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(pause_button), play_icon_image);
}

void on_play_pause(GtkWidget *widget, gpointer data)
{
    GtkWidget *play_icon_image = gtk_image_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);

	if (g_is_fullscreen)
	    gtk_button_set_image(GTK_BUTTON(full_screen_pause_button), play_icon_image);
	else
	    gtk_button_set_image(GTK_BUTTON(pause_button), play_icon_image);
}

void on_mouse_motion(GtkWidget *widget, gpointer data)
{
    g_is_moving = TRUE;
	gtk_widget_show(GTK_WIDGET(g_ctrl_window));
	gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(g_window)), cur);
	screen_width = gdk_screen_get_width(gdk_screen_get_default());
	screen_height = gdk_screen_get_height(gdk_screen_get_default());

	gtk_window_move(GTK_WINDOW(g_ctrl_window), (screen_width - CTRL_WINDOW_WIDTH) / 2, 0);
	g_is_moving = FALSE;
}

void on_scale_value_change(GtkWidget *widget, gpointer data)
{
    float scale_value = gtk_adjustment_get_value(process_adjust);
}

void on_quit_full_screen(GtkWidget *widget, gpointer data)
{
    gtk_window_unfullscreen(GTK_WINDOW(g_window));
    g_is_fullscreen = FALSE;
    gtk_widget_show(hbox);
    gtk_widget_show(menubar);
    gtk_widget_hide(g_ctrl_window);

    g_signal_handlers_block_by_func(G_OBJECT(player_widget), on_mouse_motion, NULL);	

}

void control_window_init()
{
    GtkWidget *ctrl_hbox, *quit_full_screen_button, *full_screen_stop_button;

	g_ctrl_window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_position(GTK_WINDOW(g_ctrl_window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(g_ctrl_window), CTRL_WINDOW_WIDTH, CTRL_WINDOW_HEIGHT);

	g_signal_connect(g_ctrl_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	ctrl_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, TRUE);
	gtk_container_add(GTK_CONTAINER(g_ctrl_window), ctrl_hbox);

	full_screen_pause_button = gtk_button_new_from_icon_name("media-playback-pause", GTK_ICON_SIZE_BUTTON);
	full_screen_stop_button  = gtk_button_new_from_icon_name("media-playback-stop", GTK_ICON_SIZE_BUTTON);
	quit_full_screen_button  = gtk_button_new_from_icon_name("view-restore", GTK_ICON_SIZE_BUTTON);

	gtk_box_pack_start(GTK_BOX(ctrl_hbox), full_screen_pause_button, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(ctrl_hbox), full_screen_stop_button, FALSE, TRUE, 0);
	gtk_box_pack_end(GTK_BOX(ctrl_hbox), quit_full_screen_button, FALSE, TRUE, 0);

    g_signal_connect(G_OBJECT(full_screen_pause_button), "clicked", G_CALLBACK(on_play_pause), NULL);
	g_signal_connect(G_OBJECT(full_screen_stop_button), "clicked", G_CALLBACK(on_stop), NULL);
    g_signal_connect(G_OBJECT(quit_full_screen_button), "clicked", G_CALLBACK(on_quit_full_screen), NULL);

	ctrl_process_scale = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, process_adjust);
	gtk_box_pack_start(GTK_BOX(ctrl_hbox), ctrl_process_scale, TRUE, TRUE, 0);
	gtk_scale_set_draw_value(GTK_SCALE(ctrl_process_scale), FALSE);
	gtk_scale_set_has_origin(GTK_SCALE(ctrl_process_scale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(ctrl_process_scale), 0);

	g_signal_connect(G_OBJECT(ctrl_process_scale), "value_changed", G_CALLBACK(on_scale_value_change), NULL);

}

void main_window_init()
{
   GtkWidget  *filemenu, *fileitem, *filemenu_openitem, *hbuttonbox, *full_screen_button;

   g_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_default_size(GTK_WINDOW(g_window), 800, 600);
   g_signal_connect(g_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
   gtk_container_set_border_width(GTK_CONTAINER(g_window), 0);
   gtk_window_set_title(GTK_WINDOW(g_window), "Horse Player");

   // setup box
   vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, FALSE);
   gtk_container_add(GTK_CONTAINER(g_window), vbox);
   
   hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,FALSE);

   // setup menu
   menubar = gtk_menu_bar_new();
   filemenu = gtk_menu_new();
   fileitem = gtk_menu_item_new_with_label("File");
   filemenu_openitem = gtk_menu_item_new_with_label("Open");
   gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), filemenu_openitem);

   gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileitem), filemenu);
   gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileitem);

   gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0 );
   g_signal_connect(G_OBJECT(filemenu_openitem), "activate", G_CALLBACK(on_open), g_window);

   // setup player widget
   player_widget = gtk_drawing_area_new();
   gtk_box_pack_start(GTK_BOX(vbox), player_widget, TRUE, TRUE, 0);
   gtk_widget_add_events(GTK_WIDGET(player_widget), GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK);

   g_signal_connect(player_widget, "button-press-event", G_CALLBACK(on_play_widget_button_press), NULL);
   g_signal_connect(G_OBJECT(player_widget), "motion_notify_event", G_CALLBACK(on_mouse_motion), NULL);
   g_signal_handlers_block_by_func(G_OBJECT(player_widget), on_mouse_motion, NULL);

   // setup controls
   pause_button  = gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);
   stop_button   = gtk_button_new_from_icon_name("media-playback-stop", GTK_ICON_SIZE_BUTTON);
   full_screen_button = gtk_button_new_from_icon_name("view-fullscreen", GTK_ICON_SIZE_BUTTON);

   g_signal_connect(pause_button, "clicked", G_CALLBACK(on_play_pause), NULL);
   g_signal_connect(stop_button, "clicked", G_CALLBACK(on_stop), NULL);
   g_signal_connect(full_screen_button, "clicked", G_CALLBACK(on_full_screen), NULL);

   hbuttonbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
   gtk_container_set_border_width(GTK_CONTAINER(hbuttonbox), BORDER_WIDTH);
   gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox), GTK_BUTTONBOX_START);

   gtk_box_pack_start(GTK_BOX(hbuttonbox), pause_button, FALSE, FALSE, 0);
   gtk_box_pack_start(GTK_BOX(hbuttonbox), stop_button, FALSE, FALSE, 0);

   gtk_box_pack_start(GTK_BOX(hbox), hbuttonbox, FALSE, FALSE, 0);
   gtk_box_pack_end(GTK_BOX(hbox), full_screen_button, FALSE, FALSE, 0);
   gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

   process_adjust = gtk_adjustment_new(0.00, 0.00, 100.00, 1.00, 0.00, 0.00);
   process_scale  = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, process_adjust);
   gtk_box_pack_start(GTK_BOX(hbox), process_scale, TRUE, TRUE, 0);

   gtk_scale_set_draw_value(GTK_SCALE(process_scale), FALSE);
   gtk_scale_set_has_origin(GTK_SCALE(process_scale), TRUE);
   gtk_scale_set_value_pos(GTK_SCALE(process_scale), 0);

   g_signal_connect(G_OBJECT(process_scale), "value_changed", G_CALLBACK(on_scale_value_change), NULL);

   cur = gdk_window_get_cursor(gtk_widget_get_window(g_window));
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

	main_window_init();
	control_window_init();

	gtk_widget_show_all(g_window);
	gtk_main();
    return 0;
}

