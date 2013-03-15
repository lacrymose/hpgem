//
//  FaceGeometry.cpp
//  
//
//  Created by Shavarsh Nurijanyan on 2/10/13.
//
//

namespace Geometry {

    template<unsigned int DIM>
    class FaceGeometry;

    template<unsigned int DIM>
    FaceGeometry<DIM>::FaceGeometry(ElementGeometryT*       ptrElemL,
                                    const LocalFaceNrType&  localFaceNumL,
                                    ElementGeometryT*       ptrElemR,
                                    const LocalFaceNrType&  localFaceNumR) :
        leftElementGeom_(ptrElemL),
        localFaceNumberLeft_(localFaceNumL),
        rightElementGeom_(ptrElemR),
        localFaceNumberRight_(localFaceNumR),
        faceType_(INTERNAL)
    {
            // ~OC~
            // All that remains to be done for the Face-class's data is to
            // find the mapping from one side of the face to the other.  The
            // body of the constructor is entirely for that:
        
            // This code was taken from DGFaceInternal.  declare and fill
            // two arrays with the global node numbers of the face in the
            // (possibly) different permutations in which they occur seen
            // from the two elements
        
//        typename PhysicalGeometryT::VectorOfPointIndexesT globalNodeNrsL;
//        typename PhysicalGeometryT::VectorOfPointIndexesT globalNodeNrsR;
//            //
        
       // cout << "In Face constructor=" << endl;
//        cout << "Element left=" << *ptrElemL <<endl;
//        cout << "Element right=" << *ptrElemR << endl;
//        cout << "Local left face number=" <<localFaceNumL<<endl;
//        cout << "Local right face number=" <<localFaceNumR<<endl;
//        
//        cout << "Element Left=" << ptrElemL->getReferenceGeometry() <<endl;
//        cout << "Element Left Co dimension mapping=" << ptrElemL->getReferenceGeometry()->getCodim1ReferenceGeometry(localFaceNumberLeft_) <<endl;


        //leftElementGeom_->getReferenceGeometry()->getCodim1ReferenceGeometry(localFaceNumberLeft_);

        std::vector<unsigned int> globalNodeNrsL;
        std::vector<unsigned int> globalNodeNrsR;

        const Geometry::PhysicalGeometry<DIM>* const leftPG  = leftElementGeom_->getPhysicalGeometry();
        const Geometry::PhysicalGeometry<DIM>* const rightPG = (rightElementGeom_ == NULL? NULL:rightElementGeom_->getPhysicalGeometry());

            // We try element left first because its the one we are sure exists.
        leftPG->getGlobalFaceNodeIndices(localFaceNumberLeft_, globalNodeNrsL);

            // This means internal face in old hpGEM (only left element for non-boundary)

        if (rightElementGeom_ != NULL && faceType_==INTERNAL)
        {
           rightPG->getGlobalFaceNodeIndices(localFaceNumberRight_,globalNodeNrsR);
        }
        else
        {
                // ~OC~ Otherwise just duplicate, the trafo will not be used
                // \bug: WTF is trafo?
                // (S.N) I bet, trafo is something connected to peridocity. This line will cause a creation of a boundary which has equal sets of PhysicalPoints (permutation included) from left and right. Thus the codimmappings are set accordingly.
                //
            globalNodeNrsR = globalNodeNrsL;
        }
            //  ~OC~
            //  Now we have a look at the global node indices.  The only use
            //  that is made of physical space information by the face is to
            //  generate the RefFace2RefFaceMapping.  If the two
            //  FaceDescriptors gave different sets of these indices then
            //  this is becoming a periodic face on one of the
            //  boundaries. In case the
            //  sets are identical, the ReferenceGeometry determines from
            //  the permutation of the index vectors which
            //  RefFace2RefFaceMapping needs to be used.

            //: The next to lines creating 2 std::sets, filling them with globalNodesL and globalNodesR. The sets are created via ctr which takes a to pointers and insert the data inbetween.
        cout<<"Left {";
        for(unsigned int i=0; i<globalNodeNrsL.size();++i)
        {
            cout<< globalNodeNrsL[i]<<" ";
        }
        cout<<"}"<<endl;

        cout<<"Right {";
        for(unsigned int i=0; i<globalNodeNrsR.size();++i)
        {
            cout<< globalNodeNrsR[i]<<" ";
        }
        cout<<"}"<<endl;

        SetOfGlobalNodes sL(globalNodeNrsL.data(), globalNodeNrsL.data() + globalNodeNrsL.size());
        SetOfGlobalNodes sR(globalNodeNrsR.data(), globalNodeNrsR.data() + globalNodeNrsR.size());


        if (sL == sR)// left set of global nodes for the face identical to right one. No periodicity detected!
        {
                // ~OC~
                // Equal sets algorithm checks all permutation too;
                // Find out which face2face-mapping index is needed
         //   cout << "The reference geometry " << endl;
         //   cout << this->referenceGeometryPtr() << endl;
            faceToFaceMapIndex_ = this->getReferenceGeometry()->
            getCodim0MappingIndex(globalNodeNrsL, globalNodeNrsR);
        }
        else
        {
//-MTJ-start--------------
//#ifdef MTJ
            //******************************
            // the connected elements are either refined or this is a periodic BC face
            //******************************

            // get intersection of left and right nodes set
            SetOfGlobalNodes commNodes;
            std::set_intersection( sL.begin(), sL.end(), sR.begin(), sR.end(), std::inserter( commNodes, commNodes.begin() ) );

            if (commNodes.size() > 0)
            // the connected elements share some nodes
            {
                SetOfGlobalNodes diffNodesL;
                std::set_difference( sL.begin(), sL.end(), sR.begin(), sR.end(), std::inserter(diffNodesL, diffNodesL.end() ) );

                SetOfGlobalNodes diffNodesR;
                std::set_difference( sR.begin(), sR.end(), sL.begin(), sL.end(), std::inserter(diffNodesR, diffNodesR.end() ) );

                bool foundCollinear = false;
                for(unsigned int i=0; i<globalNodeNrsL.size(); ++i)
                {
                  PointPhysical<DIM> ppR;
                  rightPG->getGlobalNodeCoordinates(globalNodeNrsR[i], ppR);

                  SetOfGlobalNodes::iterator itFound = commNodes.find(globalNodeNrsR[i]);
                  if (itFound == commNodes.end())
                  {
                    // do collinear test for all possible pairs
                    for (SetOfGlobalNodes::iterator itCommon=commNodes.begin(); (itCommon!=commNodes.end()); ++itCommon)
                    {
                      PointPhysical<DIM> pp0;
                      leftPG->getGlobalNodeCoordinates(*itCommon, pp0);

                      for (SetOfGlobalNodes::iterator itDiffL=diffNodesL.begin(); (itDiffL!=diffNodesL.end()); ++itDiffL)
                      {
                        if (diffNodesR.find(globalNodeNrsR[i]) == diffNodesR.end()) continue;

                        PointPhysical<DIM> ppL;
                        leftPG->getGlobalNodeCoordinates(*itDiffL, ppL);

                        PointPhysical<DIM> dL(ppL-pp0);
                        PointPhysical<DIM> dR(ppR-pp0);

                        double ratio = 0.;
                        unsigned int d;
                        for (d=0; d<DIM; ++d)
                        {
                          if (std::abs(dL[d]) > SmallerDoubleThanMinimalSizeOfTheMesh)
                          {
                            ratio = dR[d]/dL[d];
                            break;
                          }
                        }

                        bool isCollinear = true;
                        for (;d<DIM; ++d)
                        {
                          if (std::abs(dL[d]) > SmallerDoubleThanMinimalSizeOfTheMesh)
                          {
                            if (std::abs(dR[d]/dL[d] - ratio) > SmallerDoubleThanMinimalSizeOfTheMesh)
                            {
                              isCollinear = false;
                              break;
                            }
                          }
                        }
                        
                        if (isCollinear) 
                        {
                          // replace nodes on the right list
                          diffNodesR.erase(diffNodesR.find(globalNodeNrsR[i]));
                          globalNodeNrsR[i] = *itDiffL;
                          foundCollinear = true;
                        }
                      }  // for itDiffL
                    }  // for itCommon
                  }  // if itFound
                }  // for i
                
                if (!foundCollinear) throw "Face: incorrect pairs!";
            } // end if commNodes.size() > 0
            else
            {
//#endif
//-MTJ-end--------------
                // ~OC~
            VectorOfLocalNodes localNodeNrsL;
            VectorOfLocalNodes localNodeNrsR;
            leftPG->getLocalFaceNodeIndices(localFaceNumberLeft_, localNodeNrsL);
            rightPG->getLocalFaceNodeIndices(localFaceNumberRight_, localNodeNrsR);

            unsigned int periodicDim = Geometry::MaxInteger; // initialize to large value

            PointPhysicalT pointPhysical;

            std::vector<PointPhysicalT> projectedPointsL; // create empty!
            std::vector<PointPhysicalT> projectedPointsR;

            for (unsigned int i = 0; i < localNodeNrsL.size(); ++i)
            {
                leftPG->getNodeCoordinates(localNodeNrsL[i], pointPhysical);

                projectedPointsL.push_back(pointPhysical);

                rightPG->getNodeCoordinates(localNodeNrsR[i], pointPhysical);

                projectedPointsR.push_back(pointPhysical);
            }

                // We want to find which direction is the periodic one.
            unsigned int test[DIM];

            for (unsigned int d = 0; d < DIM; ++d) { test[d] = 0; }

            pointPhysical = projectedPointsL[0]; //Find out which one is peridoci by comparing all coordinates of one point to other points, for all dimension. If dimension is the same for every Point, than it is a face that dimension in question.
            for (unsigned int i = 0; i < localNodeNrsL.size(); ++i)
            {
                for (unsigned int d = 0; d < DIM; ++d)
                {
                    if (std::abs(projectedPointsL[i][d]-pointPhysical[d]) < SmallerDoubleThanMinimalSizeOfTheMesh)
                    {
                        test[d] += 1;
                    }
                }
            }
                //check if for that dimension is fullhouse
            for (unsigned int d = 0; d < DIM; ++d)
            {
                if (test[d] == localNodeNrsL.size())
                {
                    periodicDim = d;
                }
            }
                //  periodicDim should be our dimension, otherwise error!
            if (periodicDim==Geometry::MaxInteger)
                cout << "Shit happened. Trained monkeys are on the way to fix the problem, probably." << std::endl;

            PhysicalPointOnTheFaceT ppL;
            PhysicalPointOnTheFaceT ppR;

                // next we eliminate the periodic dimension from a Point coordinates & a
            for(unsigned int i = 0; i < globalNodeNrsR.size(); ++i)
            {
                for (unsigned int d = 0; d < periodicDim; ++d)
                {
                    ppR[d] = projectedPointsR[i][d];
                }
                for (unsigned int d = periodicDim+1; d < DIM; ++d)
                {
                    ppR[d-1] = projectedPointsR[i][d];
                }

                for (unsigned int j = 0; j < globalNodeNrsL.size(); ++j)
                {
                    for (unsigned int d = 0; d < periodicDim; ++d)
                    {
                        ppL[d] = projectedPointsL[j][d];
                    }
                    for (unsigned int d = periodicDim+1; d < DIM; ++d)
                    {
                        ppL[d-1] = projectedPointsL[j][d];
                    }
                    if (ppL == ppR)//check if now they are the same, then overwrite the Right nodeList, with LeftNodeList
                    {
                        globalNodeNrsR[i] = globalNodeNrsL[j];
                    }
                }
            }
//-MTJ-start--------------
//#ifdef MTJ
            }
//#endif
//-MTJ-end--------------
            faceToFaceMapIndex_ = this->getReferenceGeometry()->getCodim0MappingIndex(globalNodeNrsL, globalNodeNrsR);

//-MTJ-start--------------
//#ifdef MTJ
            //------- 
            // Filling refinement matrix with a correct value.
            // Initially, refinement matrix consists only reorientation transformation 
            // matrix, i.e. transformation from left face to right face.
            // Later on, it will be updated whenever the left and/or the right
            // elements are refined.

            faceToFaceMapMatrix_.resize(DIM,DIM);
            for (unsigned int j=0; j<DIM-1; ++j)
            {
                PointReference<DIM-1> pRefFace;
//                 pRefFace = 0.;  
                for (unsigned int i=0; i<DIM-1; ++i)
                {
                  pRefFace[i]=0.;
                }
                pRefFace[j]=1.;
                mapRefFaceToRefFace(pRefFace, pRefFace);

                for (unsigned int i=0; i<DIM-1; ++i)
                {
                  faceToFaceMapMatrix_(i,j) = pRefFace[i];
                }
                // the last collumn
                faceToFaceMapMatrix_(DIM-1,j) = 0.;
            }

            // the last row
            for (unsigned int j=0; j<DIM-1; ++j)
            {
              faceToFaceMapMatrix_(DIM-1,j) = 0.;
            }
            // the last collumn of the last row
            faceToFaceMapMatrix_(DIM-1,DIM-1) = 1.;
            //------- end-Filling refinement matrix

//#endif
//-MTJ-end--------------

        }
    }

