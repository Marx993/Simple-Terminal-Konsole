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
    GtkWidget *window, *terminal;

    // Initialize GTK before creating the terminal widget
    gtk_init(&argc, &argv);

    // Create the terminal widget after GTK initialization
    terminal = vte_terminal_new();
    if (!terminal) {
        g_printerr("Failed to create terminal widget\n");
        return 1;
    }

    // Create the window widget
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "STK");

    // Connect the key-press-event signal after the terminal is created
    g_signal_connect(terminal, "key-press-event", G_CALLBACK(on_key_press), NULL);

    // Enable mouse autohide feature
    vte_terminal_set_mouse_autohide(VTE_TERMINAL(terminal), TRUE);

    // Set up the shell environment and command to run in the terminal
    gchar **envp = g_get_environ();
    const char *shell = g_environ_getenv(envp, "SHELL");
    if (!shell) shell = "/bin/sh";
    gchar *command[] = { g_strdup(shell), NULL };
    g_strfreev(envp);

    // Spawn the shell inside the terminal
    vte_terminal_spawn_async(VTE_TERMINAL(terminal),
        VTE_PTY_DEFAULT,
        NULL,
        command,
        NULL,
        (GSpawnFlags)0,
        NULL, NULL,
        NULL,
        -1,
        NULL,
        child_ready,
        NULL);

    // Set up GTK signal to close the window and quit the GTK main loop
    g_signal_connect(window, "delete-event", gtk_main_quit, NULL);
    g_signal_connect(terminal, "child-exited", gtk_main_quit, NULL);

    // Add the terminal widget to the window
    gtk_container_add(GTK_CONTAINER(window), terminal);

    // Display the window and all its contents
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}
.
