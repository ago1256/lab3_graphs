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
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QGroupBox>
#include <QFrame>
#include <QMenu>
#include <cmath>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QDialog>
#include <QApplication>
#include <QTimer>
#include <QSplitter>
#include <QScrollArea>
#include <QTextEdit>
#include <ctime>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), 
      graph(new Graph()), 
      isCitySelected(false), 
      selectedCity(""), 
      selectedFlight({"", ""}),
      graphicsView(nullptr),
      scene(nullptr),
      pathResultLabel(nullptr),
      selectionInfoGroup(nullptr),
      selectionTypeLabel(nullptr),
      selectionNameLabel(nullptr),
      flightParamsLabel(nullptr),
      cityCombo(nullptr),
      fromCombo(nullptr),
      toCombo(nullptr),
      costSpin(nullptr),
      timeSpin(nullptr),
      visaSpin(nullptr),
      checkCost(nullptr),
      checkTime(nullptr),
      checkVisa(nullptr),
      addCityBtn(nullptr),
      addFlightBtn(nullptr),
      findPathBtn(nullptr),
      clearBtn(nullptr),
      contextMenu(nullptr),
      scrollArea(nullptr),
      scrollContent(nullptr),
      scrollLayout(nullptr),
      sccDetailsText(nullptr),
      mainSplitter(nullptr),
      mapBackground(nullptr) 
{
    qDebug() << "MainWindow constructor start";
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    try {
        setupUI();
        qDebug() << "UI setup complete";

        loadMapToScene();
        qDebug() << "Map loaded";
        
        loadDefaultGraph();
        qDebug() << "Default graph loaded";
        
        drawGraph();
        qDebug() << "Graph drawn";
        
        setWindowTitle("Граф городов с изменяемыми координатами");
        resize(1400, 850);
        setMinimumSize(1100, 700);
        
        qDebug() << "MainWindow constructor end";
    } catch (const std::exception& e) {
        qDebug() << "Exception in constructor:" << e.what();
        QMessageBox::critical(nullptr, "Ошибка", QString("Ошибка при создании окна: %1").arg(e.what()));
    } catch (...) {
        qDebug() << "Unknown exception in constructor";
        QMessageBox::critical(nullptr, "Ошибка", "Неизвестная ошибка при создании окна");
    }
}

MainWindow::~MainWindow()
{
    delete graph;
}

