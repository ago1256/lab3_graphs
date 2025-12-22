#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QGraphicsSimpleTextItem>
#include <QPen>
#include <QBrush>
#include <QRandomGenerator>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QGroupBox>
#include <QFrame>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), graph(new Graph()), 
      isCitySelected(false), selectedCity(""), selectedFlight({"", ""})
{
    setupUI();
    loadDefaultGraph();
    drawGraph();
    
    setWindowTitle("Граф городов");
    resize(1400, 800);
}

MainWindow::~MainWindow()
{
    delete graph;
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    
    // ========== ЛЕВАЯ ЧАСТЬ - ГРАФ ==========
    scene = new CustomScene(this);  // Используем CustomScene
    graphicsView = new QGraphicsView(scene, this);
    graphicsView->setRenderHint(QPainter::Antialiasing);
    graphicsView->setSceneRect(0, 0, 800, 600);
    graphicsView->setBackgroundBrush(QBrush(QColor(245, 248, 250)));
    
    // Подключаем сигнал клика от сцены
    connect(scene, &CustomScene::sceneClicked, this, &MainWindow::handleSceneClick);
    
    // ========== ПРАВАЯ ЧАСТЬ - ПАНЕЛЬ УПРАВЛЕНИЯ ==========
    QWidget *rightPanel = new QWidget(this);
    rightPanel->setMinimumWidth(400);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    // Панель выделенного элемента
    QGroupBox *selectionGroup = new QGroupBox("Выделенный элемент", rightPanel);
    QVBoxLayout *selectionLayout = new QVBoxLayout(selectionGroup);
    
    selectedLabel = new QLabel("Ничего не выделено", selectionGroup);
    selectedLabel->setAlignment(Qt::AlignCenter);
    selectedLabel->setStyleSheet("QLabel { font-weight: bold; font-size: 14px; padding: 10px; background-color: #f0f0f0; border-radius: 5px; }");
    
    infoLabel = new QLabel("Кликните на город или перелет", selectionGroup);
    infoLabel->setAlignment(Qt::AlignLeft);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { padding: 8px; background-color: #f8f8f8; border-radius: 3px; }");
    
    selectionLayout->addWidget(selectedLabel);
    selectionLayout->addWidget(infoLabel);
    
    // Панель добавления города
    QGroupBox *addCityGroup = new QGroupBox("Добавить город", rightPanel);
    QVBoxLayout *addCityLayout = new QVBoxLayout(addCityGroup);
    cityCombo = new QComboBox(addCityGroup);
    cityCombo->setEditable(true);
    cityCombo->setPlaceholderText("Введите название города");
    addCityBtn = new QPushButton("Добавить", addCityGroup);
    addCityLayout->addWidget(cityCombo);
    addCityLayout->addWidget(addCityBtn);
    
    // Панель перелетов
    QGroupBox *flightGroup = new QGroupBox("Перелеты", rightPanel);
    QGridLayout *flightLayout = new QGridLayout(flightGroup);
    
    flightLayout->addWidget(new QLabel("Откуда:", flightGroup), 0, 0);
    fromCombo = new QComboBox(flightGroup);
    flightLayout->addWidget(fromCombo, 0, 1);
    
    flightLayout->addWidget(new QLabel("Куда:", flightGroup), 1, 0);
    toCombo = new QComboBox(flightGroup);
    flightLayout->addWidget(toCombo, 1, 1);
    
    flightLayout->addWidget(new QLabel("Стоимость:", flightGroup), 2, 0);
    costSpin = new QSpinBox(flightGroup);
    costSpin->setRange(0, 10000);
    costSpin->setValue(300);
    costSpin->setSuffix(" $");
    flightLayout->addWidget(costSpin, 2, 1);
    
    flightLayout->addWidget(new QLabel("Время:", flightGroup), 3, 0);
    timeSpin = new QSpinBox(flightGroup);
    timeSpin->setRange(0, 48);
    timeSpin->setValue(4);
    timeSpin->setSuffix(" ч");
    flightLayout->addWidget(timeSpin, 3, 1);
    
    flightLayout->addWidget(new QLabel("Виза:", flightGroup), 4, 0);
    visaSpin = new QSpinBox(flightGroup);
    visaSpin->setRange(0, 500);
    visaSpin->setValue(0);
    visaSpin->setSuffix(" $");
    flightLayout->addWidget(visaSpin, 4, 1);
    
    addFlightBtn = new QPushButton("Добавить перелет", flightGroup);
    flightLayout->addWidget(addFlightBtn, 5, 0, 1, 2);
    
    // Панель управления
    QGroupBox *controlGroup = new QGroupBox("Управление", rightPanel);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
    
    deleteBtn = new QPushButton("Удалить выделенное", controlGroup);
    renameBtn = new QPushButton("Переименовать город", controlGroup);
    controlLayout->addWidget(deleteBtn);
    controlLayout->addWidget(renameBtn);
    
    // Панель поиска пути
    QGroupBox *pathGroup = new QGroupBox("Поиск пути", rightPanel);
    QVBoxLayout *pathLayout = new QVBoxLayout(pathGroup);
    
    QHBoxLayout *paramsLayout = new QHBoxLayout();
    checkCost = new QCheckBox("Стоимость", pathGroup);
    checkTime = new QCheckBox("Время", pathGroup);
    checkVisa = new QCheckBox("Виза", pathGroup);
    checkCost->setChecked(true);
    checkTime->setChecked(true);
    checkVisa->setChecked(false);
    paramsLayout->addWidget(checkCost);
    paramsLayout->addWidget(checkTime);
    paramsLayout->addWidget(checkVisa);
    
    pathLayout->addLayout(paramsLayout);
    findPathBtn = new QPushButton("Найти путь", pathGroup);
    pathLayout->addWidget(findPathBtn);
    
    // Статистика
    QFrame *statsFrame = new QFrame(rightPanel);
    statsFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    QVBoxLayout *statsLayout = new QVBoxLayout(statsFrame);
    
    statsLabel = new QLabel("Граф пуст", statsFrame);
    statsLabel->setAlignment(Qt::AlignCenter);
    statsLabel->setStyleSheet("QLabel { font-weight: bold; padding: 5px; }");
    
    clearBtn = new QPushButton("Очистить всё", statsFrame);
    
    statsLayout->addWidget(statsLabel);
    statsLayout->addWidget(clearBtn);
    
    // Собираем правую панель
    rightLayout->addWidget(selectionGroup);
    rightLayout->addWidget(addCityGroup);
    rightLayout->addWidget(flightGroup);
    rightLayout->addWidget(controlGroup);
    rightLayout->addWidget(pathGroup);
    rightLayout->addWidget(statsFrame);
    rightLayout->addStretch();
    
    // Основной layout
    mainLayout->addWidget(graphicsView, 3);
    mainLayout->addWidget(rightPanel, 1);
    
    // Подключаем сигналы
    connect(addCityBtn, &QPushButton::clicked, this, &MainWindow::onAddCity);
    connect(addFlightBtn, &QPushButton::clicked, this, &MainWindow::onAddFlight);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteSelected);
    connect(renameBtn, &QPushButton::clicked, this, &MainWindow::onRenameSelected);
    connect(findPathBtn, &QPushButton::clicked, this, &MainWindow::onFindPath);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearGraph);
    
    updateComboBoxes();
    updateSelectionDisplay();
}

