#ifndef QTRIANGLE_H
#define QTRIANGLE_H

#include <QGraphicsItem>
#include <QPointF>

class QTriangle : public QGraphicsItem
{
public:
    QTriangle(const QPointF& p0, const QPointF& p1, const QPointF& p2, QGraphicsItem *parent = nullptr);

protected:
    void paint(QPainter *painter,  const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;

private:
    QPointF p0, p1, p2;

};

#endif // QTRIANGLE_H