void MainWindow::setupUI()
{
    qDebug() << "Setting up UI...";
    
    try {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setContentsMargins(2, 2, 2, 2);
        mainLayout->setSpacing(2);
        
        scene = new CustomScene(this);
        graphicsView = new QGraphicsView(scene, this);
        
        graphicsView->setRenderHint(QPainter::Antialiasing);
        graphicsView->setMinimumSize(650, 550);
        graphicsView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        graphicsView->setBackgroundBrush(QBrush(QColor(245, 248, 250)));
        
        QWidget *rightPanel = new QWidget(this);
        rightPanel->setMinimumWidth(380);
        rightPanel->setMaximumWidth(420);
        QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
        rightLayout->setContentsMargins(6, 6, 6, 6);
        rightLayout->setSpacing(4);
        
        selectionInfoGroup = new QGroupBox("Выделенный элемент", rightPanel);
        selectionInfoGroup->setMinimumHeight(110);
        QVBoxLayout *selectionInfoLayout = new QVBoxLayout(selectionInfoGroup);
        selectionInfoLayout->setContentsMargins(6, 8, 6, 6);
        selectionInfoLayout->setSpacing(2);
        
        selectionTypeLabel = new QLabel(" ", selectionInfoGroup);
        selectionTypeLabel->setAlignment(Qt::AlignCenter);
        selectionTypeLabel->setStyleSheet("QLabel { color: #666; font-size: 8pt; }");
        selectionTypeLabel->setMaximumHeight(16);
        
        selectionNameLabel = new QLabel("", selectionInfoGroup);
        selectionNameLabel->setAlignment(Qt::AlignCenter);
        selectionNameLabel->setStyleSheet("QLabel { font-size: 9pt; font-weight: bold; }");
        selectionNameLabel->setWordWrap(true);
        selectionNameLabel->setMaximumHeight(20);
        
        flightParamsLabel = new QLabel("", selectionInfoGroup);
        flightParamsLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        flightParamsLabel->setWordWrap(true);
        flightParamsLabel->setMinimumHeight(70);
        flightParamsLabel->setMaximumHeight(85);
        flightParamsLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        flightParamsLabel->setStyleSheet(
            "QLabel { "
            "    padding: 3px; "
            "    background-color: #f8f8f8; "
            "    border-radius: 3px; "
            "    border: 1px solid #ddd; "
            "    font-size: 8pt; "
            "}"
        );
        
        selectionInfoLayout->addWidget(selectionTypeLabel);
        selectionInfoLayout->addWidget(selectionNameLabel);
        selectionInfoLayout->addWidget(flightParamsLabel);

        QGroupBox *addCityGroup = new QGroupBox("Добавить город", rightPanel);
        QVBoxLayout *addCityLayout = new QVBoxLayout(addCityGroup);
        addCityLayout->setContentsMargins(6, 8, 6, 6);
        addCityLayout->setSpacing(4);
        
        cityCombo = new QComboBox(addCityGroup);
        cityCombo->setEditable(true);
        cityCombo->setPlaceholderText("Название города");
        cityCombo->setMinimumHeight(26);
        
        addCityBtn = new QPushButton("Добавить город", addCityGroup);
        addCityBtn->setMinimumHeight(28);
        
        addCityLayout->addWidget(cityCombo);
        addCityLayout->addWidget(addCityBtn);
        
        QGroupBox *flightGroup = new QGroupBox("Добавить перелет", rightPanel);
        QGridLayout *flightLayout = new QGridLayout(flightGroup);
        flightLayout->setContentsMargins(6, 8, 6, 6);
        flightLayout->setSpacing(4);
        flightLayout->setColumnStretch(1, 1);
        
        flightLayout->addWidget(new QLabel("Откуда:", flightGroup), 0, 0);
        fromCombo = new QComboBox(flightGroup);
        fromCombo->setMinimumHeight(26);
        flightLayout->addWidget(fromCombo, 0, 1);
        
        flightLayout->addWidget(new QLabel("Куда:", flightGroup), 1, 0);
        toCombo = new QComboBox(flightGroup);
        toCombo->setMinimumHeight(26);
        flightLayout->addWidget(toCombo, 1, 1);
        
        flightLayout->addWidget(new QLabel("Стоимость:", flightGroup), 2, 0);
        costSpin = new QSpinBox(flightGroup);
        costSpin->setRange(0, 10000);
        costSpin->setValue(300);
        costSpin->setSuffix(" $");
        costSpin->setMinimumHeight(26);
        flightLayout->addWidget(costSpin, 2, 1);
        
        flightLayout->addWidget(new QLabel("Время:", flightGroup), 3, 0);
        timeSpin = new QSpinBox(flightGroup);
        timeSpin->setRange(0, 48);
        timeSpin->setValue(4);
        timeSpin->setSuffix(" ч");
        timeSpin->setMinimumHeight(26);
        flightLayout->addWidget(timeSpin, 3, 1);
        
        flightLayout->addWidget(new QLabel("Виза:", flightGroup), 4, 0);
        visaSpin = new QSpinBox(flightGroup);
        visaSpin->setRange(0, 500);
        visaSpin->setValue(0);
        visaSpin->setSuffix(" $");
        visaSpin->setMinimumHeight(26);
        flightLayout->addWidget(visaSpin, 4, 1);
        
        addFlightBtn = new QPushButton("Добавить перелет", flightGroup);
        addFlightBtn->setMinimumHeight(30);
        flightLayout->addWidget(addFlightBtn, 5, 0, 1, 2);
        
        QGroupBox *pathGroup = new QGroupBox("Поиск пути", rightPanel);
        QVBoxLayout *pathLayout = new QVBoxLayout(pathGroup);
        pathLayout->setContentsMargins(6, 8, 6, 6);
        pathLayout->setSpacing(6);
        
        QWidget *checkWidget = new QWidget(pathGroup);
        QHBoxLayout *checkLayout = new QHBoxLayout(checkWidget);
        checkLayout->setContentsMargins(0, 0, 0, 0);
        checkLayout->setSpacing(10);
        
        checkCost = new QCheckBox("Стоимость", checkWidget);
        checkTime = new QCheckBox("Время", checkWidget);
        checkVisa = new QCheckBox("Виза", checkWidget);
        checkCost->setChecked(true);
        checkTime->setChecked(true);
        checkVisa->setChecked(false);
        
        checkCost->setStyleSheet("QCheckBox { font-size: 8pt; }");
        checkTime->setStyleSheet("QCheckBox { font-size: 8pt; }");
        checkVisa->setStyleSheet("QCheckBox { font-size: 8pt; }");
        
        checkLayout->addWidget(checkCost);
        checkLayout->addWidget(checkTime);
        checkLayout->addWidget(checkVisa);
        checkLayout->addStretch(1);
        
        findPathBtn = new QPushButton("Найти путь", pathGroup);
        findPathBtn->setMinimumHeight(30);
        
        pathLayout->addWidget(checkWidget);
        pathLayout->addWidget(findPathBtn);
        
        QGroupBox *resultGroup = new QGroupBox("Результаты", rightPanel);
        QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);
        resultLayout->setContentsMargins(6, 8, 6, 6);
        resultLayout->setSpacing(4);
        
        pathResultLabel = new QLabel("Результаты поиска", resultGroup);
        pathResultLabel->setAlignment(Qt::AlignCenter);
        pathResultLabel->setStyleSheet("QLabel { font-weight: bold; font-size: 9pt; }");
        pathResultLabel->setMaximumHeight(20);
        
        sccDetailsText = new QTextEdit(resultGroup);
        sccDetailsText->setReadOnly(true);
        sccDetailsText->setMinimumHeight(120);
        sccDetailsText->setMaximumHeight(180);
        sccDetailsText->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sccDetailsText->setHtml("<div style='font-size: 8pt; color: #666; padding: 2px;'>Информация</div>");
        sccDetailsText->setStyleSheet(
            "QTextEdit {"
            "    background-color: #f8f8f8;"
            "    border: 1px solid #ddd;"
            "    border-radius: 3px;"
            "    padding: 4px;"
            "    font-size: 8pt;"
            "}"
        );
        
        resultLayout->addWidget(pathResultLabel);
        resultLayout->addWidget(sccDetailsText);
        
        QFrame *controlFrame = new QFrame(rightPanel);
        controlFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
        QVBoxLayout *controlLayout = new QVBoxLayout(controlFrame);
        controlLayout->setContentsMargins(6, 8, 6, 6);
        controlLayout->setSpacing(4);
        
        clearBtn = new QPushButton("Очистить граф", controlFrame);
        colorSCCBtn = new QPushButton("Раскрасить компоненты", controlFrame); 
        resetColorsBtn = new QPushButton("Сбросить цвета", controlFrame);
        
        clearBtn->setMinimumHeight(28);
        colorSCCBtn->setMinimumHeight(28);
        resetColorsBtn->setMinimumHeight(28);
        
        controlLayout->addWidget(clearBtn);
        controlLayout->addWidget(colorSCCBtn);  
        controlLayout->addWidget(resetColorsBtn);
        
        rightLayout->addWidget(selectionInfoGroup);
        rightLayout->addWidget(addCityGroup);
        rightLayout->addWidget(flightGroup);
        rightLayout->addWidget(pathGroup);
        rightLayout->addWidget(resultGroup);
        rightLayout->addWidget(controlFrame);
        rightLayout->addStretch(1);
        
        mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
        mainSplitter->addWidget(graphicsView);
        mainSplitter->addWidget(rightPanel);
        mainSplitter->setStretchFactor(0, 3);
        mainSplitter->setStretchFactor(1, 1);
        mainSplitter->setHandleWidth(4);
        
        mainLayout->addWidget(mainSplitter);
        
        connect(addCityBtn, &QPushButton::clicked, this, &MainWindow::onAddCity);
        connect(addFlightBtn, &QPushButton::clicked, this, &MainWindow::onAddFlight);
        connect(findPathBtn, &QPushButton::clicked, this, &MainWindow::onFindPath);
        connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearGraph);
        connect(colorSCCBtn, &QPushButton::clicked, this, &MainWindow::onColorSCC); 
        connect(resetColorsBtn, &QPushButton::clicked, this, &MainWindow::onResetColors);
        
        connect(scene, &CustomScene::sceneClicked, this, &MainWindow::handleSceneClick);
        connect(scene, &CustomScene::sceneRightClicked, this, &MainWindow::handleSceneRightClick);
        
        contextMenu = new QMenu(this);
        
        updateComboBoxes();
        clearSelection();
        
        qDebug() << "UI setup completed successfully";
        
    } catch (const std::exception& e) {
        qDebug() << "Exception in setupUI:" << e.what();
        throw;
    }
}

