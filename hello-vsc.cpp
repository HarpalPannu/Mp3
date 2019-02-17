#include <gtk/gtk.h>
#include <tbytevector.h> //ByteVector
#include <mpegfile.h>    //mp3 file
#include <id3v2tag.h>    //tag
#include <id3v2frame.h>  //frame
#include <attachedpictureframe.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

void quick_message(GtkWindow *parent, gchar *message)
{
    GtkWidget *dialog, *label, *content_area;
    GtkDialogFlags flags;

    // Create the widgets
    flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons("Message",
                                         parent,
                                         flags,
                                         ("_OK"),
                                         GTK_RESPONSE_NONE,
                                         NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new(message);
    gtk_label_set_line_wrap (GTK_LABEL(label),TRUE);
    gtk_widget_set_size_request(dialog,200,300);
    // Ensure that the dialog box is destroyed when the user responds

    g_signal_connect_swapped(dialog,
                             "response",
                             G_CALLBACK(gtk_widget_destroy),
                             dialog);

    // Add the label, and show everything we’ve added

    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);
}

using namespace std;
using namespace TagLib::ID3v2;
GtkWidget *window, *titleEntry, *albumArt, *artistEntry, *ablumEntry, *yearEntry, *trackEntry, *genreEntry, *commentEntry, *logLabel;
GdkPixbufLoader *loader;
GdkPixbuf *pixbuf;

void saveFile(GtkWidget *event_box, gpointer data)
{

    g_print("Save File");
    gtk_widget_destroy(GTK_WIDGET(data));

    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Save File",
                                         GTK_WINDOW(window),
                                         action,
                                         ("_Cancel"),
                                         GTK_RESPONSE_CANCEL,
                                         ("_Save"),
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);
    chooser = GTK_FILE_CHOOSER(dialog);

    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

    gtk_file_chooser_set_filename(chooser,
                                  "existing_filename");

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename(chooser);

        GError *err = NULL;
        if (!gdk_pixbuf_save(pixbuf, filename, "jpeg", &err, NULL))
        {
            g_print("Err %s", err->message);
            quick_message(GTK_WINDOW(window),err->message);
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static gboolean button_press_callback(GtkWidget *event_box, GdkEventButton *event, gpointer data)
{

    if (event->type == GDK_BUTTON_PRESS && event->button == 3)
    {
        g_print("Left\n");
        GtkWidget *popup_window;

        popup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

        gtk_window_set_resizable(GTK_WINDOW(popup_window), FALSE);
        gtk_window_set_decorated(GTK_WINDOW(popup_window), FALSE);
        gtk_window_set_skip_taskbar_hint(GTK_WINDOW(popup_window), TRUE);
        gtk_window_set_skip_pager_hint(GTK_WINDOW(popup_window), TRUE);
        gtk_widget_set_size_request(popup_window, 50, 50);
        gtk_window_set_attached_to(GTK_WINDOW(popup_window), albumArt);
        gtk_window_set_position(GTK_WINDOW(popup_window), GTK_WIN_POS_MOUSE);

        gtk_widget_set_events(popup_window, GDK_FOCUS_CHANGE_MASK);
        // g_signal_connect(G_OBJECT(popup_window),
        //                  "focus-out-event",NULL,
        //                  NULL);

        GtkWidget *box = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
        gtk_container_add(GTK_CONTAINER(popup_window), box);

        GtkWidget *button = gtk_button_new_with_label("Save");
        GtkWidget *buttons = gtk_button_new_with_label("Replace");
        gtk_container_add(GTK_CONTAINER(box), button);
        gtk_container_add(GTK_CONTAINER(box), buttons);
        g_signal_connect(button, "clicked", G_CALLBACK(saveFile), popup_window);
        gtk_widget_show_all(popup_window);
        gtk_widget_grab_focus(popup_window);
        return TRUE; //or false
    }
    return TRUE;
}

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
    gtk_widget_destroy(GTK_WIDGET(data));
    return TRUE;
}

