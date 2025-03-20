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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>

#include <libxfce4util/libxfce4util.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/libxfce4panel.h>

/*
 * Types
 */
typedef struct {
    GtkWidget *box;
    GtkWidget *entry;           /* keyword entry */
    GtkWidget *label;

    /* options */
    gchar *label_text;
    gchar *url;
    gint size;

    gboolean hide_label;

    /* options dialog */
    GtkWidget *opt_dialog;
    /* entries (options dialog) */
    GtkWidget *label_entry;
    GtkWidget *url_entry;
    GtkWidget *size_spinner;
    GtkWidget *hide_check;
} t_search;

//register the plugin

static void
smartbookmark_construct(XfcePanelPlugin *plugin);
XFCE_PANEL_PLUGIN_REGISTER(smartbookmark_construct);

static gboolean do_search(const char *url, const char *keyword)
{
#if LIBXFCE4UI_CHECK_VERSION(4, 21, 0)
    gchar *argv[] = { "xfce-open", "--launch", "WebBrowser", NULL, NULL };
#else
    gchar *argv[] = { "exo-open", "--launch", "WebBrowser", NULL, NULL };
#endif
    GError *error = NULL;
    gchar *complete_url;
    gboolean success;
    complete_url = g_strconcat(url, keyword, NULL);
    argv[3] = complete_url;
    DBG ("Do search");

    success = g_spawn_async(NULL, (gchar **)argv, NULL,
        G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error);

    if (!success) {
        xfce_dialog_show_error(NULL, error, _("Failed to send %s to your preferred browser"), complete_url);
        g_error_free(error);
    }
    g_free(complete_url);

    return success;
}


/* callback: apply the new value to the url string */
static void url_entry_changed_cb(GtkWidget *widget, t_search *search)
{
    DBG ("url_entry_changed_cb");
    search->url = g_strdup(gtk_entry_get_text(GTK_ENTRY(search->url_entry)));
}

/* callback: apply the new value to the label_text string */
static void text_entry_changed_cb(GtkWidget *widget, t_search *search)
{
    DBG ("text_entry_changed_cb");
    search->label_text = g_strdup(gtk_entry_get_text(GTK_ENTRY(search->label_entry)));
    gtk_label_set_text(GTK_LABEL(search->label), search->label_text);
}

/* callback: apply the new size to the entry widget */
static void entry_size_changed_cb(GtkWidget *widget, t_search *search)
{
    DBG ("entry_size_changed_cb");
    search->size = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(search->size_spinner));
    gtk_entry_set_width_chars(GTK_ENTRY(search->entry), search->size);
}

static gboolean hide_check_toggled_cb(GtkWidget *widget, gboolean is_active, t_search *search)
{
    DBG ("hide_check_toggled_cb");
    search->hide_label = is_active;
    gtk_widget_set_sensitive (GTK_WIDGET(search->label_entry), !is_active);
    gtk_widget_set_visible (search->label, !is_active);
    gtk_switch_set_state(GTK_SWITCH(search->hide_check), is_active);
    return TRUE;
}

static gboolean entry_buttonpress_cb(GtkWidget *entry, GdkEventButton *event, XfcePanelPlugin *plugin)
{
    GtkWidget *toplevel = gtk_widget_get_toplevel (entry);

    if (event->button != 3 && toplevel && gtk_widget_get_window(toplevel)) {
        xfce_panel_plugin_focus_widget (plugin, entry);
    }

    return FALSE;
}

