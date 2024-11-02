#include "mainwindow.h"
#include "networkmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QJsonArray>
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    initUI();

    qDebug() << "----IBUSKO---- MainWindow(QWidget *parent)";
    connect(
        &NetworkManager::instance(),
        &NetworkManager::responseReceived,
        this,
        &MainWindow::handleServerResponse);

}

MainWindow::~MainWindow() {}

void MainWindow::initUI() {
    logoutButton = new QPushButton("Выйти", this);
    connect(logoutButton, &QPushButton::clicked, this, [this](){ emit backToMain(); });

    addRoomButton = new QPushButton("Добавить комнату", this);
    connect(addRoomButton, &QPushButton::clicked, this, &MainWindow::onAddRoomButtonClicked);

    addDeviceButton = new QPushButton("Добавить устройство", this);
    connect(addDeviceButton, &QPushButton::clicked, this, &MainWindow::onAddDeviceButtonClicked);

    scenarioButton = new QPushButton("Сценарии", this);
    connect(scenarioButton, &QPushButton::clicked, this, &MainWindow::onScenarioButtonClicked);

    /*allRoomsButton = new QPushButton("Все комнаты", this);
    connect(allRoomsButton, &QPushButton::clicked, this, &MainWindow::onAllRoomsButtonClicked);*/

    addScenarioButton = new QPushButton("Добавить сценарий", this);
    connect(addScenarioButton, &QPushButton::clicked, this, &MainWindow::onAddScenarioButtonClicked);

    allDevicesButton = new QPushButton("Все устройства", this);
    connect(allDevicesButton, &QPushButton::clicked, this, &MainWindow::onAllDevicesButtonClicked);

    addDeviceButton->setFixedSize(200, 50);
    scenarioButton->setFixedSize(200, 50);
    allDevicesButton->setFixedSize(200, 50);
    addRoomButton->setFixedSize(200, 50);
    addScenarioButton->setFixedSize(200, 50);
    logoutButton->setFixedSize(100, 35);

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

    //loadRoomsFromDatabase();
    currentRoom = QString();
    /*if (roomDevices.isEmpty()) {
        QMessageBox::information(this, "Информация", "Нет добавленных комнат. Пожалуйста, добавьте комнаты.");
        onAddRoomButtonClicked();
    } else {
        updateDisplay();
    }*/
    auto addShadowEffect = [](QPushButton* button) {
        QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();
        shadowEffect->setOffset(0, 3);
        shadowEffect->setColor(QColor(0, 0, 0, 127));
        shadowEffect->setBlurRadius(5);
        button->setGraphicsEffect(shadowEffect);
    };
    addShadowEffect(scenarioButton);
    addShadowEffect(allDevicesButton);
    addShadowEffect(addDeviceButton);
    addShadowEffect(addRoomButton);
    addShadowEffect(logoutButton);
    addShadowEffect(addScenarioButton);

    setStyleSheet("QWidget {background-color: #6974e4;}");

    QString buttonStyle = "QPushButton {"
                          "background-color: #b3a2ee;"
                          "border-radius: 25px;"
                          "padding: 10px;"
                          "font: bold 16px 'New York';"
                          "}"
                          "QPushButton:hover {"
                          "background-color: #ffbaf5;"
                          "}";
    scenarioButton->setObjectName("scenarioButton");
    scenarioButton->setStyleSheet(buttonStyle);
    allDevicesButton->setObjectName("allDevicesButton");
    allDevicesButton->setStyleSheet(buttonStyle);
    logoutButton -> setStyleSheet("QPushButton {""background-color: #f78dae; ""border-radius: 10px;""padding: 6px;""font: bold 16px  'New york';""}""QPushButton:hover {""background-color: #ffbaf5;""}");
    addRoomButton->setStyleSheet(buttonStyle);
    addDeviceButton ->setStyleSheet(buttonStyle);
    addScenarioButton->setStyleSheet(buttonStyle);

}

