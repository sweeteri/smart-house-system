#include "loginwindow.h"
#include "mainwindow.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent), mainWindow(nullptr)
{
    // Setup UI components (unchanged)
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

LoginWindow::~LoginWindow() {
    if (mainWindow) {
        delete mainWindow;
    }
}


void LoginWindow::onLoginClicked() {
    QString username = loginLineEdit->text();
    QString password = passwordLineEdit->text();

    if (DatabaseManager::instance().authenticateUser(username, password)) {
        // Login successful
        errorLabel->clear();
        if (!mainWindow) {
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

bool LoginWindow::authenticateUser(const QString &username, const QString &password) {
    if (!db.isOpen()) {
        qDebug() << "Database is not open";
        return false;
    }
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос: " + query.lastError().text());
        return false;
    }

    if (query.next() && query.value(0).toInt() > 0) {
        return true;
    }
    qDebug() << "Authentication failed:" << query.lastError().text();

    return false;
}

void LoginWindow::onRegisterClicked() {
    // Existing registration logic (unchanged)
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