/* callback: called when a button is pressed into the main entry */
static gboolean entry_keypress_cb(GtkWidget *entry, GdkEventKey *event, t_search *search)
{
    const gchar *key = NULL;   /* keyword */

    switch (event->keyval) {
        case GDK_KEY_Return:
        case GDK_KEY_KP_Enter:
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
    gchar* filename;

    search = g_new0(t_search, 1);
    search->box = gtk_box_new(!xfce_panel_plugin_get_orientation(plugin), 0);
    gtk_widget_set_halign(GTK_WIDGET(search->box), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(search->box), GTK_ALIGN_CENTER);

    /* default options */
    search->url = "https://bugs.debian.org/";
    search->label_text = "BTS";
    search->size = 5;
    search->hide_label = FALSE;
    /* read config file options */
    filename = xfce_panel_plugin_save_location(plugin, TRUE);
    search_read_config(search, filename);

    search->entry = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(search->entry), search->size);

    search->label = gtk_label_new(search->label_text);
    gtk_box_pack_start(GTK_BOX(search->box), search->label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(search->box), search->entry, FALSE, FALSE, 0);
    // g_signal_connect(command->entry, "activate", G_CALLBACK(runcl), command);
    g_signal_connect(search->entry, "key-press-event", G_CALLBACK(entry_keypress_cb), search);
    g_signal_connect (search->entry, "button-press-event", G_CALLBACK(entry_buttonpress_cb), plugin);

    gtk_container_add( GTK_CONTAINER(plugin), search->box);
    gtk_widget_show_all(search->box);

    if (search->hide_label) {
        gtk_widget_hide(search->label);
    }

    DBG ("SmartBookMark created");

    return (search);
}

static void search_read_config(t_search *search, const gchar* filename)
{
    XfceRc* rcfile;
    if( (rcfile = xfce_rc_simple_open(filename, TRUE) ))
    {
        xfce_rc_set_group(rcfile, NULL);
        search->url = g_strdup(xfce_rc_read_entry(rcfile,"url","https://bugs.debian.org/"));
        search->label_text = g_strdup(xfce_rc_read_entry(rcfile,"value","DBS"));
        search->size = xfce_rc_read_int_entry(rcfile, "size", 5);
        search->hide_label = xfce_rc_read_bool_entry(rcfile, "hidelabel", FALSE);
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
        xfce_rc_write_bool_entry(rcfile, "hidelabel", search->hide_label);
        xfce_rc_flush(rcfile);
        xfce_rc_close(rcfile);
    }
}

static void search_set_size(XfcePanelPlugin *plugin,gint size, t_search *search)
{
    /*
    g_print("Not Unimplemented yet : search_set_size");
    do the resize of entry :) */
};

static void show_about(XfcePanelPlugin *plugin, t_search *search)
{
    const gchar *auth[] = {
        "Emanuele Rocca <ema@debian.org>",
        "Masse Nicolas <masse.nicolas@gmail.com>",
        "Florian Rivoal <frivoal@xfce.org>",
        NULL
    };

    gtk_show_about_dialog(NULL,
        "logo-icon-name", "system-search",
        "license", xfce_get_license_text (XFCE_LICENSE_TEXT_GPL),
        "version", PACKAGE_VERSION,
        "program-name", PACKAGE_NAME,
        "comments", _("Query websites from the Xfce panel"),
        "website", PACKAGE_URL,
        "copyright", "Copyright \302\251 2006-2025 The Xfce development team",
        "authors", auth, NULL);
}

