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
#include <QFile>
#include <QTextStream>
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
#include <QPainter>
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
      weightCostSpin(nullptr),
      weightTimeSpin(nullptr),
      weightVisaSpin(nullptr),
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
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    try {
        setupUI();
        loadMapToScene();
        loadDefaultGraph();
        drawGraph();
        
        setWindowTitle("Граф городов");
        resize(1400, 850);
        setMinimumSize(1100, 700);
        
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Ошибка", QString("Ошибка при создании окна: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(nullptr, "Ошибка", "Неизвестная ошибка при создании окна");
    }
}

MainWindow::~MainWindow()
{
    delete graph;
}

void MainWindow::setupUI()
{
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
        pathGroup->setMaximumHeight(160);
        QVBoxLayout *pathLayout = new QVBoxLayout(pathGroup);
        pathLayout->setContentsMargins(8, 8, 8, 8);
        pathLayout->setSpacing(6);

        QHBoxLayout *checkRow = new QHBoxLayout();
        checkRow->setSpacing(10);

        checkCost = new QCheckBox("стоимость", pathGroup);
        checkTime = new QCheckBox("время", pathGroup);
        checkVisa = new QCheckBox("виза", pathGroup);
        checkCost->setChecked(true);
        checkTime->setChecked(true);
        checkVisa->setChecked(false);

        checkCost->setFixedSize(80, 20);
        checkTime->setFixedSize(80, 20);
        checkVisa->setFixedSize(80, 20);

        checkCost->setStyleSheet("QCheckBox { font-size: 9pt; }");
        checkTime->setStyleSheet("QCheckBox { font-size: 9pt; }");
        checkVisa->setStyleSheet("QCheckBox { font-size: 9pt; }");

        checkRow->addWidget(checkCost);
        checkRow->addWidget(checkTime);
        checkRow->addWidget(checkVisa);
        checkRow->addStretch(1);

        QHBoxLayout *weightRow = new QHBoxLayout();
        weightRow->setSpacing(10);

        weightCostSpin = new QDoubleSpinBox(pathGroup);
        weightCostSpin->setRange(0.0, 1.0);
        weightCostSpin->setSingleStep(0.1);
        weightCostSpin->setValue(1.0);
        weightCostSpin->setDecimals(1);
        weightCostSpin->setFixedSize(55, 22);
        weightCostSpin->setStyleSheet("QDoubleSpinBox { font-size: 8pt; }");

        weightTimeSpin = new QDoubleSpinBox(pathGroup);
        weightTimeSpin->setRange(0.0, 1.0);
        weightTimeSpin->setSingleStep(0.1);
        weightTimeSpin->setValue(1.0);
        weightTimeSpin->setDecimals(1);
        weightTimeSpin->setFixedSize(55, 22);
        weightTimeSpin->setStyleSheet("QDoubleSpinBox { font-size: 8pt; }");

        weightVisaSpin = new QDoubleSpinBox(pathGroup);
        weightVisaSpin->setRange(0.0, 1.0);
        weightVisaSpin->setSingleStep(0.1);
        weightVisaSpin->setValue(1.0);
        weightVisaSpin->setDecimals(1);
        weightVisaSpin->setFixedSize(55, 22);
        weightVisaSpin->setStyleSheet("QDoubleSpinBox { font-size: 8pt; }");

        QLabel *weightLabel = new QLabel("Веса:", pathGroup);
        weightLabel->setStyleSheet("QLabel { font-size: 8pt; }");

        weightRow->addWidget(weightLabel);
        weightRow->addWidget(weightCostSpin);
        weightRow->addWidget(weightTimeSpin);
        weightRow->addWidget(weightVisaSpin);
        weightRow->addStretch(1);

        findPathBtn = new QPushButton("Найти путь", pathGroup);
        findPathBtn->setFixedHeight(28);

        pathLayout->addLayout(checkRow);
        pathLayout->addLayout(weightRow);
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
        
    } catch (const std::exception& e) {
        throw;
    }
}

void MainWindow::drawGraph()
{
    if (!scene) {
        return;
    }
    
    try {
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
        
        if (!mapBackground) {
            loadMapToScene();
        }
        
        int sceneWidth = 1200;
        int sceneHeight = 800;
        scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
        
        for (const auto& vertex_pair : vertices) {
            const Vertex& v = vertex_pair.second;
            const std::string& vertex_name = v.name;
            
            int x = v.x;
            int y = v.y;
            
            if ((x == 0 && y == 0) || x < 50 || x > sceneWidth - 50 || y < 50 || y > sceneHeight - 50) {
                x = 100 + std::rand() % (sceneWidth - 200);
                y = 100 + std::rand() % (sceneHeight - 200);
                
                try {
                    graph->set_vertex_position(vertex_name, x, y);
                } catch (...) {
                }
            }
            
            DraggableVertex *circle = new DraggableVertex(QString::fromStdString(vertex_name));
            if (!circle) {
                continue;
            }
            
            circle->setPos(x, y);
            circle->setZValue(10);
            scene->addItem(circle);
            
            cityCircles[vertex_name] = circle;
            cityPositions[vertex_name] = QPointF(x, y);
            
            QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(
                QString::fromStdString(vertex_name));
            QFont font = text->font();
            font.setPointSize(10);
            font.setBold(true);
            text->setFont(font);
            text->setPos(x - text->boundingRect().width()/2, y + 25);
            text->setZValue(11);
            scene->addItem(text);
        }
        
        const auto& edges = graph->get_all_edges();
        
        for (const auto& edge : edges) {
            drawFlightLine(edge.second);
        }
        
        if (pathResultLabel) {
            pathResultLabel->setText("Граф");
        }
        if (sccDetailsText) {
            sccDetailsText->clear();
        }
        
        updateComboBoxes();
        
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", QString("Ошибка при отрисовке графа: %1").arg(e.what()));
    }
}

