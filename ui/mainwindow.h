#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QGraphicsView>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QGroupBox>
#include <QTextEdit>
#include <QMenu>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QSplitter>
#include "QGraphicsPixmapItem"
#include <QScrollArea>
#include <unordered_map>
#include "../graph/graph.h"
#include "../algorithms/algorithms.h"
#include "customscene.h"
#include "draggablevertex.h"

class MainWindow : public QMainWindow 
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddCity();
    void onAddFlight();
    void onFindPath();
    void onClearGraph();
 
    void handleSceneClick(const QPointF& pos);
    void handleSceneRightClick(const QPointF& pos);
    
    void deleteSelected();
    void editSelectedFlight();
    void editSelectedCity();
    void onColorSCC();
    void onResetColors();

private:
    void setupUI();
    void updateComboBoxes();
    void drawGraph();
    void loadDefaultGraph();  

    void editCityPosition(const std::string& cityName);
    void updateEdgesForVertex(const std::string& vertexName);

    void selectCity(const std::string& cityName);
    void selectFlight(const std::string& from, const std::string& to);
    void clearSelection();
    void updateSelectionInfo();
    void updateFlightInfo(const std::string& from, const std::string& to);
    
    std::string findCityAtPos(const QPointF& pos);  
    std::pair<std::string, std::string> findFlightAtPos(const QPointF& pos);  
    
    void drawFlightLine(const Edge& edge);
    void loadMapToScene();
    
    std::string getNameById(const std::string& id) const;
    std::string getIdByName(const std::string& name) const;
    std::string getEdgeId(const std::string& fromName, const std::string& toName) const;

    QGraphicsView *graphicsView;
    CustomScene *scene;
    QSplitter *mainSplitter;
    QGraphicsItem *mapBackground;
    
    QLabel *pathResultLabel;
    QGroupBox *selectionInfoGroup;
    QLabel *selectionTypeLabel;
    QLabel *selectionNameLabel;
    QLabel *flightParamsLabel;
    
    QScrollArea *scrollArea;
    QWidget *scrollContent;
    QVBoxLayout *scrollLayout;
    QTextEdit *sccDetailsText;

    QComboBox *cityCombo;
    QComboBox *fromCombo;
    QComboBox *toCombo;
    QSpinBox *costSpin;
    QSpinBox *timeSpin;
    QSpinBox *visaSpin;
    QCheckBox *checkCost;
    QCheckBox *checkTime;
    QCheckBox *checkVisa;
    QPushButton *addCityBtn;
    QPushButton *addFlightBtn;
    QPushButton *findPathBtn;
    QPushButton *clearBtn;
    QPushButton *colorSCCBtn;
    QPushButton *resetColorsBtn;
    
    QMenu *contextMenu;

    std::unordered_map<std::string, DraggableVertex*> cityCircles;
    std::unordered_map<std::string, QPointF> cityPositions;
    std::unordered_map<std::string, QGraphicsLineItem*> flightLines;
    std::unordered_map<std::string, QGraphicsPolygonItem*> flightArrows;

    QDoubleSpinBox *weightCostSpin;
    QDoubleSpinBox *weightTimeSpin;
    QDoubleSpinBox *weightVisaSpin;
    QGroupBox *weightsGroup;

    std::string selectedCity;  
    std::pair<std::string, std::string> selectedFlight; 
    bool isCitySelected;

    Graph *graph;
};