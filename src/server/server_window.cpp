#include <QApplication>
#include <QtWidgets>

extern "C" {
    #include "server.h"
}

typedef QApplication Qapp;
typedef QPushButton  button;

struct data {

    int        server_fd;
    QLabel    *sts;
    QLineEdit *port_l;
};


int connect_to_server(struct data *param) {

    if (param->server_fd > 0) {
        param -> sts -> setText("Server is already running");
        return 0;
    }

    std::string port = param -> port_l -> text().toStdString();

    int port_num = stoi(port);
    int status   = server_start(port_num);

    if (status <= 0) {
        param -> port_l -> setText("");
        param -> sts ->    setText("Connection Failed");
        return -1;
    }

    param -> sts -> setText("Server Connected");
    param -> server_fd = status;
    return 0;
}

int disconnect_from_server(struct data *param) {

    if (param -> server_fd <= 0) {
        param -> sts -> setText("Server is not connected");
        param -> sts -> setText("Server status");
        return -1;
    }

    server_stop(param -> server_fd);
    param -> sts -> setText("Server Closed");
    param -> server_fd = 0;

    return 0;
}

int reset_server(struct data *param) {

    if (param->server_fd <= 0) {
        param -> sts -> setText("Server is not connected");
        return -1;
    }

    server_stop(param -> server_fd);
    param -> sts -> setText("Server status");
    param -> port_l -> clear();

    return 0;
}

int main(int argc, char **argv) {

    Qapp app(argc, argv);

    QWidget window;
    window.resize(800, 600);
    window.setWindowTitle("Client");
    window.show();

    QVBoxLayout *server_box = new QVBoxLayout(&window);

    QLabel *status          = new QLabel(
        Qapp::translate("","Server Status"));

    QLabel *port_label      = new QLabel(
        Qapp::translate("","Port Number"));

    button *connect_server  = new button(
        Qapp::translate("Client","Start Server"));

    button *disconnect      = new button(
        Qapp::translate("Client","Stop Server"));

    button *reset           = new button(
        Qapp::translate("Client","Reset Server"));

    QLineEdit *port = new QLineEdit;
    port -> setPlaceholderText("Ex : 25536");

    struct data *parameters = new struct data;
    parameters -> server_fd = 0;
    parameters -> sts       = status;
    parameters -> port_l    = port;
    
    Qapp::connect(connect_server, &button::clicked,
                  [&parameters]() {connect_to_server(parameters); }
    );

    Qapp::connect(disconnect, &button::clicked,
                  [&parameters]() {disconnect_from_server(parameters); }
    );

    Qapp::connect(reset, &button::clicked,
                  [&parameters]() {reset_server(parameters); }
    );

    server_box -> addWidget(status);
    server_box -> addWidget(port_label);
    server_box -> addWidget(port);
    server_box -> addWidget(connect_server);
    server_box -> addWidget(disconnect);
    server_box -> addWidget(reset);

    window.setLayout(server_box);

    return app.exec();
}
