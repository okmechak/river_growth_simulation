#include "geometry.hpp"

namespace River{

/*
    Branch object

*/

Branch::Branch(unsigned long int id, Point sourcePoint, double phi):id(id)
{
    auto points = splitPoint(sourcePoint, phi);

    leftPoints.push_back(points.first);
    rightPoints.push_back(points.second);
}

Branch::~Branch(){}

pair<Point, Point> Branch::splitPoint(Point p, double phi)
{
    Point pLeft, pRight;
    
    pLeft = {p.x + sin(phi) * eps/2 , p.y + cos(phi) * eps/2, p.index};
    pRight = {p.x + sin(phi) * eps/2 , p.y - cos(phi) * eps/2, p.index};

    return pair<Point, Point>{pLeft, pRight};
}

void Branch::addPoint(Point p)
{
    //FIXME: wrong functionality!
    // shift should be perpendicular to branch
    //use getDirection!
    //and what if we add first point! we should use some 
    //predefined direction information
    //leftPoints.push_back(coords[0] - eps/2);
    //leftPoints.push_back(coords[1] - eps/2);
    //rightPoints.push_back(coords[0] + eps/2);
    //rightPoints.push_back(coords[1] + eps/2);
}

void Branch::addPoint(double dl, double phi)
{ 
    auto tipVector = getDirection();
    auto newPoint = Point{ 
        dl * cos(phi) + tipVector.x, 
        dl * sin(phi) + tipVector.y};
    
    addPoint(newPoint);
}

void Branch::removeHeadPoint()
{
    leftPoints.pop_back();
    rightPoints.pop_back();
}

double Branch::width()
{
    return eps;
}

void Branch::setWidth(double eps)
{
    eps/*of object*/ = eps/*passed argument*/;
}

Point Branch::getHead()
{
    auto lastIndex = leftPoints.size() - 1;
    return Point{
        (leftPoints[lastIndex].x + rightPoints[lastIndex].x) / 2,
        (leftPoints[lastIndex].y + rightPoints[lastIndex].y) / 2, 
        leftPoints[lastIndex].index};
}

Point Branch::getDirection()
{
    auto index = leftPoints.size() - 1;
    auto dx = leftPoints[index].x - leftPoints[index - 1].x;
    auto dy = leftPoints[index].y - leftPoints[index - 1].y;
    auto dn = sqrt(dx*dx + dy*dy);
    return Point{dx/dn, dy/dn, 0};
}

Point Branch::getTail()
{
    return Point{
        (leftPoints[0].x + rightPoints[0].x) / 2,
        (leftPoints[0].y + rightPoints[0].y) / 2, 
         leftPoints[0].index};
}

bool Branch::empty()
{
    return leftPoints.empty();
}

double Branch::length()
{   
    double len = 0;
    //TODO: Test it
    for(unsigned int i = 1; i < leftPoints.size(); ++i)
    {
        auto dx = leftPoints[i].x - leftPoints[i - 1].x;
        auto dy = leftPoints[i].y - leftPoints[i - 1].y;
        len += sqrt(dx*dx + dy*dy); 
    }

    return len;
}

unsigned int Branch::size()
{
    return leftPoints.size();
}

double Branch::averageSpeed()
{
    return length()/size();
}










/*
    Geometry object

*/


Geometry::Geometry(){}

void Geometry::SetSquareBoundary(
    Point BottomBoxCorner, 
    Point TopBoxCorner, 
    double dx)
{
    boundaryPoints = {
        {BottomBoxCorner.x + (dx + eps / 2), BottomBoxCorner.y},   //node 2
        {TopBoxCorner.x,                 BottomBoxCorner.y},       //node 3
        {TopBoxCorner.x,                 TopBoxCorner.y},          //node 4
        {BottomBoxCorner.x,              TopBoxCorner.y},          //node 5
        {BottomBoxCorner.x,              BottomBoxCorner.y},       //node 6
        {BottomBoxCorner.x + (dx - eps / 2), BottomBoxCorner.y}    //node 7
    };

    boundaryLines = {
        {1, 2, Markers::Bottom},   
        {2, 3, Markers::Right},   
        {3, 4, Markers::Top},   
        {4, 5, Markers::Left}, 
        {5, 6, Markers::Bottom}
        };
}

void Geometry::addPoint(double x, double y, unsigned int marker)
{
    
}

void Geometry::addDPoint(double dx, double dy, unsigned int marker)
{
    
}

void Geometry::generateCircularBoundary()
{
    points.insert(points.end(), boundaryPoints.begin(), boundaryPoints.end());
    if(branchRelation.count(0)){
        //TODO some functionality
    }
}

void Geometry::SetEps(double eps){
    eps/*object parameter*/ = eps/*argument*/;
}


Geometry::~Geometry(){}

}