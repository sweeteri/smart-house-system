#include "mainwindow.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QListWidget>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    qDebug() << "MainWindow constructor started";


    logoutButton = new QPushButton("Выйти", this);
    connect(logoutButton, &QPushButton::clicked, this, [this](){emit backToMain();});

    addRoomButton = new QPushButton("Добавить комнату", this);
    connect(addRoomButton, &QPushButton::clicked, this, &MainWindow::onAddRoomButtonClicked);

    addDeviceButton = new QPushButton("Добавить устройство", this);
    connect(addDeviceButton, &QPushButton::clicked, this, &MainWindow::onAddDeviceButtonClicked);

    scenarioButton = new QPushButton("Сценарии", this);
    connect(scenarioButton, &QPushButton::clicked, this, &MainWindow::onScenarioButtonClicked);

    addScenarioButton = new QPushButton("Добавить сценарий", this);
    connect(addScenarioButton, &QPushButton::clicked, this, &MainWindow::onAddScenarioButtonClicked);

    allDevicesButton = new QPushButton("Все устройства", this);
    connect(allDevicesButton, &QPushButton::clicked, this, &MainWindow::onAllDevicesButtonClicked);

    addDeviceButton->setFixedSize(200, 50);
    scenarioButton->setFixedSize(200, 50);
    allDevicesButton->setFixedSize(200, 50);
    addRoomButton->setFixedSize(200, 50);

    sideMenu = new QWidget(this);
    sideMenuLayout = new QVBoxLayout(sideMenu);
    sideMenuLayout->addWidget(scenarioButton);
    sideMenuLayout->addWidget(allDevicesButton);
    roomButtonsLayout = new QVBoxLayout();
    sideMenuLayout->addLayout(roomButtonsLayout);
    sideMenuLayout->addStretch();

    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->addWidget(addRoomButton);
    headerLayout->addWidget(addDeviceButton);
    headerLayout->addWidget(addScenarioButton);
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

    loadRoomsFromDatabase();
    loadDevicesFromDatabase();

    currentRoom = QString();

    if (roomDevices.isEmpty()) {
        QMessageBox::information(this, "Информация", "Нет добавленных комнат. Пожалуйста, добавьте комнаты.");
        onAddRoomButtonClicked();
    } else {
        updateDisplay();
    }
}

MainWindow::~MainWindow() {}

void MainWindow::loadRoomsFromDatabase()
{
    roomDevices.clear();

    QSqlQuery query(DatabaseManager::instance().getDatabase());

    if (query.exec("SELECT name FROM rooms ORDER BY id")) {
        while (query.next()) {
            QString roomName = query.value(0).toString();
            roomDevices[roomName] = QVector<QString>();
        }
    } else {
        qDebug() << "Failed to load rooms from database:" << query.lastError().text();
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить комнаты из базы данных.");
    }
}
void MainWindow::loadDevicesFromDatabase()
{
    // Clear existing device data
    for (auto &devices : roomDevices) {
        devices.clear();
    }

    QSqlQuery query(DatabaseManager::instance().getDatabase());

    if (query.exec("SELECT d.name, r.name FROM devices d JOIN rooms r ON d.room_id = r.id")) {
        while (query.next()) {
            QString deviceName = query.value(0).toString();
            QString roomName = query.value(1).toString();
            roomDevices[roomName].append(deviceName);
        }
    } else {
        qDebug() << "Failed to load devices from database:" << query.lastError().text();
    }
}
bool MainWindow::addRoomToDatabase(const QString &roomName)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    query.prepare("INSERT INTO rooms (name) VALUES (:name)");
    query.bindValue(":name", roomName);

    if (!query.exec()) {
        qDebug() << "Failed to add room to database:" << query.lastError().text();
        return false;
    }

    return true;
}

void MainWindow::onAddRoomButtonClicked()
{
    bool ok;
    QString roomName = QInputDialog::getText(this, "Добавить комнату",
                                             "Введите название комнаты:", QLineEdit::Normal,
                                             "", &ok);
    if (ok && !roomName.isEmpty()) {
        if (!roomDevices.contains(roomName)) {
            if (addRoomToDatabase(roomName)) {
                roomDevices[roomName] = QVector<QString>();
                updateDisplay();
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось добавить комнату в базу данных.");
            }
        } else {
            QMessageBox::warning(this, "Ошибка", "Комната с таким названием уже существует.");
        }
    }
}

void MainWindow::updateDisplay()
{
    clearRoomButtons();

    QStringList roomNames = roomDevices.keys();
    std::sort(roomNames.begin(), roomNames.end());

    for (const QString &room : roomNames) {
        QPushButton *roomButton = new QPushButton(room, this);
        roomButton->setFixedSize(200, 50);

        connect(roomButton, &QPushButton::clicked, this, [this, room]() {
            currentRoom = room;
            displayItemsInGrid(roomDevices[room]);
        });

        roomButtonsLayout->addWidget(roomButton);
    }

    if (roomDevices.isEmpty()) {
        QLabel *warningLabel = new QLabel("Предупреждение: Нет добавленных комнат. Пожалуйста, добавьте комнаты.", this);
        warningLabel->setWordWrap(true);
        warningLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(warningLabel, 0, 0, 1, 3);
    }
}

