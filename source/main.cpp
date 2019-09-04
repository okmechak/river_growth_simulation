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

/*!
    \file main.cpp
    \brief Entry point of program and main loop handler.
    @{
    
    Main incorporates user commands handling and managing of inner simulation objects.
    User commands are simple terminal commands(run ./riversim --help to check them all).
    It setups initial conditions and parameters. Initializes all object that are used in
    simulation like: Geometry, Mesh, Triangle and Simualtion, which are all combined
    into the loop.
    Please review code for further details
    @}


    \mainpage Welcome
    @{
    ### Here you can find information about RiverSim program. From detailed description to general use cases.
   
    __RiverSim__ - is simulation of river network growth C++ program, using model based on Laplace equation.
  
    Mathematicaly, program solves Partial Differential Equation(PDE) using Finite Element Method(FEM). Solving of PDE involves setting up boundaries, mesh generation, building a linear system and finaly solution. 
  
    All these steps are done by using different open source C++ libraries: _Triangle_, _Tethex_ and  _Deal.II_, which are combined in one program - __RiverSim__.
 
    ### Plot
     - Introduction
     - Detailed Description
     - General Uses Cases
    @}
*/

#include "riversim.hpp"

using namespace River;
using namespace std;

/*! Entry point of RiverSim program.

    It glues together all user inputs(commands) and inner objects to run simulation 
*/
int main(int argc, char *argv[])
{
    //Program options
    auto vm = process_program_options(argc, argv);

    if (vm.count("help") || vm.count("version"))
        return 0;

    if (!vm.count("suppress-signature") && vm.count("verbose"))
        print_ascii_signature();

    string output_file_name = vm["output"].as<string>();

    //Model object setup
    Model mdl;

    //Timing Object setup
    Timing timing;

    string input_file;
    if(vm.count("input"))
        input_file = vm["input"].as<string>();

    //Border object setup.. Rectangular boundaries
    Border border;
    
    //Tree object setup
    Tree tree;
    
    //Geometry difference
    GeometryDifference gd;

    //Reading data from json file if it is specified so
    {
        bool q_update_border = false;
        if(vm.count("input"))
            Open(mdl, border, tree, gd, vm["input"].as<string>(), q_update_border);

        SetupModelParamsFromProgramOptions(vm, mdl);//..if there are so.

        mdl.CheckParametersConsistency();

        if(mdl.prog_opt.verbose)
            mdl.print();

        if(!q_update_border)
        {
            border.MakeRectangular(
            {mdl.width, mdl.height}, 
            mdl.boundary_ids,
            {mdl.dx},
            {1});

            tree.Initialize(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());
        }
    }
    //check of consistency between Border and Tree
    if(border.GetSourcesId() != tree.SourceBranchesID())
        throw invalid_argument("Border ids and tree ids are not the same, or are not in same order!");

    //Triangle mesh object setup
    Triangle tria;
    tria.AreaConstrain = true;
    tria.CustomConstraint = true;
    tria.MaxTriaArea = mdl.mesh.max_area;
    tria.MinAngle = mdl.mesh.min_angle;
    tria.Verbose = false;
    tria.Quite =  true;
    tria.ref = &mdl.mesh;

    //Simulation object setup
    River::Solver sim(mdl.solver_params.quadrature_degree);
    sim.field_value = mdl.field_value;
    sim.tollerance = mdl.solver_params.tollerance;
    sim.number_of_iterations = mdl.solver_params.num_of_iterrations;
    sim.num_of_static_refinments = mdl.mesh.static_refinment_steps;
    sim.num_of_adaptive_refinments = mdl.solver_params.adaptive_refinment_steps;
    sim.refinment_fraction = mdl.solver_params.refinment_fraction;
    sim.verbose = mdl.prog_opt.verbose;


    //MAIN LOOP
    int i = 0;
    print(mdl.prog_opt.verbose, "Start of main loop...");
    print(!mdl.prog_opt.verbose, "Running...");
    //forward simulation case
    if(vm["simulation-type"].as<unsigned>() == 0)
    {
        print(mdl.prog_opt.verbose, "Forward river simulation type selected.");
        //FIXME stop condition doesn't handle all conditions.
        //! [StopConditionExample]
        while(!StopConditionOfRiverGrowth(border, tree) && i < vm["number-of-steps"].as<unsigned>())
        {
        //! [StopConditionExample]
            
            print(mdl.prog_opt.verbose, "-----#" + to_string(i));
            string str = output_file_name;
            if(vm.count("save-each-step"))
                str += "_" + to_string(i);

            ForwardRiverEvolution(mdl, tria, sim, tree, border, str);
            
            timing.Record();//Timing
            Save(mdl, timing, border, tree, gd, str, input_file);
            ++i;
        }
    }
    //Backward simulation
    else if(vm["simulation-type"].as<unsigned>() == 1)
    {
        print(mdl.prog_opt.verbose, "Backward river simulation type selected.");
        while(tree.HasEmptySourceBranch() == false && i < vm["number-of-steps"].as<unsigned>())    
        {
            print(mdl.prog_opt.verbose, "-----#" + to_string(i));
            
            string str = output_file_name;
            if(vm.count("save-each-step"))
                str += "_" + to_string(i);
            
            BackwardRiverEvolution(mdl, tria, sim, tree, border, gd, str);

            timing.Record();//Timing
            Save(mdl, timing, border, tree, gd, str, input_file);
            ++i;
        }
    }
    //test simulation
    else if(vm["simulation-type"].as<unsigned>() == 2)
    {   
        print(mdl.prog_opt.verbose, "Test river simulation type selected.");

        //reinitialize geometry
        auto b_id = mdl.river_boundary_id;
        border.MakeRectangular(
            {mdl.width, mdl.height}, 
            {b_id, b_id, b_id, b_id},
            {mdl.dx},
            {1});

        tree.Initialize(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());

        auto source_branch_id = border.GetSourcesId().back();
        tree.GetBranch(source_branch_id)->AddPoint(Polar{0.1, 0});

        EvaluateSeriesParams(mdl, tria, sim, tree, border, gd, output_file_name);
        timing.Record();//Timing
        Save(mdl, timing, border, tree, gd, output_file_name);

    }
    //unhandled case
    else 
        throw invalid_argument("Invalid simulation type selected: " + to_string(vm["simulation-type"].as<unsigned>()));

    print(mdl.prog_opt.verbose, "End of main loop...");
    print(true, "Done.");

    return 0;
}