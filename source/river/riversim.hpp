#pragma once

#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>

#include <deal.II/numerics/vector_tools.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/data_out.h>

#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>

#include <tetgen.h>
#include <gmsh.h>
#include "triangle.h"
#include "tethex.hpp"

using namespace dealii;
using namespace std;
using namespace gmsh;

namespace po = boost::program_options;
namespace mdl = gmsh::model;
namespace msh = gmsh::model::mesh;
namespace geo = gmsh::model::geo;

namespace Mesh
{
  typedef vector<double> mesharr;

  struct vecTriangulateIO
  {
    vector<double> points = {};
    int numOfAttrPerPoint = 0;
    vector<double> pointAttributes = {};
    vector<int> pointMarkers = {};
    vector<int> segments = {};
    vector<int> segmentMarkers = {};
    vector<int> triangles = {};
    int numOfAttrPerTriangle = 0;
    vector<double> triangleAttributes = {};
    vector<double> triangleAreas = {};
    vector<int> neighbors = {};
    //array of coordiantes
    vector<double> holes = {};
    //array of array of coordinates
    int numOfRegions = 1;
    vector<double> regions = {};
    //out only
    vector<double> edges = {};
    vector<double> edgeMarkers = {};
  };




  class Geometry
  {
    public:
      int index;
      struct
      {
          double x;
          double y;
      } coord;
      //physical attributes
      vector<double> phys_attributes;
      int marker;
      bool is_on_edge;
      vector<double> data;
  };


 class Tethex
 {
   private:
   public:
 };



  class Triangle
  {
    private:
      int dim = 2;
      string options;

      struct triangulateio in, out, vorout;
      //z - numbering starts from zero
      bool StartNumberingFromZero = false;
      //p - reads PSLG..
      //    1) will generate Constrained Delaunay.
      //    2) if are used -s(theoretical interest),
      //        -q(angles greter than 20, quality q23), -a maximum triangle area(a0.1) or
      //        -u.. will generate
      //            conforming constrained Delaunay
      //    3) if -D - conforming Delaunay triangulation(every triangle is Delaunay)
      //    4) if -r(refine) then segments of prev coarser mesh will be preserved
      bool ReadPSLG = true;
      //B - suppress boundary markers in output file
      bool SuppressBoundaryMarkers = false;
      //P - suppress output poly file
      bool SuppressPolyFile = false;
      //N - suppress node file
      bool SuppressNodeFile = false;
      //E - suppress ele file
      bool SuppressEleFile = false;
      //e - outputs list of edges
      bool OutputEdges = true;
      //n - outputs neighbours
      bool ComputeNeighbours = true;
      //I - suppress file mesh iteration number
      bool SuppressMehsFileNumbering = false;
      //O - suppress holes
      bool SuppressHoles = false;
      //X - suppress exact arithmetics
      bool SuppressExactArithmetics = false;
      //Y - prohibits stainer points on mesh boundary
      bool SteinerPointsOnBoundary = false;
      //YY -                  ------ on any segment
      bool SteinerPointsOnSegments = false;
      //S - specify max num of added steiner points
      int MaxNumOfSteinerPoints = -1;


      

      void SetAllValuesToDefault();
      void FreeAllocatedMemory();

      string updateOptions();

      void PrintOptions(bool qDetailedDescription = true);
      void PrintGeometry(struct triangulateio &io);
      
      void SetGeometry(struct triangulateio);
      void SetGeometry(struct vecTriangulateIO &geom);
      struct triangulateio* GetGeometry();
      struct vecTriangulateIO toVectorStructure(struct triangulateio*, bool b3D = true);
      struct triangulateio toTriaStructure(struct vecTriangulateIO&);
      struct triangulateio* GetVoronoi();

    public:
      enum algorithm
      {
          CONQUER,
          FORUNE,
          ITERATOR
      };
      //not used options -u(user defined constraint), -I, -J(garbage triangles), h - help

      /*
                  Triangle specific option list
              */

      //r - refine previously generated mesh, with preserving of segments
      bool Refine = false;
      //q - quality(minimu 20 degree).. also angle can be specified by q30
      bool ConstrainAngle = false;
      double MaxAngle = 20;
      //a - maximum triangle area constrain. a0.1
      double MaxTriaArea = -1.;
      bool AreaConstrain = false;
      //D - all triangles will be delaunay. Not just constrained delaunay.
      bool DelaunayTriangles = true;
      //c - enclose convex hull with segments
      bool EncloseConvexHull = false;
      //C - check final mesh if it was conf with X
      bool CheckFinalMesh = false;
      //A - asign additional regional attribute to each triangle, and specifies it to
      //    each closed segment region it belongs. (has effect with -p and without -r)
      bool AssignRegionalAttributes = true;
      //v - outputs voronoi diagram
      bool VoronoiDiagram = false;
      //o2 - generates second order mesh
      bool SecondOrderMesh = false;
      //i - use incremental algorithm instead of divide and conqure
      //F - fortune algorithm instead of delauna
      algorithm Algorithm = CONQUER;
      //Q - quite
      bool Quite = false;
      //V - verbose
      bool Verbose = true;

      Triangle();
      ~Triangle();
      struct vecTriangulateIO Generate(struct vecTriangulateIO &geom);

  };

  class TetGen
  {
    public:
    private:
  };

  class Gmsh
  {
    public:
      //GMSH
      Gmsh();
      ~Gmsh();
      void Open();
      void Write();
      void Clear();
      //OPTIONS
      //MODEL
      void add();
      void remove();
      void list();
      void setCurrent();
      void getEntities();
      void getPhysicalGroups();
      void getEntitiesForPhysicalGroup();
      void getPhysicalGroupsForEntity();
      void addPhysicalGroup();
      void setPhysicalName();
      void getPhysicalName();
      void getBoundary();
      void addDiscreteEntity();
      void getNormal();
      //... and lot of other..
      //MESH
      void generate();
      void partition();
      void refine();
      void setOrder();
      void getNodes();
      void setNodes(vector<double> nodes, int dim = 2, int tag = 1);//<- implement first
      void setElements(vector<int> elements, int elType = 2, int dim = 2, int tag = 1);
      void getElements();//<- implement first
      void getJacobians();
      //... and lot of other
      //FIELD
      //GEO
      //GEOMESH
      //OCC
      //VIEW <-propably I will need this
      //PLUGIN
      //GRAPHICS
      //FLTK <- important one
      void StartUserInterface();

    private:
      string modelName = "basic";
      string fileName = "rivermesh.msh";
      int dim = 1;

      vector<int> evaluateTags(int size, int tag0)
      {
        auto tags = vector<int>(size);
        iota(begin(tags), end(tags), tag0);
        return tags;
      }

  };

} //namespace mesh






class RiverSim
{
  public:
    RiverSim(po::variables_map &vm);
    ~RiverSim();
    void run();

  private:
    void geo_mesh_generator();
    void gmsh_mesh_generator();
    void mesh_covertor();
    void make_grid();
    void make_custom_grid();
    void setup_system();
    void assemble_system();
    void solve();
    void output_results() const;

    const static int dim = 2;

    Triangulation<dim> triangulation;
    FE_Q<dim> fe;
    DoFHandler<dim> dof_handler;
    SparsityPattern sparsity_pattern;
    SparseMatrix<double> system_matrix;
    Vector<double> solution;
    Vector<double> system_rhs;

    //options fro command line
    po::variables_map option_map;
};