        //! Ctor for boundary faces.
        template<unsigned int DIM>
    FaceGeometry<DIM>::FaceGeometry(ElementGeometryT* ptrElemL, const LocalFaceNrType& localFaceNumL, const FaceType& boundaryLabel):
    leftElementGeom_(ptrElemL),
    rightElementGeom_(NULL),
    localFaceNumberLeft_(localFaceNumL),
    localFaceNumberRight_(Geometry::MaxInteger),
    faceToFaceMapIndex_(0),
    faceType_(boundaryLabel)
    { }
        /// The referenceGeometry is returned. It is taken from left element, to always ensure its existence
        // should use the same interface as in the Element!
    template<unsigned int DIM>
    const ReferenceGeometry<DIM-1>*
    FaceGeometry<DIM>::getReferenceGeometry()const
    {
        return leftElementGeom_->getReferenceGeometry()->getCodim1ReferenceGeometry(localFaceNumberLeft_);
    }
 
        
//        //! Return the unique number of this face.
//        const IDType& id() const
//    {
//                return _id;
//    }


        /*! \brief Return a wrapped elemental function which can be evaluated at
         *  reference face points. In this case the function is defined on the
         *  left element. */
//        template <class FType>
//        ElementDataTraceL<dim, FType, Face>
//        transform2RefFaceL(FType f) const
//    {
//                return ElementDataTraceL<dim, FType, Face>(//this,
//                                                   f);
//    }
        
