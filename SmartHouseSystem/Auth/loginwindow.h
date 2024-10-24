#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>

#include "mainwindow.h"
#include "registrationwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginWindow;
}
QT_END_NAMESPACE

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    Ui::LoginWindow *ui;
    QLineEdit *loginLineEdit;
    QLineEdit *passwordLineEdit;
    QComboBox *roleComboBox;

    QLabel *errorLabel;

    QPushButton *loginButton;
    QPushButton *registerButton;

    MainWindow *mainWindow;
    RegistrationWindow *registrationWindow=nullptr;
};
#endif // LOGINAPP_H
