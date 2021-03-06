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

#include <chrono>
#include <fstream>

#include "AcousticWave.h"
#include "AcousticWaveLinear.h"
#include "Base/TimeIntegration/AllTimeIntegrators.h"

#include "Logger.h"

using namespace hpgem;

auto& name = Base::register_argument<std::string>('n', "meshName",
                                                  "name of the mesh", true);
auto& polynomialOrder = Base::register_argument<std::size_t>(
    'p', "order", "polynomial order of the solution", true);
auto& solverId = Base::register_argument<std::size_t>(
    's', "solverId",
    "integer to indicate if you want to use AcousticWave (0) or "
    "AcousticWaveLinear (1)",
    true);

auto& numberOfOutputFrames = Base::register_argument<std::size_t>(
    'O', "numberOfOutputFrames", "Number of frames to output", false, 1);
auto& startTime = Base::register_argument<double>(
    'S', "startTime", "start time of the simulation", false, 0.0);
auto& endTime = Base::register_argument<double>(
    'T', "endTime", "end time of the simulation", false, 1.0);
auto& dt = Base::register_argument<double>(
    'd', "timeStepSize", "time step of the simulation", false, 0.01);

int main(int argc, char** argv) {
    Base::parse_options(argc, argv);

    // Set parameters for the PDE.
    const std::size_t dimension = 2;  // Either 1, 2 or 3
    const TimeIntegration::ButcherTableau* const ptrButcherTableau =
        TimeIntegration::AllTimeIntegrators::Instance().getRule(4, 4);
    const double c = 1.0;

    std::vector<std::string> variableNames;
    variableNames.push_back("v");
    for (std::size_t i = 0; i < dimension; i++) {
        std::string variableName = "s" + std::to_string(i);
        variableNames.push_back(variableName);
    }

    // Compute parameters for PDE
    const std::size_t numberOfVariables = dimension + 1;

    if (solverId.getValue() == 1) {
        // Create problem solver 'test', that can solve the acoustic wave
        // equations.
        AcousticWaveLinear<dimension> test(
            numberOfVariables, polynomialOrder.getValue(), ptrButcherTableau);

        // Create the mesh
        test.readMesh(name.getValue());

        // Set the material parameter
        test.setMaterialParameter(c);

        // Set the names for the output file
        test.setOutputNames("output", "acousticWaveLinear",
                            "acousticWaveLinear", variableNames);

        // Solve the problem over time interval [startTime,endTime].
        // Start Measuring elapsed time
        std::chrono::time_point<std::chrono::system_clock> startClock, endClock;
        startClock = std::chrono::system_clock::now();

        // Solve the problem
        test.solve(startTime.getValue(), endTime.getValue(), dt.getValue(),
                   numberOfOutputFrames.getValue(), true);

        // Measure elapsed time
        endClock = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = endClock - startClock;
        std::cout << "Elapsed time for solving the PDE: "
                  << elapsed_seconds.count() << "s\n";
    } else {
        // Create problem solver 'test', that can solve the acoustic wave
        // equations.
        AcousticWave<dimension> test(
            numberOfVariables, polynomialOrder.getValue(), ptrButcherTableau);

        // Create the mesh
        test.readMesh(name.getValue());

        // Set the material parameter
        test.setMaterialParameter(c);

        // Set the names for the output file
        test.setOutputNames("output", "acousticWave", "acousticWave",
                            variableNames);

        // Solve the problem over time interval [startTime,endTime].
        // Start Measuring elapsed time
        std::chrono::time_point<std::chrono::system_clock> startClock, endClock;
        startClock = std::chrono::system_clock::now();

        // Solve the problem
        test.solve(startTime.getValue(), endTime.getValue(), dt.getValue(),
                   numberOfOutputFrames.getValue(), true);

        // Measure elapsed time
        endClock = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = endClock - startClock;
        std::cout << "Elapsed time for solving the PDE: "
                  << elapsed_seconds.count() << "s\n";
    }

    return 0;
}