void MainWindow::drawFlightLine(const Edge& edge)
{
    std::string from_name = graph->get_vertex_name_by_id(edge.start);
    std::string to_name = graph->get_vertex_name_by_id(edge.finish);
    
    if (from_name.empty() || to_name.empty()) {
        return;
    }
    
    auto from_it = cityPositions.find(from_name);
    auto to_it = cityPositions.find(to_name);
    
    if (from_it == cityPositions.end() || to_it == cityPositions.end()) {
        return;
    }
    
    QPointF fromPos = from_it->second;
    QPointF toPos = to_it->second;
    
    std::string edge_key = from_name + "_" + to_name;
    
    if (flightLines.find(edge_key) != flightLines.end()) {
        scene->removeItem(flightLines[edge_key]);
        delete flightLines[edge_key];
        flightLines.erase(edge_key);
    }
    if (flightArrows.find(edge_key) != flightArrows.end()) {
        scene->removeItem(flightArrows[edge_key]);
        delete flightArrows[edge_key];
        flightArrows.erase(edge_key);
    }
    
    double dx = toPos.x() - fromPos.x();
    double dy = toPos.y() - fromPos.y();
    double length = sqrt(dx*dx + dy*dy);
    
    if (length == 0) {
        return;
    }
    
    double offset = 15;
    double startX = fromPos.x() + (dx / length) * offset;
    double startY = fromPos.y() + (dy / length) * offset;
    double endX = toPos.x() - (dx / length) * offset;
    double endY = toPos.y() - (dy / length) * offset;
    
    QGraphicsLineItem *line = new QGraphicsLineItem(startX, startY, endX, endY);
    if (!line) {
        return;
    }
    
    line->setPen(QPen(QColor(80, 80, 80), 2, Qt::SolidLine, Qt::RoundCap));
    line->setZValue(1);
    scene->addItem(line);
    
    flightLines[edge_key] = line;
    
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
    
    flightArrows[edge_key] = arrow;
}

