/*
 This file forms part of hpGEM. This package has been developed over a number of years by various people at the University of Twente and a full list of contributors can be found at
 http://hpgem.org/about-the-code/team
 
 This code is distributed using BSD 3-Clause License. A copy of which can found below.
 
 
 Copyright (c) 2014, University of Twente
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MESHMANIPULATOR_H_
#define MESHMANIPULATOR_H_


#include <vector>
#include <fstream>

//for enum support... (FaceType)
#include "Geometry/FaceGeometry.hpp"
#include "FaceCacheData.hpp"
#include "Mesh.hpp"

namespace Geometry {
    class PointPhysical;
    class PointReference;
}

namespace Base {
    class BasisFunctionSet;
    class OrientedBasisFunctionSet;
    class Face;
    class MeshMoverBase;
    template<class V>
    class LevelTree;
    class Element;
    class ConfigurationData;
    class Edge;

    struct HalfFaceDescription {
        std::vector<std::size_t> nodeList;
        std::size_t elementNum;
        std::size_t localFaceIndex;
    };

    class MeshManipulator //: public MeshRefiner <DIM>
    {
    public:

        typedef std::size_t PointIndexT;
        typedef Element ElementT;
        typedef Face FaceT;
        typedef MeshMoverBase MeshMoverBaseT;
        typedef Geometry::PointPhysical PointPhysicalT;
        typedef Base::BasisFunctionSet BasisFunctionSetT;

        typedef LevelTree<ElementT> ElementLevelTreeT;
        typedef LevelTree<FaceT> FaceLevelTreeT;

        typedef std::vector<FaceT*> ListOfFacesT;
        typedef std::vector<ElementT*> ListOfElementsT;
        typedef std::vector<ElementT* > VectorOfElementPtrT;
        typedef std::vector<PointPhysicalT > VectorOfPhysicalPointsT;
        typedef std::vector<PointIndexT> VectorOfPointIndicesT;
        typedef std::vector<const BasisFunctionSetT*> CollectionOfBasisFunctionSets;
        typedef std::vector<ElementLevelTreeT*> VecOfElementLevelTreePtrT;
        typedef std::vector<FaceLevelTreeT*> VecOfFaceLevelTreePtrT;


        //typedef typename ElementLevelTreeT::iterator        ElementIteratorT;
        //typedef typename FaceLevelTreeT::iterator           FaceIteratorT;

        typedef typename ListOfElementsT::const_iterator ConstElementIterator;
        typedef typename ListOfElementsT::iterator ElementIterator;

        typedef typename ListOfFacesT::const_iterator ConstFaceIterator;
        typedef typename ListOfFacesT::iterator FaceIterator;


    public:
        /// idRangeBegin is the begining of the range, from where the Element's ids should be assigned.
        /// In case of multiple meshes, one has to take care of empty intersection of those ranges!!!
        MeshManipulator(const ConfigurationData* configData, bool xPer = 0, bool yPer = 0, bool zPer = 0, std::size_t orderOfFEM = 1, std::size_t idRangeBegin = 0,
                int nrOfElementMatrixes = 0, int nrOfElementVectors = 0, int nrOfFaceMatrixes = 0, int nrOfFaceVectors = 0);

        MeshManipulator(const MeshManipulator& other);

        virtual ~MeshManipulator();



        void createDefaultBasisFunctions(std::size_t order);

        ElementT* addElement(const VectorOfPointIndicesT& globalNodeIndexes);

        bool addFace(ElementT* leftElementPtr, std::size_t leftElementLocalFaceNo,
                ElementT* rightElementPtr, std::size_t rightElementLocalFaceNo,
                const Geometry::FaceType& faceType = Geometry::FaceType::WALL_BC);

        //void addEdge(std::vector< Element*> elements, std::vector<std::size_t> localEdgeNrs);
        void addEdge();

        void addVertex();

        std::size_t getNumberOfElements(IteratorType part = IteratorType::LOCAL) const {
            return theMesh_.getNumberOfElements(part);
        }

        std::size_t getNumberOfFaces(IteratorType part = IteratorType::LOCAL) const {
            return theMesh_.getNumberOfFaces(part);
        }

        std::size_t getNumberOfEdges(IteratorType part = IteratorType::LOCAL) const {
            return theMesh_.getNumberOfEdges(part);
        }

        std::size_t getNumberOfVertices(IteratorType part = IteratorType::LOCAL) const {
            return theMesh_.getNumberOfVertices(part);
        }

        std::size_t getNumberOfNodes()const {
            return theMesh_.getNumberOfNodes();
        }

        /// *****************Iteration through the Elements*******************

        ConstElementIterator elementColBegin(IteratorType part = IteratorType::LOCAL)const {
            return theMesh_.elementColBegin(part);
        }

        ConstElementIterator elementColEnd(IteratorType part = IteratorType::LOCAL)const {
            return theMesh_.elementColEnd(part);
        }

        ElementIterator elementColBegin(IteratorType part = IteratorType::LOCAL) {
            return theMesh_.elementColBegin(part);
        }

        ElementIterator elementColEnd(IteratorType part = IteratorType::LOCAL) {
            return theMesh_.elementColEnd(part);
        }

        ConstFaceIterator faceColBegin(IteratorType part = IteratorType::LOCAL)const {
            return theMesh_.faceColBegin(part);
        }

        ConstFaceIterator faceColEnd(IteratorType part = IteratorType::LOCAL)const {
            return theMesh_.faceColEnd(part);
        }

        FaceIterator faceColBegin(IteratorType part = IteratorType::LOCAL) {
            return theMesh_.faceColBegin(part);
        }

        FaceIterator faceColEnd(IteratorType part = IteratorType::LOCAL) {
            return theMesh_.faceColEnd(part);
        }

        std::vector< Edge*>::const_iterator edgeColBegin(IteratorType part = IteratorType::LOCAL)const {
            return theMesh_.edgeColBegin(part);
        }

        std::vector< Edge*>::const_iterator edgeColEnd(IteratorType part = IteratorType::LOCAL)const {
            return theMesh_.edgeColEnd(part);
        }

        std::vector< Edge*>::iterator edgeColBegin(IteratorType part = IteratorType::LOCAL) {
            return theMesh_.edgeColBegin(part);
        }

        std::vector< Edge*>::iterator edgeColEnd(IteratorType part = IteratorType::LOCAL) {
            return theMesh_.edgeColEnd(part);
        }

        std::vector< Node*>::const_iterator vertexColBegin(IteratorType part = IteratorType::LOCAL)const {
            return theMesh_.vertexColBegin(part);
        }

        std::vector< Node*>::const_iterator vertexColEnd(IteratorType part = IteratorType::LOCAL)const {
            return theMesh_.vertexColEnd(part);
        }

        std::vector< Node*>::iterator vertexColBegin(IteratorType part = IteratorType::LOCAL) {
            return theMesh_.vertexColBegin(part);
        }

        std::vector< Node*>::iterator vertexColEnd(IteratorType part = IteratorType::LOCAL) {
            return theMesh_.vertexColEnd(part);
        }
        /// *****************Iteration through the Elements*******************

        void createRectangularMesh(const PointPhysicalT& BottomLeft, const PointPhysicalT& TopRight, const VectorOfPointIndicesT& LinearNoElements);

        /**
         * Crates a mesh of simplices for the specified cube
         * \param [in] BottomLeft the bottomleft corner of the cube
         * \param [in] TopRight The topRight corner of the cube
         * \param [in] LinearNoElements A vector detailing the amount of refinement you want per direction
         * This routine generates the same mesh structure as createRectangularMesh, but then refines each of the cubes into
         * (DIM-1)^2+1 tetrahedra
         */
        void createTriangularMesh(PointPhysicalT BottomLeft, PointPhysicalT TopRight, const VectorOfPointIndicesT& LinearNoElements);

        void readCentaurMesh(const std::string& filename);

