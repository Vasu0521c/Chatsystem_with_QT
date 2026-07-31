#include <gtk/gtk.h>
#include <glib-unix.h>

#include "client.h"

#include <stdio.h>

struct data {

    int source_id,
        server_fd,
        msg_count,
        msg_status;

    GtkWidget *label,
              *scroll_window,
              *connect_button,
              *disconnect_button,
              *v_box, 
              *ip_entry,
              *port_entry,
              *message_entry,
              *i_pan;
};


int connect_to_server([[maybe_unused]]GtkButton *button,      gpointer parameters);
int send_msg_to_server([[maybe_unused]]GtkButton *button,     gpointer parameters);
int disconnect_from_server([[maybe_unused]]GtkButton *button, gpointer parameters);

int receive_message(int fd, GIOCondition condition, gpointer data);
void display_message(struct data *temp,   char *msg);


int connect_to_server([[maybe_unused]]GtkButton *button, gpointer parameters) {
    
    struct data *temp = (struct data *) parameters;

    const char *ip_address  = gtk_editable_get_text(GTK_EDITABLE(temp -> ip_entry));
    const char *char_port   = gtk_editable_get_text(GTK_EDITABLE(temp -> port_entry));
    
    int port_number = atoi(char_port);
    int server_id   = server_connect((char *)ip_address, port_number);

    if (server_id == -1) {
        printf("server connection failed\n"
                     "Try again");
        return -1;
    }
    temp -> server_fd = server_id;
    gtk_label_set_text(GTK_LABEL(temp -> label), "Connected to Server");

    gtk_widget_set_visible(temp -> i_pan,             1);
    gtk_widget_set_visible(temp -> connect_button,    0);
    gtk_widget_set_visible(temp -> disconnect_button, 1);
    
    temp -> source_id = g_unix_fd_add(server_id, G_IO_IN | G_IO_HUP | G_IO_ERR, receive_message, parameters);

    return 0;
}

int send_msg_to_server([[maybe_unused]]GtkButton *button, gpointer parameters) {

    struct data *temp = (struct data *)parameters;
    int server_id     = temp -> server_fd;

    if (server_id == -1 || server_id == 0) {
        printf("Check connection to server and try again\n");
        return -1;
    }

    int  length        = gtk_entry_get_text_length(GTK_ENTRY(temp -> message_entry));
    const char *msg    = gtk_editable_get_text(GTK_EDITABLE(temp -> message_entry));
    temp -> msg_status = 0;

    send_msg(server_id, (char *)msg, length);

    display_message(temp, (char *)msg);

    gtk_label_set_text(GTK_LABEL(temp -> label), "Messege Sending...");
    gtk_label_set_text(GTK_LABEL(temp -> label), "Message Sent");
    return 0;
}

void clear_messages(struct data *temp) {

    int x = temp->msg_count;
    while (x--) {
        gtk_box_remove(GTK_BOX(temp -> v_box), gtk_widget_get_first_child(temp -> v_box));
    }
    temp -> msg_count = 0;
}

int disconnect_from_server([[maybe_unused]]GtkButton *button, gpointer parameters) {

    struct data *temp = (struct data *)parameters;

    g_source_remove(temp -> source_id);

    close(temp -> server_fd);

    gtk_widget_set_visible(temp -> i_pan,             0);
    gtk_widget_set_visible(temp -> connect_button,    1);
    gtk_widget_set_visible(temp -> disconnect_button, 0);

    clear_messages(temp);

    gtk_label_set_text(GTK_LABEL(temp -> label), "Disconnected from server");
    gtk_label_set_text(GTK_LABEL(temp -> label), "Client Status");
    return 0;
}

int receive_message(int fd, GIOCondition condition, gpointer data) {

    if(condition & (G_IO_HUP | G_IO_ERR)) {
        close(fd);
        return 0;
    }
    struct data *temp  = (struct data *)data;
    temp -> msg_status = 1;

    char *msg = recv_msg(fd);
    display_message((struct data *)data, msg);
    
    return 1;
}

void display_message(struct data *temp, char *msg) {

    GtkWidget *text_box;
    GtkWidget *sender_box;

    text_box = gtk_label_new(msg);

    sender_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    if(temp -> msg_status == 0) {
        gtk_widget_set_halign(sender_box, GTK_ALIGN_END);
        gtk_editable_set_text(GTK_EDITABLE(temp -> message_entry), "");
    } else {
        gtk_widget_set_halign(sender_box, GTK_ALIGN_START);
    }

    temp -> msg_count++;
    
    gtk_widget_set_margin_bottom(sender_box, 10);
    gtk_widget_set_margin_start(sender_box,  10);
    gtk_widget_set_margin_end(sender_box,    30);
    gtk_widget_set_margin_top(sender_box,    10);

    gtk_box_append(GTK_BOX(sender_box),    text_box);
    gtk_box_append(GTK_BOX(temp -> v_box), sender_box);
}

