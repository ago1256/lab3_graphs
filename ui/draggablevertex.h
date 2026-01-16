#pragma once

#include <QGraphicsEllipseItem>
#include <QObject>
#include <QGraphicsSceneMouseEvent>

class DraggableVertex : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
    
public:
    DraggableVertex(const QString& name, QGraphicsItem *parent = nullptr);
    
    QString getName() const { return vertexName; }
    void setName(const QString& name) { vertexName = name; }
    
signals:
    void vertexMoved(const QString& name, const QPointF& newPos);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    
private:
    QString vertexName;
    QPointF dragStartPos;
    bool isDragging;
};