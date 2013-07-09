/*
 * TecplotOutputTest.cpp
 *
 *  Created on: Feb 16, 2013
 *      Author: nicorivas
 */

#include "Base/MeshManipulator.hpp"
#include "Geometry/PointPhysical.hpp"
#include "Output/TecplotDiscontinuousSolutionWriter.hpp"

#include <vector>
#include <iostream>
#include <fstream>

int main()
{
    Base::ConfigurationData config;
    
    config.numberOfUnknowns_       = 1;
    config.numberOfTimeLevels_     = 1;
    config.numberOfBasisFunctions_ = 1;

    // Create a mesh
    Geometry::PointPhysical<2> bottomLeft, topLeft;
    std::vector<unsigned int> numElementsOneD(2);

    bottomLeft[0]=0;
    bottomLeft[1]=0;

    topLeft[0]=8;
    topLeft[1]=8;

    numElementsOneD[0]=8;
    numElementsOneD[1]=8;

    Base::MeshManipulator<2> myTwoDDemoMesh(&config);

    myTwoDDemoMesh.createRectangularMesh(bottomLeft,topLeft,numElementsOneD);
    myTwoDDemoMesh.outputMesh(std::cout);

    // TecPlot Output
    std::ofstream file2D;
    file2D.open ("out.dat");

    int dimensionsToWrite[2] = {0,1};

    Output::TecplotDiscontinuousSolutionWriter<2> out(file2D,"RectangularMesh",dimensionsToWrite,"xy");
    out.write(&myTwoDDemoMesh,"holi",false);

}