void MainWindow::handleSceneClick(const QPointF& pos)
{
    qDebug() << "Click at position:" << pos.x() << "," << pos.y();
    
    // Сначала проверяем клик по городу
    std::string city = findCityAtPos(pos);
    if (!city.empty()) {
        qDebug() << "Selected city:" << QString::fromStdString(city);
        selectCity(city);
        return;
    }
    
    // Затем проверяем клик по перелету
    auto flight = findFlightAtPos(pos);
    if (!flight.first.empty()) {
        qDebug() << "Selected flight:" << QString::fromStdString(flight.first) 
                 << "->" << QString::fromStdString(flight.second);
        selectFlight(flight.first, flight.second);
        return;
    }
    
    // Если клик мимо - снимаем выделение
    qDebug() << "Click outside any element";
    clearSelection();
}

std::string MainWindow::findCityAtPos(const QPointF& pos)
{
    // Простая проверка: ищем город в радиусе 25 пикселей
    for (const auto& [cityName, cityPos] : cityPositions) {
        double dx = pos.x() - cityPos.x();
        double dy = pos.y() - cityPos.y();
        double distance = sqrt(dx*dx + dy*dy);
        
        if (distance <= 25) {  // Радиус круга + небольшой запас
            return cityName;
        }
    }
    return "";
}

