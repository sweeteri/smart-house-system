#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include "registrationwindow.h"

class MainWindow;

class LoginWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void login_success(const QString &role);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void handleLoginResponse(const QJsonObject &response);

public slots:
    void showLoginWindow();
private:
    QLineEdit *loginLineEdit;
    QLineEdit *passwordLineEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QLabel *errorLabel;

    RegistrationWindow *registrationWindow;
};

#endif // LOGINWINDOW_H
