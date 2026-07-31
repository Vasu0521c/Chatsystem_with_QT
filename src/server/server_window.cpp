#include "qboxlayout.h"
#include "qpushbutton.h"
#include <QApplication>
#include <QtWidgets>

typedef QApplication Qapp;
typedef QPushButton  button;

struct data {
    
    QLabel *sts;
};


int connect_to_server(struct data *param) {

    return 0;
}

int disconnect_from_server(struct data *param) {

    return 0;
}

int reset_server(struct data *param) {
    
    return 0;
}

int main(int argc, char **argv) {

    Qapp app(argc, argv);

    QWidget window;
    window.resize(800, 600);
    window.setWindowTitle("Client");
    window.show();

    QVBoxLayout *server_box = new QVBoxLayout(&window);

    QLabel *status = new QLabel(
        Qapp::translate("","Server Status"));

    button *connect_server = new button(
        Qapp::translate("Client","Connect to Server"));

    button *disconnect = new button(
        Qapp::translate("Client","Disconnect from Server"));

    button *reset= new button(
        Qapp::translate("Client","Reset Server"));

    struct data *parameters = new struct data;
    parameters -> sts = status;

    Qapp::connect(connect_server, &button::clicked,
                  [](struct data *parameters) {connect_to_server(parameters); }
    );

    Qapp::connect(disconnect, &button::clicked,
                  [](struct data *parameters) {disconnect_from_server(parameters); }
    );

    Qapp::connect(reset, &button::clicked,
                  [](struct data *parameters) {reset_server(parameters); }
    );

    server_box -> addWidget(status);
    server_box -> addWidget(connect_server);
    server_box -> addWidget(disconnect);
    server_box -> addWidget(reset);

    window.setLayout(server_box);

    status -> show();
    connect_server -> show();
    disconnect -> show();
    reset-> show();

    return app.exec();
}
