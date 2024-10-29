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
    allDevicesButton = new QPushButton("Все устройства", this);
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
    qDebug() << "loadRoomsFromDatabase started";

    roomDevices.clear();

    QSqlQuery query(DatabaseManager::instance().getDatabase());

    if (query.exec("SELECT name FROM rooms")) {
        while (query.next()) {
            QString roomName = query.value(0).toString();
            roomDevices[roomName] = QVector<QString>();
            qDebug() << "Loaded room:" << roomName;
        }
        if (roomDevices.isEmpty()) {
            qDebug() << "No rooms found in the database.";
        }
    } else {
        qDebug() << "Failed to load rooms from database:" << query.lastError().text();
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить комнаты из базы данных.");
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
    clearDisplay();

    for (const QString &room : roomDevices.keys()) {
        QPushButton *roomButton = new QPushButton(room, this);
        roomButton->setFixedSize(200, 50);

        connect(roomButton, &QPushButton::clicked, this,[this , room]() {
            displayItemsInGrid(roomDevices[room]);
        });

        roomButtonsLayout->addWidget(roomButton);
    }

    if (roomDevices.isEmpty()) {
        clearDisplay();
        QLabel *warningLabel = new QLabel("Предупреждение: Нет добавленных комнат. Пожалуйста, добавьте комнаты.", this);
        warningLabel->setWordWrap(true);
        warningLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(warningLabel , 0 , 0 , 1 , 3);
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
    bool ok;
    QString deviceName = QInputDialog::getText(this, "Добавить устройство",
                                               "Введите название устройства:", QLineEdit::Normal,
                                               "", &ok);

    if (ok && !deviceName.isEmpty()) {
        bool roomSelected = false;
        QString selectedRoom;

        QStringList roomsList = roomDevices.keys();
        selectedRoom = QInputDialog::getItem(this, "Выберите комнату",
                                             "Выберите комнату:", roomsList, 0, false);

        if (!selectedRoom.isEmpty()) {
            roomSelected = true;
        }

        if (roomSelected) {
            if (addDeviceToRoom(selectedRoom, deviceName)) {
                QMessageBox::information(this, "Успех",
                                         QString("Устройство '%1' добавлено в '%2'.").arg(deviceName).arg(selectedRoom));
            } else {
                QMessageBox::warning(this, "Ошибка",
                                     QString("Не удалось добавить устройство '%1' в '%2'.").arg(deviceName).arg(selectedRoom));
            }
        } else {
            QMessageBox::warning(this, "Ошибка", "Комната не выбрана.");
        }

        updateDisplay();
    }
}

void MainWindow::onScenarioButtonClicked(){}
void MainWindow::onAllDevicesButtonClicked(){}
void MainWindow::displayItemsInGrid(const QVector<QString> &items){}
