#include <gtk/gtk.h>

/* I'm going to be lazy and use some global variables to
 * store the position of the widget within the fixed
 * container */
gint x = 50;
gint y = 50;
   GtkWidget *window;
/* This callback function moves the button to a new position
 * in the Fixed container. */
static void move_button(GtkWidget *widget,
                        GtkWidget *fixed)
{
    x = (x + 30) % 300;
    y = (y + 50) % 300;
    gtk_fixed_move(GTK_FIXED(fixed), widget, x, y);
}
gboolean
on_popup_focus_out (GtkWidget *widget,
                    GdkEventFocus *event,
                    gpointer data)
{
  gtk_widget_destroy (widget);
  return TRUE;
}
static void printz(){
    g_print("hz\n");
    GtkWidget *popup_window;

  popup_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (popup_window), __TIME__);
  gtk_container_set_border_width (GTK_CONTAINER (popup_window), 10);
  gtk_window_set_resizable (GTK_WINDOW (popup_window), FALSE);
 // gtk_window_set_decorated (GTK_WINDOW (popup_window), FALSE);
  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (popup_window), TRUE);
  gtk_window_set_skip_pager_hint (GTK_WINDOW (popup_window), TRUE);
  gtk_widget_set_size_request (popup_window, 150, 150);
  gtk_window_set_transient_for (GTK_WINDOW (popup_window), GTK_WINDOW (window));
  gtk_window_set_position (GTK_WINDOW (popup_window), GTK_WIN_POS_CENTER);

  gtk_widget_set_events (popup_window, GDK_FOCUS_CHANGE_MASK);
  g_signal_connect (G_OBJECT (popup_window),
                    "focus-out-event",
                    G_CALLBACK (on_popup_focus_out),
                    NULL);

//   GdkColor color;
//   gdk_color_parse ("#3b3131", &color);
//   gtk_widget_modify_bg (GTK_WIDGET (popup_window), GTK_STATE_NORMAL, &color);

  gtk_widget_show_all (popup_window);
  gtk_widget_grab_focus (popup_window);
}



int main(int argc,
         char *argv[])
{
    /* GtkWidget is the storage type for widgets */
 
    GtkWidget *fixed;
    GtkWidget *button;
    gint i;
    GtkWidget *menu_bar, *menu_item, *file_menu, *help_menu;


    /* Initialise GTK */
    gtk_init(&argc, &argv);

    /* Create a new window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Fixed Container");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 600);
    gtk_window_set_resizable (GTK_WINDOW(window), FALSE);


     GClosure* closure = g_cclosure_new(printz, 0, 0);

    // Set up the accelerator group.
    GtkAccelGroup* accel_group = gtk_accel_group_new();
     GtkAccelFlags accel_flags;
    gtk_accel_group_connect(accel_group,
                            GDK_KEY_S,
                            GDK_CONTROL_MASK,
                            accel_flags,
                            closure);
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    /* Here we connect the "destroy" event to a signal handler */
    g_signal_connect(window, "destroy",G_CALLBACK(gtk_main_quit), NULL);

    /* Sets the border width of the window. */
   // gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    /* Create a Fixed Container */
    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);
    gtk_widget_show(fixed);

    menu_bar = gtk_menu_bar_new();
	file_menu = gtk_menu_new();
	help_menu = gtk_menu_new();


    menu_item = gtk_menu_item_new_with_label("File");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), file_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	menu_item = gtk_menu_item_new_with_label("Help");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), help_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	menu_item = gtk_menu_item_new_with_label("Open");
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(printz), NULL);

    menu_item = gtk_menu_item_new_with_label("Save");
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(printz), NULL);

	menu_item = gtk_menu_item_new_with_label("Exit");
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(gtk_main_quit), NULL);

	menu_item = gtk_menu_item_new_with_label("About");
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(printz), NULL);

    button = gtk_button_new_with_label ("Press me");

    gtk_widget_set_size_request (menu_bar,500,30);
    gtk_fixed_put(GTK_FIXED(fixed), menu_bar,0, 0);
  

   

    /* Display the window */
    gtk_widget_show_all(window);

    /* Enter the event loop */
    gtk_main();

    return 0;
}