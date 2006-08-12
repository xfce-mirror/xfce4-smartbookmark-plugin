/*
 *  xfce4-smartbookmark-plugin
 *  Copyright (C) 2005 2006 Emanuele Rocca <ema@debian.org>
 *
 *  lot of code borrowed from:
 *  xfce4-minicmd-plugin
 *  Copyright (C) 2003 Biju Philip Chacko (botsie@users.sf.net)
 *  Copyright (C) 2003 Eduard Roccatello (master@spine-group.org)
 *
 *  and from:
 *  xfce4-netload-plugin
 *  Copyright (C) 2005 Benedikt Meurer <benedikt.meurer@unix-ag.uni-siegen.de>
 *  Copyright (C) 2005 Bernhard Walle  <bernhard.walle@gmx.de>
 *  Copyright (C) 2005 Hendrik Scholz  <hscholz@raisdorf.net>
 *
 *  Port to xfce4-panel 4.4 by Masse Nicolas <masse.nicolas@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>

#include <libxfce4util/libxfce4util.h>
#include <libxfcegui4/dialogs.h>
#include <libxfce4panel/xfce-panel-plugin.h>

/*
 * Types
 */
typedef struct {
    GtkWidget *ebox;
    GtkWidget *entry;           /* keyword entry */
    GtkWidget *label;

    /* options */
    gchar *label_text;
    gchar *url;
    gint size;

    /* options dialog */
    GtkWidget *opt_dialog;
    /* entries (options dialog) */
    GtkWidget *label_entry;
    GtkWidget *url_entry;
    GtkWidget *size_spinner;
} t_search;

//register the plugin

static void
smartbookmark_construct(XfcePanelPlugin *plugin);
XFCE_PANEL_PLUGIN_REGISTER_INTERNAL(smartbookmark_construct);

static gboolean do_search(const char *url, const char *keyword)
{
    DBG ("Do search");
    gchar *execute;
    gboolean success;
    execute = g_strconcat("xfbrowser4  \"", url, NULL);//works better for me
    //execute = g_strconcat("x-www-browser \"", url, NULL);
    execute = g_strconcat(execute, keyword, NULL);
    execute = g_strconcat(execute, "\"", NULL);

    success = exec_command(execute);
    g_free(execute);

    return success;
}


/* redraw the plugin */
static void update_search(t_search *search) {
    DBG ("Update search");
    gtk_widget_hide(GTK_WIDGET(search->ebox));
    gtk_widget_hide(search->label);
    gtk_label_set_text(GTK_LABEL(search->label), search->label_text);
    gtk_widget_show(GTK_WIDGET(search->ebox));
    gtk_widget_show(search->label);
}

/* apply the new values to: url, label_text, size */
static void search_apply_options_cb(t_search *search)
{
    DBG ("Apply options");
    search->url = g_strdup(gtk_entry_get_text(GTK_ENTRY(search->url_entry)));
    search->label_text = g_strdup(gtk_entry_get_text(GTK_ENTRY(search->label_entry)));
    search->size = (gint)(gtk_spin_button_get_value(GTK_SPIN_BUTTON(search->size_spinner)));
    gtk_entry_set_width_chars(GTK_ENTRY(search->entry), search->size);
    update_search(search);
}

/* callback: apply the new value to the url string */
static void url_entry_activate_cb(GtkWidget *widget, t_search *search)
{
    DBG ("Activate url_entry");
    search->url = g_strdup(gtk_entry_get_text(GTK_ENTRY(search->url_entry)));
    update_search(search);
}

/* callback: apply the new value to the label_text string */
static void text_entry_activate_cb(GtkWidget *widget, t_search *search)
{
    DBG ("text_entry_activate_cb");
    search->label_text = g_strdup(gtk_entry_get_text(GTK_ENTRY(search->label_entry)));
    update_search(search);
}

