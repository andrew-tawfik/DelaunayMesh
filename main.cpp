#include "point.h"
#include "triangle.h"
#include "mesh.h"
#include <iostream>
#include <vector>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "qtriangle.h"

void printTriangleDetails(const Triangle& tri, int triIndex)
{
    std::cout << "Triangle " << triIndex << ": ";
    tri.printPoints();
    std::cout << "Perimeter: " << tri.getPerimeter() << std::endl;
    std::cout << "Area: " << tri.getArea() << std::endl;
    std::cout << "Angles: ";
    for (int i = 0; i < 3; ++i)
    {
        std::cout << tri.getAng(i) << " ";
    }

    std::cout << std::endl;
    std::cout << "Circumcenter: (" << tri.getCircumcenter().getX() << ", " << tri.getCircumcenter().getY() << ")\n" << std::endl;
}




int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create the points and mesh
    std::vector<Point> testCaseHex =
        {
            Point(22, 8.6),
            Point(18.5, 14.7),
            Point(11.5, 14.7),
            Point(8, 8.6),
            Point(11.5, 2.5),
            Point(18.5, 2.5),
        };

    std::vector<Point> testCaseRect =
        {

            Point(7.5, 2.0),
            Point(15.0, 2.0),
            Point(22.5, 2.0),
            Point(22.5, 7.0),
            Point(22.5, 12.0),
            Point(15.0, 12.0),
            Point(7.5, 12.0),
            Point(7.5, 7.0)
        };

    std::vector<Point> testCaseInner =
        {

            Point(7.5, 2.0),
            Point(15.0, 2.0),
            Point(22.5, 2.0),
            Point(22.5, 7.0),
            Point(22.5, 12.0),
            Point(15.0, 12.0),
            Point(7.5, 12.0),
            Point(7.5, 7.0),
            //Point(15, 7),
            //Point(11.25, 4.5),
            //Point(18.75, 9.5),
            //Point(11.25, 9.5),
            //Point(18.75, 4.5)
        };



    Mesh k(testCaseRect);
    k.setTriVector({ k.superTriangle() });
    k.buildMesh();

    std::vector<Triangle> meshTriangles = k.getTriVector();

    for (int i = 0; i < meshTriangles.size(); ++i)
    {
        std::cout << "Triangle " << i << ": ";
        meshTriangles[i].printPoints();
        std::cout << "\ta) Neighbour at index:  " << meshTriangles[i].getNeighbourIndex(0) << std::endl;
        std::cout << "\tb) Neighbour at index:  " << meshTriangles[i].getNeighbourIndex(1) << std::endl;
        std::cout << "\tc) Neighbour at index:  " << meshTriangles[i].getNeighbourIndex(2) << std::endl;

    }


    QGraphicsScene scene;

    scene.setSceneRect(-7.5, -10, 45, 45); // Adjust size as needed


    for (const auto& triangle : meshTriangles)
    {
        QPointF p1(triangle.getPoint(0).getX(), triangle.getPoint(0).getY());
        QPointF p2(triangle.getPoint(1).getX(), triangle.getPoint(1).getY());
        QPointF p3(triangle.getPoint(2).getX(), triangle.getPoint(2).getY());
        scene.addItem(new QTriangle(p1, p2, p3));
    }


    // Create the graphics view
    QGraphicsView view(&scene);
    view.setTransform(QTransform().scale(1, -1)); // Flip vertically
    view.setRenderHint(QPainter::Antialiasing);
    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Triangulation Visualization"));

    view.scale(25, 25);
    view.show();


    return app.exec();

}