int client_window(GtkApplication *app) {

    //Window creation
    GtkWidget *main_window;
    GtkWidget *scrollable_window;
    
    //Grids and button
    GtkWidget *box;
    GtkWidget *grid;
    GtkWidget *input_box;
    GtkWidget *connect_server;
    GtkWidget *disconnect_server;
    GtkWidget *send_msg;

    //Entries for control window
    GtkWidget *ip_address_entry;
    GtkWidget *port_number_entry;

    //labels for control window
    GtkWidget *client;
    GtkWidget *ip_address_label;
    GtkWidget *port_number_label;

    //input string for sending messeges
    GtkWidget *input_entry;

    //pan to split the window
    GtkWidget *control_pan;
    GtkWidget *interact_pan;

    ip_address_entry  = gtk_entry_new();
    port_number_entry = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(ip_address_entry), "127.0.0.1");
    gtk_editable_set_text(GTK_EDITABLE(port_number_entry), "17112");

    gtk_entry_set_placeholder_text(GTK_ENTRY(ip_address_entry),  "Ex : 167.12.32.11");
    gtk_entry_set_placeholder_text(GTK_ENTRY(port_number_entry), "Ex : 13000");

    main_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(main_window), "client");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1200, 800);

    // scrollable window for chat
    scrollable_window = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrollable_window, 1);

    control_pan = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_position(GTK_PANED(control_pan), 300);

    gtk_paned_set_resize_start_child(GTK_PANED(control_pan), 0);
    gtk_paned_set_shrink_start_child(GTK_PANED(control_pan), 0);
    gtk_paned_set_shrink_end_child(GTK_PANED(control_pan),   0);

    interact_pan = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_paned_set_position(GTK_PANED(interact_pan), 700);
    gtk_paned_set_shrink_start_child(GTK_PANED(interact_pan), FALSE);
    gtk_paned_set_resize_start_child(GTK_PANED(interact_pan), FALSE);
    gtk_paned_set_shrink_end_child(GTK_PANED(interact_pan),   FALSE);
    gtk_paned_set_resize_end_child(GTK_PANED(interact_pan),   FALSE);
    
    gtk_paned_set_position(GTK_PANED(interact_pan), 700);

    gtk_widget_set_visible(interact_pan, 0);

    //labels for user knowledge
    client            = gtk_label_new("Client Status");
    ip_address_label  = gtk_label_new("IP address   ");
    port_number_label = gtk_label_new("Port Number  ");

    gtk_widget_set_margin_bottom(GTK_WIDGET(client), 20);

    //buttons with label for functions
    connect_server    = gtk_button_new_with_label("Connect to Server");
    send_msg          = gtk_button_new_with_label("Send");
    disconnect_server = gtk_button_new_with_label("Disconnect Server");

    gtk_widget_set_size_request(send_msg, 100, 60);
    gtk_widget_set_halign(send_msg, GTK_ALIGN_END);

    gtk_widget_set_visible(disconnect_server, 0);
    
    input_entry = gtk_entry_new();
    gtk_widget_set_hexpand(input_entry, 1);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    //A struct for transmissiting data between functions
    struct data *parameters      = malloc(sizeof(struct data));
    memset(parameters, 0, sizeof(struct data));

    parameters -> label          = client;
    parameters -> ip_entry       = ip_address_entry;
    parameters -> port_entry     = port_number_entry;
    parameters -> message_entry  = input_entry;
    parameters -> i_pan          = interact_pan;
    parameters -> v_box          = box;
    parameters -> scroll_window  = scrollable_window;
    parameters -> connect_button = connect_server;
    parameters -> disconnect_button = disconnect_server;
    

    g_signal_connect(connect_server,    "clicked", G_CALLBACK(connect_to_server),      parameters);
    g_signal_connect(send_msg,          "clicked", G_CALLBACK(send_msg_to_server),     parameters);
    g_signal_connect(disconnect_server, "clicked", G_CALLBACK(disconnect_from_server), parameters);


    grid = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    gtk_widget_set_margin_bottom(grid, 20);
    gtk_widget_set_margin_start(grid,  20);
    gtk_widget_set_margin_end(grid,    20);
    gtk_widget_set_margin_top(grid,    20);

    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    gtk_box_append(GTK_BOX(grid), client);

    gtk_box_append(GTK_BOX(grid), ip_address_label);
    gtk_box_append(GTK_BOX(grid), ip_address_entry);

    gtk_box_append(GTK_BOX(grid), port_number_label);
    gtk_box_append(GTK_BOX(grid), port_number_entry);

    gtk_box_append(GTK_BOX(grid), connect_server);
    gtk_box_append(GTK_BOX(grid), disconnect_server);


    input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    gtk_widget_set_margin_bottom(input_box, 20);
    gtk_widget_set_margin_start(input_box,  20);
    gtk_widget_set_margin_end(input_box,    20);
    gtk_widget_set_margin_top(input_box,    20);

    gtk_box_append(GTK_BOX(input_box), input_entry);
    gtk_box_append(GTK_BOX(input_box), send_msg);


    //window assignments
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollable_window), box);

    gtk_paned_set_start_child(GTK_PANED(control_pan), grid);
    gtk_paned_set_end_child(GTK_PANED(control_pan),   interact_pan);

    gtk_paned_set_start_child(GTK_PANED(interact_pan),scrollable_window);
    gtk_paned_set_end_child(GTK_PANED(interact_pan),  input_box);


    gtk_window_set_child(GTK_WINDOW(main_window),    control_pan);
    gtk_window_present(GTK_WINDOW(main_window));
    
    return 0;
}

int main() {

    GtkApplication *app;
    [[maybe_unused]]int status;

    app = gtk_application_new("mychat.client.window", G_APPLICATION_DEFAULT_FLAGS); 
    g_signal_connect(app, "activate", G_CALLBACK(client_window), NULL);

    status = g_application_run(G_APPLICATION(app), 0, 0);
    g_object_unref(app);
    return 0; 
}
