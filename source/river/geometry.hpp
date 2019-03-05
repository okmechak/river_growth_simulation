#pragma once

#include <utility>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <math.h>

#include "common.hpp"
#include "tethex.hpp"

using namespace std;


namespace River
{


class GeomTag
{
  public:
    GeomTag(int idVal = 0, int rVal = 0):
      branchId{idVal},
      regionTag{rVal}
    {};
    int branchId = 0;
    int regionTag = 0;
};

class GeomPolar : public Polar
{
  public:
    GeomPolar() = default;
    GeomPolar(double r, double phiVal, 
      int branchIdVal = 0, 
      int regionTagVal = 0,
      double meshSizeVal = 1.);

    int branchId = 0, regionTag = 0;
    double meshSize = 1.;
};

class GeomLine
{
  public:
    GeomLine(unsigned int p1Val, unsigned int p2Val, 
      int branchIdVal = 0, int regionTagVal = 0);
    unsigned int p1, p2;
    int branchId = 0, 
       regionTag = 0;
    
};

/*
  Point struct and feew functions to work with it
*/
class GeomPoint
{
  private:
    double eps = 1e-20;

  public:
    double x, y;
    int branchId = 0, regionTag = 0;
    double meshSize = 1.;

    ~GeomPoint() = default;
    GeomPoint(double xval, double yval, 
        int branchIdVal = 0, int regionTagVal = 0, double msize = 1.);
    GeomPoint(const GeomPolar &p);

    double norm() const;
    GeomPoint getNormalized();
    GeomPoint& rotate(double phi);
    GeomPolar getPolar() const;
    GeomPoint& normalize();
    double angle() const;
    double angle(GeomPoint &p) const;
    GeomPoint& print();

    GeomPoint& operator=(const GeomPoint& p) = default;
    GeomPoint operator+(const GeomPoint& p) const;
    GeomPoint& operator+=(const GeomPoint& p);
    GeomPoint operator-(const GeomPoint& p) const;
    GeomPoint& operator-=(const GeomPoint& p);
    double operator*(const GeomPoint& p) const;
    GeomPoint operator*(const double gain) const;
    GeomPoint& operator*=(const double gain);
    GeomPoint operator/(const double gain) const;
    GeomPoint& operator/=(const double gain);
    bool operator==(const GeomPoint& p) const;
    double operator[](const int index) const;

    friend ostream& operator<<(ostream& write, const GeomPoint & p);
};



class Branch
{
public:
  unsigned long int id;
  string name = "branch";
  vector<GeomPoint> leftPoints;
  vector<GeomPoint> rightPoints;

  Branch(unsigned long int id, GeomPoint sourcePoint, double phi, double epsVal = 1e-10);
  ~Branch() = default;

  //modify branch
  Branch& addPoint(GeomPoint p);
  Branch& addDPoint(GeomPoint p);
  Branch& addPolar(GeomPolar p, bool bRelativeAngle = true);
  Branch& removeHeadPoint();
  Branch& shrink(double dl);
  double getWidth();
  Branch& setWidth(double epsVal);

  //geom entities
  GeomPoint getHead();
  double getHeadAngle();
  double getTailAngle();
  GeomPoint getTail();

  //statistics
  bool empty();
  double length();
  unsigned int size();
  double averageSpeed();
  //lot of others to be implemented

  //debug
  Branch& print();

private:
  pair<GeomPoint, GeomPoint> splitPoint(GeomPoint p, double phi);
  GeomPoint mergePoints(GeomPoint p1, GeomPoint p2);
  double eps = 3e-2;
  double tailAngle;
};





/*
  Geometry class

  this is simiple geometry class used to easilly do 
  different operations with geometry which describes river
  mesh. This class strongly dependce of Branch class
*/

class Geometry
{
public:
  //Geometry region indicators/markers
  enum Markers
  {
    None = 0,
    Bottom,
    Right,
    Top,
    Left,
    River
  };



  //Segments or Edges or Elements

  Geometry() = default;
  ~Geometry() = default;

  Branch& initiateRootBranch(unsigned int id = 1);
  void addPoints(vector<GeomPoint> points);
  void addDPoints(vector<GeomPoint> dpoints);
  void addPolar(GeomPolar p, bool bRelativeAngle = true);
  void SetSquareBoundary(
      GeomPoint BottomBoxCorner,
      GeomPoint TopBoxCorner,
      double dx);

  //TODO implement constraint on eps and dl values!!!
  //important
  pair<unsigned int, unsigned int> AddBiffurcation(unsigned int id, double dl);

  vector<unsigned int> GetTipIds();
  vector<pair<GeomPoint, double>> GetTipPoints();
  vector<GeomPolar> GetTipPolars();
  void InitiateMesh(tethex::Mesh & meshio);
  void SetEps(double epsVal);
  void clear();


private:
  double eps = 3e-2;
  double bifAngle = M_PI/5.;

  //mesh size of different regions
  double tipMeshSize = 0.0001;
  double riverMeshSize = 0.005;
  double boundariesMeshSize = 0.1;

  unsigned int rootBranchId = 0;//0 means no root/first Branch
  
  //Nodes and lines - main interface of this class
  vector<GeomPoint> points;
  vector<GeomLine> lines;

  //branches functionality
  map<unsigned int, pair<unsigned int, unsigned int>> branchRelation;
  //all branches structure is strored in *branches*. To get position of some branch in *branches* structure use branchIndexes
  map<unsigned int, unsigned int> branchIndexes;
  vector<Branch> branches;

  inline Branch* get_branch(int id);
  inline bool q_branch(int id);
  inline bool q_branch_childs(int id);
  inline pair<int, int> get_branch_childs(int id);
  void add_branch(Branch& branch, int id);
  void add_branch_relation(int baseId, int leftId, int rightId);

  //Boundary
  //Box parameters
  vector<GeomPoint> boundaryPoints;
  vector<GeomLine> boundaryLines;
  double dx = 0.5;


  //river geometry parameters
  double alpha = M_PI/3.; // biffurcation angle
  double len = 0.1; // biffurcation initial length 
  
  pair<GeomPoint, double> GetEndPointOfSquareBoundary();
  void InserBranchTree(unsigned int id, double phi, bool isRoot = false);
  void generate_ids_of_tip_branches(vector<int> &IdsOfBranchesAtTip, int branchId);
  void generateCircularBoundary();
  //at generation time of whole circular boundary
  //it gives us a points of crossection of boundaries
  GeomPoint mergedLeft(double phi);
  GeomPoint mergedRight(double phi);
  GeomPoint mergedCenter(double phi);
  unsigned int generateID(unsigned int prevID, bool isRight = false);
};

} // namespace River