void openDialog()
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File",
                                         GTK_WINDOW(window),
                                         action, "_Cancel",
                                         GTK_RESPONSE_CANCEL, "_Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        TagLib::MPEG::File mp3File(filename);
        Tag *mp3Tag;
        FrameList listOfMp3Frames;
        AttachedPictureFrame *pictureFrame;

        mp3Tag = mp3File.ID3v2Tag();
        cout << mp3Tag->album() << endl;
        if (mp3Tag)
        {
            listOfMp3Frames = mp3Tag->frameListMap()["APIC"]; //look for picture frames only
            if (!listOfMp3Frames.isEmpty())
            {

                pictureFrame = static_cast<AttachedPictureFrame *>(listOfMp3Frames.front()); //cast Frame * to AttachedPictureFrame*
                loader = gdk_pixbuf_loader_new();
                gdk_pixbuf_loader_write_bytes(loader, g_bytes_new(pictureFrame->picture().data(), pictureFrame->picture().size()), NULL);
                pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
                gtk_image_set_from_pixbuf(GTK_IMAGE(albumArt), gdk_pixbuf_scale_simple(pixbuf, 300, 250, GDK_INTERP_BILINEAR));
            }

            else
            {
                cerr << "there seem to be no picture frames (APIC) frames in this file" << endl
                     << endl;
                gtk_image_clear(GTK_IMAGE(albumArt));
            }
            const char *format = "<b>%s</b>";
            char *markup = g_markup_printf_escaped(format, "Tags Loaded");
            gtk_label_set_markup(GTK_LABEL(logLabel), markup);
            gtk_entry_set_text(GTK_ENTRY(titleEntry), mp3Tag->title().toCString());
            gtk_entry_set_text(GTK_ENTRY(artistEntry), mp3Tag->artist().toCString());
            gtk_entry_set_text(GTK_ENTRY(ablumEntry), mp3Tag->album().toCString());
            gtk_entry_set_text(GTK_ENTRY(yearEntry), std::to_string(mp3Tag->year()).c_str());
            gtk_entry_set_text(GTK_ENTRY(trackEntry), std::to_string(mp3Tag->track()).c_str());
            gtk_entry_set_text(GTK_ENTRY(genreEntry), mp3Tag->genre().toCString());
            gtk_entry_set_text(GTK_ENTRY(commentEntry), mp3Tag->comment().toCString());
        }
        else
        {
            const char *format = "<b>%s</b>";
            char *markup = g_markup_printf_escaped(format, "Error Loading Tags Loaded");
            gtk_label_set_markup(GTK_LABEL(logLabel), markup);
        }
    }

    gtk_widget_destroy(dialog);
}

