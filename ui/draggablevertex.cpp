#include "draggablevertex.h"
#include <QBrush>
#include <QPen>
#include <QDebug>

DraggableVertex::DraggableVertex(const QString& name, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent), vertexName(name), isDragging(false)
{
    setRect(-10, -10, 20, 20);
    setBrush(QBrush(QColor(100, 150, 255)));
    setPen(QPen(Qt::darkBlue, 1));
    
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
    setAcceptHoverEvents(true);
    setZValue(10);
}

void DraggableVertex::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    QGraphicsEllipseItem::mousePressEvent(event);
}

void DraggableVertex::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    QGraphicsEllipseItem::mouseMoveEvent(event);
}

void DraggableVertex::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}