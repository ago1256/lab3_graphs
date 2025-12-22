#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include "graph.h"
#include "customscene.h"  // ДОБАВЛЕНО

class MainWindow : public QMainWindow 
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddCity();
    void onAddFlight();
    void onDeleteSelected();
    void onRenameSelected();
    void onFindPath();
    void onClearGraph();
    
    // Слот для обработки кликов на сцене
    void handleSceneClick(const QPointF& pos);

private:
    void setupUI();
    void updateComboBoxes();
    void drawGraph();
    void loadDefaultGraph();
    
    // Функции выделения
    void selectCity(const std::string& cityName);
    void selectFlight(const std::string& from, const std::string& to);
    void clearSelection();
    void updateSelectionDisplay();
    
    // Проверка клика
    std::string findCityAtPos(const QPointF& pos);
    std::pair<std::string, std::string> findFlightAtPos(const QPointF& pos);
    
    // UI элементы
    QGraphicsView *graphicsView;
    CustomScene *scene;  // Изменено на CustomScene
    
    // Панель информации
    class QLabel *selectedLabel;
    class QLabel *infoLabel;
    class QLabel *statsLabel;
    
    // Элементы управления
    class QComboBox *cityCombo;
    class QComboBox *fromCombo;
    class QComboBox *toCombo;
    class QSpinBox *costSpin;
    class QSpinBox *timeSpin;
    class QSpinBox *visaSpin;
    class QCheckBox *checkCost;
    class QCheckBox *checkTime;
    class QCheckBox *checkVisa;
    class QPushButton *addCityBtn;
    class QPushButton *addFlightBtn;
    class QPushButton *deleteBtn;
    class QPushButton *renameBtn;
    class QPushButton *findPathBtn;
    class QPushButton *clearBtn;
    
    // Графические элементы (для быстрого доступа)
    std::unordered_map<std::string, QGraphicsEllipseItem*> cityCircles;
    std::unordered_map<std::string, QPointF> cityPositions;
    std::unordered_map<std::string, QGraphicsLineItem*> flightLines;
    
    // Текущее выделение
    std::string selectedCity;
    std::pair<std::string, std::string> selectedFlight;
    bool isCitySelected;
    
    // Граф
    Graph *graph;
};