static void printz()
{

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

    gtk_widget_set_size_request(aboutBtn, 100, 30);

    gtk_fixed_put(GTK_FIXED(about), aboutBtn, 125, 350);

    GtkWidget *buildTime = gtk_label_new(NULL);
    gtk_widget_set_size_request(buildTime, 350, 30);

    char str[50];
    sprintf(str, "Build On : %s -- %s", __TIME__, __DATE__);
    const char *format = "<b>%s</b>";
    char *markup = g_markup_printf_escaped(format, str);
    gtk_label_set_markup(GTK_LABEL(buildTime), markup);
    gtk_fixed_put(GTK_FIXED(about), buildTime, 0, 35);

    GtkWidget *gtkVersion = gtk_label_new(NULL);
    gtk_widget_set_size_request(gtkVersion, 350, 30);
    sprintf(str, "Gtk Version : %d.%d.%d", gtk_get_major_version(), gtk_get_minor_version(), gtk_get_micro_version());
    markup = g_markup_printf_escaped(format, str);
    gtk_label_set_markup(GTK_LABEL(gtkVersion), markup);
    gtk_fixed_put(GTK_FIXED(about), gtkVersion, 0, 70);

    GtkWidget *tagLibVersion = gtk_label_new(NULL);
    gtk_widget_set_size_request(tagLibVersion, 350, 30);
    sprintf(str, "Taglib Version : %d.%d", TAGLIB_MAJOR_VERSION, TAGLIB_MINOR_VERSION);
    markup = g_markup_printf_escaped(format, str);
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
    GtkWidget *menu_bar, *menu_item, *file_menu, *help_menu;

    /* Initialise GTK */
    gtk_init(&argc, &argv);

    /* Create a new window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Fixed Container");
    gtk_widget_set_size_request(window, 430, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    GClosure *closure = g_cclosure_new(printz, NULL, NULL);
    GtkAccelGroup *accel_group = gtk_accel_group_new();
    gtk_accel_group_connect(accel_group, GDK_KEY_S, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED, closure);
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
    g_signal_connect(menu_item, "activate", G_CALLBACK(openDialog), NULL);

    menu_item = gtk_menu_item_new_with_label("Save");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item);
    g_signal_connect(menu_item, "activate", G_CALLBACK(printz), NULL);

    menu_item = gtk_menu_item_new_with_label("Exit");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), menu_item);
    g_signal_connect(menu_item, "activate", G_CALLBACK(gtk_main_quit), NULL);

    menu_item = gtk_menu_item_new_with_label("About");
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), menu_item);
    g_signal_connect(menu_item, "activate", G_CALLBACK(printz), NULL);

    gtk_widget_set_size_request(menu_bar, 430, 30);
    gtk_fixed_put(GTK_FIXED(fixed), menu_bar, 0, 0);

    GtkWidget *titleLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(titleLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(titleLabel), "<b>Title : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), titleLabel, 5, 35);

    titleEntry = gtk_entry_new();
    gtk_widget_set_size_request(titleEntry, 350, 30);
    gtk_fixed_put(GTK_FIXED(fixed), titleEntry, 65, 35);

    GtkWidget *artistLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(artistLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(artistLabel), "<b>Artist : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), artistLabel, 5, 70);

    artistEntry = gtk_entry_new();
    gtk_widget_set_size_request(artistEntry, 350, 30);
    gtk_fixed_put(GTK_FIXED(fixed), artistEntry, 65, 70);

    GtkWidget *ablumLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(ablumLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(ablumLabel), "<b>Ablum : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), ablumLabel, 5, 105);

    ablumEntry = gtk_entry_new();
    gtk_widget_set_size_request(ablumEntry, 350, 30);
    gtk_fixed_put(GTK_FIXED(fixed), ablumEntry, 65, 105);

    GtkWidget *yearLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(yearLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(yearLabel), "<b>Year : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), yearLabel, 5, 140);

    yearEntry = gtk_entry_new();
    gtk_widget_set_size_request(yearEntry, -1, 30);
    gtk_entry_set_max_length(GTK_ENTRY(yearEntry), 5);
    gtk_entry_set_width_chars(GTK_ENTRY(yearEntry), 5);
    gtk_fixed_put(GTK_FIXED(fixed), yearEntry, 65, 140);

    GtkWidget *trackLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(trackLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(trackLabel), "<b>Track : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), trackLabel, 5, 175);

    trackEntry = gtk_entry_new();
    gtk_widget_set_size_request(trackEntry, -1, 30);
    gtk_entry_set_max_length(GTK_ENTRY(trackEntry), 5);
    gtk_entry_set_width_chars(GTK_ENTRY(trackEntry), 5);
    gtk_fixed_put(GTK_FIXED(fixed), trackEntry, 65, 175);

    GtkWidget *genreLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(genreLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(genreLabel), "<b>Genre : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), genreLabel, 5, 210);

    genreEntry = gtk_entry_new();
    gtk_widget_set_size_request(genreEntry, -1, 30);
    // gtk_entry_set_max_length(GTK_ENTRY(genreEntry), 5);
    gtk_entry_set_width_chars(GTK_ENTRY(genreEntry), 5);
    gtk_fixed_put(GTK_FIXED(fixed), genreEntry, 65, 210);

    GtkWidget *commentLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(commentLabel, 60, 30);
    gtk_label_set_markup(GTK_LABEL(commentLabel), "<b>Comment : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), commentLabel, 8, 245);

    commentEntry = gtk_entry_new();
    gtk_widget_set_size_request(commentEntry, 320, 30);
    gtk_fixed_put(GTK_FIXED(fixed), commentEntry, 95, 245);

    GtkWidget *infoLabel = gtk_text_view_new();
    gtk_widget_set_size_request(infoLabel, 285, 100);
    gtk_fixed_put(GTK_FIXED(fixed), infoLabel, 130, 140);

    albumArt = gtk_image_new();
    gtk_widget_set_size_request(albumArt, 300, 250);

    GtkWidget *event_box = gtk_event_box_new();

    gtk_container_add(GTK_CONTAINER(event_box), albumArt);

    g_signal_connect(G_OBJECT(event_box),
                     "button_press_event",
                     G_CALLBACK(button_press_callback),
                     albumArt);
    gtk_fixed_put(GTK_FIXED(fixed), event_box, 65, 300);

    logLabel = gtk_label_new(NULL);
    gtk_widget_set_size_request(logLabel, 430, 30);
    gtk_label_set_markup(GTK_LABEL(logLabel), "<b>Info : </b>");
    gtk_fixed_put(GTK_FIXED(fixed), logLabel, 0, 570);

    /* Display the window */
    gtk_widget_show_all(window);

    /* Enter the event loop */
    gtk_main();

    return 0;
}