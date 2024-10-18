#include "loginwindow.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QApplication>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent), mainWindow(nullptr)

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

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(loginLineEdit);
    layout->addWidget(passwordLineEdit);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);
    layout->addWidget(errorLabel);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    setGeometry(200, 200, 300, 300);
}

LoginWindow::~LoginWindow(){
    if (mainWindow) {
        delete mainWindow;
    }
}

void LoginWindow::onLoginClicked()
{
    QString login = loginLineEdit->text();
    QString password = passwordLineEdit->text();

    if (login == "admin" && password == "1234") {
        errorLabel->clear();

        if(!mainWindow){
            mainWindow = new MainWindow(nullptr);
            connect(mainWindow, &MainWindow::backToMain, this, [this]() {
                this->show();
                mainWindow->hide();
            });
        }

        mainWindow->show();
        QApplication::setActiveWindow(mainWindow);
        this->hide();
    } else {
        errorLabel->setText("Неправильный логин или пароль. Попробуйте еще раз.");
    }
}


void LoginWindow::onRegisterClicked()
{
    if (!registrationWindow) {
        registrationWindow = new RegistrationWindow();
        connect(registrationWindow, &RegistrationWindow::goBackToLogin, this, [this]() {
            this->show();
        });
    }

    connect(registrationWindow, &RegistrationWindow::registrationSuccess, this, [this]() {
        this->show();
    });

    registrationWindow->show();
    this->hide();
}

