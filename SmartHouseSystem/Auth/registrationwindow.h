#ifndef REGISTRATIONWINDOW_H
#define REGISTRATIONWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSqlDatabase>

class RegistrationWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationWindow(QWidget *parent = nullptr);

signals:
    void goBackToLogin();
    void registrationSuccess();

private slots:
    void onRegisterClicked();
    void handleRegistrationResponse(const QJsonObject &response);

private:
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
    QComboBox *roleComboBox;
    QLineEdit *confirmPasswordInput;
    QPushButton *registerButton;
    QPushButton *backToLoginButton;
    QSqlDatabase db;
    QFrame *buttonFrame;
};

#endif // REGISTRATIONWINDOW_H