static gboolean entry_buttonpress_cb(GtkWidget *entry, GdkEventButton *event, gpointer data)
{
    static Atom atom = 0;
    GtkWidget *toplevel = gtk_widget_get_toplevel (entry);

    if (event->button != 3 && toplevel && toplevel->window) {
        XClientMessageEvent xev;

        if (G_UNLIKELY(!atom))
            atom = XInternAtom (GDK_DISPLAY(), "_NET_ACTIVE_WINDOW", FALSE);

        xev.type = ClientMessage;
        xev.window = GDK_WINDOW_XID (toplevel->window);
        xev.message_type = atom;
        xev.format = 32;
        xev.data.l[0] = 0;
        xev.data.l[1] = 0;
        xev.data.l[2] = 0;
        xev.data.l[3] = 0;
        xev.data.l[4] = 0;

        XSendEvent (GDK_DISPLAY (), GDK_ROOT_WINDOW (), False,
                    StructureNotifyMask, (XEvent *) & xev);

                gtk_widget_grab_focus (entry);
    }

        return FALSE;
}

/* callback: called when a button is pressed into the main entry */
static gboolean entry_keypress_cb(GtkWidget *entry, GdkEventKey *event, t_search *search)
{
    const gchar *key = NULL;   /* keyword */

    switch (event->keyval) {
        case GDK_Return:
            key = gtk_entry_get_text(GTK_ENTRY(entry));

            if (do_search(search->url, key)) {
                gtk_entry_set_text(GTK_ENTRY(entry), "");  /* clear the entry */
            }
            return TRUE;
        default:
            /* hand over to default signal handler */
            return FALSE;
    }
}

static void search_read_config(t_search *search, const gchar* filename);

static t_search *search_new(XfcePanelPlugin *plugin)
{
    t_search *search;
    GtkWidget *box, *align;
    gchar* filename;
    
    search = g_new0(t_search, 1);
    search->ebox = gtk_event_box_new();
    align = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_container_add(GTK_CONTAINER(search->ebox), align);
    box = gtk_vbox_new(FALSE, 0);

    /* default options */
    search->url = "http://bugs.debian.org/";
    search->label_text = "BTS";
    search->size = 5;
    /* read config file options */
    filename = xfce_panel_plugin_save_location(plugin, TRUE);
    search_read_config(search, filename);

    gtk_container_add(GTK_CONTAINER(align), box);
    search->entry = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(search->entry), search->size);

    search->label = gtk_label_new(search->label_text);
    gtk_box_pack_start(GTK_BOX(box), search->label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), search->entry, FALSE, FALSE, 0);
    // g_signal_connect(command->entry, "activate", G_CALLBACK(runcl), command);
    g_signal_connect(search->entry, "key-press-event", G_CALLBACK(entry_keypress_cb), search);
    g_signal_connect (search->entry, "button-press-event", G_CALLBACK(entry_buttonpress_cb), NULL);

    gtk_container_add( GTK_CONTAINER(plugin), search->ebox);
    xfce_panel_plugin_add_action_widget(plugin, search->ebox);
    gtk_widget_show_all(search->ebox);
    /*
    filename = xfce_panel_plugin_save_location(plugin, TRUE);
    search_read_config(search, filename);
    */
    DBG ("SmartBookMark created");

    return (search);
}

/*
static gboolean search_control_new(Control * ctrl)
{
    t_search *search;

    search = search_new();

    gtk_container_add(GTK_CONTAINER(ctrl->base), search->ebox);

    ctrl->data = (gpointer) search;
    ctrl->with_popup = FALSE;

    gtk_widget_set_size_request(ctrl->base, -1, -1);

    return (TRUE);
}

static void search_free(Control * ctrl)
{
    t_search *search;

    g_return_if_fail(ctrl != NULL);
    g_return_if_fail(ctrl->data != NULL);

    search = (t_search *) ctrl->data;

    g_free(search);
}
*/

static void search_read_config(t_search *search, const gchar* filename)
{
    XfceRc* rcfile;
    if( (rcfile = xfce_rc_simple_open(filename, TRUE) ))
    {
        xfce_rc_set_group(rcfile, NULL);
        search->url = g_strdup(xfce_rc_read_entry(rcfile,"url","http://bugs.debian.org/"));
        search->label_text = g_strdup(xfce_rc_read_entry(rcfile,"value","DBS"));
        search->size = xfce_rc_read_int_entry(rcfile, "size", 5);
    }
}

