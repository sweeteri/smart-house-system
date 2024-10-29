#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMap>
#include <QString>
#include <QVector>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void backToMain();

private slots:
    void onScenarioButtonClicked();
    void onAllDevicesButtonClicked();
    void onAddRoomButtonClicked();
    void onAddDeviceButtonClicked();
    void onAddScenarioButtonClicked();


private:
    void clearDisplay();
    void updateDisplay();
    void displayItemsInGrid(const QVector<QString> &items);
    void loadRoomsFromDatabase();
    void loadDevicesFromDatabase();
    bool addRoomToDatabase(const QString &roomName);
    bool addDeviceToRoom(const QString &roomName, const QString &deviceName);
    void clearRoomButtons();
    bool addScenario(const QString &scenarioName);
    QPushButton *logoutButton;
    QPushButton *scenarioButton;
    QPushButton *allDevicesButton;
    QPushButton *addRoomButton;
    QPushButton *addDeviceButton;
    QPushButton *addScenarioButton;

    QWidget *displayWidget;
    QGridLayout *gridLayout;
    QVBoxLayout *roomButtonsLayout;
    QWidget *sideMenu;
    QVBoxLayout *sideMenuLayout;
    QVBoxLayout *devicesButtonsLayout;

    QString currentRoom;
    QMap<QString, QVector<QString>> roomDevices;

};

#endif // MAINWINDOW_H