void MainWindow::clearDisplay()
{
    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}
bool MainWindow::addDeviceToRoom(const QString &roomName, const QString &deviceName)
{
    if (!roomDevices.contains(roomName)) {
        return false;
    }

    roomDevices[roomName].append(deviceName);

    QSqlQuery query(DatabaseManager::instance().getDatabase());

    query.prepare("SELECT id FROM rooms WHERE name = :name");
    query.bindValue(":name", roomName);

    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to find room ID:" << query.lastError().text();
        return false;
    }

    int roomId = query.value(0).toInt();

    query.prepare("INSERT INTO devices (room_id, name) VALUES (:room_id, :name)");
    query.bindValue(":room_id", roomId);
    query.bindValue(":name", deviceName);

    if (!query.exec()) {
        qDebug() << "Failed to add device to database:" << query.lastError().text();
        return false;
    }

    return true;
}
void MainWindow::onAddDeviceButtonClicked()
{
    if (currentRoom.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите комнату для добавления устройства.");
        return;
    }

    QStringList predefinedDevices = {"Лампа", "Термостат", "Камера наблюдения", "Датчик движения"};
    QString selectedDevice = QInputDialog::getItem(this, "Добавить устройство", "Выберите устройство:", predefinedDevices, 0, false);

    if (selectedDevice.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Устройство не выбрано.");
        return;
    }

    if (addDeviceToRoom(currentRoom, selectedDevice)) {
        QMessageBox::information(this, "Успех", QString("Устройство '%1' добавлено в '%2'.").arg(selectedDevice).arg(currentRoom));
        updateDisplay();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось добавить устройство.");
    }
}

void MainWindow::onScenarioButtonClicked(){
    QVector<QString> allScenarios;

    QSqlQuery query(DatabaseManager::instance().getDatabase());
    if (query.exec("SELECT name FROM scenarios")) {
        while (query.next()) {
            QString scenarioName = query.value(0).toString();
            allScenarios.append(scenarioName);
        }
    } else {
        qDebug() << "Failed to load scenarios from database:" << query.lastError().text();
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить сценарии из базы данных.");
    }

    displayItemsInGrid(allScenarios);
}
bool MainWindow::addScenario(const QString &scenarioName)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    query.prepare("INSERT INTO scenarios (name) VALUES (:name)");
    query.bindValue(":name", scenarioName);

    if (!query.exec()) {
        qDebug() << "Failed to add scenario to database:" << query.lastError().text();
        QMessageBox::warning(this, "Ошибка", "Не удалось добавить сценарий в базу данных.");
        return false;
    }

    return true;
}
void MainWindow::onAddScenarioButtonClicked()
{
    bool ok;
    QString scenarioName = QInputDialog::getText(this, "Добавить сценарий",
                                                 "Введите название сценария:", QLineEdit::Normal,
                                                 "", &ok);
    if (ok && !scenarioName.isEmpty()) {
        if (addScenario(scenarioName)) {
            QMessageBox::information(this, "Успех", "Сценарий добавлен.");
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось добавить сценарий.");
        }
    }
}
void MainWindow::onAllDevicesButtonClicked(){
    QVector<QString> allDevices;

    QSqlQuery query(DatabaseManager::instance().getDatabase());
    if (query.exec("SELECT name FROM devices")) {
        while (query.next()) {
            QString deviceName = query.value(0).toString();
            allDevices.append(deviceName);
        }
    } else {
        qDebug() << "Failed to load devices from database:" << query.lastError().text();
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить устройства из базы данных.");
    }

    displayItemsInGrid(allDevices);
}
void MainWindow::displayItemsInGrid(const QVector<QString> &items)
{
    clearDisplay();  // Clear previous items

    int row = 0, col = 0;
    for (const QString &device : items) {
        QPushButton *deviceButton = new QPushButton(device, this);
        deviceButton->setFixedSize(100, 100);
        deviceButton->setCheckable(true);

        deviceButton->setStyleSheet(
            "QPushButton { background-color: lightgray; border: 1px solid black; }"
            "QPushButton:checked { background-color: green; color: white; }"
            "QPushButton:!checked { background-color: red; color: white; }"
            );

        connect(deviceButton, &QPushButton::clicked, this, [=]() {
            bool isOn = deviceButton->isChecked();
            if (isOn) {
                qDebug() << device << "turned ON";
            } else {
                qDebug() << device << "turned OFF";
            }
        });

        gridLayout->addWidget(deviceButton, row, col);

        if (++col >= 3) {
            col = 0;
            ++row;
        }
    }

    if (items.isEmpty()) {
        QLabel *noDevicesLabel = new QLabel("Добавьте устройства", this);
        noDevicesLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(noDevicesLabel, 0, 0, 1, 3);
    }
}
void MainWindow::clearRoomButtons()
{
    QLayoutItem *item;
    while ((item = roomButtonsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}
