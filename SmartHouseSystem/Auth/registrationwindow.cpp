#include "registrationwindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>
#include <QJsonObject>
#include "networkmanager.h"

RegistrationWindow::RegistrationWindow(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Введите имя пользователя");

    passwordInput = new QLineEdit(this);
    passwordInput->setPlaceholderText("Введите пароль");
    passwordInput->setEchoMode(QLineEdit::Password);

    confirmPasswordInput = new QLineEdit(this);
    confirmPasswordInput->setPlaceholderText("Подтвердите пароль");
    confirmPasswordInput->setEchoMode(QLineEdit::Password);

    roleComboBox = new QComboBox(this);
    roleComboBox->addItem("admin");
    roleComboBox->addItem("user");

    registerButton = new QPushButton("Зарегистрироваться", this);
    connect(registerButton, &QPushButton::clicked, this, &RegistrationWindow::onRegisterClicked);

    backToLoginButton = new QPushButton("Назад к входу", this);
    connect(backToLoginButton, &QPushButton::clicked, this, [this]() {
        emit goBackToLogin();
        this->close();

    });

    buttonFrame = new QFrame(this);
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonFrame);

    buttonLayout->addWidget(usernameInput);
    buttonLayout->addWidget(passwordInput);
    buttonLayout->addWidget(confirmPasswordInput);
    buttonLayout->addWidget(roleComboBox);
    buttonLayout->addWidget(registerButton);
    buttonLayout->addWidget(backToLoginButton);

    layout->addWidget(buttonFrame);

    setAutoFillBackground(true);
    QPalette palette = this->palette();
    setPalette(palette);

    setStyleSheet("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,  "
                  "stop: 0.5 #333464, stop: 1 #7a54a6);");


    buttonFrame->setStyleSheet("QFrame { background-color: rgb(191, 161, 249, 50); border-radius: 10px; padding: 40px; margin: 70px}");

    usernameInput->setStyleSheet("QLineEdit {"
                                 "background-color: rgb(191, 161, 249, 50); "
                                 "border-radius: 15px;"
                                 "color: white;"
                                 "padding: 10px;"
                                 "}");
    passwordInput->setStyleSheet("QLineEdit {"
                                 "background-color: rgb(191, 161, 249, 50); "
                                 "border-radius: 15px;"
                                 "color: white;"
                                 "padding: 10px;"
                                 "}");

    confirmPasswordInput->setStyleSheet("QLineEdit {"
                                        "background-color: rgb(191, 161, 249, 50); "
                                        "border-radius: 15px;"
                                        "color: white;"
                                        "padding: 10px;"
                                        "}");

    registerButton->setStyleSheet("QPushButton {"
                                  "background-color: #c8a7ff; "
                                  "border-radius: 15px;"
                                  "padding: 10px;"
                                  "font:  bold 14px  'New york';"
                                  "}");
    roleComboBox->setStyleSheet("QComboBox {"
                                "background-color: rgb(191, 161, 249, 50); "
                                "border-radius: 15px;"
                                "color: white;"
                                "padding: 10px;"
                                "}");
    backToLoginButton->setStyleSheet("QPushButton {"
                                     "background-color: #c8a7ff; "
                                     "border-radius: 15px;"
                                     "padding: 10px;"
                                     "font:  bold 14px  'New york';"
                                     "}");
}


void RegistrationWindow::onRegisterClicked()
{
    QString username = usernameInput->text();
    QString password = passwordInput->text();
    QString confirmPassword = confirmPasswordInput->text();
    QString role = roleComboBox->currentText();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Имя пользователя и пароль не могут быть пустыми!");
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают!");
        return;
    }
    QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString hashedPassword = QString(passwordHash.toHex());
    QJsonObject request;
    request["action"] = "register";
    request["username"] = username;
    request["password"] = hashedPassword;
    request["role"] = role;

    connect(&NetworkManager::instance(), &NetworkManager::responseReceived,
            this, &RegistrationWindow::handleRegistrationResponse);
    NetworkManager::instance().sendRequest(request);
}

void RegistrationWindow::handleRegistrationResponse(const QJsonObject &response) {
    if (response["success"].toBool()) {
        QMessageBox::information(this, "Регистрация", response["message"].toString());
        emit registrationSuccess();
        this->close();
    } else {
        QMessageBox::critical(this, "Ошибка", response["message"].toString());
    }
}


