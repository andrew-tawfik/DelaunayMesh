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
        Point(22, 8.6), Point(18.5, 14.7), Point(11.5, 14.7), Point(8, 8.6),
        Point(11.5, 2.5), Point(18.5, 2.5), Point(14, 8.6),
    };

    std::vector<Point> testCaseRect =
    {
        Point(7.5, 2.0), Point(10.0, 2.0), Point(12.5, 2.0), Point(15.0, 2.0),
        Point(17.5, 2.0), Point(20.0, 2.0), Point(22.5, 2.0), Point(22.5, 4.0),
        Point(22.5, 5.0), Point(22.5, 6.0), Point(22.5, 7.0), Point(22.5, 8.5),
        Point(22.5, 10.0), Point(22.5, 12.0), Point(20.0, 12.0), Point(17.5, 12.0),
        Point(15.0, 12.0), Point(12.5, 12.0), Point(10.0, 12.0), Point(7.5, 12.0),
        Point(7.5, 10.0), Point(7.5, 8.5), Point(7.5, 7.0), Point(7.5, 5.0),
        Point(7.5, 4.0), Point(7.5, 2.0)
    };

    std::vector<Point> testCaseInner =
    {
        Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0), Point(22.5, 7.0), Point(22.5, 12.0),
        Point(15.0, 12.0), Point(7.5, 12.0), Point(7.5, 7.0), Point(15, 7), Point(11.25, 4.5),
        Point(18.75, 9.5), Point(18.75, 4.5), Point(11.25, 9.5), Point(11.25, 7),
        Point(18.75, 7),
        Point(13.125, 10.75)
    };

    std::vector<Point> testCasePicture =
    {
        Point(7.5, 2.0), Point(15.0, 2.0), Point(22.5, 2.0), Point(22.5, 7.0),
        Point(22.5, 12.0),Point(15.0, 12.0), Point(7.5, 12.0),Point(7.5, 7.0),

        Point(10.0, 3.0), Point(20.0, 3.0), Point(10.0, 5.0), Point(20.0, 5.0), Point(10.0, 7.0),
        Point(20.0, 7.0), Point(10.0, 9.0), Point(20.0, 9.0), Point(10.0, 11.0), Point(20.0, 11.0),
        Point(12.5, 4.0), Point(17.5, 4.0), Point(12.5, 6.0), Point(17.5, 6.0), Point(12.5, 8.0),
        Point(17.5, 8.0), Point(12.5, 10.0), Point(17.5, 10.0), Point(15.0, 8.5), Point(15.0, 5.5),
        Point(13.0, 8.0), Point(18.0, 8.0), Point(13.0, 5.0), Point(18.0, 5.0), Point(11.0, 10.0),
        Point(19.0, 10.0), Point(11.0, 4.0), Point(19.0, 4.0), Point(16.5, 7.0), Point(16.5, 9.0),
        Point(13.5, 7.0), Point(13.5, 9.0), Point(14.0, 4.5), Point(16.0, 4.5), Point(14.0, 10.5),
        Point(16.0, 10.5), Point(12.0, 6.5), Point(18.0, 6.5), Point(12.0, 9.5), Point(18.0, 9.5),
        Point(14.0, 6.5), Point(16.0, 6.5), Point(14.0, 9.5), Point(16.0, 9.5)
    };


    Mesh k(testCasePicture);
    k.setTriVector({ k.superTriangle() });
    k.buildMesh();
    //k.removeHelperTriangles();

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

    Triangle t = Triangle(Point(7.5, 2), Point(15, 2), Point(15, 12));
    Point ptT = Point(7.5, 7);
    std::cout << t.isInCircumcircle(ptT) << std::endl;

    for (int i = 0; i < meshTriangles.size(); ++i)
    {
        const auto& triangle = meshTriangles[i];
        QPointF p1(triangle.getPoint(0).getX(), triangle.getPoint(0).getY());
        QPointF p2(triangle.getPoint(1).getX(), triangle.getPoint(1).getY());
        QPointF p3(triangle.getPoint(2).getX(), triangle.getPoint(2).getY());
        QTriangle* qTriangle = new QTriangle(p1, p2, p3, i);


        // Set whether to show the triangle index on display
        qTriangle->setShowIndex(false); // Change to false to hide the index

        scene.addItem(qTriangle);
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
