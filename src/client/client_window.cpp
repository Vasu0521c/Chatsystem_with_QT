#include "qframe.h"
#include "qsizepolicy.h"
#include <QApplication>
#include <QtWidgets>
#include <unistd.h>

extern "C" {
    #include "client.h"
}

typedef QApplication qapp;
typedef QPushButton  button;

struct data {

    int           msg_status,
                  server_fd;

    QLabel       *status_label;
    QVBoxLayout  *vbox;

    QLineEdit    *ip,
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

void display_message(struct data *param) {

    int sts = param -> msg_status;

    return;
}

int main(int argc, char **argv) {

    qapp app(argc, argv);

    QMainWindow window;

    QWidget *center_widget = new QWidget();

    QFrame *options_frame  = new QFrame();
    QFrame *msg_frame      = new QFrame();

    QScrollArea *scroll_area = new QScrollArea();

    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    QLabel *status     = new QLabel("Client Status");
    QLabel *ip_label   = new QLabel("IP address");
    QLabel *port_label = new QLabel("Port Number");

    QLineEdit *ip_address  = new QLineEdit();
    QLineEdit *port_number = new QLineEdit();
    QLineEdit *message     = new QLineEdit();

    QHBoxLayout *main_layout    = new QHBoxLayout(center_widget);
    QVBoxLayout *options_layout = new QVBoxLayout(options_frame);
    QVBoxLayout *msg_layout     = new QVBoxLayout(msg_frame);

    QVBoxLayout *msgs_holder_box = new QVBoxLayout(scroll_area);
    QHBoxLayout *msg_field_area = new QHBoxLayout();
    
    button *connect_      = new button("connect to server");
    button *disconnect_   = new button("disconnect");
    button *send_         = new button("send");

    window.setWindowTitle("Client");
    window.setMinimumSize(900, 900);
    window.resize(901,901);

    window.setCentralWidget(center_widget);

    main_layout -> addWidget(splitter);

    splitter -> setChildrenCollapsible(0);
    splitter -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    splitter -> addWidget(options_frame);
    splitter -> addWidget(msg_frame);
    
    options_frame -> setFrameShape(QFrame::StyledPanel);
    options_frame -> setFrameShadow(QFrame::Raised);

    msg_frame  -> setFrameShape(QFrame::StyledPanel);
    msg_frame  -> setFrameShadow(QFrame::Raised);
    msg_frame  -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    msg_layout -> addWidget(scroll_area, 1);
    msg_layout -> addLayout(msg_field_area,1);

    ip_address  -> setPlaceholderText("Ex : 133.53.23.6");
    port_number -> setPlaceholderText("Ex : 18394");

    //connectivity controls
    options_layout -> setAlignment(Qt::AlignCenter);
    options_layout -> addWidget(status);

    options_layout -> addWidget(ip_label);
    options_layout -> addWidget(ip_address);

    options_layout -> addWidget(port_label);
    options_layout -> addWidget(port_number);

    options_layout -> addWidget(connect_);
    options_layout -> addWidget(disconnect_);


    for (int i = 0; i < 7; i++) {
        options_layout -> setStretch(i, 0);
    }

    // message controls
    msg_field_area -> addWidget(message, 1);
    msg_field_area -> addWidget(send_);

    struct data *parameters    = new struct data;
    parameters -> status_label = status;
    parameters -> ip           = ip_address;
    parameters -> port         = port_number;
    parameters -> msg          = message;
    parameters -> vbox         = msgs_holder_box;

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