void MainWindow::loadRoomsFromDatabase()
{
    QJsonObject request;
    request["action"] = "loadRooms";
    NetworkManager::instance().sendRequest(request);
}

void MainWindow::onAllDevicesButtonClicked()
{
    loadDevicesFromDatabase();
}

void MainWindow::loadDevicesFromDatabase()
{
    QJsonObject request;
    request["action"] = "loadAllDevices";
    NetworkManager::instance().sendRequest(request);
}

void MainWindow::onAddRoomButtonClicked()
{
    QStringList predefinedRooms = {"Ванная", "Гостиная", "Детская", "Гараж", "Сауна", "Кухня"};
    QString selectedRoom = QInputDialog::getItem(this, "Добавить помещение", "Выберите помещение:", predefinedRooms, 0, false);

    if (selectedRoom.isEmpty() || roomDevices.contains(selectedRoom)) {
        QMessageBox::warning(this, "Ошибка", roomDevices.contains(selectedRoom) ? "Комната уже существует." : "Не выбрано.");
        return;
    }

    QJsonObject request;
    request["action"] = "addRoom";
    request["roomName"] = selectedRoom;
    NetworkManager::instance().sendRequest(request);
}
void MainWindow::requestRoomDevices(const QString &roomName) {
    QJsonObject request;
    request["action"] = "loadRoomDevices";
    request["room"] = roomName;
    NetworkManager::instance().sendRequest(request);

}
void MainWindow::onAddDeviceButtonClicked()
{
    if (currentRoom.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите комнату.");
        return;
    }

    QStringList predefinedDevices = {"Лампа", "Термостат", "Камера", "Датчик движения"};
    QString selectedDevice = QInputDialog::getItem(this, "Добавить устройство", "Выберите устройство:", predefinedDevices, 0, false);

    if (selectedDevice.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Не выбрано.");
        return;
    }

    QJsonObject request;
    request["action"] = "addDevice";
    request["roomName"] = currentRoom;
    request["deviceName"] = selectedDevice;
    NetworkManager::instance().sendRequest(request);
}
void MainWindow::onScenarioButtonClicked() {
    QJsonObject request;
    //request["action"] = "loadScenarios";
    request["action"] = "loadRooms";
    NetworkManager::instance().sendRequest(request);
}
void MainWindow::onAddScenarioButtonClicked()
{
    QStringList scenarios = {"Наступила ночь", "Наступило утро", "Стало холодно", "Стало жарко", "Наступила зима"};
    QString selectedScenario = QInputDialog::getItem(this, "Добавить сценарий", "Выберите сценарий:", scenarios, 0, false);

    if (!selectedScenario.isEmpty()) {
        QJsonObject request;
        request["action"] = "addScenario";
        request["scenarioName"] = selectedScenario;
        NetworkManager::instance().sendRequest(request);
    } else {
        QMessageBox::information(this, "Информация", "Добавление сценария отменено.");
    }
}

void MainWindow::handleServerResponse(const QJsonObject &response)
{
    QString action = response["action"].toString();
    qDebug() << "-----IBUSKO----- Received response from server:" << action;

    if (action == "loadRooms") {
        handleLoadRoomsResponse(response);
    } else if (action == "loadAllDevices") {
        handleLoadAllDevicesResponse(response);
    } else if (action == "addRoom") {
        handleAddRoomResponse(response);
    } else if (action == "addDevice") {
        handleAddDeviceResponse(response);
    } else if (action=="loadScenarios"){
        handleLoadScenariosResponse(response);
    }else if(action=="addScenario"){
        handleAddScenarioResponse(response);
    }else if(action=="roomDevices")
        handleLoadRoomDevicesResponse(response);
}

