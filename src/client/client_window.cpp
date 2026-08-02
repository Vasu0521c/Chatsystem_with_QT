#include <QApplication>
#include <QtWidgets>
#include <unistd.h>

extern "C" {
    #include "client.h"
}

typedef QApplication qapp;
typedef QPushButton  button;

struct data {

    int       server_fd;
    QLabel    *status_label;
    QLineEdit *ip,
              *msg,
              *port;
};

int connect_server(struct data *param) {

    if (param -> server_fd >= 0) {
        param ->  status_label -> setText("Server is already connected");
        return 0;
    }

    std::string temp_str = param -> ip -> text().toStdString();
    char *ip_address     = (char *)temp_str.c_str();
    int port_number      = stoi(param -> port -> text().toStdString());
    int status = server_connect(ip_address, port_number);

    if (status <= 0) {
        param -> status_label -> setText("Connection Failed");
        return -1;
    }

    param -> status_label -> setText("Server Connected");
    param -> server_fd = status;
    return 0;
}

int disconnect_server(struct data *param) {

    if (param -> server_fd <= 0) {
        param -> status_label -> setText("No connection");
        return -1;
    }
    close(param -> server_fd);
    param -> server_fd = 0;

    return 0;
}

int send_message(struct data *param) {

    std::string temp = param -> msg -> text().toStdString();

    if (!temp.length()) {

        return -1;
    }

    int length    = temp.length();
    char *message = (char *)temp.c_str();

    send_msg(param -> server_fd, message, length - 1);
    param -> msg -> setText("");

    return 0;
}

int main(int argc, char **argv) {

    qapp app(argc, argv);

    QWidget window;
    QWidget *placeholder1 = new QWidget();
    QWidget *placeholder2 = new QWidget();

    QScrollArea *message_area = new QScrollArea();

    QLabel *status     = new QLabel("Client Status");
    QLabel *ip_label   = new QLabel("IP address");
    QLabel *port_label = new QLabel("Port Number");

    QSplitter *horizontal_split = new QSplitter();
    QSplitter *vertical_split   = new QSplitter();

    QLineEdit *ip_address  = new QLineEdit();
    QLineEdit *port_number = new QLineEdit();
    QLineEdit *message     = new QLineEdit();

    QVBoxLayout *connection_layout = new QVBoxLayout();
    QHBoxLayout *message_layout    = new QHBoxLayout();
    
    button *connect_    = new button("connect to server");
    button *disconnect_ = new button("disconnect");
    button *send_       = new button("send");

    window.setWindowTitle("Client");
    window.setMinimumSize(900, 900);
    window.resize(901,901);

    horizontal_split -> setOrientation(Qt::Horizontal);
    horizontal_split -> setSizes({400, 400});
    horizontal_split -> setParent(&window);
    horizontal_split -> addWidget(placeholder1);
    horizontal_split -> addWidget(vertical_split);
    
    vertical_split   -> setOrientation(Qt::Vertical);
    vertical_split   -> addWidget(message_area);
    vertical_split   -> addWidget(placeholder2);

    ip_address  -> setPlaceholderText("Ex : 133.53.23.6");
    port_number -> setPlaceholderText("Ex : 18394");

    message     -> setMinimumSize(300, 50);

    message     -> setTextMargins(10, 10, 10, 10);
    message     -> setContentsMargins(10, 10, 10, 10);

    placeholder1      -> setLayout(connection_layout);
    placeholder1      -> setMinimumSize(400, 400);
    placeholder1      -> setContentsMargins(10, 10, 10, 10);
    placeholder1      -> resize(401, 401);
    placeholder2      -> setLayout(message_layout);

    connection_layout -> setParent(placeholder1);
    connection_layout -> setSpacing(10);
    connection_layout -> setContentsMargins(10, 10, 10, 10);
    //connectivity controls
    connection_layout -> addWidget(status);

    connection_layout -> addWidget(ip_label);
    connection_layout -> addWidget(ip_address);

    connection_layout -> addWidget(port_label);
    connection_layout -> addWidget(port_number);

    connection_layout -> addWidget(connect_);
    connection_layout -> addWidget(disconnect_);

    //message actions
    message_layout    -> addWidget(message);
    message_layout    -> addWidget(send_);

    struct data *parameters    = new struct data;
    parameters -> status_label = status;
    parameters -> ip           = ip_address;
    parameters -> port         = port_number;
    parameters -> msg          = message;

    qapp::connect(connect_, &button::clicked,
                  [&parameters]() { connect_server(parameters); }
    );

    qapp::connect(disconnect_, &button::clicked,
                  [&parameters]() { disconnect_server(parameters); }
    );

    qapp::connect(send_, &button::clicked,
                  [&parameters]() { send_message(parameters); }
    );
    
    window.show();
    return app.exec();
}