static void search_write_config(XfcePanelPlugin *plugin, t_search *search)
{
    XfceRc* rcfile;
    gchar *filename = xfce_panel_plugin_save_location(plugin, TRUE);
    
    if( (filename!=NULL) && (rcfile = xfce_rc_simple_open(filename, FALSE)) )
    {
        xfce_rc_set_group(rcfile, NULL);
        xfce_rc_write_entry(rcfile, "url", search->url);
        xfce_rc_write_entry(rcfile, "value", search->label_text);
        xfce_rc_write_int_entry(rcfile, "size", search->size);
        xfce_rc_flush(rcfile);
        xfce_rc_close(rcfile);
    }   
}

static void search_set_size(XfcePanelPlugin *plugin,gint size, t_search *search)
{
    g_print("Not Unimplemented yet : search_set_size");
    /* do the resize of entry :) */
};

/* options dialog */
static void search_create_options(XfcePanelPlugin *plugin, t_search *search)
{
    GtkWidget *hbox, *vbox;
    xfce_panel_plugin_block_menu(plugin);
    GtkWidget *urllabel, *textlabel, *sizelabel;
    DBG ("search_create_options");
    search->opt_dialog  = gtk_dialog_new_with_buttons(_("Preferences"),
                                             NULL, GTK_DIALOG_NO_SEPARATOR,
                                             GTK_STOCK_CLOSE, GTK_RESPONSE_OK,
                                             NULL);
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(search->opt_dialog)->vbox), vbox);

    DBG ("Creating hbox");
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

    /* text label */
    textlabel = gtk_label_new(_("Label:"));
    gtk_widget_show(textlabel);
    gtk_box_pack_start(GTK_BOX(hbox), textlabel, FALSE, FALSE, 5);

    /* text entry */
    search->label_entry = gtk_entry_new();
    gtk_widget_show(search->label_entry);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(search->label_entry), FALSE, FALSE, 0);
    /* text field */
    if(search->label_text)
        gtk_entry_set_text(GTK_ENTRY(search->label_entry), search->label_text);
    //DBG("connect signal");
    g_signal_connect (GTK_WIDGET(search->label_entry), "activate", G_CALLBACK (text_entry_activate_cb), search);

    /* size label */
    sizelabel = gtk_label_new(_("Size:"));
    gtk_widget_show(sizelabel);
    gtk_box_pack_start(GTK_BOX(hbox), sizelabel, FALSE, FALSE, 5);

    /* size spinner */
    GtkObject* spinner_adj = gtk_adjustment_new (search->size, 2.0, 10.0, 1.0, 5.0, 5.0);
    search->size_spinner = gtk_spin_button_new(GTK_ADJUSTMENT(spinner_adj), 1.0, 0);
    gtk_box_pack_start(GTK_BOX(hbox), search->size_spinner, FALSE, FALSE, 0);
    gtk_widget_show(search->size_spinner);

    DBG ("Creating second hbox");
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
    /* url label */
    urllabel = gtk_label_new(_("URL:  "));
    gtk_label_set_use_markup(GTK_LABEL(urllabel), TRUE);
    gtk_widget_show(urllabel);
    gtk_box_pack_start(GTK_BOX(hbox), urllabel, FALSE, FALSE, 5);
    /* url entry */
    search->url_entry = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(search->url_entry), 32);
    gtk_widget_show(search->url_entry);
    /* url field */
    if(search->url!=NULL)
        gtk_entry_set_text(GTK_ENTRY(search->url_entry), search->url);
    g_signal_connect (GTK_WIDGET(search->url_entry), "activate", G_CALLBACK (url_entry_activate_cb), search);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(search->url_entry), FALSE, FALSE, 0);
    
    gtk_dialog_run (GTK_DIALOG(search->opt_dialog));
    search_apply_options_cb(search);
    gtk_widget_destroy(search->opt_dialog);
    xfce_panel_plugin_unblock_menu(plugin);
}


/* create plugin widgets and connect to signals */

static void
smartbookmark_construct(XfcePanelPlugin *plugin)
{
    DBG ("Creating SmartBookMark");
    t_search *search = search_new(plugin);
    xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

    g_signal_connect (plugin, "size-changed",
                      G_CALLBACK (search_set_size), search);

    g_signal_connect (plugin, "save",
                      G_CALLBACK (search_write_config), search);

    xfce_panel_plugin_menu_show_configure (plugin);
    g_signal_connect (plugin, "configure-plugin",
                      G_CALLBACK (search_create_options), search);
    //cc->read_config = search_read_config;
}