void MainWindow::drawGraph()
{
    qDebug() << "Drawing graph...";
    
    if (!scene) {
        qDebug() << "Scene is null!";
        return;
    }
    
    try {
           QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem* item : items) {
            // Удаляем только элементы графа (вершины, ребра, текст)
            // Фон (mapBackground) оставляем
            if (item != mapBackground && item->zValue() >= -90) {
                scene->removeItem(item);
                delete item;
            }
        }
        
        // Очищаем контейнеры
        cityCircles.clear();
        cityPositions.clear();
        flightLines.clear();
        flightArrows.clear();
        clearSelection();
        
        const auto& vertices = graph->get_all_vertices();
        int vertexCount = vertices.size();
        
        if (vertexCount == 0) {
            if (pathResultLabel) {
                pathResultLabel->setText("Граф пуст");
            }
            if (selectionTypeLabel) {
                selectionTypeLabel->setText("Граф пуст");
            }
            if (selectionNameLabel) {
                selectionNameLabel->setText("");
            }
            if (flightParamsLabel) {
                flightParamsLabel->setText("Добавьте города");
            }
            return;
        }
        
        // Если карта еще не загружена, загружаем ее
        if (!mapBackground) {
            loadMapToScene();
        }
        
        // Устанавливаем размер сцены
        int sceneWidth = 1200;
        int sceneHeight = 800;
        scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
        
        for (const auto& vertex : vertices) {
            const Vertex& v = vertex.second;
            int x = v.x;
            int y = v.y;
            
            if ((x == 0 && y == 0) || x < 50 || x > sceneWidth - 50 || y < 50 || y > sceneHeight - 50) {
                x = 100 + std::rand() % (sceneWidth - 200);
                y = 100 + std::rand() % (sceneHeight - 200);
                
                try {
                    graph->set_vertex_position(vertex.first, x, y);
                } catch (...) {

                }
            }
            
            DraggableVertex *circle = new DraggableVertex(QString::fromStdString(vertex.first));
            if (!circle) {
                qDebug() << "Failed to create circle for vertex:" << QString::fromStdString(vertex.first);
                continue;
            }
            
            circle->setPos(x, y);
            circle->setZValue(10);  // Убедимся, что вершины поверх карты
            scene->addItem(circle);
            
            cityCircles[vertex.first] = circle;
            cityPositions[vertex.first] = QPointF(x, y);
            
            QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(
                QString::fromStdString(vertex.first));
            QFont font = text->font();
            font.setPointSize(10);
            font.setBold(true);
            text->setFont(font);
            text->setPos(x - text->boundingRect().width()/2, y + 25);
            text->setZValue(11);
            scene->addItem(text);
        }
        
        const auto& edges = graph->get_all_edges();
        qDebug() << "Edge count:" << edges.size();
        
        for (const auto& edge : edges) {
            drawFlightLine(edge.second);
        }
        
        if (pathResultLabel) {
            pathResultLabel->setText("Граф ");
        }
        if (sccDetailsText) {
            sccDetailsText->clear();
            sccDetailsText->setHtml(
                "<div style='padding: 10px; color: #666; text-align: center; font-size: 10pt;'>"
                " "
                "</div>"
            );
        }
        
        updateComboBoxes();
        
        qDebug() << "Graph drawing complete";
        
    } catch (const std::exception& e) {
        qDebug() << "Exception in drawGraph:" << e.what();
        QMessageBox::warning(this, "Ошибка", QString("Ошибка при отрисовке графа: %1").arg(e.what()));
    }
}

