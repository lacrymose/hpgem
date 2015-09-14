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

#include "Geometry/FaceGeometry.h"
#include "PhysicalElement.h"
#include "FaceCacheData.h"
#include "Mesh.h"
#include "GlobalNamespaceBase.h"
#include "BasisFunctionSet.h"
#include "MeshManipulatorBase.h"

namespace Base
{
    template<std::size_t DIM>
    class MeshManipulator;
}

template<std::size_t DIM>
std::ostream& operator<<(std::ostream&, const Base::MeshManipulator<DIM>&);
namespace Geometry
{
    template<std::size_t DIM>
    class PointPhysical;
    template<std::size_t DIM>
    class PointReference;
}

namespace Base
{
    class BasisFunctionSet;
    class OrientedBasisFunctionSet;
    class Face;
    template<std::size_t DIM>
    class MeshMoverBase;
    template<class V>
    class LevelTree;
    class Element;
    struct ConfigurationData;
    class Edge;
    
    struct HalfFaceDescription
    {
        std::vector<std::size_t> nodesList;
        std::size_t elementNum;
        std::size_t localFaceIndex;
    };
    
    //class is made final so we don't have to create a v-table specifically for the destructor
    template<std::size_t DIM>
    class MeshManipulator final : public MeshManipulatorBase
    {
    public:
        
        using PointIndexT = std::size_t;
        using ElementT = Element;
        using FaceT = Face;
        using MeshMoverBaseT = MeshMoverBase<DIM>;
        using BasisFunctionSetT = Base::BasisFunctionSet;

        using ElementLevelTreeT = LevelTree<ElementT>;
        using FaceLevelTreeT = LevelTree<FaceT>;

        using ListOfFacesT = std::vector<FaceT*>;
        using ListOfElementsT = std::vector<ElementT*>;
        using VectorOfElementPtrT = std::vector<ElementT* >;
        using VectorOfPointIndicesT = std::vector<PointIndexT>;
        using CollectionOfBasisFunctionSets = std::vector<std::shared_ptr<const BasisFunctionSetT>>;
        using VecOfElementLevelTreePtrT = std::vector<ElementLevelTreeT*>;
        using VecOfFaceLevelTreePtrT = std::vector<FaceLevelTreeT*>;
        
        using ConstElementIterator = ListOfElementsT::const_iterator;
        using ElementIterator = ListOfElementsT::iterator;

        using ConstFaceIterator = ListOfFacesT::const_iterator;
        using FaceIterator = ListOfFacesT::iterator;

    public:
        /// idRangeBegin is the beginning of the range, from where the Element's ids should be assigned.
        /// In case of multiple meshes, one has to take care of empty intersection of those ranges!!!
        MeshManipulator(const ConfigurationData* configData, BoundaryType xPer = BoundaryType::SOLID_WALL, BoundaryType yPer = BoundaryType::SOLID_WALL, BoundaryType zPer = BoundaryType::SOLID_WALL, std::size_t orderOfFEM = 1, std::size_t idRangeBegin = 0, std::size_t numberOfElementMatrixes = 0, std::size_t numberOfElementVectors = 0, std::size_t numberOfFaceMatrixes = 0, std::size_t numberOfFaceVectors = 0);

        MeshManipulator(const MeshManipulator& other);

        ~MeshManipulator();

        /// creates some cheap, easy to construct basis function set (monomials) to use as a placeholder
        /// \bug this function needs to exist because elements must have a set of basis functions at all time
        /// so the result of this function is passed to the constructor of Element
        void createDefaultBasisFunctions(std::size_t order);

        /// \brief automatically creates the DG basis function set most appropriate for the shape of the element and sets that set as the basis function set to use
        /// \details This function takes the default conforming basis functions and cuts them off at element boundaries. The resulting basis functions have much better
        /// conditioning properties than monomials. Due to the nature of DG, this creates 'interior' basis functions only. (All basis functions are associated with the element)
        /// It is allowed to change the basis function during a computation, but you have to re-initialise the expansion coefficients, since they are expected to change
        /// such that the current solution stays the same. (Stored old solutions are not affected, but they may require a change back to the original basis functions before being
        /// usable again)
        void useDefaultDGBasisFunctions();