#ifdef HPGEM_USE_QHULL
        /**
         * \brief create an unstructured triangular mesh
         * \details An iterative mesh generator based on "A Simple Mesh Generator in Matlab" (Persson & Strang, 2004)
         * The initial mesh uses the same structure as createTriangularMesh.
         * This algorithm will still function if the bounding box of the domain is only known approximately
         * If there is a very large difference between the smallest desired edge length and the largest desired edge length (ratio > ~4), it will usually help to overestimate the domain size
         * The domain has to be described implicitly by a distance function, such that domainDescription(p) < 0 means p is inside the domain and the gradient of domaindescription is normal to the boundary
         * Local element refinement is possible by providing desired relative edge lengths of the output mesh. The scaling of this function has no effect on the resulting mesh.
         * If the edge scaling function returns NaN for some part of the domain, it is expanded exponentially from the known parts
         * This routine cannot deal very well with concave(>pi) or very sharp corners(~<pi/4), by placing fixed points on these location, performance can be greatly improved
         * Mesh quality is not guaraneed if growFactor is much larger or smaller than 1
         * @param BottomLeft The bottom left corner of the bounding box of the domain
         * @param TopRight The top right corner of the bounding box of the domain
         * @param TotalNoNodes The desired amount of nodes in the mesh
         * @param domainDescription A function that maps PointPhysicals to doubles, such that negative numbers signify points inside the mesh
         * @param fixedPoints coordinates of point that MUST be in the mesh, no matter what
         * @param relativeEdgeLength Allow r-refinement
         * @param growFactor specify how much larger than its neighbours an element may be in areas where relativeEdgeLengths returns NaN
         */
        void createUnstructuredMesh(PointPhysicalT BottomLeft, PointPhysicalT TopRight, std::size_t TotalNoNodes, std::function<double(PointPhysicalT) > domainDescription, std::vector<PointPhysicalT> fixedPoints = {}, std::function<double(PointPhysicalT) > relativeEdgeLength = [](PointPhysicalT)
        {
            return 1.;
        }, double growFactor = 1.1);

        /**
         * \brief improve the mesh quality of an existing mesh
         * \details An iterative mesh generator based on "A Simple Mesh Generator in Matlab" (Persson & Strang, 2004)
         * The domain has to be described implicitly by a distance function, such that domainDescription(p) < 0 means p is inside the domain and the gradient of domaindescription is normal to the boundary
         * Local element refinement is possible by providing desired relative edge lengths of the output mesh. The scaling of this function has no effect on the resulting mesh.
         * If the edge scaling function returns NaN for some part of the domain, it is expanded exponentially from the known parts
         * This routine cannot deal very well with concave(>pi) or very sharp corners(~<pi/4), by placing fixed points on these location, performance can be greatly improved
         * If no implicit description of the domain is available, fixing all boundary nodes usually prevents the other nodes from escaping. In this case domainDescription can return -1. for all p.
         * \todo the current implementation throws away all data, this behaviour should be replaced by an interpolation scheme
         * \todo this algoritm boils down to alternatingly doing delaunay triangulations and moving nodes according to an optimally damped mass spring system. This is currently done using a relatively crude implementation.
         * Once there is a proper coupling between mercury and hpGEM, some thought should be given to the improvement of this algorithm
         * @param domainDescription A function that maps PointPhysicals to doubles, such that negative numbers signify points inside the mesh
         * @param fixedPointIdxs pointIndexes of point that MUST remain in the same location, no matter what
         * @param relativeEdgeLength Allow r-refinement
         * @param growFactor specify how much larger than its neighbours an element may be in areas where relativeEdgeLengths returns NaN
         */
        void updateMesh(std::function<double(PointPhysicalT) > domainDescription, std::vector<std::size_t> fixedPointIdxs = {}, std::function<double(PointPhysicalT) > relativeEdgeLength = [](PointPhysicalT)
        {
            return 1.;
        }, double growFactor = 1.1);
