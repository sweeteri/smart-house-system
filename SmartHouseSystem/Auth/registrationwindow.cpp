#include "registrationwindow.h"
#include <QMessageBox>

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
    roleComboBox->addItem("Взрослый");
    roleComboBox->addItem("Ребенок");
    roleComboBox->addItem("Гость");

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

    if (password != confirmPassword) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают!");
        return;
    }

    QMessageBox::information(this, "Регистрация", "Вы успешно зарегистрировались!");

    emit registrationSuccess();
    this->close();
}