        /*! \brief Return a wrapped elemental function which can be evaluated at
         *  reference face points. In this case the function is defined on the
         *  right element. */
//        template <class FType>
//        ElementDataTraceR<dim, FType, Face>
//        transform2RefFaceR(FType f) const
//    {
//                return ElementDataTraceR<dim, FType, Face>(//this,
//                                                   f);
//    }
        
        /*! Map a point in coordinates of the reference geometry of the face to
         *  the reference geometry of the left (L) element. */
    template<unsigned int DIM>
    void
    FaceGeometry<DIM>::mapRefFaceToRefElemL(const ReferencePointOnTheFaceT& pRefFace,
                                     ReferencePointT& pRefEl) const
    {
        leftElementGeom_->getReferenceGeometry()->getCodim1MappingPtr(localFaceNumberLeft_)->transform(pRefFace, pRefEl);
    }
        
        /*! Map a point in coordinates of the reference geometry of the face to
         *  the reference geometry of the right (R) element. */
    template<unsigned int DIM>
    void
    FaceGeometry<DIM>::mapRefFaceToRefElemR(const ReferencePointOnTheFaceT& pRefFace,
                                     ReferencePointT& pRefEl) const
    {
        // In the L function we have assumed the point pRefFace to be
        // given in coordinates of the system used by the reference face
        // on the L side. That means that now to make sure that the
        // point transformed from the right side of the face onto the
        // right element is the same as the one on the left side of the
        // face, we have to use the refFace2RefFace mapping.
        
        ReferencePointOnTheFaceT pOtherSide(pRefFace);
        
//-MTJ-start--------------
//#ifdef MTJ
        PointReference<DIM-1> pRefFaceR = pRefFace;

        // get reference face coordinates on the right side,
        // apply refinement mapping Q: p = Q * p
        for (unsigned int j=0; j<DIM-1; ++j)
        {
            pOtherSide[j] = 0.;

            for (unsigned int i=0; i<DIM-1; ++i)
                pOtherSide[j] += (faceToFaceMapMatrix_(j,i)*pRefFace[i]);
            
            pOtherSide[j] += faceToFaceMapMatrix_(j,DIM-1);
        }

        // get reference element coordinates
        rightElementGeom_->getReferenceGeometry()->getCodim1MappingPtr(localFaceNumberRight_)->transform(pOtherSide, pRefEl);
//#else
//-MTJ-end--------------
        mapRefFaceToRefFace(pRefFace, pOtherSide);
        rightElementGeom_->getReferenceGeometry()->getCodim1MappingPtr(localFaceNumberRight_)->transform(pOtherSide, pRefEl);
//-MTJ-start--------------
//#endif
//-MTJ-end--------------

    }
        