        /// \brief automatically creates conforming basis functions, even for mixed meshes
        /// \details For p=1, this creates a nodal basis function set associated with the mesh nodes. For higher polynomial orders it will add hierarchic basis functions associated
        /// with edges, faces and the interior of elements. For the basis functions to be truly conforming you will need some sort of global assembly structure that respects these
        /// associations (GlobalPETScMatrix does this). If you are building such an assembly structure yourself you may assume that the basisFunctions are indexed such that
        /// interior basis functions of the element are first. They are followed by the face basis functions(ordered by local face number primary and the different basis functions secondary)
        /// They are followed by the edge basis functions (using the same ordering) and finally the nodal basis functions (also using the same ordering)
        /// It is allowed to change the basis function during a computation, but you have to re-initialise the expansion coefficients, since they are expected to change
        /// such that the current solution stays the same. (Stored old solutions are not affected, but they may require a change back to the original basis functions before being
        /// usable again)
        void useDefaultConformingBasisFunctions();

        ElementT* addElement(const VectorOfPointIndicesT& globalNodeIndexes);

        bool addFace(ElementT* leftElementPtr, std::size_t leftElementLocalFaceNo, ElementT* rightElementPtr, std::size_t rightElementLocalFaceNo, const Geometry::FaceType& faceType = Geometry::FaceType::WALL_BC);

        void addEdge();

        void addNode();

        std::size_t getNumberOfElements(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.getNumberOfElements(part);
        }
        
        std::size_t getNumberOfFaces(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.getNumberOfFaces(part);
        }
        
        std::size_t getNumberOfEdges(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.getNumberOfEdges(part);
        }
        
        std::size_t getNumberOfNodes(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.getNumberOfNodes(part);
        }
        
        std::size_t getNumberOfNodeCoordinates() const
        {
            return theMesh_.getNumberOfNodeCoordinates();
        }
        
        /// *****************Iteration through the Elements*******************
        
        ConstElementIterator elementColBegin(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.elementColBegin(part);
        }
        
        ConstElementIterator elementColEnd(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.elementColEnd(part);
        }
        
        ElementIterator elementColBegin(IteratorType part = IteratorType::LOCAL)
        {
            return theMesh_.elementColBegin(part);
        }
        
        ElementIterator elementColEnd(IteratorType part = IteratorType::LOCAL)
        {
            return theMesh_.elementColEnd(part);
        }
        
        ConstFaceIterator faceColBegin(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.faceColBegin(part);
        }
        
        ConstFaceIterator faceColEnd(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.faceColEnd(part);
        }
        
        FaceIterator faceColBegin(IteratorType part = IteratorType::LOCAL)
        {
            return theMesh_.faceColBegin(part);
        }
        
        FaceIterator faceColEnd(IteratorType part = IteratorType::LOCAL)
        {
            return theMesh_.faceColEnd(part);
        }
        
        std::vector<Edge*>::const_iterator edgeColBegin(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.edgeColBegin(part);
        }
        
        std::vector<Edge*>::const_iterator edgeColEnd(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.edgeColEnd(part);
        }
        
        std::vector<Edge*>::iterator edgeColBegin(IteratorType part = IteratorType::LOCAL)
        {
            return theMesh_.edgeColBegin(part);
        }
        
        std::vector<Edge*>::iterator edgeColEnd(IteratorType part = IteratorType::LOCAL)
        {
            return theMesh_.edgeColEnd(part);
        }
        
        std::vector<Node*>::const_iterator nodeColBegin(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.nodeColBegin(part);
        }
        
        std::vector<Node*>::const_iterator nodeColEnd(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.nodeColEnd(part);
        }
        