void MainWindow::drawFlightLine(const Edge& edge)
{
    auto fromIt = cityPositions.find(edge.start);
    auto toIt = cityPositions.find(edge.finish);
    
    if (fromIt == cityPositions.end() || toIt == cityPositions.end()) {
        return;
    }
    
    QPointF fromPos = fromIt->second;
    QPointF toPos = toIt->second;
    
    double dx = toPos.x() - fromPos.x();
    double dy = toPos.y() - fromPos.y();
    double length = sqrt(dx*dx + dy*dy);
    
    if (length == 0) return;
    
    double offset = 15;
    double startX = fromPos.x() + (dx / length) * offset;
    double startY = fromPos.y() + (dy / length) * offset;
    double endX = toPos.x() - (dx / length) * offset;
    double endY = toPos.y() - (dy / length) * offset;
    
    QGraphicsLineItem *line = new QGraphicsLineItem(startX, startY, endX, endY);
    if (!line) return;
    
    line->setPen(QPen(QColor(80, 80, 80), 2, Qt::SolidLine, Qt::RoundCap));
    line->setZValue(1);
    scene->addItem(line);
    
    flightLines[edge.name] = line;
    
    double angle = atan2(dy, dx);
    double arrowSize = 6;
    
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
    
    flightArrows[edge.name] = arrow;
}

void MainWindow::loadDefaultGraph()
{
    qDebug() << "Loading default graph...";
    
    try {
        QString citiesFile = "data/data_for_graph.txt";
        
        if (!QFile::exists(citiesFile)) {
            qDebug() << "Cities file not found at:" << citiesFile;
            
            QStringList possiblePaths = {
                "data/data_for_graph.txt",
                "../data/data_for_graph.txt",
                "../../data/data_for_graph.txt",
                "./data/data_for_graph.txt"
            };
            
            bool fileFound = false;
            for (const QString& path : possiblePaths) {
                if (QFile::exists(path)) {
                    citiesFile = path;
                    fileFound = true;
                    qDebug() << "Found file at:" << path;
                    break;
                }
            }
            
            if (!fileFound) {
                qDebug() << "No cities file found. Starting with empty graph.";
                return; 
            }
        }
        
        QFile file(citiesFile);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            std::vector<std::string> cities;
            
            qDebug() << "Reading cities from file:" << citiesFile;
            
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (!line.isEmpty()) {
                    cities.push_back(line.toStdString());
                    qDebug() << "City from file:" << line;
                }
            }
            file.close();
            
            qDebug() << "Total cities read:" << cities.size();
            
            if (!cities.empty()) {
                try {
                    graph->generate_graph(cities, 20, 3);
                    qDebug() << "Graph generated successfully";
                } catch (const std::exception& e) {
                    qDebug() << "Error generating graph:" << e.what();
                }
            } else {
                qDebug() << "Cities file is empty";
            }
        } else {
            qDebug() << "Failed to open file:" << citiesFile;
        }
        
    } catch (const std::exception& e) {
        qDebug() << "Exception in loadDefaultGraph:" << e.what();
    }
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
        int sceneWidth = 1200;
        int sceneHeight = 800;
        int x = 100 + std::rand() % (sceneWidth - 200);
        int y = 100 + std::rand() % (sceneHeight - 200);
        
        graph->add_vertex(name.toStdString(), x, y);
        
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
            pathResultLabel->setText("Путь не найден");
            sccDetailsText->setHtml(
                "<div style='color: #666; padding: 15px; text-align: center;'>"
                "Путь между городами не найден"
                "</div>"
            );
        } else {
            QString pathStr;
            for (size_t i = 0; i < path.verts.size(); ++i) {
                pathStr += QString::fromStdString(path.verts[i]);
                if (i < path.verts.size() - 1) pathStr += " → ";
            }
            
            pathResultLabel->setText("Путь найден:");
            
            QString htmlDetails = "<div style='padding: 8px; color: #2c3e50; font-size: 10pt; line-height: 1.4;'>";
            htmlDetails += "<b>Маршрут:</b><br>";
            htmlDetails += pathStr + "<br><br>";
            htmlDetails += "<b>Параметры:</b><br>";

            htmlDetails += QString("• Стоимость: $%1<br>").arg(path.cost[0]);
            htmlDetails += QString("• Время: %1 ч<br>").arg(path.cost[1]);
            htmlDetails += QString("• Виза: $%1<br>").arg(path.cost[2]);

            htmlDetails += QString("<br><b>Взвешенная сумма: %1</b>").arg(path.weighted_sum);
            htmlDetails += "</div>";

            sccDetailsText->setHtml(htmlDetails);
            
            clearSelection();
            for (size_t i = 0; i < path.verts.size() - 1; i++) {
                std::string fromCity = path.verts[i];
                std::string toCity = path.verts[i + 1];
                
                if (cityCircles.find(fromCity) != cityCircles.end()) {
                    cityCircles[fromCity]->setBrush(QBrush(QColor(255, 200, 100)));
                }
                if (cityCircles.find(toCity) != cityCircles.end()) {
                    cityCircles[toCity]->setBrush(QBrush(QColor(255, 200, 100)));
                }
                
                std::string flightKey = fromCity + "_" + toCity;
                if (flightLines.find(flightKey) != flightLines.end()) {
                    flightLines[flightKey]->setPen(QPen(QColor(255, 100, 100), 3));
                }
            }
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
        
        // Удаляем только элементы графа, не фон
        QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem* item : items) {
            if (item != mapBackground && item->zValue() >= -90) {
                scene->removeItem(item);
                delete item;
            }
        }
        
        cityCircles.clear();
        cityPositions.clear();
        flightLines.clear();
        flightArrows.clear();
        
        updateComboBoxes();
        clearSelection();
        
        // Обновляем текст
        if (pathResultLabel) {
            pathResultLabel->setText("Граф очищен");
        }
        if (sccDetailsText) {
            sccDetailsText->setHtml(
                "<div style='color: #666; padding: 15px; text-align: center;'>"
                "Граф очищен"
                "</div>"
            );
        }
    }
}

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

