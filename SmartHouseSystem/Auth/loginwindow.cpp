#include "loginwindow.h"
#include "registrationwindow.h"
#include "networkmanager.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QDir>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent), registrationWindow(nullptr)
{
    loginLineEdit = new QLineEdit(this);
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("Войти", this);

    errorLabel = new QLabel(this);

    loginLineEdit->setPlaceholderText("Логин");
    passwordLineEdit->setPlaceholderText("Пароль");

    registerButton = new QPushButton("Зарегистрироваться", this);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    QFrame *buttonFrame = new QFrame(this);
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonFrame);
    buttonLayout->addWidget(loginLineEdit);
    buttonLayout->addWidget(passwordLineEdit);
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    buttonLayout->addWidget(errorLabel);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(buttonFrame);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    setGeometry(200, 200, 300, 300);

    centralWidget->setStyleSheet("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,  "
                                 "stop: 0.5 #333464, stop: 1 #7a54a6);");


    buttonFrame->setStyleSheet("QFrame { background-color: rgb(191, 161, 249, 50); border-radius: 10px; padding: 40px; margin: 70px}");

    loginLineEdit ->setStyleSheet("QLineEdit {"
                                 "background-color: rgb(191, 161, 249, 50);"
                                 "border-radius: 15px;"
                                 "color: white;"
                                 "padding: 10px;"
                                 "}");
    passwordLineEdit ->setStyleSheet("QLineEdit {"
                                    "background-color: rgb(191, 161, 249, 50); "
                                    "border-radius: 15px;"
                                    "color: white;"
                                    "padding: 10px;"
                                    "}");
    loginButton ->setStyleSheet("QPushButton {"
                               "background-color: #c8a7ff; "
                               "border-radius: 15px;"
                               "padding: 10px;"

                               "font:  bold 14px  'New york';"

                               "}");
    errorLabel ->setStyleSheet("QLabel {"
                                "background-color: rgb(191, 161, 249, 0); "
                              "border-radius: 15px;"
                              "padding: 10px;"
                              "font: bold 14px  'New york';"
                              "margin: 0;"
                              "}");
    registerButton ->setStyleSheet("QPushButton {"
                                  "background-color: #c8a7ff; "
                                  "border-radius: 15px;"
                                  "padding: 10px;"
                                  "font: bold 14px  'Oswald';"
                                  "}");
}

LoginWindow::~LoginWindow() {

}

void LoginWindow::showLoginWindow() {
    this->show();
}
void LoginWindow::onLoginClicked() {
    qDebug() << "Login button clicked. Attempting to send login request.";
    QString username = loginLineEdit->text();
    QString password = passwordLineEdit->text();
    QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString hashedPassword = QString(passwordHash.toHex());
    QJsonObject request;
    request["action"] = "login";
    request["username"] = username;
    request["password"] = hashedPassword;

    connect(&NetworkManager::instance(), &NetworkManager::responseReceived,
            this, &LoginWindow::handleLoginResponse);
    NetworkManager::instance().sendRequest(request);
}

void LoginWindow::onRegisterClicked() {
    if (!registrationWindow) {
        registrationWindow = new RegistrationWindow();
        connect(registrationWindow, &RegistrationWindow::goBackToLogin, this, [this]() {
            this->show();
        });
        connect(registrationWindow, &RegistrationWindow::registrationSuccess, this, [this]() {
            this->show();
        });
    }

    registrationWindow->show();
    this->hide();
}

void LoginWindow::handleLoginResponse(const QJsonObject &response) {
    if (response["authenticated"].toBool()) {
        errorLabel->clear();
        QString userRoleFromServer = response["role"].toString();
        qDebug() << "----IBUSKO---- LoginWindow::handleLoginRespons success";
        emit login_success(userRoleFromServer);
        this->hide();
        QMessageBox msgBox;
        msgBox.setText(response["message"].toString());
        msgBox.setWindowTitle("Success");
        msgBox.setStyleSheet("QMessageBox { background-color: #e7c9ef; }"
                             "QPushButton { background-color: #b3a2ee;""border-radius: 5px; }"
                             "QPushButton:hover { background-color: rgb(114, 7, 168, 40); }");
        msgBox.exec();

    } else {
        errorLabel->setText(response["message"].toString());
    }
}