        std::vector<Node*>::iterator nodeColBegin(IteratorType part = IteratorType::LOCAL)
        {
            return theMesh_.nodeColBegin(part);
        }
        
        std::vector<Node*>::iterator nodeColEnd(IteratorType part = IteratorType::LOCAL)
        {
            return theMesh_.nodeColEnd(part);
        }
        /// *****************Iteration through the Elements*******************
        
        void createRectangularMesh(const Geometry::PointPhysical<DIM>& BottomLeft, const Geometry::PointPhysical<DIM>& TopRight, const VectorOfPointIndicesT& LinearNoElements);

        /**
         * Crates a mesh of simplices for the specified cube
         * \param [in] BottomLeft the bottomleft corner of the cube
         * \param [in] TopRight The topRight corner of the cube
         * \param [in] LinearNoElements A vector detailing the amount of refinement you want per direction
         * This routine generates the same mesh structure as createRectangularMesh, but then refines each of the cubes into
         * (DIM-1)^2+1 tetrahedra
         */
        void createTriangularMesh(Geometry::PointPhysical<DIM> BottomLeft, Geometry::PointPhysical<DIM> TopRight, const VectorOfPointIndicesT& LinearNoElements);

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
        void createUnstructuredMesh(Geometry::PointPhysical<DIM> BottomLeft, Geometry::PointPhysical<DIM> TopRight, std::size_t TotalNoNodes, std::function<double(Geometry::PointPhysical<DIM>)> domainDescription, std::vector<Geometry::PointPhysical<DIM> > fixedPoints = {}, std::function<double(Geometry::PointPhysical<DIM>)> relativeEdgeLength = [](Geometry::PointPhysical<DIM>)
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
         * @param isOnPeriodicBoundary A function that returns true if the point passed is on a periodic boundary. This should only return true on the 'master' half of the periodic boundary. Note that support for periodic boundaries is not yet thoroughly tested and might be fragile. (Make sure to allow a small tolerance/snapping distance.)
         * @param mapPeriodicNode A function that returns a point on the other periodic boundary. You may assume isOnPeriodicBoundary returns true for the argument of this function. To prevent infinite duplication you should specify only half the periodic boundary as being periodic. The other half will implicitly also become periodic because it receives periodic nodes from this function
         * @param isOnOtherPeriodicBoundary A function that return true if the point is on a periodic boundary, but isOnPeriodicBoundary returns false
         * @param safeNonPeriodicNode A function that maps the receiving end of the periodic boundary to the interior of the domain so that non-periodic nodes that stray near the periodic boundary can be safed
         * @param dontConnect specify a group of nodes that should not be connected by elements, for example because they are part of a concave boundary. Note that this can have unexpected effect if the nodes you specify are not fixed
         */
        void updateMesh(std::function<double(Geometry::PointPhysical<DIM>)> domainDescription, std::vector<std::size_t> fixedPointIdxs = {}, std::function<double(Geometry::PointPhysical<DIM>)> relativeEdgeLength = [](Geometry::PointPhysical<DIM>)
        {   
            return 1.;
        }, double growFactor = 1.1, std::function<bool(Geometry::PointPhysical<DIM>)> isOnPeriodicBoundary = [](Geometry::PointPhysical<DIM>){return false;}, std::function<Geometry::PointPhysical<DIM>(Geometry::PointPhysical<DIM>)> mapPeriodicNode = nullptr, std::function<bool(Geometry::PointPhysical<DIM>)> isOnOtherPeriodicBoundary = [](Geometry::PointPhysical<DIM>){return false;}, std::function<Geometry::PointPhysical<DIM>(Geometry::PointPhysical<DIM>)> safeNonPeriodicNode = nullptr, std::vector<std::size_t> dontConnect = {});
#endif

        //! Set MeshMoverBase object pointer, for moving meshes if needed
        void setMeshMover(const MeshMoverBaseT * const meshMover);

        void move();

        // ********THESE SHOULD BE REPLACED by ITERABLE EDITIONS LATER**********
        
        //! Get const list of elements
        const ListOfElementsT& getElementsList(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.getElementsList(part);
        }
        
        //! Get non-const list of elements
        ListOfElementsT& getElementsList(IteratorType part = IteratorType::LOCAL)
        {
            return theMesh_.getElementsList(part);
        }
        
        //! Get const list of faces
        const ListOfFacesT& getFacesList(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.getFacesList(part);
        }
        
        //! Get non-const list of faces
        ListOfFacesT& getFacesList(IteratorType part = IteratorType::LOCAL)
        {
            return theMesh_.getFacesList(part);
        }
        
        const std::vector<Edge*>& getEdgesList(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.getEdgesList(part);
        }
        
        std::vector<Edge*>& getEdgesList(IteratorType part = IteratorType::LOCAL)
        {
            return theMesh_.getEdgesList(part);
        }
        
        const std::vector<Node*>& getNodesList(IteratorType part = IteratorType::LOCAL) const
        {
            return theMesh_.getNodesList(part);
        }
        
        std::vector<Node*>& getNodesList(IteratorType part = IteratorType::LOCAL)
        {
            return theMesh_.getNodesList(part);
        }
        // ************************************************************************
        
        //! Changes the default set of basisFunctions for this mesh and all of its elements. Ignores any conforming basisFunctionset that nay be linked to faces/edges/...
        /// Using this to set the hpGEM provided conforming or DG basis functions is deprecated: The routines useDefaultDGBasisFunctionSet and useDefaultConformingBasisFunctionSet can do this more flexibly and also support mixed meshes
        void setDefaultBasisFunctionSet(BasisFunctionSetT* bFSet);

        //! Adds vertex based degrees of freedom to the set of basisfunctions for this mesh and all of its vertices. This routine will assume that the first basisfunctionset connects to the first vertex (local numbering) and so on
        /// Using this to set the hpGEM provided conforming basis functions is deprecated: The routine useDefaultConformingBasis is more flexible and can also deal with mixed meshes
        void addVertexBasisFunctionSet(const std::vector<const BasisFunctionSet*>& bFsets);
                
        //! Adds face based degrees of freedom to the set of basisfunctions for this mesh and all of its faces. This routine will assume that all needed orientations are available in the collection of basisfunctionsets
        /// Using this to set the hpGEM provided conforming basis functions is deprecated: The routine useDefaultConformingBasis is more flexible and can also deal with mixed meshes
        void addFaceBasisFunctionSet(const std::vector<const OrientedBasisFunctionSet*>& bFsets);
                
        //! Adds edge based degrees of freedom to the set of basisfunctions for this mesh and all of its edges. This routine will assume that all needed orientations are available in the collection of basisfunctionsets
        /// Using this to set the hpGEM provided conforming basis functions is deprecated: The routine useDefaultConformingBasis is more flexible and can also deal with mixed meshes
        void addEdgeBasisFunctionSet(const std::vector<const OrientedBasisFunctionSet*>& bFsets);

        const std::vector<Geometry::PointPhysical<DIM> >& getNodeCoordinates() const
        {
            return theMesh_.getNodeCoordinates();
        }
        
        std::vector<Geometry::PointPhysical<DIM> >& getNodeCoordinates()
        {
            return theMesh_.getNodeCoordinates();
        }
        /**
         * Retrieves the Mesh as stored in this MeshManipulator
         */
        Mesh<DIM>& getMesh();
        const Mesh<DIM>& getMesh() const;
        
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
        
        Mesh<DIM> theMesh_;

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

        //when the mesh is updated, persistently store original node coordinates to see if retriangulation is in order
        std::vector<Geometry::PointPhysical<DIM> > oldNodeLocations_;
    };
    

}

#include "MeshManipulator_Impl.h"

#endif /* MESHMANIPULATOR_H_ */