std::pair<std::string, std::string> MainWindow::findFlightAtPos(const QPointF& pos)
{
    // Упрощенная проверка: ищем ближайший перелет
    const auto& edges = graph->get_all_edges();
    double minDistance = 15;  // Максимальное расстояние для клика
    std::pair<std::string, std::string> closestFlight = {"", ""};
    
    for (const auto& edge : edges) {
        auto fromIt = cityPositions.find(edge.second.start);
        auto toIt = cityPositions.find(edge.second.finish);
        
        if (fromIt == cityPositions.end() || toIt == cityPositions.end()) {
            continue;
        }
        
        QPointF p1 = fromIt->second;
        QPointF p2 = toIt->second;
        
        // Проверяем расстояние от точки до отрезка
        double A = pos.x() - p1.x();
        double B = pos.y() - p1.y();
        double C = p2.x() - p1.x();
        double D = p2.y() - p1.y();
        
        double dot = A * C + B * D;
        double len_sq = C * C + D * D;
        double param = (len_sq != 0) ? dot / len_sq : -1;
        
        double xx, yy;
        
        if (param < 0) {
            xx = p1.x();
            yy = p1.y();
        } else if (param > 1) {
            xx = p2.x();
            yy = p2.y();
        } else {
            xx = p1.x() + param * C;
            yy = p1.y() + param * D;
        }
        
        double dx = pos.x() - xx;
        double dy = pos.y() - yy;
        double distance = sqrt(dx*dx + dy*dy);
        
        if (distance < minDistance) {
            minDistance = distance;
            closestFlight = {edge.second.start, edge.second.finish};
        }
    }
    
    return closestFlight;
}

void MainWindow::selectCity(const std::string& cityName)
{
    clearSelection();
    selectedCity = cityName;
    isCitySelected = true;
    
    // Подсвечиваем город
    if (cityCircles.find(cityName) != cityCircles.end()) {
        cityCircles[cityName]->setBrush(QBrush(QColor(255, 200, 100))); // Оранжевый
        cityCircles[cityName]->setPen(QPen(Qt::darkRed, 2));
    }
    
    // Обновляем ComboBox
    cityCombo->setCurrentText(QString::fromStdString(cityName));
    
    updateSelectionDisplay();
}

void MainWindow::selectFlight(const std::string& from, const std::string& to)
{
    clearSelection();
    selectedFlight = {from, to};
    isCitySelected = false;
    
    // Подсвечиваем перелет
    std::string flightKey = from + "_" + to;
    if (flightLines.find(flightKey) != flightLines.end()) {
        flightLines[flightKey]->setPen(QPen(QColor(255, 100, 100), 3)); // Красный
    }
    
    // Обновляем ComboBox
    fromCombo->setCurrentText(QString::fromStdString(from));
    toCombo->setCurrentText(QString::fromStdString(to));
    
    updateSelectionDisplay();
}

void MainWindow::clearSelection()
{
    // Сбрасываем подсветку всех городов
    for (auto& [cityName, circle] : cityCircles) {
        circle->setBrush(QBrush(QColor(100, 150, 255))); // Синий
        circle->setPen(QPen(Qt::darkBlue, 1));
    }
    
    // Сбрасываем подсветку всех перелетов
    for (auto& [flightKey, line] : flightLines) {
        line->setPen(QPen(QColor(80, 80, 80), 2));
    }
    
    selectedCity = "";
    selectedFlight = {"", ""};
    isCitySelected = false;
    updateSelectionDisplay();
}

