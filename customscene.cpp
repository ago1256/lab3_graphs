#include "customscene.h"

CustomScene::CustomScene(QObject *parent)
    : QGraphicsScene(parent)
{
}

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // Передаем сигнал о клике
    emit sceneClicked(event->scenePos());
    
    // Вызываем базовую реализацию
    QGraphicsScene::mousePressEvent(event);
}