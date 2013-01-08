#include <gtk/gtk.h>
#include <lunar-calendar/lunar-calendar.h>

int main(int argc, char **argv)
{
    GtkWidget *window = NULL;
    GtkWidget *align = NULL;
    GtkWidget *calendar = NULL;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "DtkCalendar Demo");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    align = gtk_alignment_new(0.0, 0.0, 0.0, 0.0);
    gtk_alignment_set_padding(align, 10, 10, 10, 10);
    calendar = lunar_calendar_new();
    gtk_widget_set_size_request(calendar, 380, 280);
    gtk_container_add(GTK_CONTAINER(align), calendar);
    gtk_container_add(GTK_CONTAINER(window), align);

    gtk_widget_show(calendar);
    gtk_widget_show(align);
    gtk_widget_show(window);
  
    gtk_main();

    return 0;
}