void MainWindow::updateSelectionDisplay()
{
    if (isCitySelected && !selectedCity.empty()) {
        QString cityName = QString::fromStdString(selectedCity);
        selectedLabel->setText("Выбран город: " + cityName);
        
        // Информация о городе
        QString infoText = "Город: <b>" + cityName + "</b>\n\n";
        try {
            Vertex vertex = graph->get_vertex(selectedCity);
            infoText += QString("Исходящих перелетов: %1\n").arg(vertex.out_edges.size());
            infoText += QString("Входящих перелетов: %1\n\n").arg(vertex.in_edges.size());
            
            auto neighbors = graph->get_neighbors(selectedCity);
            if (!neighbors.empty()) {
                infoText += "Связан с городами:\n";
                for (size_t i = 0; i < neighbors.size(); ++i) {
                    infoText += QString::fromStdString(neighbors[i]);
                    if (i < neighbors.size() - 1) infoText += ", ";
                    if (i % 3 == 2) infoText += "\n";
                }
            }
        } catch (...) {
            infoText += "Ошибка получения информации";
        }
        
        infoLabel->setText(infoText);
        
    } else if (!isCitySelected && !selectedFlight.first.empty()) {
        QString flightText = QString::fromStdString(selectedFlight.first) + 
                           " → " + QString::fromStdString(selectedFlight.second);
        selectedLabel->setText("Выбран перелет: " + flightText);
        
        // Информация о перелете
        QString infoText = "Перелет: <b>" + flightText + "</b>\n\n";
        try {
            Edge edge = graph->get_edge(selectedFlight.first, selectedFlight.second);
            if (!edge.params.empty()) {
                infoText += "Параметры:\n";
                if (edge.params.size() >= 1) {
                    infoText += QString("• Стоимость: $%1\n").arg(edge.params[0]);
                }
                if (edge.params.size() >= 2) {
                    infoText += QString("• Время полета: %1 ч\n").arg(edge.params[1]);
                }
                if (edge.params.size() >= 3) {
                    infoText += QString("• Виза: $%1\n").arg(edge.params[2]);
                }
            }
        } catch (...) {
            infoText += "Ошибка получения информации о перелете";
        }
        
        infoLabel->setText(infoText);
        
    } else {
        selectedLabel->setText("Ничего не выделено");
        infoLabel->setText("Кликните на:\n• Город (синий круг)\n• Перелет (серая линия)");
    }
}

