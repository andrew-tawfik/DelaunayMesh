#include "qtriangle.h"
#include <QPainter>


QTriangle::QTriangle(const QPointF& p0, const QPointF& p1, const QPointF& p2, QGraphicsItem *parent)
    : QGraphicsItem(parent), p0(p0), p1(p1), p2(p2){}

void QTriangle::paint(QPainter *painter,  const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPolygonF polygon;
    polygon << p0 << p1 << p2;

    painter->setBrush(Qt::white);
    QPen pen(Qt::black);
    pen.setWidth(0);
    painter->setPen(pen);
    painter->drawPolygon(polygon);
}

QRectF QTriangle::boundingRect() const
{
    qreal minX = std::min({p0.x(), p1.x(), p2.x()});
    qreal minY = std::min({p0.y(), p1.y(), p2.y()});
    qreal maxX = std::max({p0.x(), p1.x(), p2.x()});
    qreal maxY = std::max({p0.y(), p1.y(), p2.y()});

    return QRectF(minX, minY, maxX - minX, maxY - minY);
}
