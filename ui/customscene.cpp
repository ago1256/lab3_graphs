#include "customscene.h"

CustomScene::CustomScene(QObject *parent)
    : QGraphicsScene(parent)
{
}

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit sceneClicked(event->scenePos());
    } else if (event->button() == Qt::RightButton) {
        emit sceneRightClicked(event->scenePos());
    }
    
    QGraphicsScene::mousePressEvent(event);
}