void MainWindow::drawGraph()
{
    scene->clear();
    cityCircles.clear();
    cityPositions.clear();
    flightLines.clear();
    clearSelection();
    
    const auto& vertices = graph->get_all_vertices();
    int vertexCount = vertices.size();
    
    if (vertexCount == 0) {
        statsLabel->setText("Граф пуст");
        return;
    }
    
    // Располагаем города по кругу
    int i = 0;
    for (const auto& vertex : vertices) {
        double angle = 2 * 3.141592653589793 * i / vertexCount;
        int centerX = 400;
        int centerY = 300;
        int radius = std::min(200, 30000 / (vertexCount * 10));
        
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);
        
        // Создаем круг для города
        QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(x - 20, y - 20, 40, 40);
        circle->setBrush(QBrush(QColor(100, 150, 255)));
        circle->setPen(QPen(Qt::darkBlue, 1));
        circle->setZValue(10);
        scene->addItem(circle);
        
        cityCircles[vertex.first] = circle;
        cityPositions[vertex.first] = QPointF(x, y);
        
        // Название города
        QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(
            QString::fromStdString(vertex.first));
        text->setPos(x - text->boundingRect().width()/2, y + 25);
        scene->addItem(text);
        
        i++;
    }
    
    // Рисуем перелеты
    const auto& edges = graph->get_all_edges();
    for (const auto& edge : edges) {
        auto fromIt = cityPositions.find(edge.second.start);
        auto toIt = cityPositions.find(edge.second.finish);
        
        if (fromIt == cityPositions.end() || toIt == cityPositions.end()) {
            continue;
        }
        
        QPointF fromPos = fromIt->second;
        QPointF toPos = toIt->second;
        
        // Рассчитываем направление
        double dx = toPos.x() - fromPos.x();
        double dy = toPos.y() - fromPos.y();
        double length = sqrt(dx*dx + dy*dy);
        
        if (length == 0) continue;
        
        // Отступаем от границ кругов
        double offset = 20;
        double startX = fromPos.x() + (dx / length) * offset;
        double startY = fromPos.y() + (dy / length) * offset;
        double endX = toPos.x() - (dx / length) * offset;
        double endY = toPos.y() - (dy / length) * offset;
        
        // Линия перелета
        QGraphicsLineItem *line = new QGraphicsLineItem(startX, startY, endX, endY);
        line->setPen(QPen(QColor(80, 80, 80), 2, Qt::SolidLine, Qt::RoundCap));
        line->setZValue(1);
        scene->addItem(line);
        
        flightLines[edge.second.name] = line;
        
        // Стрелка
        double angle = atan2(dy, dx);
        double arrowSize = 10;
        
        QPointF arrowP1 = QPointF(endX, endY) - 
                         QPointF(cos(angle) * arrowSize + sin(angle) * arrowSize,
                                sin(angle) * arrowSize - cos(angle) * arrowSize);
        QPointF arrowP2 = QPointF(endX, endY) - 
                         QPointF(cos(angle) * arrowSize - sin(angle) * arrowSize,
                                sin(angle) * arrowSize + cos(angle) * arrowSize);
        
        QGraphicsPolygonItem *arrow = new QGraphicsPolygonItem();
        arrow->setPolygon(QPolygonF() << QPointF(endX, endY) << arrowP1 << arrowP2);
        arrow->setBrush(QBrush(QColor(80, 80, 80)));
        arrow->setPen(QPen(QColor(80, 80, 80)));
        arrow->setZValue(2);
        scene->addItem(arrow);
    }
    
    statsLabel->setText(QString("Городов: %1\nПерелетов: %2")
                       .arg(graph->vertex_count())
                       .arg(graph->edge_count()));
}

// Остальные функции (updateComboBoxes, onAddCity, onAddFlight и т.д.)
// остаются такими же как в предыдущей версии...

void MainWindow::updateComboBoxes()
{
    QString currentCity = cityCombo->currentText();
    QString currentFrom = fromCombo->currentText();
    QString currentTo = toCombo->currentText();
    
    cityCombo->clear();
    fromCombo->clear();
    toCombo->clear();
    
    const auto& vertices = graph->get_all_vertices();
    for (const auto& vertex : vertices) {
        QString name = QString::fromStdString(vertex.first);
        cityCombo->addItem(name);
        fromCombo->addItem(name);
        toCombo->addItem(name);
    }
    
    if (!currentCity.isEmpty() && cityCombo->findText(currentCity) != -1)
        cityCombo->setCurrentText(currentCity);
    if (!currentFrom.isEmpty() && fromCombo->findText(currentFrom) != -1)
        fromCombo->setCurrentText(currentFrom);
    if (!currentTo.isEmpty() && toCombo->findText(currentTo) != -1)
        toCombo->setCurrentText(currentTo);
}

void MainWindow::onAddCity()
{
    QString name = cityCombo->currentText().trimmed();
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название города");
        return;
    }
    
    if (graph->has_vertex(name.toStdString())) {
        QMessageBox::warning(this, "Ошибка", "Город уже существует");
        return;
    }
    
    try {
        graph->add_vertex(name.toStdString());
        drawGraph();
        updateComboBoxes();
        selectCity(name.toStdString());
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
    }
}

void MainWindow::onAddFlight()
{
    QString from = fromCombo->currentText();
    QString to = toCombo->currentText();
    
    if (from.isEmpty() || to.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите города");
        return;
    }
    
    if (from == to) {
        QMessageBox::warning(this, "Ошибка", "Города должны быть разными");
        return;
    }
    
    std::vector<int> params = {
        costSpin->value(),
        timeSpin->value(),
        visaSpin->value()
    };
    
    try {
        graph->add_edge(from.toStdString(), to.toStdString(), params);
        drawGraph();
        updateComboBoxes();
        selectFlight(from.toStdString(), to.toStdString());
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
    }
}

