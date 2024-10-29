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

private:
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
    QComboBox *roleComboBox;
    QLineEdit *confirmPasswordInput;
    QPushButton *registerButton;
    QPushButton *backToLoginButton;
    QSqlDatabase db;

    bool userExists(const QString &username);
    bool registerUser(const QString &username, const QString &password, const QString &role);
};

#endif // REGISTRATIONWINDOW_H
