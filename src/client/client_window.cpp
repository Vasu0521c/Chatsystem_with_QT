#include "qframe.h"
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

    QMainWindow window;

    QWidget *center = new QWidget();

    QFrame *options   = new QFrame();
    QFrame *msg_frame = new QFrame();

    QScrollArea *message_area = new QScrollArea();

    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    QLabel *status     = new QLabel("Client Status");
    QLabel *ip_label   = new QLabel("IP address");
    QLabel *port_label = new QLabel("Port Number");

    QLineEdit *ip_address  = new QLineEdit();
    QLineEdit *port_number = new QLineEdit();
    QLineEdit *message     = new QLineEdit();

    QVBoxLayout *connection_layout = new QVBoxLayout(options);
    QVBoxLayout *message_layout    = new QVBoxLayout(msg_frame);

    QHBoxLayout *msg_area = new QHBoxLayout();
    QVBoxLayout *messages = new QVBoxLayout();
    
    button *connect_      = new button("connect to server");
    button *disconnect_   = new button("disconnect");
    button *send_         = new button("send");

    window.setWindowTitle("Client");
    window.setMinimumSize(900, 900);
    window.resize(901,901);

    window.setCentralWidget(center);

    splitter -> setParent(center);
    splitter -> addWidget(options);
    splitter -> addWidget(msg_frame);
    
    options -> setFrameShape(QFrame::StyledPanel);
    options -> setFrameShadow(QFrame::Raised);

    msg_frame -> setFrameShape(QFrame::StyledPanel);
    msg_frame -> setFrameShadow(QFrame::Raised);

    message_layout -> addWidget(message_area, 1);
    message_layout -> addLayout(msg_area);
    
    ip_address  -> setPlaceholderText("Ex : 133.53.23.6");
    port_number -> setPlaceholderText("Ex : 18394");

    //connectivity controls
    connection_layout -> addWidget(status);

    connection_layout -> addWidget(ip_label);
    connection_layout -> addWidget(ip_address);

    connection_layout -> addWidget(port_label);
    connection_layout -> addWidget(port_number);

    connection_layout -> addWidget(connect_);
    connection_layout -> addWidget(disconnect_);


    // message controls
    msg_area -> addWidget(message, 1);
    msg_area -> addWidget(send_);

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
