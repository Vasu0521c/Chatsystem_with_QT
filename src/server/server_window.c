#include <gtk/gtk.h>
#include "server.h"

struct data {
    int server_fd;
    GtkWidget *server_status_label,
              *port_number;
};

void button_server_start([[maybe_unused]]GtkWidget *button, gpointer parameters) {

   struct data *temp     = (struct data *)parameters;
   const char *port_temp = gtk_editable_get_text(GTK_EDITABLE(temp -> port_number));

   int port_number = atoi(port_temp);
   int status      = server_start(port_number);

   if (status == -1) {
       g_print("server connection failed\n");
       gtk_label_set_text(GTK_LABEL(temp -> server_status_label), "Server status : Failed");
   }
   else {
       g_print("server connected %d\n", status);
       gtk_label_set_text(GTK_LABEL(temp -> server_status_label), "Server status : Connected");
   }
   temp -> server_fd = status;
}

void button_server_stop(GtkWidget *button, gpointer parameters) {

   struct data *temp = (struct data *)parameters;
   int status = server_stop(temp -> server_fd);
   if (status != 0) {
       g_print("server still running\n");
       gtk_label_set_text(GTK_LABEL(temp -> server_status_label), "Server status : Server Still Running");
   }
   else {
       g_print("server stopped\n");
       gtk_label_set_text(GTK_LABEL(temp -> server_status_label), "Server status : Stopped");
   }
   (void)button;
}

void button_server_reset(GtkWidget *button, gpointer parameters) {

    struct data *temp = (struct data *)parameters;
    button_server_stop(button, parameters);
    gtk_label_set_text(GTK_LABEL(temp -> server_status_label),  "Server Status"); 
    gtk_editable_set_text(GTK_EDITABLE(temp -> port_number), "");
   (void)button;
}

int window(GtkApplication *app) {

    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *information;
    GtkWidget *port_number_label;
    GtkWidget *port_number_entry;
    GtkWidget *button_start;
    GtkWidget *button_stop;
    GtkWidget *button_reset;

    port_number_entry = gtk_entry_new();

    gtk_entry_set_placeholder_text(GTK_ENTRY(port_number_entry), "Ex : 13000");

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Server" );
    gtk_window_set_default_size(GTK_WINDOW(window),800 ,600);

    information       = gtk_label_new("Connection Status");
    port_number_label = gtk_label_new("Port Number : ");

    button_start = gtk_button_new_with_label("Start Server");
    button_stop  = gtk_button_new_with_label("Stop Server");
    button_reset = gtk_button_new_with_label("Reset server");

    struct data *parameters = malloc(sizeof(struct data));
    parameters -> server_status_label = information;
    parameters -> port_number         = port_number_entry;

    g_signal_connect(button_start, "clicked", G_CALLBACK(button_server_start), parameters);
    g_signal_connect(button_stop,  "clicked", G_CALLBACK(button_server_stop),  parameters);
    g_signal_connect(button_reset, "clicked", G_CALLBACK(button_server_reset), parameters);

    grid = gtk_grid_new();

    gtk_widget_set_margin_bottom(grid, 20);
    gtk_widget_set_margin_top(grid,    20);
    gtk_widget_set_margin_start(grid,  20);
    gtk_widget_set_margin_end(grid,    20);
    
    gtk_grid_set_column_spacing(GTK_GRID(grid), 25);
    gtk_grid_set_row_spacing(GTK_GRID(grid),    25);

    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    gtk_grid_attach(GTK_GRID(grid), information,       1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), port_number_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), port_number_entry, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), button_start,      1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button_stop,       1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button_reset,      1, 4, 1, 1);

    gtk_window_set_child(GTK_WINDOW(window), grid);

    gtk_window_present(GTK_WINDOW(window));

    return 0;
}

int main() {

    GtkApplication *app;
    int status __attribute__((unused));

    app = gtk_application_new("mychat.server.application",G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(window), NULL);

    status = (g_application_run(G_APPLICATION(app), 0, 0));
    g_object_unref(app);
    return 0;
}
