/*
 This file forms part of hpGEM. This package has been developed over a number of
 years by various people at the University of Twente and a full list of
 contributors can be found at http://hpgem.org/about-the-code/team

 This code is distributed using BSD 3-Clause License. A copy of which can found
 below.


 Copyright (c) 2014, University of Twente
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software without
 specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// this file should contain all relevant information about how the integrands
// look like and what problem is solved

#define _USE_MATH_DEFINES

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <ctime>

#include "Base/CommandLineOptions.h"

#include "DGMaxLogger.h"
#include "ElementInfos.h"
#include "DGMaxProgramUtils.h"

#include "Algorithms/DGMaxEigenvalue.h"
#include "Algorithms/DGMaxHarmonic.h"
#include "Algorithms/DGMaxTimeIntegration.h"
#include "Algorithms/DivDGMaxEigenvalue.h"
#include "Algorithms/DivDGMaxHarmonic.h"

#include "ProblemTypes/Harmonic/SampleHarmonicProblems.h"
#include "ProblemTypes/Time/SampleTestProblems.h"
#include "Utils/HomogeneousBandStructure.h"
#include "Utils/BandstructureGNUPlot.h"

using namespace hpgem;

auto& p = Base::register_argument<std::size_t>(
    'p', "order", "polynomial order of the solution", true);
auto& meshFile = Base::register_argument<std::string>(
    'm', "meshFile", "The hpgem meshfile to use", true);
auto& numEigenvalues = Base::register_argument<std::size_t>(
    'e', "eigenvalues", "The number of eigenvalues to compute", false, 24);

// Temporary
const std::size_t DIM = 2;

/**
 * main routine. Suggested usage: make a problem, solve it, tell the user your
 * results.
 */
int main(int argc, char** argv) {
    Base::parse_options(argc, argv);
    initDGMaxLogging();
    DGMax::printArguments(argc, argv);

    DGMaxLogger.assert_always(DIM >= 2 && DIM <= 3,
                              "Can only handle 2D and 3D problems.");

    // set up timings
    time_t start, end, initialised, solved;
    time(&start);
    // set up problem and decide flux type
    // DGMax problem(new MaxwellData(numElements.getValue(), p.getValue()), new
    // Base::ConfigurationData(DIM, 1, p.getValue(), 1), new MatrixAssemblyIP);
    const std::size_t numberOfTimeLevels = 1;
    // TODO: LC: this should be determined by the discretization, but this is
    // currently not possible yet, as we get a dependency loop (discretization
    // requires DGMax, which requires the configurationData, which would then
    // require the discretization).
    const std::size_t numberOfUnknowns = 2;
    Base::ConfigurationData configData(numberOfUnknowns, numberOfTimeLevels);
    try {
        double stab = (p.getValue() + 1) * (p.getValue() + 3);
        DivDGMaxDiscretization<DIM>::Stab divStab;
        // Values from the Jelmer fix code.
        divStab.stab1 = 100;
        // Note: for 2D harmonic it looks like that we need 10 instead of 0.01.
        divStab.stab2 = 0.01;
        divStab.stab3 = 10.0;

        auto epsilon = [&](const Geometry::PointPhysical<DIM>& p) {
            // TODO: Hardcoded structure
            return jelmerStructure(p, 0);
        };

        std::unique_ptr<Base::MeshManipulator<DIM>> mesh(
            DGMax::readMesh<DIM>(meshFile.getValue(), &configData, epsilon));
        // base.createCentaurMesh(std::string("SmallIW_Mesh4000.hyb"));
        // base.createCentaurMesh(std::string("BoxCylinder_Mesh6000.hyb"));
        // TODO: LC: this does seem rather arbitrary and should probably be done
        // by the solver

        ///////////////////
        // Harmonic code //
        ///////////////////
        for (auto* element : mesh->getElementsList()) {
            element->setNumberOfTimeIntegrationVectors(3);
        }

        SampleHarmonicProblems<DIM> harmonicProblem(
            SampleHarmonicProblems<DIM>::CONSTANT, 1);

        //        DGMaxHarmonic<DIM> harmonicSolver(*mesh, p.getValue());
        //        harmonicSolver.solve(harmonicProblem, stab);
        //        auto errors = harmonicSolver.computeError(
        //            {DGMaxDiscretizationBase::L2,
        //            DGMaxDiscretizationBase::HCurl}, harmonicProblem);
        //        std::cout << "L2 error" << errors[DGMaxDiscretizationBase::L2]
        //                  << std::endl;
        //        std::cout << "HCurl error " <<
        //        errors[DGMaxDiscretizationBase::HCurl]
        //                  << std::endl;
        // Seems to be broken :(
        //        DivDGMaxHarmonic<DIM> harmonicSolver(*mesh);
        //        harmonicSolver.solve(harmonicProblem, divStab, p.getValue());
        //        std::cout << "L2 error "
        //                  << harmonicSolver.computeL2Error(harmonicProblem)
        //                  << std::endl;

        return 0;

        /////////////////////
        // Eigenvalue code //
        /////////////////////

        //        DGMaxEigenvalue solver (base, p.getValue());
        DivDGMaxEigenvalue<DIM> solver(*mesh, p.getValue(), divStab);
        KSpacePath<DIM> path = KSpacePath<DIM>::cubePath(20);
        EigenvalueProblem<DIM> input(path, numEigenvalues.getValue());
        std::unique_ptr<AbstractEigenvalueResult<DIM>> result =
            solver.solve(input);
        if (Base::MPIContainer::Instance().getProcessorID() == 0) {
            result->printFrequencies();
        }

        ///////////////////////////
        // Time dependent solver //
        ///////////////////////////

        //        DGMaxTimeIntegration timeSolver (base);
        //        SampleTestProblems testProblem
        //        (SampleTestProblems::SARMANY2013); TimeIntegrationParameters
        //        parameters; parameters.stab = stab;
        //        parameters.configureTraditional(DGMaxTimeIntegration::CO2, 1,
        //        p.getValue(), numElements.getValue());
        //        parameters.snapshotStride = 100;
        //        timeSolver.solve(testProblem, parameters);
        //
        //        timeSolver.writeTimeSnapshots("domokos.dat");
        //        timeSolver.printErrors({DGMaxDiscretization::L2,
        //        DGMaxDiscretization::HCurl}, testProblem);

        // Quick way of plotting all the eigenvalue results against theory
        // Probably should be split off at some point to output the
        // bandstructure and do this plotting separately.
        std::array<LinearAlgebra::SmallVector<DIM>, DIM> reciprocalVectors;
        for (std::size_t i = 0; i < DIM; ++i) {
            reciprocalVectors[i].set(0);
            reciprocalVectors[i][i] = 2.0 * M_PI;
        }

        HomogeneousBandStructure<DIM> structure(reciprocalVectors);
        std::vector<std::string> points({"G", "X"});
        if (DIM == 2) {
            points.emplace_back("M");
        } else {
            points.emplace_back("S");
            points.emplace_back("R");
        }
        BandstructureGNUPlot<DIM> output(path, points, structure, result.get());
        output.plot("gnutest.in");

        time(&end);
        DGMaxLogger(INFO, "DGMax finished in %s", end - start);
    } catch (const char* message) {
        DGMaxLogger(ERROR, message);
    }
    // No need to clean globalData/configData, these are cleaned by the baseAPI.
    return 0;
}
