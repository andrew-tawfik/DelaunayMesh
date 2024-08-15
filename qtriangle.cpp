#include "qtriangle.h"
#include <QPainter>
#include <QString>

// Constructor for QTriangle, initializing with three points and an index
QTriangle::QTriangle(const QPointF& p0, const QPointF& p1, const QPointF& p2, int iIndex, QGraphicsItem *parent)
    : QGraphicsItem(parent), p0(p0), p1(p1), p2(p2), iIndex(iIndex) {}

// Paints the triangle on the QGraphicsItem
void QTriangle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option); // Suppress unused variable warnings
    Q_UNUSED(widget); // Suppress unused variable warnings

    // Create a QPolygonF from the triangle's three points
    QPolygonF polygon;
    polygon << p0 << p1 << p2;

    // Set the brush and pen for drawing the triangle
    painter->setBrush(Qt::white); // Fill color
    QPen pen(Qt::black); // Outline color
    pen.setWidth(0); // Set pen width
    painter->setPen(pen);

    // Draw the triangle
    painter->drawPolygon(polygon);

    // Draw the index if bShowIndex is true
    if (bShowIndex)
    {
        // Calculate the centroid of the triangle
        QPointF centroid((p0.x() + p1.x() + p2.x()) / 3, (p0.y() + p1.y() + p2.y()) / 3);

        // Save the current painter state
        painter->save();

        // Flip the painter to correctly display text (because of coordinate system inversion)
        painter->translate(centroid);
        painter->scale(1, -1);
        painter->translate(-centroid);

        // Set font size for the index text
        QFont font = painter->font();
        font.setPointSize(1); // Adjust the font size as needed
        painter->setFont(font);

        // Draw the index at the centroid of the triangle
        painter->drawText(centroid, QString::number(iIndex));

        // Restore the painter to its original state
        painter->restore();
    }
}

// Returns the bounding rectangle of the triangle
QRectF QTriangle::boundingRect() const
{
    // Compute the bounding rectangle of the triangle
    qreal minX = std::min({p0.x(), p1.x(), p2.x()});
    qreal minY = std::min({p0.y(), p1.y(), p2.y()});
    qreal maxX = std::max({p0.x(), p1.x(), p2.x()});
    qreal maxY = std::max({p0.y(), p1.y(), p2.y()});

    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

// Sets whether to show the index on the triangle and triggers a repaint
void QTriangle::setShowIndex(bool bShow)
{
    this->bShowIndex = bShow;
    update(); // Request a repaint to apply the change
}
