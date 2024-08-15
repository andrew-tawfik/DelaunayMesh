#ifndef QTRIANGLE_H
#define QTRIANGLE_H

#include <QGraphicsItem>
#include <QPointF>

// QTriangle class inherits from QGraphicsItem to represent a triangle in a QGraphicsScene.
class QTriangle : public QGraphicsItem
{
public:
    // Constructor to initialize the triangle with three points and an optional parent item.
    QTriangle(const QPointF& p0, const QPointF& p1, const QPointF& p2, int iIndex, QGraphicsItem *parent = nullptr);

    // Method to toggle the display of the triangle's index.
    void setShowIndex(bool show);

protected:
    // Method to paint the triangle, called by the QGraphicsView.
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // Method to define the bounding rectangle of the triangle.
    QRectF boundingRect() const override;

private:
    // Triangle's vertices.
    QPointF p0, p1, p2;

    // Index to identify the triangle.
    int iIndex;

    // Flag to determine whether to show the index or not.
    bool bShowIndex;
};

#endif // QTRIANGLE_H