void MainWindow::handleLoadRoomsResponse(const QJsonObject &response)
{
    QJsonArray roomsArray = response["rooms"].toArray();

    for (const QJsonValue &roomValue : roomsArray) {
        QString roomName = roomValue.toObject()["name"].toString();
        roomDevices[roomName] = QVector<QString>();
    }
    updateDisplay();
}
void MainWindow::handleLoadRoomDevicesResponse(const QJsonObject &response) {
    QString roomName = response["room"].toString();
    QJsonArray devicesArray = response["devices"].toArray();

    QVector<QString> devices;
    for (const QJsonValue &deviceValue : devicesArray) {
        devices.append(deviceValue.toString());
    }

    roomDevices[roomName] = devices;
    updateDisplay();
}
void MainWindow::handleLoadAllDevicesResponse(const QJsonObject &response)
{
    QVector<QString> allDevices;
    qDebug()<<"devices loaded";
    QJsonArray devices = response["devices"].toArray();
    for (const QJsonValue &value : devices) {
        allDevices.append(value.toString());
    }
    displayItemsInGrid(allDevices);
}
void MainWindow::handleAddRoomResponse(const QJsonObject &response)
{
    QString status = response["status"].toString();
    QString roomName = response["roomName"].toString();

    if (status == "success") {
        if (!roomDevices.contains(roomName)) {
            roomDevices[roomName] = QVector<QString>();
        }
        updateDisplay();
        QMessageBox::information(this, "Success", "Room added successfully: " + roomName);
    } else {
        QMessageBox::warning(this, "Error", "Failed to add room: " + response["message"].toString());
    }
}

void MainWindow::handleAddDeviceResponse(const QJsonObject &response)
{
    QString status = response["status"].toString();
    QString deviceName = response["deviceName"].toString();
    QString roomName = response["roomName"].toString();

    if (status == "success") {
        if (roomDevices.contains(roomName)) {
            roomDevices[roomName].append(deviceName);
        } else {
            roomDevices[roomName] = QVector<QString>({deviceName});
        }

        loadDevicesFromDatabase();
        QMessageBox::information(this, "Success", "Device added successfully: " + deviceName);
    } else {
        QMessageBox::warning(this, "Error", "Failed to add device: " + response["message"].toString());
    }
}
void MainWindow::handleAddScenarioResponse(const QJsonObject &response)
{
    QMessageBox::information(this, "Сценарий", response["message"].toString());
}

void MainWindow::handleLoadScenariosResponse(const QJsonObject &response)
{
    QVector<QString> allScenarios;
    QJsonArray scenarios = response["scenarios"].toArray();

    for (const QJsonValue &value : scenarios) {
        allScenarios.append(value.toString());
    }
    displayItemsInGrid(allScenarios);
}

void MainWindow::displayItemsInGrid(const QVector<QString> &items)
{
    clearGridLayout(gridLayout);

    int row = 0, col = 0;
    for (const QString &item : items) {
        QPushButton *button = new QPushButton(item, this);
        button->setFixedSize(150, 50);
        button->setCheckable(true);
        QString buttonStyle = "QPushButton {"
                              "background-color: #b3a2ee;"
                              "border-radius: 25px;"
                              "padding: 10px;"
                              "font: bold 16px 'New York';"
                              "}"
                              "QPushButton:hover {"
                              "background-color: #ffbaf5;" /* Темнее исходного */
                              "}";
        button->setObjectName(item);
        button->setStyleSheet(buttonStyle);
        connect(button, &QPushButton::clicked, this, [button]() {
            button->setStyleSheet(button->isChecked() ? "background-color: green;" : "background-color: red;");
        });
        gridLayout->addWidget(button, row, col);
        if (++col >= 3) {
            col = 0;
            ++row;
        }
    }
}

void MainWindow::clearGridLayout(QLayout *layout)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
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
        QString buttonStyle = "QPushButton {"
                              "background-color: #b3a2ee;"
                              "border-radius: 25px;"
                              "padding: 10px;"
                              "font: bold 16px 'New York';"
                              "}"
                              "QPushButton:hover {"
                              "background-color: #ffbaf5;" /* Темнее исходного */
                              "}";
        roomButton->setObjectName(room);
        roomButton->setStyleSheet(buttonStyle);

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

void MainWindow::clearRoomButtons()
{
    QLayoutItem *item;
    while ((item = sideMenuLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}
