#pragma once

#include <QGraphicsScene>
#include <QObject>
#include <QGraphicsSceneMouseEvent>

class CustomScene : public QGraphicsScene
{
    Q_OBJECT
    
public:
    explicit CustomScene(QObject *parent = nullptr);
    
signals:
    void sceneClicked(const QPointF& pos);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};