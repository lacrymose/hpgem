#include "RunnableEVTestCase.h"

#include "Logger.h"

namespace DGMax {

template <std::size_t DIM>
EVConvergenceResult RunnableEVTestCase<DIM>::runWithResults(bool breakOnError) {
    EVConvergenceResult convergenceResult;

    for (std::size_t level = 0; level < getNumberOfLevels(); ++level) {
        std::unique_ptr<AbstractEigenvalueResult<DIM>> result =
            runInternal(level);

        bool hasResult (result);
        logger.assert_always(hasResult, "Null result");

        convergenceResult.addLevel(result->frequencies(0));

        if (getExpected() != nullptr) {
            bool correct = compareWithExpected(level, *result);

            if (!correct && breakOnError) {
                break;
            }
        }
    }
    return convergenceResult;
}

template <std::size_t DIM>
bool RunnableEVTestCase<DIM>::compareWithExpected(
    std::size_t level, const AbstractEigenvalueResult<DIM>& result) const {

    auto expected = getExpected();
    if (expected == nullptr || expected->getNumberOfLevels() <= level) {
        return true;
    }
    const std::vector<double>& expectedLevel = expected->getLevel(level);
    const std::vector<double>& resultLevel = result.frequencies(0);

    if (expectedLevel.size() != resultLevel.size()) {
        logger(ERROR,
               "Different number of eigenvalues expected % got % at level %",
               expectedLevel.size(), resultLevel.size(), level);
        return false;
    }
    for (std::size_t i = 0; i < expectedLevel.size(); ++i) {
        double diff = std::abs(expectedLevel[i] - resultLevel[i]);
        // Note the use of !(diff < tol) as this returns true if diff is
        // NaN, using (diff >= tol) would return false if diff is NaN.
        if (!(diff < getTolerance())) {
            logger(ERROR,
                   "Different %-th eigenvalue at level %: Expected % got %", i,
                   level, expectedLevel[i], resultLevel[i]);
            return false;
        }
    }
    return true;
}

template class RunnableEVTestCase<2>;
template class RunnableEVTestCase<3>;

}  // namespace DGMax
