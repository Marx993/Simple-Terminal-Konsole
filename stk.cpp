#include <gtk/gtk.h>
#include <vte/vte.h>

static void
child_ready(VteTerminal *terminal, GPid pid, GError *error, gpointer user_data)
{
    if (!terminal) return;
    if (pid == -1) gtk_main_quit();
}

static gboolean
on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    VteTerminal *term = VTE_TERMINAL(widget);

    if ((event->state & GDK_CONTROL_MASK) &&
        (event->state & GDK_SHIFT_MASK) &&
        event->keyval == GDK_KEY_C)
    {
        vte_terminal_copy_clipboard_format(term, VTE_FORMAT_TEXT);
        return TRUE;
    }

    if ((event->state & GDK_CONTROL_MASK) &&
        (event->state & GDK_SHIFT_MASK) &&
        event->keyval == GDK_KEY_V)
    {
        vte_terminal_paste_clipboard(term);
        return TRUE;
    }

    return FALSE;
}

int
main(int argc, char *argv[])
{
    GtkWidget *window;
    VteTerminal *term;

    /* Initialize GTK before creating the terminal widget */
    gtk_init(&argc, &argv);

    /* Create the window widget */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "STK");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    /* Create the terminal widget after GTK initialization */
    term = VTE_TERMINAL(vte_terminal_new());
    if (!term) {
        g_printerr("Failed to create terminal widget\n");
        return 1;
    }

    /* Connect the key-press-event signal after the terminal is created */
    g_signal_connect(term, "key-press-event", G_CALLBACK(on_key_press), NULL);

    /* Enable mouse autohide feature */
    vte_terminal_set_mouse_autohide(term, TRUE);

    /* Set up the shell environment and command to run in the terminal */
    const char *shell = g_getenv("SHELL");
    if (!shell) shell = "/bin/sh";

    gchar *child_argv[2];
    child_argv[0] = g_strdup(shell);
    child_argv[1] = NULL;

    /* Spawn the shell inside the terminal */
    vte_terminal_spawn_async(term,
        VTE_PTY_DEFAULT,
        NULL,
        (char **)child_argv,
        NULL,
        G_SPAWN_DEFAULT,
        NULL, NULL,
        NULL,
        -1,
        NULL,
        (GAsyncReadyCallback)child_ready,
        NULL);

    /* free duplicated argv first element (child was spawned already) */
    g_free(child_argv[0]);

    /* Set up GTK signal to close the window and quit the GTK main loop */
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(term, "child-exited", G_CALLBACK(gtk_main_quit), NULL);

    /* Add the terminal widget to the window */
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(term));

    /* Display the window and all its contents */
    gtk_widget_show_all(window);

    /* Start the GTK main loop */
    gtk_main();

    return 0;
}