void MainWindow::loadDefaultGraph()
{
    try {
        QString citiesFile = "data/data_for_graph.txt";
        
        if (!QFile::exists(citiesFile)) {
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
                    break;
                }
            }
            
            if (!fileFound) {
                return;
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
                try {
                    graph->generate_graph(cities, 17, 3);
                } catch (const std::exception& e) {
                }
            }
        }
        
    } catch (const std::exception& e) {
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
    QString from_name = fromCombo->currentText();
    QString to_name = toCombo->currentText();
    
    if (from_name.isEmpty() || to_name.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите города");
        return;
    }
    
    if (from_name == to_name) {
        QMessageBox::warning(this, "Ошибка", "Города должны быть разными");
        return;
    }
    
    if (!graph->has_vertex(from_name.toStdString()) || 
        !graph->has_vertex(to_name.toStdString())) {
        QMessageBox::warning(this, "Ошибка", "Один из городов не найден");
        return;
    }
    
    std::vector<int> params = {
        costSpin->value(),
        timeSpin->value(),
        visaSpin->value()
    };
    
    try {
        graph->add_edge(from_name.toStdString(), to_name.toStdString(), params);
        drawGraph();
        updateComboBoxes();
        selectFlight(from_name.toStdString(), to_name.toStdString());
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
    
    std::vector<bool> consider_params = {
        checkCost->isChecked(),
        checkTime->isChecked(),
        checkVisa->isChecked()
    };
    
    std::vector<double> weights = {
        weightCostSpin->value(),
        weightTimeSpin->value(),
        weightVisaSpin->value()
    };
    
    try {
        std::vector<Path> all_paths = dijkstra_all_optimal_paths(*graph, from.toStdString(), to.toStdString(), consider_params, weights);
        
        if (all_paths.empty() || all_paths[0].verts.empty()) {
            pathResultLabel->setText("Путь не найден");
            sccDetailsText->setHtml(
                "<div style='color: #666; padding: 15px; text-align: center;'>"
                "Путь между городами не найден"
                "</div>"
            );
            return;
        }
        
        QVector<QColor> pathColors = {
            QColor(255, 100, 100),   
            QColor(100, 255, 100),  
            QColor(100, 100, 255), 
            QColor(255, 255, 100),  
            QColor(255, 100, 255),   
            QColor(100, 255, 255),   
            QColor(255, 180, 100),  
            QColor(180, 100, 255)     
        };
        
        clearSelection();
        
        QString htmlDetails = "<div style='padding: 8px; color: #2c3e50; font-size: 10pt; line-height: 1.4;'>";
        
        if (all_paths.size() == 1) {
            htmlDetails += "<b>Найден 1 оптимальный путь:</b><br><br>";
        } else {
            htmlDetails += QString("<b>Найдено %1 оптимальных путей (одинаковая стоимость):</b><br><br>")
                          .arg(all_paths.size());
        }
        
        for (size_t i = 0; i < all_paths.size(); ++i) {
            const Path& path = all_paths[i];
            QColor pathColor = pathColors[i % pathColors.size()];
            
            htmlDetails += QString("<div style='margin-bottom: 15px; padding: 10px; "
                                  "border-left: 4px solid %1; background-color: %2;'>")
                          .arg(pathColor.darker(130).name())
                          .arg(pathColor.lighter(150).name());
            
            htmlDetails += QString("<b style='color: %1;'>Путь %2:</b><br>")
                          .arg(pathColor.darker(150).name())
                          .arg(i + 1);
            
            QString pathStr;
            for (size_t j = 0; j < path.verts.size(); ++j) {
                pathStr += QString::fromStdString(path.verts[j]);
                if (j < path.verts.size() - 1) pathStr += " → ";
            }
            
            htmlDetails += pathStr + "<br><br>";
            
            htmlDetails += "<b>Параметры:</b><br>";
            
            htmlDetails += QString("• Стоимость: $%1").arg(path.cost[0]);
            if (checkCost->isChecked()) {
                htmlDetails += QString(" (вес: %1 = %2)").arg(weights[0], 0, 'f', 1)
                                                     .arg(path.cost[0] * weights[0], 0, 'f', 2);
            }
            htmlDetails += "<br>";
            
            htmlDetails += QString("• Время: %1 ч").arg(path.cost[1]);
            if (checkTime->isChecked()) {
                htmlDetails += QString(" (вес: %1 = %2)").arg(weights[1], 0, 'f', 1)
                                                     .arg(path.cost[1] * weights[1], 0, 'f', 2);
            }
            htmlDetails += "<br>";
            
            htmlDetails += QString("• Виза: $%1").arg(path.cost[2]);
            if (checkVisa->isChecked()) {
                htmlDetails += QString(" (вес: %1 = %2)").arg(weights[2], 0, 'f', 1)
                                                     .arg(path.cost[2] * weights[2], 0, 'f', 2);
            }
            
            double weighted_sum = 0.0;
            for (size_t k = 0; k < path.cost.size() && k < weights.size(); ++k) {
                if (consider_params[k]) {
                    weighted_sum += path.cost[k] * weights[k];
                }
            }
            
            htmlDetails += QString("<br><b>Взвешенная сумма: %1</b>").arg(weighted_sum, 0, 'f', 2);
            htmlDetails += "</div>";
            
            for (size_t j = 0; j < path.verts.size() - 1; j++) {
                std::string fromCity = path.verts[j];
                std::string toCity = path.verts[j + 1];
                
                if (cityCircles.find(fromCity) != cityCircles.end()) {
                    cityCircles[fromCity]->setBrush(QBrush(pathColor.lighter(120)));
                    cityCircles[fromCity]->setPen(QPen(pathColor.darker(150), 2));
                }
                
                std::string flightKey = fromCity + "_" + toCity;
                if (flightLines.find(flightKey) != flightLines.end()) {
                    flightLines[flightKey]->setPen(QPen(pathColor, 3));
                    flightLines[flightKey]->setZValue(10 + i); 
                }
            }
        }
        
        for (size_t i = 0; i < all_paths.size(); ++i) {
            const Path& path = all_paths[i];
            if (!path.verts.empty()) {
                std::string lastCity = path.verts.back();
                if (cityCircles.find(lastCity) != cityCircles.end()) {
                    cityCircles[lastCity]->setBrush(QBrush(pathColors[i % pathColors.size()].lighter(120)));
                    cityCircles[lastCity]->setPen(QPen(pathColors[i % pathColors.size()].darker(150), 2));
                }
            }
        }
        
        htmlDetails += "</div>";
        
        pathResultLabel->setText(QString("Найдено путей: %1").arg(all_paths.size()));
        sccDetailsText->setHtml(htmlDetails);
        
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
    
    std::vector<std::string> cityNames = graph->get_all_vertex_names();
    
    for (const auto& name : cityNames) {
        QString qname = QString::fromStdString(name);
        cityCombo->addItem(qname);
        fromCombo->addItem(qname);
        toCombo->addItem(qname);
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
                graph->rename_vertex(selectedCity, newName.toStdString());
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
            
            QGroupBox *paramsGroup = new QGroupBox("Параметры");
            QGridLayout *paramsLayout = new QGridLayout(paramsGroup);
            
            paramsLayout->addWidget(new QLabel("Стоимость ($):"), 0, 0);
            QSpinBox *newCostSpin = new QSpinBox();
            newCostSpin->setRange(0, 10000);
            newCostSpin->setValue(edge.params.size() > 0 ? edge.params[0] : costSpin->value());
            paramsLayout->addWidget(newCostSpin, 0, 1);
            
            paramsLayout->addWidget(new QLabel("Время (ч):"), 1, 0);
            QSpinBox *newTimeSpin = new QSpinBox();
            newTimeSpin->setRange(0, 48);
            newTimeSpin->setValue(edge.params.size() > 1 ? edge.params[1] : timeSpin->value());
            paramsLayout->addWidget(newTimeSpin, 1, 1);
            
            paramsLayout->addWidget(new QLabel("Виза ($):"), 2, 0);
            QSpinBox *newVisaSpin = new QSpinBox();
            newVisaSpin->setRange(0, 500);
            newVisaSpin->setValue(edge.params.size() > 2 ? edge.params[2] : visaSpin->value());
            paramsLayout->addWidget(newVisaSpin, 2, 1);
            
            QHBoxLayout *buttonLayout = new QHBoxLayout();
            QPushButton *saveBtn = new QPushButton("Сохранить");
            QPushButton *cancelBtn = new QPushButton("Отмена");
            buttonLayout->addWidget(saveBtn);
            buttonLayout->addWidget(cancelBtn);
            
            layout->addWidget(paramsGroup);
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

void MainWindow::updateEdgesForVertex(const std::string& vertex_name)
{
    if (cityCircles.find(vertex_name) == cityCircles.end()) {
        return;
    }
    
    auto outEdges = graph->get_to_vertices(vertex_name);
    
    for (const auto& to_vertex_name : outEdges) {
        std::string edge_key = vertex_name + "_" + to_vertex_name;
        
        if (flightLines.find(edge_key) != flightLines.end()) {
            scene->removeItem(flightLines[edge_key]);
            delete flightLines[edge_key];
            flightLines.erase(edge_key);
            
            if (flightArrows.find(edge_key) != flightArrows.end()) {
                scene->removeItem(flightArrows[edge_key]);
                delete flightArrows[edge_key];
                flightArrows.erase(edge_key);
            }
        }
        
        if (graph->has_edge(vertex_name, to_vertex_name)) {
            try {
                Edge edge = graph->get_edge(vertex_name, to_vertex_name);
                drawFlightLine(edge);
            } catch (...) {
            }
        }
    }
    
    auto inEdges = graph->get_from_vertices(vertex_name);
    for (const auto& from_vertex_name : inEdges) {
        std::string edge_key = from_vertex_name + "_" + vertex_name;
  
        if (flightLines.find(edge_key) != flightLines.end()) {
            scene->removeItem(flightLines[edge_key]);
            delete flightLines[edge_key];
            flightLines.erase(edge_key);

            if (flightArrows.find(edge_key) != flightArrows.end()) {
                scene->removeItem(flightArrows[edge_key]);
                delete flightArrows[edge_key];
                flightArrows.erase(edge_key);
            }
        }
        
        if (graph->has_edge(from_vertex_name, vertex_name)) {
            try {
                Edge edge = graph->get_edge(from_vertex_name, vertex_name);
                drawFlightLine(edge);
            } catch (...) {
            }
        }
    }
}

void MainWindow::selectCity(const std::string& city_name)
{
    clearSelection();
    selectedCity = city_name;
    isCitySelected = true;

    if (cityCircles.find(city_name) != cityCircles.end()) {
        cityCircles[city_name]->setBrush(QBrush(QColor(100, 255, 150)));
        cityCircles[city_name]->setPen(QPen(Qt::darkRed, 2));
    }
    
    updateSelectionInfo();
}

void MainWindow::selectFlight(const std::string& from_name, const std::string& to_name)
{
    clearSelection();
    selectedFlight = {from_name, to_name};
    isCitySelected = false;

    std::string edge_key = from_name + "_" + to_name;
    
    if (flightLines.find(edge_key) != flightLines.end()) {
        flightLines[edge_key]->setPen(QPen(QColor(255, 100, 100), 3));
    }
    
    updateSelectionInfo();
    updateFlightInfo(from_name, to_name);
}

void MainWindow::clearSelection()
{
    for (auto& [city_name, circle] : cityCircles) {
        circle->setBrush(QBrush(QColor(100, 150, 255)));
        circle->setPen(QPen(Qt::darkBlue, 1));
    }
    
    for (auto& [edge_key, line] : flightLines) {
        line->setPen(QPen(QColor(80, 80, 80), 2));
    }
    
    for (auto& [edge_key, arrow] : flightArrows) {
        arrow->setBrush(QBrush(QColor(80, 80, 80)));
        arrow->setPen(QPen(QColor(80, 80, 80)));
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
            paramsText += "<b>Параметры:</b><br>";
            
            QStringList paramNames = {"Стоимость", "Время", "Виза"};
            
            for (size_t i = 0; i < edge.params.size() && i < 3; ++i) {
                paramsText += QString("• %1: %2").arg(paramNames[i]).arg(edge.params[i]);
                
                if (i == 0) paramsText += " $";
                else if (i == 1) paramsText += " ч";
                else if (i == 2) paramsText += " $";
                
                paramsText += "<br>";
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
    const auto& vertexNames = graph->get_all_vertex_names();
    double minDistance = 10;
    std::pair<std::string, std::string> closestFlight = {"", ""};

    for (const auto& fromName : vertexNames) {
        auto neighbors = graph->get_to_vertices(fromName);
        for (const auto& toName : neighbors) {
            if (!graph->has_edge(fromName, toName)) {
                continue;
            }
            
            auto fromIt = cityPositions.find(fromName);
            auto toIt = cityPositions.find(toName);
            
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
                closestFlight = {fromName, toName};
            }
        }
    }
    
    return closestFlight;
}

void MainWindow::handleSceneClick(const QPointF& pos)
{
    std::string city_name = findCityAtPos(pos);
    if (!city_name.empty()) {
        selectCity(city_name);
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
    try {
        std::vector<std::vector<std::string>> components = find_scc(*graph);
        
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
        
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", 
                           QString("Ошибка при поиске компонент связности: %1")
                           .arg(e.what()));
    }
}

void MainWindow::onResetColors()
{
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
    if (mapBackground) return;
    
    int sceneWidth = 1200;
    int sceneHeight = 800;
    
    QPixmap backgroundPixmap(sceneWidth, sceneHeight);
    backgroundPixmap.fill(Qt::transparent);
    
    QPainter painter(&backgroundPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QLinearGradient oceanGradient(0, 0, 0, sceneHeight);
    oceanGradient.setColorAt(0.0, QColor(225, 245, 255));
    oceanGradient.setColorAt(0.5, QColor(173, 216, 230));
    oceanGradient.setColorAt(1.0, QColor(135, 206, 235));
    painter.fillRect(0, 0, sceneWidth, sceneHeight, oceanGradient);
    
    auto lonToX = [sceneWidth](double lon) -> double {
        return ((lon + 180.0) / 360.0) * sceneWidth;
    };
    
    auto latToYSimple = [sceneHeight](double lat) -> double {
        return ((90.0 - lat) / 180.0) * sceneHeight * 1.05;
    };
        
    auto latToYUsed = latToYSimple;
    
    QPainterPath northAmerica;
    northAmerica.moveTo(lonToX(-170), latToYUsed(65));
    northAmerica.lineTo(lonToX(-165), latToYUsed(60));
    northAmerica.lineTo(lonToX(-155), latToYUsed(58));
    northAmerica.lineTo(lonToX(-145), latToYUsed(56));
    northAmerica.lineTo(lonToX(-135), latToYUsed(55));
    northAmerica.lineTo(lonToX(-130), latToYUsed(52));
    northAmerica.lineTo(lonToX(-125), latToYUsed(50));
    northAmerica.lineTo(lonToX(-120), latToYUsed(48));
    northAmerica.lineTo(lonToX(-118), latToYUsed(45));
    northAmerica.lineTo(lonToX(-115), latToYUsed(42));
    northAmerica.lineTo(lonToX(-112), latToYUsed(38));
    northAmerica.lineTo(lonToX(-110), latToYUsed(35));
    northAmerica.lineTo(lonToX(-108), latToYUsed(32));
    northAmerica.lineTo(lonToX(-105), latToYUsed(30));
    northAmerica.lineTo(lonToX(-102), latToYUsed(28));
    northAmerica.lineTo(lonToX(-100), latToYUsed(25));
    northAmerica.lineTo(lonToX(-98), latToYUsed(22));
    northAmerica.lineTo(lonToX(-96), latToYUsed(20));
    northAmerica.lineTo(lonToX(-94), latToYUsed(18));
    northAmerica.lineTo(lonToX(-92), latToYUsed(16));
    northAmerica.lineTo(lonToX(-90), latToYUsed(14));
    northAmerica.lineTo(lonToX(-88), latToYUsed(12));
    northAmerica.lineTo(lonToX(-86), latToYUsed(10));
    northAmerica.lineTo(lonToX(-84), latToYUsed(9));
    northAmerica.lineTo(lonToX(-82), latToYUsed(8));
    northAmerica.lineTo(lonToX(-80), latToYUsed(12));
    northAmerica.lineTo(lonToX(-78), latToYUsed(18));
    northAmerica.lineTo(lonToX(-76), latToYUsed(25));
    northAmerica.lineTo(lonToX(-74), latToYUsed(30));
    northAmerica.lineTo(lonToX(-72), latToYUsed(35));
    northAmerica.lineTo(lonToX(-70), latToYUsed(40));
    northAmerica.lineTo(lonToX(-68), latToYUsed(45));
    northAmerica.lineTo(lonToX(-66), latToYUsed(48));
    northAmerica.lineTo(lonToX(-64), latToYUsed(50));
    northAmerica.lineTo(lonToX(-62), latToYUsed(52));
    northAmerica.lineTo(lonToX(-60), latToYUsed(55));
    northAmerica.lineTo(lonToX(-65), latToYUsed(60));
    northAmerica.lineTo(lonToX(-70), latToYUsed(65));
    northAmerica.lineTo(lonToX(-75), latToYUsed(68));
    northAmerica.lineTo(lonToX(-80), latToYUsed(70));
    northAmerica.lineTo(lonToX(-85), latToYUsed(72));
    northAmerica.lineTo(lonToX(-90), latToYUsed(73));
    northAmerica.lineTo(lonToX(-95), latToYUsed(74));
    northAmerica.lineTo(lonToX(-100), latToYUsed(73));
    northAmerica.lineTo(lonToX(-105), latToYUsed(72));
    northAmerica.lineTo(lonToX(-110), latToYUsed(70));
    northAmerica.lineTo(lonToX(-115), latToYUsed(68));
    northAmerica.lineTo(lonToX(-120), latToYUsed(66));
    northAmerica.lineTo(lonToX(-125), latToYUsed(64));
    northAmerica.lineTo(lonToX(-130), latToYUsed(62));
    northAmerica.lineTo(lonToX(-135), latToYUsed(60));
    northAmerica.lineTo(lonToX(-140), latToYUsed(62));
    northAmerica.lineTo(lonToX(-145), latToYUsed(64));
    northAmerica.lineTo(lonToX(-150), latToYUsed(65));
    northAmerica.lineTo(lonToX(-155), latToYUsed(66));
    northAmerica.lineTo(lonToX(-160), latToYUsed(65));
    northAmerica.closeSubpath();
    
    QPainterPath greenland;
    greenland.moveTo(lonToX(-65), latToYUsed(60));
    greenland.lineTo(lonToX(-60), latToYUsed(61));
    greenland.lineTo(lonToX(-55), latToYUsed(63));
    greenland.lineTo(lonToX(-50), latToYUsed(65));
    greenland.lineTo(lonToX(-45), latToYUsed(68));
    greenland.lineTo(lonToX(-40), latToYUsed(72));
    greenland.lineTo(lonToX(-35), latToYUsed(75));
    greenland.lineTo(lonToX(-30), latToYUsed(77));
    greenland.lineTo(lonToX(-35), latToYUsed(79));
    greenland.lineTo(lonToX(-40), latToYUsed(77));
    greenland.lineTo(lonToX(-45), latToYUsed(75));
    greenland.lineTo(lonToX(-50), latToYUsed(72));
    greenland.lineTo(lonToX(-55), latToYUsed(69));
    greenland.lineTo(lonToX(-60), latToYUsed(66));
    greenland.lineTo(lonToX(-65), latToYUsed(63));
    greenland.closeSubpath();
            
    QPainterPath southAmerica;
    southAmerica.moveTo(lonToX(-95), latToYUsed(8));
    southAmerica.lineTo(lonToX(-93), latToYUsed(5));
    southAmerica.lineTo(lonToX(-91), latToYUsed(2));
    southAmerica.lineTo(lonToX(-89), latToYUsed(0));
    southAmerica.lineTo(lonToX(-87), latToYUsed(-2));
    southAmerica.lineTo(lonToX(-85), latToYUsed(-5));
    southAmerica.lineTo(lonToX(-83), latToYUsed(-10));
    southAmerica.lineTo(lonToX(-81), latToYUsed(-15));
    southAmerica.lineTo(lonToX(-79), latToYUsed(-20));
    southAmerica.lineTo(lonToX(-77), latToYUsed(-25));
    southAmerica.lineTo(lonToX(-75), latToYUsed(-30));
    southAmerica.lineTo(lonToX(-73), latToYUsed(-35));
    southAmerica.lineTo(lonToX(-71), latToYUsed(-40));
    southAmerica.lineTo(lonToX(-69), latToYUsed(-45));
    southAmerica.lineTo(lonToX(-67), latToYUsed(-50));
    southAmerica.lineTo(lonToX(-65), latToYUsed(-52));
    southAmerica.lineTo(lonToX(-63), latToYUsed(-54));
    southAmerica.lineTo(lonToX(-61), latToYUsed(-52));
    southAmerica.lineTo(lonToX(-59), latToYUsed(-50));
    southAmerica.lineTo(lonToX(-57), latToYUsed(-48));
    southAmerica.lineTo(lonToX(-55), latToYUsed(-46));
    southAmerica.lineTo(lonToX(-53), latToYUsed(-44));
    southAmerica.lineTo(lonToX(-51), latToYUsed(-42));
    southAmerica.lineTo(lonToX(-49), latToYUsed(-40));
    southAmerica.lineTo(lonToX(-47), latToYUsed(-38));
    southAmerica.lineTo(lonToX(-45), latToYUsed(-36));
    southAmerica.lineTo(lonToX(-43), latToYUsed(-34));
    southAmerica.lineTo(lonToX(-41), latToYUsed(-32));
    southAmerica.lineTo(lonToX(-39), latToYUsed(-30));
    southAmerica.lineTo(lonToX(-37), latToYUsed(-28));
    southAmerica.lineTo(lonToX(-35), latToYUsed(-26));
    southAmerica.lineTo(lonToX(-33), latToYUsed(-24));
    southAmerica.lineTo(lonToX(-31), latToYUsed(-22));
    southAmerica.lineTo(lonToX(-29), latToYUsed(-20));
    southAmerica.lineTo(lonToX(-27), latToYUsed(-18));
    southAmerica.lineTo(lonToX(-25), latToYUsed(-16));
    southAmerica.lineTo(lonToX(-23), latToYUsed(-14));
    southAmerica.lineTo(lonToX(-21), latToYUsed(-12));
    southAmerica.lineTo(lonToX(-19), latToYUsed(-10));
    southAmerica.lineTo(lonToX(-17), latToYUsed(-8));
    southAmerica.lineTo(lonToX(-15), latToYUsed(-6));
    southAmerica.lineTo(lonToX(-13), latToYUsed(-4));
    southAmerica.lineTo(lonToX(-11), latToYUsed(-2));
    southAmerica.lineTo(lonToX(-9), latToYUsed(0));
    southAmerica.lineTo(lonToX(-11), latToYUsed(2));
    southAmerica.lineTo(lonToX(-13), latToYUsed(4));
    southAmerica.lineTo(lonToX(-15), latToYUsed(6));
    southAmerica.lineTo(lonToX(-17), latToYUsed(8));
    southAmerica.lineTo(lonToX(-19), latToYUsed(10));
    southAmerica.lineTo(lonToX(-85), latToYUsed(8));
        
    QPainterPath eurasia;
    eurasia.moveTo(lonToX(-10), latToYUsed(45));
    eurasia.lineTo(lonToX(-5), latToYUsed(50));
    eurasia.lineTo(lonToX(0), latToYUsed(52));
    eurasia.lineTo(lonToX(5), latToYUsed(55));
    eurasia.lineTo(lonToX(10), latToYUsed(58));
    eurasia.lineTo(lonToX(15), latToYUsed(60));
    eurasia.lineTo(lonToX(20), latToYUsed(62));
    eurasia.lineTo(lonToX(25), latToYUsed(64));
    eurasia.lineTo(lonToX(30), latToYUsed(66));
    eurasia.lineTo(lonToX(40), latToYUsed(68));
    eurasia.lineTo(lonToX(50), latToYUsed(70));
    eurasia.lineTo(lonToX(60), latToYUsed(72));
    eurasia.lineTo(lonToX(70), latToYUsed(73));
    eurasia.lineTo(lonToX(80), latToYUsed(72));
    eurasia.lineTo(lonToX(90), latToYUsed(70));
    eurasia.lineTo(lonToX(100), latToYUsed(68));
    eurasia.lineTo(lonToX(110), latToYUsed(65));
    eurasia.lineTo(lonToX(120), latToYUsed(62));
    eurasia.lineTo(lonToX(130), latToYUsed(58));
    eurasia.lineTo(lonToX(140), latToYUsed(55));
    eurasia.lineTo(lonToX(150), latToYUsed(52));
    eurasia.lineTo(lonToX(145), latToYUsed(48));
    eurasia.lineTo(lonToX(140), latToYUsed(42));
    eurasia.lineTo(lonToX(135), latToYUsed(38));
    eurasia.lineTo(lonToX(130), latToYUsed(35));
    eurasia.lineTo(lonToX(125), latToYUsed(32));
    eurasia.lineTo(lonToX(120), latToYUsed(30));
    eurasia.lineTo(lonToX(115), latToYUsed(28));
    eurasia.lineTo(lonToX(110), latToYUsed(25));
    eurasia.lineTo(lonToX(105), latToYUsed(22));
    eurasia.lineTo(lonToX(100), latToYUsed(20));
    eurasia.lineTo(lonToX(95), latToYUsed(18));
    eurasia.lineTo(lonToX(90), latToYUsed(16));
    eurasia.lineTo(lonToX(85), latToYUsed(14));
    eurasia.lineTo(lonToX(80), latToYUsed(12));
    eurasia.lineTo(lonToX(75), latToYUsed(10));
    eurasia.lineTo(lonToX(70), latToYUsed(12));
    eurasia.lineTo(lonToX(65), latToYUsed(14));
    eurasia.lineTo(lonToX(60), latToYUsed(16));
    eurasia.lineTo(lonToX(55), latToYUsed(18));
    eurasia.lineTo(lonToX(50), latToYUsed(20));
    eurasia.lineTo(lonToX(45), latToYUsed(22));
    eurasia.lineTo(lonToX(40), latToYUsed(24));
    eurasia.lineTo(lonToX(35), latToYUsed(26));
    eurasia.lineTo(lonToX(30), latToYUsed(28));
    eurasia.lineTo(lonToX(25), latToYUsed(30));
    eurasia.lineTo(lonToX(20), latToYUsed(32));
    eurasia.lineTo(lonToX(15), latToYUsed(34));
    eurasia.lineTo(lonToX(10), latToYUsed(36));
    eurasia.lineTo(lonToX(5), latToYUsed(38));
    eurasia.lineTo(lonToX(0), latToYUsed(40));
    eurasia.lineTo(lonToX(-5), latToYUsed(42));
    eurasia.closeSubpath();
    
    QPainterPath africa;
    africa.moveTo(lonToX(-18), latToYUsed(32));
    africa.lineTo(lonToX(-10), latToYUsed(33));
    africa.lineTo(lonToX(-5), latToYUsed(34));
    africa.lineTo(lonToX(0), latToYUsed(35));
    africa.lineTo(lonToX(5), latToYUsed(34));
    africa.lineTo(lonToX(10), latToYUsed(33));
    africa.lineTo(lonToX(15), latToYUsed(32));
    africa.lineTo(lonToX(20), latToYUsed(31));
    africa.lineTo(lonToX(25), latToYUsed(30));
    africa.lineTo(lonToX(30), latToYUsed(29));
    africa.lineTo(lonToX(32), latToYUsed(26));
    africa.lineTo(lonToX(34), latToYUsed(22));
    africa.lineTo(lonToX(36), latToYUsed(18));
    africa.lineTo(lonToX(38), latToYUsed(14));
    africa.lineTo(lonToX(40), latToYUsed(10));
    africa.lineTo(lonToX(42), latToYUsed(6));
    africa.lineTo(lonToX(43), latToYUsed(2));
    africa.lineTo(lonToX(42), latToYUsed(0));
    africa.lineTo(lonToX(40), latToYUsed(-2));
    africa.lineTo(lonToX(38), latToYUsed(-4));
    africa.lineTo(lonToX(36), latToYUsed(-6));
    africa.lineTo(lonToX(34), latToYUsed(-8));
    africa.lineTo(lonToX(32), latToYUsed(-10));
    africa.lineTo(lonToX(30), latToYUsed(-12));
    africa.lineTo(lonToX(28), latToYUsed(-14));
    africa.lineTo(lonToX(26), latToYUsed(-16));
    africa.lineTo(lonToX(24), latToYUsed(-18));
    africa.lineTo(lonToX(22), latToYUsed(-20));
    africa.lineTo(lonToX(20), latToYUsed(-22));
    africa.lineTo(lonToX(18), latToYUsed(-24));
    africa.lineTo(lonToX(16), latToYUsed(-26));
    africa.lineTo(lonToX(14), latToYUsed(-28));
    africa.lineTo(lonToX(12), latToYUsed(-30));
    africa.lineTo(lonToX(10), latToYUsed(-32));
    africa.lineTo(lonToX(8), latToYUsed(-34));
    africa.lineTo(lonToX(6), latToYUsed(-32));
    africa.lineTo(lonToX(4), latToYUsed(-30));
    africa.lineTo(lonToX(2), latToYUsed(-28));
    africa.lineTo(lonToX(0), latToYUsed(-26));
    africa.lineTo(lonToX(-2), latToYUsed(-24));
    africa.lineTo(lonToX(-4), latToYUsed(-22));
    africa.lineTo(lonToX(-6), latToYUsed(-20));
    africa.lineTo(lonToX(-8), latToYUsed(-18));
    africa.lineTo(lonToX(-10), latToYUsed(-16));
    africa.lineTo(lonToX(-12), latToYUsed(-14));
    africa.lineTo(lonToX(-14), latToYUsed(-12));
    africa.lineTo(lonToX(-16), latToYUsed(-10));
    africa.lineTo(lonToX(-15), latToYUsed(-8));
    africa.lineTo(lonToX(-14), latToYUsed(-6));
    africa.lineTo(lonToX(-13), latToYUsed(-4));
    africa.lineTo(lonToX(-12), latToYUsed(-2));
    africa.lineTo(lonToX(-11), latToYUsed(0));
    africa.lineTo(lonToX(-10), latToYUsed(2));
    africa.lineTo(lonToX(-9), latToYUsed(4));
    africa.lineTo(lonToX(-8), latToYUsed(6));
    africa.lineTo(lonToX(-7), latToYUsed(8));
    africa.lineTo(lonToX(-6), latToYUsed(10));
    africa.lineTo(lonToX(-5), latToYUsed(12));
    africa.lineTo(lonToX(-6), latToYUsed(14));
    africa.lineTo(lonToX(-7), latToYUsed(16));
    africa.lineTo(lonToX(-8), latToYUsed(18));
    africa.lineTo(lonToX(-9), latToYUsed(20));
    africa.lineTo(lonToX(-10), latToYUsed(22));
    africa.lineTo(lonToX(-11), latToYUsed(24));
    africa.lineTo(lonToX(-12), latToYUsed(26));
    africa.lineTo(lonToX(-13), latToYUsed(28));
    africa.lineTo(lonToX(-14), latToYUsed(30));
    africa.lineTo(lonToX(-15), latToYUsed(32));
    africa.closeSubpath();

    QPainterPath australia;
    australia.moveTo(lonToX(115), latToYUsed(-12));
    australia.lineTo(lonToX(125), latToYUsed(-15));
    australia.lineTo(lonToX(135), latToYUsed(-18));
    australia.lineTo(lonToX(145), latToYUsed(-25));
    australia.lineTo(lonToX(150), latToYUsed(-32));
    australia.lineTo(lonToX(145), latToYUsed(-37));
    australia.lineTo(lonToX(140), latToYUsed(-35));
    australia.lineTo(lonToX(135), latToYUsed(-33));
    australia.lineTo(lonToX(130), latToYUsed(-30));
    australia.lineTo(lonToX(125), latToYUsed(-28));
    australia.lineTo(lonToX(120), latToYUsed(-26));
    australia.lineTo(lonToX(115), latToYUsed(-24));
    australia.lineTo(lonToX(110), latToYUsed(-22));
    australia.lineTo(lonToX(112), latToYUsed(-18));
    australia.lineTo(lonToX(114), latToYUsed(-15));
    australia.closeSubpath();

    QPainterPath antarctica;
    antarctica.moveTo(lonToX(-180), latToYUsed(-80));
    for (int lon = -180; lon <= 180; lon += 5) {
        double lat = -78 + 2 * sin(lon * 0.03) + 1.5 * cos(lon * 0.05);
        antarctica.lineTo(lonToX(lon), latToYUsed(lat));
    }
    antarctica.lineTo(lonToX(180), latToYUsed(-80));
    antarctica.lineTo(lonToX(-180), latToYUsed(-80));
    antarctica.closeSubpath();

    QPen continentPen(QColor(50, 50, 50), 1.2);
    
    painter.setPen(continentPen);
    painter.setBrush(QBrush(QColor(173, 216, 230, 180)));
    painter.drawPath(northAmerica);
    
    painter.setBrush(QBrush(QColor(240, 248, 255, 200)));
    painter.drawPath(greenland);
    
    painter.setBrush(QBrush(QColor(255, 165, 0, 180)));
    painter.drawPath(southAmerica);
    
    painter.setBrush(QBrush(QColor(144, 238, 144, 180)));
    painter.drawPath(eurasia);
    
    painter.setBrush(QBrush(QColor(255, 255, 100, 180)));
    painter.drawPath(africa);
    
    painter.setBrush(QBrush(QColor(255, 182, 193, 180)));
    painter.drawPath(australia);
    
    painter.setBrush(QBrush(QColor(176, 224, 230, 200)));
    painter.drawPath(antarctica);
    
    QFont font = painter.font();
    font.setPointSize(9);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QColor(30, 30, 30, 200));
    
    painter.drawText(lonToX(-100), latToYUsed(55), "Сев. Америка");
    painter.drawText(lonToX(-40), latToYUsed(75), "Гренландия");
    painter.drawText(lonToX(-50), latToYUsed(-25), "Юж. Америка");
    painter.drawText(lonToX(30), latToYUsed(55), "Евразия");
    painter.drawText(lonToX(15), latToYUsed(5), "Африка");
    painter.drawText(lonToX(125), latToYUsed(-25), "Австралия");
    painter.drawText(lonToX(0), latToYUsed(-80), "Антарктида"); 
    
    painter.setPen(QPen(QColor(100, 100, 200, 60), 0.5));
    
    painter.setPen(QPen(QColor(100, 100, 255, 100), 1.0, Qt::DashLine));
    painter.drawLine(0, latToYUsed(0), sceneWidth, latToYUsed(0));
    
    painter.setPen(QPen(QColor(255, 100, 100, 80), 0.5));
    painter.drawLine(0, latToYUsed(23.5), sceneWidth, latToYUsed(23.5));
    painter.drawLine(0, latToYUsed(-23.5), sceneWidth, latToYUsed(-23.5));
    
    painter.setPen(QPen(QColor(100, 200, 255, 80), 0.5));
    painter.drawLine(0, latToYUsed(66.5), sceneWidth, latToYUsed(66.5));
    painter.drawLine(0, latToYUsed(-66.5), sceneWidth, latToYUsed(-66.5));
    
    painter.end();
    
    QGraphicsPixmapItem* backgroundItem = new QGraphicsPixmapItem(backgroundPixmap);
    backgroundItem->setZValue(-1000);
    backgroundItem->setOpacity(0.85);
    scene->addItem(backgroundItem);
    mapBackground = backgroundItem;
}

std::string MainWindow::getNameById(const std::string& id) const 
{
    auto vertexNames = graph->get_all_vertex_names();
    for (const auto& name : vertexNames) {
        try {
            if (graph->get_vertex(name).id == id) {
                return name;
            }
        } catch (...) {
            continue;
        }
    }
    return "";
}

std::string MainWindow::getIdByName(const std::string& name) const 
{
    try {
        return graph->get_vertex(name).id;
    } catch (...) {
        return "";
    }
}

std::string MainWindow::getEdgeId(const std::string& fromName, const std::string& toName) const 
{
    try {
        std::string fromId = getIdByName(fromName);
        std::string toId = getIdByName(toName);
        return fromId + "_" + toId;
    } catch (...) {
        return "";
    }
}