#include "qtriangle.h"
#include <QPainter>
#include <QString>


QTriangle::QTriangle(const QPointF& p0, const QPointF& p1, const QPointF& p2, int iIndex, QGraphicsItem *parent)
    : QGraphicsItem(parent), p0(p0), p1(p1), p2(p2), iIndex(iIndex){}

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

    if (bShowIndex)
    {
        // Draw the index at the centroid of the triangle
        QPointF centroid((p0.x() + p1.x() + p2.x()) / 3, (p0.y() + p1.y() + p2.y()) / 3);

        // Save the painter state
        painter->save();

        // Flip the painter
        painter->translate(centroid);
        painter->scale(1, -1);
        painter->translate(-centroid);

        // Set the font size
        QFont font = painter->font();
        font.setPointSize(1); // Adjust the font size as needed
        painter->setFont(font);

        // Draw the text
        painter->drawText(centroid, QString::number(iIndex));

        // Restore the painter state
        painter->restore();
    }
}

QRectF QTriangle::boundingRect() const
{
    qreal minX = std::min({p0.x(), p1.x(), p2.x()});
    qreal minY = std::min({p0.y(), p1.y(), p2.y()});
    qreal maxX = std::max({p0.x(), p1.x(), p2.x()});
    qreal maxY = std::max({p0.y(), p1.y(), p2.y()});

    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

void QTriangle::setShowIndex(bool bShow)
{
    this->bShowIndex = bShow;
    update(); // Request a repaint to apply the change
}