        /*! Map from reference face coordinates on the left side to those on the
         *  right side. */
        template<unsigned int DIM>
    void
    FaceGeometry<DIM>::mapRefFaceToRefFace(const ReferencePointOnTheFaceT& pIn,
                                     ReferencePointOnTheFaceT& pOut) const
    {
        getReferenceGeometry()->getCodim0MappingPtr(faceToFaceMapIndex_)->transform(pIn, pOut);
    }
        
//        typedef auto_ptr<const Ref2RefSpaceMapping<dim-1, dim> >
//        RefFace2RefElementMapping;
        
    //! Return a Mapping (not pointer or reference! Ok, wrapped by auto_ptr)
    template<unsigned int DIM>
    typename FaceGeometry<DIM>::RefFaceToRefElementMapping
    FaceGeometry<DIM>::refFaceToRefElemMapL() const
    {
        return RefFaceToRefElementMapping(leftElementGeom_->getReferenceGeometry()->getCodim1MappingPtr(localFaceNumberLeft_));
    }
    
    //! Return a mapping to the right reference element.
    template<unsigned int DIM>
    typename FaceGeometry<DIM>::RefFaceToRefElementMapping 
    FaceGeometry<DIM>::refFaceToRefElemMapR() const
    {
        const MappingReferenceToReference<DIM-1, DIM-1>* const m1Ptr = this->getReferenceGeometry()->getCodim0MappingPtr(faceToFaceMapIndex_);
        const MappingReferenceToReference<DIM-1, DIM>* const m2Ptr =  leftElementGeom_->getReferenceGeometry()->getCodim1MappingPtr(localFaceNumberRight_);
        
        return RefFaceToRefElementMapping(new ConcatenatedMapping<DIM-1, DIM-1, DIM>(*m1Ptr, *m2Ptr));
    }
    