#endif

        //realy? y u no operator>>? -FB
        void outputMesh(std::ostream& os)const;


        //! Set MeshMoverBase object pointer, for moving meshes if needed
        void setMeshMover(const MeshMoverBaseT * const meshMover);

        void move();

        // ******************THESE SHOULD BE DELETED LATER***********************//actually, these should be replaced by iterable editions of the levelTree -FB
        //! Get const list of elements

        const ListOfElementsT& getElementsList(IteratorType part = IteratorType::LOCAL) const {
            return theMesh_.getElementsList(part);
        }

        //! Get non-const list of elements

        ListOfElementsT& getElementsList(IteratorType part = IteratorType::LOCAL) {
            return theMesh_.getElementsList(part);
        }

        //! Get const list of faces

        const ListOfFacesT& getFacesList(IteratorType part = IteratorType::LOCAL) const {
            return theMesh_.getFacesList(part);
        }

        //! Get non-const list of faces

        ListOfFacesT& getFacesList(IteratorType part = IteratorType::LOCAL) {
            return theMesh_.getFacesList(part);
        }

        const std::vector<Edge*>& getEdgesList(IteratorType part = IteratorType::LOCAL) const {
            return theMesh_.getEdgesList(part);
        }

        std::vector<Edge*>& getEdgesList(IteratorType part = IteratorType::LOCAL) {
            return theMesh_.getEdgesList(part);
        }

        const std::vector<Node*>& getVerticesList(IteratorType part = IteratorType::LOCAL) const {
            return theMesh_.getVerticesList(part);
        }

        std::vector<Node*>& getVerticesList(IteratorType part = IteratorType::LOCAL) {
            return theMesh_.getVerticesList(part);
        }
        // ************************************************************************

        //! Changes the default set of basisFunctions for this mesh and all of its elements. Ignores any conforming basisFunctionset that nay be linked to faces/edges/...
        void setDefaultBasisFunctionSet(BasisFunctionSetT* bFSet);

        //! Adds vertex based degrees of freedom to the set of basisfunctions for this mesh and all of its vertices. This routine will assume that the first basisfunctionset connects to the first vertex (local numbering) and so on
        void addVertexBasisFunctionSet(CollectionOfBasisFunctionSets& bFsets); ///\TODO support for mixed meshes

        //! Adds face based degrees of freedom to the set of basisfunctions for this mesh and all of its faces. This routine will assume that all needed orientations are available in the collection of basisfunctionsets
        void addFaceBasisFunctionSet(std::vector<const OrientedBasisFunctionSet*>& bFsets); ///\TODO support for mixed meshes

        //! Adds edge based degrees of freedom to the set of basisfunctions for this mesh and all of its edges. This routine will assume that all needed orientations are available in the collection of basisfunctionsets
        void addEdgeBasisFunctionSet(std::vector<const OrientedBasisFunctionSet*>& bFsets); ///\TODO support for mixed meshes

        int dimension() const;

        const std::vector<PointPhysicalT>& getNodes()const
        {
            return theMesh_.getNodes();
        }

        std::vector<PointPhysicalT>& getNodes()
        {
            return theMesh_.getNodes();
        }

        //routines that deal with level trees
        //---------------------------------------------------------------------
        //! Get the number of mesh-tree.
        /*int getNumberOfMeshes() const;

        //! Create a new (empty) mesh-tree.
        void createNewMeshTree();

        //! Get the element container of a specific mesh-tree.
        ElementLevelTreeT* ElCont(int meshTreeIdx) const;

        //! Get the face container of a specific mesh-tree.
        FaceLevelTreeT* FaCont(int meshTreeIdx) const;

        //! Some mesh generator: centaur / rectangular / triangle / tetrahedra / triangular-prism.
        void someMeshGenerator(int meshTreeIdx);

        //! Set active mesh-tree.
        void setActiveMeshTree(std::size_t meshTreeIdx);

        //! Get active mesh-tree index.
        int getActiveMeshTree() const;

        //! Reset active mesh-tree.
        void resetActiveMeshTree();

        //! Get maximum h-level of a specific mesh-tree.
        std::size_t getMaxLevel(int meshTreeIdx) const;

        //! Set active level of a specific mesh-tree.
        void setActiveLevel(std::size_t meshTreeIdx, int level);

        //! Get active level of a specific mesh-tree.
        int getActiveLevel(int meshTreeIdx) const;

        //! Reset active level of a specific mesh-tree.
        void resetActiveLevel(int meshTreeIdx);

        //! Duplicate mesh contents including all refined meshes.
        void duplicate(std::size_t fromMeshTreeIdx, std::size_t toMeshTreeIdx, std::size_t upToLevel);

        //! Refine a specific mesh-tree.
        void doRefinement(std::size_t meshTreeIdx, int refinementType);*/

        /**
         * Retrieves the Mesh as stored in this MeshManipulator
         * @return 
         */
        Mesh& getMesh();
        const Mesh& getMesh() const;
        //---------------------------------------------------------------------
    private:

        //!Does the actual reading for 2D centaur meshes
        void readCentaurMesh2D(std::ifstream& centaurFile);

        //!Does the actual reading for 3D centaur meshes
        void readCentaurMesh3D(std::ifstream& centaurFile);

        //!Construct the faces based on connectivity information about elements and nodes
        void faceFactory();

        //!Construct the faces based on connectivity information about elements and nodes
        void edgeFactory();
        
        //! Do refinement on the elements.
        /*void doElementRefinement(std::size_t meshTreeIdx);

        //! Do refinement on the faces.
        void doFaceRefinement(std::size_t meshTreeIdx);

        //! Check whether the two elements may be connected by a face or not.
        void pairingCheck(const ElementIterator elL, std::size_t locFaceNrL,
                const ElementIterator elR, std::size_t locFaceNrR,
                int& pairingValue, bool& sizeOrder);*/

        //! Check whether the two elements may be connected by a face or not in periodic face case.
        //void                            periodicPairingCheck(const FaceIteratorT fa,
        //                                                     const ElementIteratorT elL, std::size_t localFaceNrL,
        //                                                     const ElementIteratorT elR, std::size_t localFaceNrR,
        //                                                     int& pairingValue, bool& sizeOrder);
        //---------------------------------------------------------------------



    private:

        Mesh theMesh_;

        const ConfigurationData* configData_;
        //! Periodicity in x-direction.
        bool periodicX_;

        //! Periodicity in y-direction.
        bool periodicY_;

        //! Periodicity in z-direction.
        bool periodicZ_;

        /// Pointer to MeshMoverBase, in order to move points in the mesh, when needed by user.
        const MeshMoverBaseT* meshMover_;

        //! Collection of additional basis function set, if p-refinement is applied
        CollectionOfBasisFunctionSets collBasisFSet_;

        //const BasisFunctionSetT*        defaultSetOfBasisFunctions_;

        //! Active mesh-tree.
        //int activeMeshTree_;

        //! Number of mesh-tree.
        //int numMeshTree_;

        //! Vector elements LevelTree.
        //VecOfElementLevelTreePtrT vecOfElementTree_;

        //! Vector faces LevelTree.
        //VecOfFaceLevelTreePtrT vecOfFaceTree_;

        int numberOfElementMatrixes_;
        int numberOfFaceMatrixes_;
        int numberOfElementVectors_;
        int numberOfFaceVectors_;

        //when the mesh is updated, persistently store original node coordinates to see if retriangulation is in order
        std::vector<PointPhysicalT> oldNodeLocations_;
    };

}





#endif /* MESHMANIPULATOR_H_ */
