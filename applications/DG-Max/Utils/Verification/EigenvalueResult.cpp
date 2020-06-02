#include "EigenvalueResult.h"

#include <iomanip>
#include <iostream>

namespace DGMax {

void EigenvalueResult::printFrequencyTable(std::vector<double> theoretical) {
    std::size_t max = maxNumberOfFrequencies();

    // Header line
    std::cout << "|";
    for (std::size_t i = 0; i < max; ++i) {
        if (i < theoretical.size()) {
            std::cout << " " << std::setprecision(4) << std::setw(6)
                      << theoretical[i] << " |";
        } else {
            // When lacking an theoretical frequency, just print a placeholder
            std::cout << " f_ " << std::setw(4) << i << " |";
        }
    }
    std::cout << std::endl << "|";  // For the header bottom line
    for (std::size_t i = 0; i < max; ++i) {
        std::cout << "--------|";
    }
    std::cout << std::endl;
    // Frequencies
    for (auto &levelFrequency : frequencyLevels_) {
        std::cout << "|";
        for (double &frequency : levelFrequency) {
            std::cout << " " << std::setprecision(4) << std::setw(6)
                      << frequency << " |";
        }
        std::cout << std::endl;
    }
    // Spacing newline
    std::cout << std::endl;
}

void EigenvalueResult::printErrorTable(std::vector<double> theoretical) {

    std::size_t max = maxNumberOfFrequencies();

    std::vector<std::vector<double>> errors;
    // Compute actual errors
    for (auto &levelFrequencies : frequencyLevels_) {
        std::vector<double> temp;
        for (std::size_t i = 0;
             i < levelFrequencies.size() && i < theoretical.size(); ++i) {
            temp.emplace_back(std::abs(levelFrequencies[i] - theoretical[i]));
        }
        errors.emplace_back(temp);
    }

    // Printing
    // Header line
    std::cout << "|";
    for (std::size_t i = 0; i < max; ++i) {
        std::cout << " ";  // Separator from previous column
        if (i < theoretical.size()) {
            std::cout << std::setprecision(4) << std::setw(8) << theoretical[i];
        } else {
            // Place holder
            std::cout << "f_" << std::setw(8) << std::right << i;
        }
        std::cout << "        "  // Space of second column
                  << " |";
    }
    std::cout << std::endl << "|";  // For the header bottom line
    for (std::size_t i = 0; i < max; ++i) {
        std::cout << "----------|-------|";
    }
    std::cout << std::endl;
    // Frequencies
    for (std::size_t i = 0; i < errors.size(); ++i) {
        auto &meshErrors = errors[i];
        std::cout << "|";
        for (std::size_t j = 0; j < meshErrors.size(); ++j) {
            double &error = meshErrors[j];
            std::cout << " " << std::scientific << std::setprecision(2)
                      << std::setw(6) << error << " |";
            if (i == 0 || errors[i - 1].size() <= j) {
                // For first row there is no convergence speed
                // so print a placeholder instead.
                std::cout << "     - |";
            } else {
                double factor = errors[i - 1][j] / error;
                std::cout << " " << std::fixed << std::setprecision(2)
                          << std::setw(5) << factor << " |";
            }
        }
        std::cout << std::endl;
    }
    // Spacing newline
    std::cout << std::endl;
}

bool EigenvalueResult::equals(const EigenvalueResult &other, double tolerance) {
    if (frequencyLevels_.size() != other.frequencyLevels_.size()) {
        return false;
    }
    std::size_t levels = frequencyLevels_.size();
    for (std::size_t i = 0; i < levels; ++i) {
        if (frequencyLevels_[i].size() != other.frequencyLevels_[i].size()) {
            return false;
        }
        for (std::size_t j = 0; j < frequencyLevels_[i].size(); ++j) {
            double diff = frequencyLevels_[i][j] - other.frequencyLevels_[i][j];
            // Note the use of !(diff < tol) as this returns true if diff is
            // NaN, using (diff >= tol) would return false if diff is NaN.
            if (!(std::abs(diff) < tolerance)) {
                return false;
            }
        }
    }
    return true;
}

}  // namespace DGMax
