#include "registrationwindow.h"
#include "databasemanager.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>

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

    layout->addWidget(usernameInput);
    layout->addWidget(passwordInput);
    layout->addWidget(confirmPasswordInput);
    layout->addWidget(roleComboBox);
    layout->addWidget(registerButton);
    layout->addWidget(backToLoginButton);
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

    if (DatabaseManager::instance().registerUser(username, password, role)) {
        QMessageBox::information(this, "Регистрация", "Вы успешно зарегистрировались!");
        emit registrationSuccess();
        this->close();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось зарегистрировать пользователя. Попробуйте еще раз.");
    }
}

bool RegistrationWindow::userExists(const QString &username)
{
    if (!db.isOpen()) {
        return false;
    }
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }

    QMessageBox::critical(this, "Database Error", "Failed to check user existence: " + query.lastError().text());
    return false;
}

bool RegistrationWindow::registerUser(const QString &username, const QString &password, const QString &role)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, password, role) VALUES (:username, :password, :role)");
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue(role);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", "Failed to register user: " + query.lastError().text());
        return false;
    }

    return true;
}