/* options dialog */
static void search_create_options(XfcePanelPlugin *plugin, t_search *search)
{
    GtkWidget *grid, *vbox;
    GtkWidget *urllabel, *textlabel, *sizelabel;
    GtkAdjustment *spinner_adj;
    xfce_panel_plugin_block_menu(plugin);
    DBG ("search_create_options");
    search->opt_dialog  = xfce_titled_dialog_new_with_mixed_buttons(_("Smartbookmark"),
                                             GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (plugin))),
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             "window-close-symbolic", _("_Close"), GTK_RESPONSE_OK,
                                             NULL);

    xfce_titled_dialog_set_subtitle (XFCE_TITLED_DIALOG (search->opt_dialog), _("Preferences"));
    gtk_window_set_icon_name  (GTK_WINDOW (search->opt_dialog), "system-search");

    vbox = gtk_dialog_get_content_area (GTK_DIALOG(search->opt_dialog));

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing (GTK_GRID(grid), 12);
    gtk_grid_set_row_spacing (GTK_GRID(grid), 6);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 12);
    gtk_box_pack_start(GTK_BOX(vbox), grid, FALSE, FALSE, 0);

    /* text label */
    textlabel = gtk_label_new(_("Label:"));
    gtk_label_set_xalign (GTK_LABEL (textlabel), 0.0f);
    gtk_widget_show(textlabel);
    gtk_grid_attach(GTK_GRID(grid), textlabel, 0, 0, 1, 1);

    /* text entry */
    search->label_entry = gtk_entry_new();
    gtk_widget_set_hexpand (GTK_WIDGET (search->label_entry), TRUE);
    gtk_widget_show(search->label_entry);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(search->label_entry), 1, 0, 1, 1);
    gtk_widget_set_sensitive (GTK_WIDGET(search->label_entry), !search->hide_label);
    /* text field */
    if(search->label_text)
        gtk_entry_set_text(GTK_ENTRY(search->label_entry), search->label_text);
    //DBG("connect signal");
    g_signal_connect (GTK_WIDGET(search->label_entry), "changed", G_CALLBACK (text_entry_changed_cb), search);

    /* Hide label option */
    search->hide_check = gtk_switch_new();
    gtk_widget_set_tooltip_text(GTK_WIDGET(search->hide_check),_("Hide label"));
    gtk_widget_set_valign (GTK_WIDGET (search->hide_check), GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand (GTK_WIDGET (search->hide_check), FALSE);
    gtk_switch_set_active(GTK_SWITCH(search->hide_check),
                                 search->hide_label);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(search->hide_check), 2, 0, 1, 1);
    g_signal_connect (GTK_WIDGET(search->hide_check), "state-set", G_CALLBACK (hide_check_toggled_cb), search);

    /* url label */
    urllabel = gtk_label_new(_("URL:  "));
    gtk_label_set_xalign (GTK_LABEL (urllabel), 0.0f);
    gtk_label_set_use_markup(GTK_LABEL(urllabel), TRUE);
    gtk_grid_attach(GTK_GRID(grid), urllabel, 0, 1, 1, 1);
    /* url entry */
    search->url_entry = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(search->url_entry), 42);
    gtk_widget_show(search->url_entry);
    /* url field */
    if(search->url!=NULL)
        gtk_entry_set_text(GTK_ENTRY(search->url_entry), search->url);
    g_signal_connect (GTK_WIDGET(search->url_entry), "changed", G_CALLBACK (url_entry_changed_cb), search);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(search->url_entry), 1, 1, 2, 1);

    /* size label */
    sizelabel = gtk_label_new(_("Size:"));
    gtk_label_set_xalign (GTK_LABEL (sizelabel), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), sizelabel, 0, 2, 1, 1);
    /* size spinner */
    spinner_adj = gtk_adjustment_new (search->size, 2.0, 30.0, 1.0, 5.0, 0);
    search->size_spinner = gtk_spin_button_new(GTK_ADJUSTMENT(spinner_adj), 1.0, 0);
    gtk_widget_set_halign (GTK_WIDGET (search->size_spinner), GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(search->size_spinner), 1, 2, 1, 1);
    g_signal_connect (GTK_WIDGET(search->size_spinner), "value-changed", G_CALLBACK (entry_size_changed_cb), search);

    gtk_widget_show_all(search->opt_dialog);
    gtk_dialog_run (GTK_DIALOG(search->opt_dialog));
    gtk_widget_destroy(search->opt_dialog);
    xfce_panel_plugin_unblock_menu(plugin);
    search_write_config(plugin, search);
}


/* create plugin widgets and connect to signals */

static void
smartbookmark_construct(XfcePanelPlugin *plugin)
{
    t_search *search = search_new(plugin);
    DBG ("Creating SmartBookMark");
    xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

    g_signal_connect (plugin, "size-changed",
                      G_CALLBACK (search_set_size), search);

    g_signal_connect (plugin, "save",
                      G_CALLBACK (search_write_config), search);

    xfce_panel_plugin_menu_show_about (plugin);
    g_signal_connect (plugin, "about",
                      G_CALLBACK (show_about), search);

    xfce_panel_plugin_menu_show_configure (plugin);
    g_signal_connect (plugin, "configure-plugin",
                      G_CALLBACK (search_create_options), search);
}