    /*!Compute the normal vector at a given point (in face coords).
     
     Faces turn up in DG discretizations as domains for some of the
     integrals. For these integrals, the face must be able to provide the
     geometric information. This is completely included in the normal
     vector, which gives
     <UL>
     <LI> the direction of the normal vector oriented from left (L) to
     right (R) element; that way, for a boundary face, it is an external
     normal vector;
     <LI> the transformation of the integration element between reference
     face and physical space: this scalar is given by the 2-norm of the
     returned normal vector.
     </UL> */
    template<unsigned int DIM>
    void
    FaceGeometry<DIM>::getNormalVector(const ReferencePointOnTheFaceT& pRefFace, PointPhysicalT& v) const
    {
        // first Jacobian (mapping reference face -> reference element)
        Jacobian<DIM-1, DIM>    j1;
        
        leftElementGeom_->getReferenceGeometry()->getCodim1MappingPtr(localFaceNumberLeft_) // this is the refFace2RefElemMap
        ->calcJacobian(pRefFace, j1);
        
        // second Jacobian (mapping reference element -> phys. element),
        // for this we first need the points coordinates on the
        // reference element
        ReferencePointT pRefElement;
        mapRefFaceToRefElemL(pRefFace, pRefElement);
        
        Jacobian<DIM, DIM> j2;
        
        leftElementGeom_->calcJacobian(pRefElement, j2);

        Jacobian<DIM-1, DIM> j3;
        
        j2.multiplyJacobiansInto(j1, j3);
        
        j3.computeWedgeStuffVector(v);
        
        double det = j2.determinant();
                
        //sgn==(x > 0) - (x < 0)
        v *= ((det>0)-(det<0)) *
        OutwardNormalVectorSign<DIM>(leftElementGeom_->getReferenceGeometry()->getCodim1MappingPtr(localFaceNumberLeft_));
    }

//-MTJ-start--------------
//#ifdef MTJ
    template<unsigned int DIM>
    void FaceGeometry<DIM>::copyFromParent(const Face<DIM>& fa)
        {
            faceToFaceMapIndex_ = fa.faceToFaceMapIndex_;
            faceToFaceMapMatrix_ = fa.faceToFaceMapMatrix_;
        }

