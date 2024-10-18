#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{

    roomDevices["Спальня"] = {"Лампа", "Кондиционер"};
    roomDevices["Гостиная"] = {"Телевизор", "Вентилятор"};
    roomDevices["Детская"] = {"Лампа", "Ночник"};
    roomDevices["Прихожая"] = {"Обогреватель", "Лампа"};

    scenarios = {"Наступила ночь", "Наступило утро", "Похолодало"};

    logoutButton = new QPushButton("Выйти", this);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutButtonClicked);

    scenarioButton = new QPushButton("Сценарии", this);
    allDevicesButton = new QPushButton("Все устройства", this);
    bedroomButton = new QPushButton("Спальня", this);
    livingRoomButton = new QPushButton("Гостиная", this);
    kidsRoomButton = new QPushButton("Детская", this);
    hallwayButton = new QPushButton("Прихожая", this);

    scenarioButton->setFixedSize(200, 50);
    allDevicesButton->setFixedSize(200, 50);
    bedroomButton->setFixedSize(200, 50);
    livingRoomButton->setFixedSize(200, 50);
    kidsRoomButton->setFixedSize(200, 50);
    hallwayButton->setFixedSize(200, 50);

    connect(scenarioButton, &QPushButton::clicked, this, &MainWindow::onScenarioButtonClicked);
    connect(allDevicesButton, &QPushButton::clicked, this, &MainWindow::onAllDevicesButtonClicked);
    connect(bedroomButton, &QPushButton::clicked, this, &MainWindow::onBedroomButtonClicked);
    connect(livingRoomButton, &QPushButton::clicked, this, &MainWindow::onLivingRoomButtonClicked);
    connect(kidsRoomButton, &QPushButton::clicked, this, &MainWindow::onKidsRoomButtonClicked);
    connect(hallwayButton, &QPushButton::clicked, this, &MainWindow::onHallwayButtonClicked);

    QWidget *sideMenu = new QWidget(this);
    QVBoxLayout *sideMenuLayout = new QVBoxLayout(sideMenu);
    sideMenuLayout->addWidget(scenarioButton);
    sideMenuLayout->addWidget(allDevicesButton);
    sideMenuLayout->addWidget(bedroomButton);
    sideMenuLayout->addWidget(livingRoomButton);
    sideMenuLayout->addWidget(kidsRoomButton);
    sideMenuLayout->addWidget(hallwayButton);
    sideMenuLayout->addStretch();

    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->addStretch();
    headerLayout->addWidget(logoutButton);

    displayWidget = new QWidget(this);
    gridLayout = new QGridLayout(displayWidget);
    displayWidget->setLayout(gridLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(sideMenu);
    mainLayout->addWidget(displayWidget);

    QVBoxLayout *windowLayout = new QVBoxLayout(this);
    windowLayout->addLayout(headerLayout);
    windowLayout->addLayout(mainLayout);

    setLayout(windowLayout);
    setWindowTitle("Умный дом");
    resize(800, 600);
}

MainWindow::~MainWindow() {}

void MainWindow::clearDisplay()
{
    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

void MainWindow::displayItemsInGrid(const QVector<QString> &items)
{
    clearDisplay();

    int row = 0, col = 0;
    for (const QString &item : items) {
        QPushButton *btn = new QPushButton(item, this);
        btn->setFixedSize(100, 100);
        gridLayout->addWidget(btn, row, col);
        col++;
        if (col > 2) {
            col = 0;
            row++;
        }
    }
}


void MainWindow::onLogoutButtonClicked()
{
    emit backToMain();
}


void MainWindow::onScenarioButtonClicked()
{
    displayItemsInGrid(scenarios);
}


void MainWindow::onAllDevicesButtonClicked()
{
    int totalDeviceCount = 0;
    for (const QString &room : roomDevices.keys()) {
        totalDeviceCount += roomDevices[room].size();
    }

    if (totalDeviceCount == 0) {
        return;
    }

    QVector<QString> allDevices;
    allDevices.reserve(totalDeviceCount);

    for (const QString &room : roomDevices.keys()) {
        allDevices += roomDevices[room];
    }

    displayItemsInGrid(allDevices);
}

void MainWindow::onBedroomButtonClicked()
{
    displayItemsInGrid(roomDevices["Спальня"]);
}

void MainWindow::onLivingRoomButtonClicked()
{
    displayItemsInGrid(roomDevices["Гостиная"]);
}


void MainWindow::onKidsRoomButtonClicked()
{
    displayItemsInGrid(roomDevices["Детская"]);
}


void MainWindow::onHallwayButtonClicked()
{
    displayItemsInGrid(roomDevices["Прихожая"]);
}