void MainWindow::onDeleteSelected()
{
    if (isCitySelected && !selectedCity.empty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Удаление",
            QString("Удалить город '%1' и все перелеты?")
                .arg(QString::fromStdString(selectedCity)),
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            try {
                graph->remove_vertex(selectedCity);
                drawGraph();
                updateComboBoxes();
                clearSelection();
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        }
        
    } else if (!isCitySelected && !selectedFlight.first.empty()) {
        QString flightText = QString("%1 → %2")
                            .arg(QString::fromStdString(selectedFlight.first))
                            .arg(QString::fromStdString(selectedFlight.second));
        
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Удаление",
            QString("Удалить перелет %1?").arg(flightText),
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            try {
                graph->remove_edge(selectedFlight.first, selectedFlight.second);
                drawGraph();
                updateComboBoxes();
                clearSelection();
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Ничего не выделено");
    }
}

void MainWindow::onRenameSelected()
{
    if (isCitySelected && !selectedCity.empty()) {
        QString newName = QInputDialog::getText(this, "Переименовать город",
                                               "Новое название:", 
                                               QLineEdit::Normal, 
                                               QString::fromStdString(selectedCity));
        
        if (!newName.isEmpty() && newName != QString::fromStdString(selectedCity)) {
            try {
                graph->change_vertex(selectedCity, newName.toStdString());
                drawGraph();
                updateComboBoxes();
                selectCity(newName.toStdString());
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
            }
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Выделите город для переименования");
    }
}

void MainWindow::onFindPath()
{
    QString from = fromCombo->currentText();
    QString to = toCombo->currentText();
    
    if (from.isEmpty() || to.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите города");
        return;
    }
    
    if (from == to) {
        QMessageBox::warning(this, "Ошибка", "Города должны быть разными");
        return;
    }
    
    std::vector<bool> params = {
        checkCost->isChecked(),
        checkTime->isChecked(),
        checkVisa->isChecked()
    };
    
    try {
        Path path = graph->dijkstra_alg(from.toStdString(), to.toStdString(), params);
        
        if (path.verts.empty()) {
            QMessageBox::information(this, "Результат", "Путь не найден");
        } else {
            QString pathStr;
            for (size_t i = 0; i < path.verts.size(); ++i) {
                pathStr += QString::fromStdString(path.verts[i]);
                if (i < path.verts.size() - 1) pathStr += " → ";
            }
            
            QString info = QString("<b>Оптимальный путь:</b><br>%1<br><br>"
                                  "<b>Параметры:</b><br>")
                .arg(pathStr);
            
            if (path.cost.size() > 0) info += QString("• Стоимость: $%1<br>").arg(path.cost[0]);
            if (path.cost.size() > 1) info += QString("• Время: %1 ч<br>").arg(path.cost[1]);
            if (path.cost.size() > 2) info += QString("• Виза: $%1<br>").arg(path.cost[2]);
            
            info += QString("<br><b>Взвешенная сумма:</b> %1").arg(path.weighted_sum);
            
            QMessageBox::information(this, "Путь найден", info);
        }
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
    }
}

void MainWindow::onClearGraph()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Очистка",
        "Очистить весь граф?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        graph->clear();
        drawGraph();
        updateComboBoxes();
        clearSelection();
    }
}

void MainWindow::loadDefaultGraph()
{
    QString citiesFile = "data/data_for_graph.txt";
    
    if (!QFile::exists(citiesFile)) {
        QDir dir("data");
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        
        QFile file(citiesFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            QStringList defaultCities = {
                "Москва", "Лондон", "Париж", "Берлин", "Токио",
                "Нью-Йорк", "Пекин", "Дубай", "Сингапур", "Сидней"
            };
            
            for (const QString& city : defaultCities) {
                out << city << "\n";
            }
            file.close();
        }
    }
    
    QFile file(citiesFile);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        std::vector<std::string> cities;
        
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                cities.push_back(line.toStdString());
            }
        }
        file.close();
        
        if (!cities.empty()) {
            graph->generate_graph(cities, 40, 3);
        }
    }
}