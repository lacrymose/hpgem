/* 
 * File:   discontinuousSolutionWriter.h
 * Author: irana
 *
 * Created on November 10, 2014, 2:58 PM
 */

#ifndef DISCONTINUOUSSOLUTIONWRITER_HH
#define	DISCONTINUOUSSOLUTIONWRITER_HH

#include <ostream>
#include <vector>

namespace Base
{
    class MeshManipulator;
    class Element;
}

namespace Geometry
{
    class PointReference;
}

namespace Output
{
    ///class to write the data of a single element.
    class SingleElementWriter
    {
    public:
        ///function that actually writes the data for one node on one element.
        /// it is purely virtual, since there is no default for what needs to be
        /// written.
        virtual void writeOutput(const Base::Element*, const Geometry::PointReference&, std::ostream&) = 0;
    };
    
    /// \brief This class prints the solution in every element.
    ///
    /// Class to print the coordinates and solution in every node in every element.
    /// Example usage for Laplace problem: 
    /// DiscontinuousSolutionWriter testWriter(outStream, "title", "01", "u");
    /// testWriter.write(mesh);
    /// Then open gnuplot and plot with the command: splot "output.dat"
    /// Has only been tested for 2D geometries.
    class GNUPlotDiscontinuousSolutionWriter
    {
    public:
        
        ///Constructor: Initialise the output stream and write the header.
        GNUPlotDiscontinuousSolutionWriter(std::ostream& output, const std::string& fileTitle, const std::string& dimensionsToWrite, const std::string& resultVariableName);

        /// Write the data to the stream ouput_.
        void write(const Base::MeshManipulator* mesh, SingleElementWriter* writeDataClass);

        ///Destructor: just flush the output stream, the rest will be destructed 
        /// automatically.
        ~GNUPlotDiscontinuousSolutionWriter()
        {
            output_.flush();
        }
        
    private:
        
        ///stream where all output will be written to.
        std::ostream& output_;

        ///Number of physical dimensions of the domain of the problem.
        const std::size_t nDimensionsToWrite_;
    };
}

#endif	/* DISCONTINUOUSSOLUTIONWRITER_HH */
