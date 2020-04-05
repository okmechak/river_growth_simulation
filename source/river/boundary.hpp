/*
    riversim - river growth simulation.
    Copyright (c) 2019 Oleg Kmechak
    Report issues: github.com/okmechak/RiverSim/issues

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
*/

/*! \file boundary.hpp
    \brief Generate initial predefinide boundary geometry and its source positions.
    
    \details First step at beginning of solving this problem is defining a region. It should be quite flexible, cos
    for testing purposes we want to use simple rectangular region, but for real models we need almost any shape
    with any source position on this boundary.
    For additional information see \ref River::Boundary.
*/

#pragma once

///\cond
#include <vector>
#include <string>
#include <map>
///\endcond

#include "GeometryPrimitives.hpp"

using namespace std;

/*! \namespace River
    \brief River namespace holds everything that is related to River simulation.
*/
namespace River
{   
    enum t_boundary 
    {
        DIRICHLET = 0, 
        NEUMAN
    }; 

    /*! \struct BoundaryCondition
        \brief
        Describes boudary condition type.
    */
    struct BoundaryCondition
    {
        t_boundary type = DIRICHLET;
        double value = 0;

        friend ostream& operator <<(ostream& write, const BoundaryCondition & boundary_condition);
    };

    typedef size_t t_vert_pos;

    typedef unsigned t_boundaries_ids, t_source_id;
    
    typedef map<t_boundaries_ids, BoundaryCondition> BoundaryConditions;

    typedef map<t_source_id, t_vert_pos> Sources;

    /*! \brief 
        Line - holds indexes of __p1__, __p2__ vertices and id.
        Its structure is depended on Mesh data structure.
    */
    class Line
    {
        public:
            ///Constructor.
            Line(const t_vert_pos p1v, const t_vert_pos p2v, const t_boundaries_ids boundary_id):
                p1(p1v), 
                p2(p2v), 
                boundary_id(boundary_id)
            {};

            Line(const Line& line) = default;

            ///Point index.
            t_vert_pos p1, p2;

            ///Line Index.
            t_boundaries_ids boundary_id;

            friend ostream& operator <<(ostream& write, const Line & line);
            bool operator==(const Line& line) const
            {
                return p1 == line.p1 && p2 == line.p2 && boundary_id == line.boundary_id;
            }
    };

    class SimpleBoundary
    {
        public:
            vector<Point> vertices;
            vector<Line> lines;
            Sources sources;
            bool inner_boundary = false;
            Point hole;
            string name = "";
            void Append(SimpleBoundary simple_boundary);
            void ReplaceElement(t_vert_pos vertice_pos, SimpleBoundary simple_boundary);

            bool operator==(const SimpleBoundary& simple_boundary) const;
            friend ostream& operator <<(ostream& write, const SimpleBoundary & boundary);
    };

    class Boundaries: public vector<SimpleBoundary>//todo replace this with map<id, simple_boundary>
    {
        public:
            typedef map<unsigned, pair<Point, double>> trees_interface_t;

            Boundaries(
                vector<SimpleBoundary> simple_boundaries = {})
            {
                this->reserve(simple_boundaries.size());
                for(auto &sb: simple_boundaries)
                    this->push_back(sb);
            }

            void MakeRectangular(double width = 1, double height = 1, double source_x_position = 0.25);
            
            void MakeRectangularWithHole(double width = 1, double height = 1, double source_x_position = 0.25);

            void Check();

            SimpleBoundary& GetOuterBoundary();

            vector<t_source_id> GetSourcesIds() const;

            vector<Point> GetHolesList() const;

            trees_interface_t GetSourcesIdsPointsAndAngles() const;

            friend ostream& operator <<(ostream& write, const Boundaries & boundary);

        //private:fix me todo

            static double NormalAngle(const Point& left, const Point& center, const Point& right);

            static pair<t_vert_pos, t_vert_pos> GetAdjacentVerticesPositions(
                const t_vert_pos vertices_size, 
                const t_vert_pos vertice_pos);

            static double GetVerticeNormalAngle(
                const vector<Point>& vertices, 
                const t_vert_pos vertice_pos, 
                bool inner_boundary);
    };
}