#include <gtk/gtk.h>

/* I'm going to be lazy and use some global variables to
 * store the position of the widget within the fixed
 * container */
gint x = 50;
gint y = 50;
GtkWidget *window;
/* This callback function moves the button to a new position
 * in the Fixed container. */

gboolean on_popup_focus_out(GtkWidget *widget,
                   GdkEventFocus *event,
                   gpointer data)
{
    gtk_widget_destroy(widget);
    return TRUE;
}

gboolean closeAboutDialog(GtkWidget *widget,
                   gpointer data)
{
   gtk_widget_destroy (GTK_WIDGET(data));
    return TRUE;
}


static void printz()
{
    g_print("hz\n");
    GtkWidget *popup_window;

    popup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(popup_window), "About");
   
    gtk_window_set_resizable(GTK_WINDOW(popup_window), FALSE);
    // gtk_window_set_decorated (GTK_WINDOW (popup_window), FALSE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(popup_window), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(popup_window), TRUE);
    gtk_widget_set_size_request(popup_window, 350, 400);
    gtk_window_set_transient_for(GTK_WINDOW(popup_window), GTK_WINDOW(window));
    gtk_window_set_position(GTK_WINDOW(popup_window), GTK_WIN_POS_CENTER_ON_PARENT);

    gtk_widget_set_events(popup_window, GDK_FOCUS_CHANGE_MASK);
    g_signal_connect(G_OBJECT(popup_window),
                     "focus-out-event",
                     G_CALLBACK(on_popup_focus_out),
                     NULL);

    GtkWidget *about = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(popup_window), about);
    GtkWidget *aboutBtn = gtk_button_new_with_label("Close");
   g_signal_connect(aboutBtn, "clicked", G_CALLBACK(closeAboutDialog), popup_window);
    gtk_widget_set_size_request(aboutBtn,80,30);
    gtk_fixed_put(GTK_FIXED(about), aboutBtn, 130, 350);

    GtkWidget *buildTime = gtk_label_new(NULL);
    gtk_widget_set_size_request(buildTime, 350, 30);
  
    char str[50];
    sprintf(str,"Build On : %s -- %s",__TIME__,__DATE__);
    const char *format = "<b>%s</b>";
    char *markup = g_markup_printf_escaped (format, str);
    gtk_label_set_markup(GTK_LABEL(buildTime), markup);
    gtk_fixed_put(GTK_FIXED(about), buildTime, 0, 35);

     GtkWidget *gtkVersion = gtk_label_new(NULL);
    gtk_widget_set_size_request(gtkVersion, 350, 30);
    sprintf(str,"Build On : %s -- %s",__TIME__,__DATE__);
    markup = g_markup_printf_escaped (format, str);
    gtk_label_set_markup(GTK_LABEL(gtkVersion), markup);
    gtk_fixed_put(GTK_FIXED(about), gtkVersion, 0, 70);



     GtkWidget *tagLibVersion = gtk_label_new(NULL);
    gtk_widget_set_size_request(tagLibVersion, 350, 30);
    sprintf(str,"Build On : %s -- %s",__TIME__,__DATE__);
    markup = g_markup_printf_escaped (format, str);
    gtk_label_set_markup(GTK_LABEL(tagLibVersion), markup);
    gtk_fixed_put(GTK_FIXED(about), tagLibVersion, 0, 105);

    gtk_widget_show_all(popup_window);
    gtk_widget_grab_focus(popup_window);
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
    gtk_widget_set_size_request(window, 430, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    GClosure *closure = g_cclosure_new(printz, 0, 0);

    // Set up the accelerator group.
    GtkAccelGroup *accel_group = gtk_accel_group_new();
    GtkAccelFlags accel_flags;
    gtk_accel_group_connect(accel_group,
                            GDK_KEY_S,
                            GDK_CONTROL_MASK,
                            accel_flags,
                            closure);
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    /* Here we connect the "destroy" event to a signal handler */
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

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

    button = gtk_button_new_with_label("Press me");

    gtk_widget_set_size_request(menu_bar, 430, 30);
    gtk_fixed_put(GTK_FIXED(fixed), menu_bar, 0, 0);

    GtkWidget *titleLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(titleLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(titleLabel), "<b>Title : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), titleLabel, 5, 35);

    GtkWidget *titleEntry = gtk_entry_new();
    gtk_widget_set_size_request(titleEntry, 350, 30);
    gtk_fixed_put(GTK_FIXED(fixed), titleEntry, 65, 35);

    GtkWidget *artistLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(artistLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(artistLabel), "<b>Artist : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), artistLabel, 5, 70);

    GtkWidget *artistEntry = gtk_entry_new();
    gtk_widget_set_size_request(artistEntry, 350, 30);
    gtk_fixed_put(GTK_FIXED(fixed), artistEntry, 65, 70);

    GtkWidget *ablumLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(ablumLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(ablumLabel), "<b>Ablum : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), ablumLabel, 5, 105);

    GtkWidget *ablumEntry = gtk_entry_new();
    gtk_widget_set_size_request(ablumEntry, 350, 30);
    gtk_fixed_put(GTK_FIXED(fixed), ablumEntry, 65, 105);

    GtkWidget *yearLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(yearLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(yearLabel), "<b>Year : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), yearLabel, 5, 140);

    GtkWidget *yearEntry = gtk_entry_new();
    gtk_widget_set_size_request(yearEntry, -1, 30);
    gtk_entry_set_max_length(GTK_ENTRY(yearEntry), 5);
    gtk_entry_set_width_chars(GTK_ENTRY(yearEntry), 5);
    gtk_fixed_put(GTK_FIXED(fixed), yearEntry, 65, 140);

    GtkWidget *trackLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(trackLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(trackLabel), "<b>Track : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), trackLabel, 5, 175);

    GtkWidget *trackEntry = gtk_entry_new();
    gtk_widget_set_size_request(trackEntry, -1, 30);
    gtk_entry_set_max_length(GTK_ENTRY(trackEntry), 5);
    gtk_entry_set_width_chars(GTK_ENTRY(trackEntry), 5);
    gtk_fixed_put(GTK_FIXED(fixed), trackEntry, 65, 175);

    GtkWidget *discLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(discLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(discLabel), "<b>Disc : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), discLabel, 5, 210);

    GtkWidget *discEntry = gtk_entry_new();
    gtk_widget_set_size_request(discEntry, -1, 30);
    gtk_entry_set_max_length(GTK_ENTRY(discEntry), 5);
    gtk_entry_set_width_chars(GTK_ENTRY(discEntry), 5);
    gtk_fixed_put(GTK_FIXED(fixed), discEntry, 65, 210);

    GtkWidget *commentLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(commentLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(commentLabel), "<b>Comment : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), commentLabel, 8, 245);

    GtkWidget *commentEntry = gtk_entry_new();
    gtk_widget_set_size_request(commentEntry, 320, 30);
    gtk_fixed_put(GTK_FIXED(fixed), commentEntry, 95, 245);

    GtkWidget *infoLabel = gtk_text_view_new();
    gtk_widget_set_size_request(infoLabel, 285, 100);
    gtk_fixed_put(GTK_FIXED(fixed), infoLabel, 130, 140);

    GtkWidget *albumArt = gtk_image_new();
    gtk_widget_set_size_request(albumArt, 300, 250);
    gtk_image_set_from_file (GTK_IMAGE(albumArt),"file.jpg");
    gtk_fixed_put(GTK_FIXED(fixed), albumArt, 65, 300);

    GtkWidget *logLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(logLabel, 430, 30);
    gtk_label_set_markup(GTK_LABEL(logLabel), "<b>Info : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), logLabel, 0, 570);

    /* Display the window */
    gtk_widget_show_all(window);

    /* Enter the event loop */
    gtk_main();

    return 0;
}