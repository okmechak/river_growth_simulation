#pragma once

#include "utilities.hpp"
#include "geometry.hpp"
#include "mesh.hpp"
#include "solver.hpp"

namespace River
{
    namespace SimpleGeo{
    Geometry CustomRiverTree(double dl = 0.1, double eps = 0.03);
    Geometry Box();
    Geometry SingleTip();
    }
    /* 
        TODO: 
        Implementation of some standard workflows 
        like:
            - forward river evolution
            - backward river evolution
            - statistics and postrocessing
            - evaluation of parameters
            - other
    
    */


}