    template<unsigned int DIM>
    void FaceGeometry<DIM>::invertFaceToFaceMapMatrix()
        {

            // invert the matrix
            double a=faceToFaceMapMatrix_(0,0);
            double b=faceToFaceMapMatrix_(0,1);
            double c=faceToFaceMapMatrix_(0,2);
            double d=faceToFaceMapMatrix_(1,0);
            double e=faceToFaceMapMatrix_(1,1);
            double f=faceToFaceMapMatrix_(1,2);
            double g=faceToFaceMapMatrix_(2,0);
            double h=faceToFaceMapMatrix_(2,1);
            double k=faceToFaceMapMatrix_(2,2);
            
            double det=a*(e*k-f*h) - b*(k*d-f*g) + c*(d*h-e*g);
            
            faceToFaceMapMatrix_(0,0) = (e*k-f*h)/det;
            faceToFaceMapMatrix_(1,0) = (f*g-d*k)/det;
            faceToFaceMapMatrix_(2,0) = (d*h-e*g)/det;
            faceToFaceMapMatrix_(0,1) = (c*h-b*k)/det;
            faceToFaceMapMatrix_(1,1) = (a*k-c*g)/det;
            faceToFaceMapMatrix_(2,1) = (g*b-a*h)/det;
            faceToFaceMapMatrix_(0,2) = (b*f-c*e)/det;
            faceToFaceMapMatrix_(1,2) = (c*d-a*f)/det;
            faceToFaceMapMatrix_(2,2) = (a*e-b*d)/det;
        }

    // Q_new = R * Q_old * L
    template<unsigned int DIM>
    void FaceGeometry<DIM>::recalculateRefinementMatrix(MatrixT& Lmat, MatrixT& Rmat)
        {
            MatrixT aa(faceToFaceMapMatrix_.getNRows(),faceToFaceMapMatrix_.getNCols());

//             Base::matrix_prod(faceToFaceMapMatrix_, Lmat, aa);
//             Base::matrix_prod(Rmat, aa, faceToFaceMapMatrix_);
        }
    
    template<unsigned int DIM>
    void FaceGeometry<DIM>::printRefMatrix() const
        {
            std::cout << "("<< faceToFaceMapIndex_ << ") refMatrix: " << faceToFaceMapMatrix_ << "\n";
        }
//#endif
//-MTJ-end--------------
};