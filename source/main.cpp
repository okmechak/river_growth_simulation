/*! \file main.cpp
    \brief Entry point of program and main loop handler.
    
    Main incorporates user commands handling and managing of inner simulation objects.
    User commands are simple terminal commands(run ./riversim --help to check them all).
    It setups initial conditions and parameters. Initializes all object that are used in
    simulation like: Geometry, Mesh, Triangle and Simualtion, which are all combined
    into the loop.
    Please review code for further details
*/

#include "riversim.hpp"

using namespace River;
using namespace std;
///Entry point of RiverSim program.
/** It glues all together user inputs(commands) and inner objects to run simulation */
int main(int argc, char *argv[])
{
    /**
        Program options
    */
    auto vm = process_program_options(argc, argv);

    if (vm.count("help") || vm.count("version"))
        return 0;

    if (!vm.count("supprchess-signature"))
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
    if(!vm.count("input"))
        border.MakeRectangular(
            {mdl.width, mdl.height}, 
            mdl.boundary_ids,
            {mdl.dx},
            {1});
    
    //Tree object setup
    Tree tree;
    if(!vm.count("input"))
        tree.Initialize(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());

    //Geometry difference
    GeometryDifference gd;

    if(border.GetSourcesId() != tree.SourceBranchesID())
        throw invalid_argument("Border ids and tree ids are not the same, or are not in same order!");

    if(vm.count("input"))
        Open(mdl, border, tree, gd, vm["input"].as<string>());
    SetupModelParamsFromProgramOptions(vm, mdl);//..if there are so.

    //Triangle mesh object setup
    Triangle tria;
    tria.AreaConstrain = true;
    tria.CustomConstraint = true;
    tria.MaxTriaArea = mdl.mesh.max_area;
    tria.MinAngle = mdl.mesh.min_angle;
    tria.Verbose = vm["verbose"].as<bool>();
    tria.Quite =  vm["quiet"].as<bool>();
    tria.ref = &mdl.mesh;

    //Simulation object setup
    River::Solver sim(mdl.solver_params.quadrature_degree);
    sim.field_value = mdl.field_value;
    //TEMPORAL
    //sim.num_of_refinments = mdl.solver_params.refinment_steps;
    sim.refinment_fraction = mdl.solver_params.refinment_fraction;


    //MAIN LOOP
    int i = 0;
    if(vm["simulation-type"].as<int>() == 0)
        //forward simulation case
        //FIXME stop condition doesn't handle all conditions.
        while(!StopConditionOfRiverGrowth(border, tree) && i < vm["number-of-steps"].as<int>())
        {
            cout << "-------" << endl;
            cout << "  "<<i<< endl;
            cout << "-------" << endl;

            string str = output_file_name;
            if(vm.count("save-each-step"))
                str += "_" + to_string(i);

            ForwardRiverEvolution(mdl, tria, sim, tree, border, str);
            
            timing.Record();//Timing
            Save(mdl, timing, border, tree, gd, str, input_file);
            ++i;
        }
    else if(vm["simulation-type"].as<int>() == 1)
    {
        //backward simmulation
        bool stop_flag = false;
        while(stop_flag == false && tree.HasEmptySourceBranch() == false && i < vm["number-of-steps"].as<int>())    
        {
            cout << "-------" << endl;
            cout << "  "<<i<< endl;
            cout << "-------" << endl;
            
            string str = output_file_name;
            if(vm.count("save-each-step"))
                str += "_" + to_string(i);
            
            stop_flag = BackwardRiverEvolution(mdl, tria, sim, tree, border, gd, str);

            timing.Record();//Timing
            Save(mdl, timing, border, tree, gd, str, input_file);
            ++i;
        }
    }
    else if(vm["simulation-type"].as<int>() == 2)
    {   
        cout << "-------------------------" << endl;
        cout << "Series parameters testing" << endl;
        cout << "-------------------------" << endl;

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
    else 
        throw invalid_argument("Invalid simulation type value");

    return 0;
}