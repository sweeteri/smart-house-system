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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void backToMain();

private slots:
    void onLogoutButtonClicked();
    void onScenarioButtonClicked();
    void onAllDevicesButtonClicked();
    void onBedroomButtonClicked();
    void onLivingRoomButtonClicked();
    void onKidsRoomButtonClicked();
    void onHallwayButtonClicked();

private:
    void clearDisplay();
    void displayItemsInGrid(const QVector<QString> &items);

    QPushButton *logoutButton;
    QPushButton *scenarioButton;
    QPushButton *allDevicesButton;
    QPushButton *bedroomButton;
    QPushButton *livingRoomButton;
    QPushButton *kidsRoomButton;
    QPushButton *hallwayButton;

    QMap<QString, QVector<QString>> roomDevices;
    QVector<QString> scenarios;

    QWidget *displayWidget;
    QGridLayout *gridLayout;
};

#endif // MAINWINDOW_H