void MainWindow::deleteSelected()
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
    }
}

void MainWindow::editSelectedCity()
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
    }
}

void MainWindow::editSelectedFlight()
{
    if (!isCitySelected && !selectedFlight.first.empty()) {
        std::string fromCity = selectedFlight.first;
        std::string toCity = selectedFlight.second;
        
        try {
            Edge edge;
            try {
                edge = graph->get_edge(fromCity, toCity);
            } catch (...) {
                edge.params = {costSpin->value(), timeSpin->value(), visaSpin->value()};
            }
            
            QDialog dialog(this);
            dialog.setWindowTitle("Изменить параметры перелета");
            dialog.setMinimumWidth(300);
            QVBoxLayout *layout = new QVBoxLayout(&dialog);
            layout->setContentsMargins(15, 15, 15, 15);
            layout->setSpacing(12);
            
            QHBoxLayout *costLayout = new QHBoxLayout();
            costLayout->addWidget(new QLabel("Стоимость ($):"));
            QSpinBox *newCostSpin = new QSpinBox();
            newCostSpin->setRange(0, 10000);
            newCostSpin->setValue(edge.params.size() > 0 ? edge.params[0] : costSpin->value());
            costLayout->addWidget(newCostSpin);
            
            QHBoxLayout *timeLayout = new QHBoxLayout();
            timeLayout->addWidget(new QLabel("Время (ч):"));
            QSpinBox *newTimeSpin = new QSpinBox();
            newTimeSpin->setRange(0, 48);
            newTimeSpin->setValue(edge.params.size() > 1 ? edge.params[1] : timeSpin->value());
            timeLayout->addWidget(newTimeSpin);
            
            QHBoxLayout *visaLayout = new QHBoxLayout();
            visaLayout->addWidget(new QLabel("Виза ($):"));
            QSpinBox *newVisaSpin = new QSpinBox();
            newVisaSpin->setRange(0, 500);
            newVisaSpin->setValue(edge.params.size() > 2 ? edge.params[2] : visaSpin->value());
            visaLayout->addWidget(newVisaSpin);
            
            QHBoxLayout *buttonLayout = new QHBoxLayout();
            QPushButton *saveBtn = new QPushButton("Сохранить");
            QPushButton *cancelBtn = new QPushButton("Отмена");
            buttonLayout->addWidget(saveBtn);
            buttonLayout->addWidget(cancelBtn);
            
            layout->addLayout(costLayout);
            layout->addLayout(timeLayout);
            layout->addLayout(visaLayout);
            layout->addLayout(buttonLayout);
            
            connect(saveBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
            connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
            
            if (dialog.exec() == QDialog::Accepted) {
                std::vector<int> newParams = {
                    newCostSpin->value(),
                    newTimeSpin->value(),
                    newVisaSpin->value()
                };
                
                if (graph->has_edge(fromCity, toCity)) {
                    graph->remove_edge(fromCity, toCity);
                }
                
                graph->add_edge(fromCity, toCity, newParams);
                
                drawGraph();
                updateComboBoxes();
            
                if (graph->has_edge(fromCity, toCity)) {
                    selectFlight(fromCity, toCity);
                } else {
                    selectCity(fromCity);
                }
            }
            
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", "Ошибка при изменении перелета");
            clearSelection();
        } catch (...) {
            QMessageBox::warning(this, "Ошибка", "Неизвестная ошибка");
            clearSelection();
        }
    }
}

void MainWindow::editCityPosition(const std::string& cityName)
{
    try {
        auto currentPos = graph->get_vertex_position(cityName);
        
        bool okX, okY;
        int x = QInputDialog::getInt(this, "Изменить позицию X", 
                                    "Координата X (0-1200):", 
                                    currentPos.first, 
                                    0, 1200, 1, &okX);
        if (!okX) return;
        
        int y = QInputDialog::getInt(this, "Изменить позицию Y", 
                                    "Координата Y (0-800):", 
                                    currentPos.second, 
                                    0, 800, 1, &okY);
        if (!okY) return;
        
        graph->set_vertex_position(cityName, x, y);
        
        if (cityCircles.find(cityName) != cityCircles.end()) {
            cityCircles[cityName]->setPos(x, y);
            cityPositions[cityName] = QPointF(x, y);

            QList<QGraphicsItem*> items = scene->items();
            for (QGraphicsItem* item : items) {
                if (QGraphicsSimpleTextItem* text = dynamic_cast<QGraphicsSimpleTextItem*>(item)) {
                    if (text->text() == QString::fromStdString(cityName)) {
                        text->setPos(x - text->boundingRect().width()/2, y + 25);
                        break;
                    }
                }
            }
            
            updateEdgesForVertex(cityName);
        }

        selectCity(cityName);
        
    } catch (...) {
        QMessageBox::warning(this, "Ошибка", "Не удалось изменить позицию города");
    }
}

void MainWindow::updateEdgesForVertex(const std::string& vertexName)
{
    auto outEdges = graph->get_to_vertices(vertexName);
    for (const auto& toVertex : outEdges) {
        std::string edgeName = vertexName + "_" + toVertex;
        if (flightLines.find(edgeName) != flightLines.end()) {
            scene->removeItem(flightLines[edgeName]);
            delete flightLines[edgeName];
            flightLines.erase(edgeName);
            
            if (flightArrows.find(edgeName) != flightArrows.end()) {
                scene->removeItem(flightArrows[edgeName]);
                delete flightArrows[edgeName];
                flightArrows.erase(edgeName);
            }
            
            try {
                Edge edge = graph->get_edge(vertexName, toVertex);
                drawFlightLine(edge);
            } catch (...) {
            }
        }
    }
    
    auto inEdges = graph->get_from_vertices(vertexName);
    for (const auto& fromVertex : inEdges) {
        std::string edgeName = fromVertex + "_" + vertexName;
        if (flightLines.find(edgeName) != flightLines.end()) {
            scene->removeItem(flightLines[edgeName]);
            delete flightLines[edgeName];
            flightLines.erase(edgeName);
            
            if (flightArrows.find(edgeName) != flightArrows.end()) {
                scene->removeItem(flightArrows[edgeName]);
                delete flightArrows[edgeName];
                flightArrows.erase(edgeName);
            }
            
            try {
                Edge edge = graph->get_edge(fromVertex, vertexName);
                drawFlightLine(edge);
            } catch (...) {
   
            }
        }
    }
}

void MainWindow::selectCity(const std::string& cityName)
{
    clearSelection();
    selectedCity = cityName;
    isCitySelected = true;
    
    if (cityCircles.find(cityName) != cityCircles.end()) {
        cityCircles[cityName]->setBrush(QBrush(QColor(100, 255, 150)));
        cityCircles[cityName]->setPen(QPen(Qt::darkRed, 2));
    }
    
    updateSelectionInfo();
}

void MainWindow::selectFlight(const std::string& from, const std::string& to)
{
    clearSelection();
    selectedFlight = {from, to};
    isCitySelected = false;
    
    std::string flightKey = from + "_" + to;
    if (flightLines.find(flightKey) != flightLines.end()) {
        flightLines[flightKey]->setPen(QPen(QColor(255, 100, 100), 3));
    }
    
    updateSelectionInfo();
    updateFlightInfo(from, to);
}

void MainWindow::clearSelection()
{
    for (auto& [cityName, circle] : cityCircles) {
        circle->setBrush(QBrush(QColor(100, 150, 255)));
        circle->setPen(QPen(Qt::darkBlue, 1));
    }
    
    for (auto& [flightKey, line] : flightLines) {
        line->setPen(QPen(QColor(80, 80, 80), 2));
    }
    
    selectedCity = "";
    selectedFlight = {"", ""};
    isCitySelected = false;
    updateSelectionInfo();
}

void MainWindow::updateSelectionInfo()
{
    if (isCitySelected && !selectedCity.empty()) {
        cityCombo->setCurrentText(QString::fromStdString(selectedCity));
        fromCombo->setCurrentText(QString::fromStdString(selectedCity));
        toCombo->setCurrentText(QString::fromStdString(selectedCity));
        
        selectionTypeLabel->setText("Выделен город");
        selectionNameLabel->setText(QString::fromStdString(selectedCity));
        
        try {
            auto pos = graph->get_vertex_position(selectedCity);
            flightParamsLabel->setText(
                QString("Координаты: X=%1, Y=%2").arg(pos.first).arg(pos.second)
            );
        } catch (...) {
            flightParamsLabel->setText("Координаты: не определены");
        }
        
    } else if (!isCitySelected && !selectedFlight.first.empty()) {
        fromCombo->setCurrentText(QString::fromStdString(selectedFlight.first));
        toCombo->setCurrentText(QString::fromStdString(selectedFlight.second));
        
        selectionTypeLabel->setText("Выделен перелет");
        selectionNameLabel->setText(
            QString::fromStdString(selectedFlight.first) + 
            " → " + 
            QString::fromStdString(selectedFlight.second));
        
    } else {
        selectionTypeLabel->setText("Ничего не выделено");
        selectionNameLabel->setText("");
        flightParamsLabel->setText("Выберите город или перелет");
    }
}

void MainWindow::updateFlightInfo(const std::string& from, const std::string& to)
{
    try {
        Edge edge = graph->get_edge(from, to);
        
        QString paramsText = "<div style='font-size: 9pt; line-height: 1.0; margin: 0; padding: 1px;'>";
        
        if (!edge.params.empty()) {
            paramsText += "<b>Параметры:</b> ";
            
            if (edge.params.size() >= 1) {
                paramsText += QString("Цена: $%1 ").arg(edge.params[0]);
            }
            if (edge.params.size() >= 2) {
                paramsText += QString("| Время: %1 ч").arg(edge.params[1]);
            }
            if (edge.params.size() >= 3 && edge.params[2] > 0) {
                paramsText += QString(" | Виза $%1").arg(edge.params[2]);
            }
            
            if (!edge.weights.empty()) {
                bool hasWeights = false;
                QString weightsStr;
                for (size_t i = 0; i < edge.weights.size() && i < 3; ++i) {
                    if (edge.weights[i] > 0.01) {
                        if (hasWeights) weightsStr += " ";
                        weightsStr += QString("%1").arg(edge.weights[i], 0, 'f', 1);
                        hasWeights = true;
                    }
                }
                if (hasWeights) {
                    paramsText += QString("<br><b>Веса:</b> %1").arg(weightsStr);
                }
            }
        } else {
            paramsText += "Нет параметров";
        }
        
        paramsText += "</div>";
        flightParamsLabel->setText(paramsText);
        
    } catch (const std::exception& e) {
        flightParamsLabel->setText("<div style='font-size: 8pt;'>Ошибка данных</div>");
    }
}

std::string MainWindow::findCityAtPos(const QPointF& pos)
{
    for (const auto& [cityName, cityPos] : cityPositions) {
        double dx = pos.x() - cityPos.x();
        double dy = pos.y() - cityPos.y();
        double distance = sqrt(dx*dx + dy*dy);
        
        if (distance <= 20) {
            return cityName;
        }
    }
    return "";
}

std::pair<std::string, std::string> MainWindow::findFlightAtPos(const QPointF& pos)
{
    const auto& edges = graph->get_all_edges();
    double minDistance = 10;
    std::pair<std::string, std::string> closestFlight = {"", ""};
    
    for (const auto& edge : edges) {
        auto fromIt = cityPositions.find(edge.second.start);
        auto toIt = cityPositions.find(edge.second.finish);
        
        if (fromIt == cityPositions.end() || toIt == cityPositions.end()) {
            continue;
        }
        
        QPointF p1 = fromIt->second;
        QPointF p2 = toIt->second;
        
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

void MainWindow::handleSceneClick(const QPointF& pos)
{
    qDebug() << "Click at:" << pos;
    
    std::string city = findCityAtPos(pos);
    if (!city.empty()) {
        selectCity(city);
        return;
    }

    auto flight = findFlightAtPos(pos);
    if (!flight.first.empty()) {
        selectFlight(flight.first, flight.second);
        return;
    }

    clearSelection();
}

void MainWindow::handleSceneRightClick(const QPointF& pos)
{
    std::string city = findCityAtPos(pos);
    if (!city.empty()) {
        selectCity(city);
    
        contextMenu->clear();
        contextMenu->addAction("Удалить город", this, &MainWindow::deleteSelected);
        contextMenu->addAction("Переименовать", this, &MainWindow::editSelectedCity);
        contextMenu->addAction("Изменить позицию", this, [this, city]() {
            editCityPosition(city);
        });
        contextMenu->popup(graphicsView->mapToGlobal(
            graphicsView->mapFromScene(pos)));
        return;
    }
    
    auto flight = findFlightAtPos(pos);
    if (!flight.first.empty()) {
        selectFlight(flight.first, flight.second);
        
        contextMenu->clear();
        contextMenu->addAction("Удалить перелет", this, &MainWindow::deleteSelected);
        contextMenu->addAction("Изменить параметры", this, &MainWindow::editSelectedFlight);
        contextMenu->popup(graphicsView->mapToGlobal(
            graphicsView->mapFromScene(pos)));
        return;
    }
}

void MainWindow::onColorSCC()
{
    qDebug() << "Coloring Strongly Connected Components...";
    
    try {
        std::vector<std::vector<std::string>> components = graph->find_scc();
        
        qDebug() << "Found" << components.size() << "SCC components";
        
        if (components.empty()) {
            pathResultLabel->setText("Компоненты сильной связности");
            sccDetailsText->setHtml(
                "<div style='color: #666; padding: 15px; text-align: center;'>"
                "Граф не содержит компонент сильной связности"
                "</div>"
            );
            QMessageBox::information(this, "Компоненты сильной связности", 
                                   "Граф не содержит компонент сильной связности");
            return;
        }
        
        QVector<QColor> colors = {
            QColor(255, 100, 100),
            QColor(100, 255, 100),  
            QColor(100, 100, 255), 
            QColor(255, 255, 100),  
            QColor(255, 100, 255), 
            QColor(100, 255, 255),
            QColor(255, 180, 100),  
            QColor(180, 255, 100), 
            QColor(180, 100, 255),   
            QColor(255, 150, 150)   
        };
        
        for (auto& [cityName, circle] : cityCircles) {
            circle->setBrush(QBrush(QColor(100, 150, 255)));
            circle->setPen(QPen(Qt::darkBlue, 1));
        }

        for (auto& [flightKey, line] : flightLines) {
            line->setPen(QPen(QColor(80, 80, 80), 2));
        }
        
        for (auto& [flightKey, arrow] : flightArrows) {
            arrow->setBrush(QBrush(QColor(80, 80, 80)));
            arrow->setPen(QPen(QColor(80, 80, 80)));
        }

        QString htmlText = "<div style='padding: 12px;'>";
        htmlText += "<h3 style='margin-top: 0; color: #2c3e50; border-bottom: 1px solid #ddd; padding-bottom: 5px;'>"
                   "КСС: " + QString::number(components.size()) + "</h3>";
        
        for (size_t i = 0; i < components.size(); ++i) {
            QColor color = colors[i % colors.size()];
            
            htmlText += "<div style='margin: 12px 0; padding: 10px; "
                       "border: 2px solid " + color.darker(150).name() + "; "
                       "border-radius: 5px; background-color: " + color.lighter(120).name() + ";'>";
            
            htmlText += "<b style='color: " + color.darker(200).name() + "; font-size: 10.5pt;'>"
                       "Компонента " + QString::number(i + 1) + " (" +
                       QString::number(components[i].size()) + " вершин):</b><br>";
            
            htmlText += "<div style='margin-left: 15px; margin-top: 6px; font-size: 10pt;'>";
            
            for (size_t j = 0; j < components[i].size(); ++j) {
                const std::string& cityName = components[i][j];
                
                htmlText += "<span style='font-weight: bold; color: #2c3e50;'>" + 
                           QString::fromStdString(cityName) + "</span>";
                
                if (j < components[i].size() - 1) {
                    htmlText += (j % 5 == 4) ? ",<br>" : ", ";
                }
                
                if (cityCircles.find(cityName) != cityCircles.end()) {
                    cityCircles[cityName]->setBrush(QBrush(color));
                    cityCircles[cityName]->setPen(QPen(color.darker(150), 2));
                }
            }
            
            htmlText += "</div></div>";
            
            for (size_t j = 0; j < components[i].size(); ++j) {
                const std::string& fromCity = components[i][j];
                
                auto neighbors = graph->get_to_vertices(fromCity);
                for (const auto& toCity : neighbors) {
                    bool inSameComponent = false;
                    for (const auto& city : components[i]) {
                        if (city == toCity) {
                            inSameComponent = true;
                            break;
                        }
                    }
                    
                    if (inSameComponent) {
                        std::string flightKey = fromCity + "_" + toCity;
                        if (flightLines.find(flightKey) != flightLines.end()) {
                            flightLines[flightKey]->setPen(QPen(color.darker(120), 3)); 
                            flightLines[flightKey]->setZValue(3);
                        }
                        if (flightArrows.find(flightKey) != flightArrows.end()) {
                            flightArrows[flightKey]->setBrush(QBrush(color.darker(120))); 
                            flightArrows[flightKey]->setPen(QPen(color.darker(120)));
                            flightArrows[flightKey]->setZValue(4);
                        }
                    }
                }
            }
        }
        
        htmlText += "</div>";
        
        pathResultLabel->setText("Компоненты сильной связности найдены");
        sccDetailsText->setHtml(htmlText);

        sccDetailsText->verticalScrollBar()->setValue(0);
        
        QTimer::singleShot(100, this, [this, components]() {
            QString message = QString("Найдено %1 компонент(ы) сильной связности.\n")
                                    .arg(components.size());
        });
        
    } catch (const std::exception& e) {
        qDebug() << "Error finding SCC:" << e.what();
        QMessageBox::warning(this, "Ошибка", 
                           QString("Ошибка при поиске компонент связности: %1")
                           .arg(e.what()));
    }
}

void MainWindow::onResetColors()
{
    // Перерисовываем граф (фон сохранится)
    drawGraph();
    
    pathResultLabel->setText("Цвета сброшены");
    sccDetailsText->setHtml(
        "<div style='color: #666; padding: 15px; text-align: center;'>"
        "Цвета вершин и ребер сброшены к стандартным"
        "</div>"
    );
    
    clearSelection();
}

void MainWindow::loadMapToScene()
{
    if (!scene) return;
    
    if (mapBackground) {
        return;
    }
    
    QString mapPath;
QStringList possiblePaths = {
    "world_map.jpg",  // В корне проекта
    ":/images/world_map.jpg",  // Если используете ресурсы Qt
     QApplication::applicationDirPath() + "/world_map.jpg"
    };

    
    for (const QString& path : possiblePaths) {
        if (QFile::exists(path)) {
            mapPath = path;
            qDebug() << "Found map at:" << path;
            break;
        }
    }
    
    if (mapPath.isEmpty()) {
        qDebug() << "Map not found at any location, using solid background";
        // Создаем простой фон
        QGraphicsRectItem *background = new QGraphicsRectItem(0, 0, 1200, 800);
        background->setBrush(QBrush(QColor(200, 220, 240)));  
        background->setZValue(-100);
        background->setOpacity(0.7);
        scene->addItem(background);
        mapBackground = background;  // Теперь можно присвоить, так как mapBackground - QGraphicsItem*
        return;
    }
    
    QPixmap worldMap(mapPath);
    if (worldMap.isNull()) {
        qDebug() << "Failed to load map pixmap from:" << mapPath;
        // Создаем простой фон
        QGraphicsRectItem *background = new QGraphicsRectItem(0, 0, 1200, 800);
        background->setBrush(QBrush(QColor(200, 220, 240)));  
        background->setZValue(-100);
        background->setOpacity(0.7);
        scene->addItem(background);
        mapBackground = background;  // Теперь можно присвоить
        return;
    }
    
    // Увеличим контраст и уменьшим яркость для лучшей видимости вершин
    QImage image = worldMap.toImage();
    
    for (int y = 0; y < image.height(); ++y) {
        QRgb *scanLine = reinterpret_cast<QRgb*>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            QColor color = QColor::fromRgb(scanLine[x]);
            // Уменьшаем яркость и увеличиваем контраст
            color = color.darker(130); 
            scanLine[x] = color.rgb();
        }
    }
    
    QPixmap adjustedMap = QPixmap::fromImage(image);
    // Масштабируем карту под размер сцены
    adjustedMap = adjustedMap.scaled(1200, 800, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    
    // Создаем фон
    QGraphicsPixmapItem *background = new QGraphicsPixmapItem(adjustedMap);
    background->setZValue(-100);  // Очень низкий z-value, чтобы был фоном
    background->setOpacity(0.5);  // Полупрозрачность для лучшей видимости вершин
    scene->addItem(background);
    mapBackground = background;  // Теперь можно присвоить
    
    qDebug() << "Map loaded successfully with opacity 0.5";
}