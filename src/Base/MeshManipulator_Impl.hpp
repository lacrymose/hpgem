#ifndef meshmanipulator_impl_h_
#define meshmanipulator_impl_h_
//***********************************************************************************************************************
//***********************************************************************************************************************
//***********************************************************************************************************************
namespace Base
{
    //This is a small struct used for storing the halfFaces that are used 
    ///Half faces are used by the faceFactory. These are face which only know one element they are associated with


    
    
    bool compareHalfFace(HalfFaceDescription first, HalfFaceDescription second);

    template<unsigned int DIM>
    MeshManipulator<DIM>::MeshManipulator(const ConfigurationData* config, bool xPer, bool yPer, bool zPer, unsigned int orderOfFEM, unsigned int idRangeBegin):
        configData_(config),
        periodicX_(xPer),
        periodicY_(yPer),
        periodicZ_(zPer),
        counter_(idRangeBegin),
        activeMeshTree_(0), 
        numMeshTree_(0)
    {
        cout << "******Mesh creation started!**************"<<endl;
        
        
        for (unsigned int i=0;i<DIM;++i)
        {
            if (i==0)
                cout <<"Boundries: " <<(periodicX_? "Periodic  ":"Solid Wall")<<" in X direction"<<endl;
            if (i==1)
                cout <<"Boundries: " << (periodicY_? "Periodic ":"Solid Wall")<<" in Y direction"<<endl;
            if (i==2)
                cout <<"Boundries: " << (periodicZ_? "Periodic ":"Solid Wall")<<" in Z direction"<<endl;
        }
        //createDefaultBasisFunctions(orderOfFEM);
        createNewMeshTree();
        cout << "******Mesh creation is finished!**********"<<endl;
    }

    template<unsigned int DIM>
    MeshManipulator<DIM>::MeshManipulator(const MeshManipulator& other):
                 configData_(other.configData_),
                 counter_(other.counter_),
                 elements_(other.elements_),
                 faces_(other.faces_),
                 points_(other.points_),
                 periodicX_(other.periodicX_),
                 periodicY_(other.periodicY_),
                 periodicZ_(other.periodicZ_),
                 meshMover_(other.meshMover_),
                 defaultSetOfBasisFunctions_(other.defaultSetOfBasisFunctions_),
                 collBasisFSet_(other.collBasisFSet_),
                 activeMeshTree_(other.activeMeshTree_),
                 numMeshTree_(other.numMeshTree_),
                 vecOfElementTree_(other.vecOfElementTree_),
                 vecOfFaceTree_(other.vecOfFaceTree_)
    {

    }

    template<unsigned int DIM>
    MeshManipulator<DIM>::~MeshManipulator()
    {
        for (typename ListOfElementsT::iterator it=elements_.begin(); it!=elements_.end();++it)
        {
            ElementT* el = *it;
            delete el;
        }
        
        for (typename CollectionOfBasisFunctionSets::iterator bit=collBasisFSet_.begin(); bit!=collBasisFSet_.end();++bit)
        {
            BasisFunctionSetT* bf = *bit;
            delete bf;
        }
        
        delete meshMover_;
        delete defaultSetOfBasisFunctions_;


        // Kill all elements in all mesh-tree
        while (!vecOfElementTree_.empty())
        {
            delete vecOfElementTree_.back();
            vecOfElementTree_.pop_back();
        }
    }

    template<unsigned int DIM>
    Base::Element<DIM>* 
    MeshManipulator<DIM>::addElement(const VectorOfPointIndicesT& globalNodeIndexes)
    {
        
        unsigned int numOfUnknowns       = configData_->numberOfUnknowns_;
        unsigned int numOfTimeLevels     = configData_->numberOfTimeLevels_;
        unsigned int numOfBasisFunctions = configData_->numberOfBasisFunctions_;
        
        unsigned int id                 = counter_++;
        
        
        
        ElementT*  myElement = new ElementT(globalNodeIndexes, collBasisFSet_[0], points_, numOfUnknowns, numOfTimeLevels, numOfBasisFunctions, id);
        
        
            //cout << "Element= "<<*myElement;
        elements_.push_back(myElement);
        
        return myElement;
    }

    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::move()
    {
        for (int i=0;i<points_.size();i++)
        {
            if (meshMover_!=NULL)
            {
                meshMover_->movePoint(points_[i]);
            }
        }
    }

    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::setMeshMover(const MeshMoverBase<DIM>* meshMover)
    {
        meshMover_ = meshMover;
    }

    template<unsigned int DIM>
    bool 
    MeshManipulator<DIM>::addFace(ElementT* leftElementPtr, unsigned int leftElementLocalFaceNo, ElementT* rightElementPtr, unsigned int rightElementLocalFaceNo, const Geometry::FaceType& faceType)
    {
        //std::cout << "-----------------------\n";
        
        //std::cout << "{Left Element" << (*leftElementPtr) <<" , FaceIndex=" <<leftElementLocalFaceNo<<"}"<<endl;
        if (rightElementPtr!=NULL)
        {
            //std::cout << "{Right Element" << (*rightElementPtr) << " , FaceIndex=" <<rightElementLocalFaceNo<<"}"<<endl;
            
            Face<DIM>* face = new Face<DIM>(leftElementPtr, leftElementLocalFaceNo, rightElementPtr, rightElementLocalFaceNo);
            
            faces_.push_back(face);
        }
        else 
        {
            //std::cout << "This is a boundary face" << std::endl;
            Face<DIM>* bFace = new Face<DIM>(leftElementPtr, leftElementLocalFaceNo, faceType);
            faces_.push_back(bFace);
        }
        // std::cout << "-----------------------\n";
    }

    template<unsigned int DIM>
    void 
    MeshManipulator<DIM>::outputMesh(ostream& os)const
    {
        for (int i=0;i<points_.size();i++)
        {
            os<<"Node " <<i<<" "<<points_[i]<<endl;
        }
        
        int elementNum=0;
        
        for (typename ListOfElementsT::const_iterator cit=elements_.begin(); cit !=elements_.end(); ++cit)
        {
            os << "Element " <<elementNum <<" " <<*(*cit)<<endl;
            elementNum++;
        }
        
        int faceNum=0;
        for (typename ListOfFacesT::const_iterator cit=faces_.begin(); cit !=faces_.end(); ++cit)
        {
            /// \bug need at output routine for the face to test this.
            // os << "Face " <<faceNum <<" " <<(*cit)<<endl;
            faceNum++;
        }

    }

    template<unsigned int DIM>
    void 
    MeshManipulator<DIM>::createRectangularMesh(const PointPhysicalT& BottomLeft, const PointPhysicalT& TopRight, const  VectorOfPointIndicesT& linearNoElements)
    {
        if (linearNoElements.size() != DIM)
        {
            cout << "The number of Linear Intervals has to map the size of the problem and current it does not"<<endl;
            throw(10);
        }
        
        //Stage 1 : Precompute some required values;
        ///////
        
        //This store the size length of the domain i.e. it is DIM sized vector
        PointPhysicalT delta_x(DIM);
        
        for (int i=0;i<DIM;i++)
        {
            delta_x[i]=(TopRight[i]-BottomLeft[i])/(linearNoElements[i]);
        }

        //This stores the number of nodes in each coDIMension i.e. if you have 2 by 2 element it is 3 nodes 
        std::vector<unsigned int> numOfNodesInEachSubspace(DIM), numOfElementsInEachSubspace(DIM);
        
        numOfNodesInEachSubspace[0]=1;
        numOfElementsInEachSubspace[0]=1;
        
        //This will be the total number of nodes required in the problem
        unsigned int totalNumOfNodes,totalNumOfElements, verticesPerElement;
        
        totalNumOfNodes=(linearNoElements[0]+1);
        
        totalNumOfElements=(linearNoElements[0]);
        
        verticesPerElement=2;
        int powerOf2;
        
        for (unsigned int iDIM=1; iDIM<DIM;++iDIM)
        {
            totalNumOfNodes*=(linearNoElements[iDIM]+1);
            totalNumOfElements*=(linearNoElements[iDIM]);
            verticesPerElement*=2;
            
            numOfElementsInEachSubspace[iDIM]=numOfElementsInEachSubspace[iDIM-1]*(linearNoElements[iDIM-1]);
            numOfNodesInEachSubspace[iDIM]=numOfNodesInEachSubspace[iDIM-1]*(linearNoElements[iDIM-1]+1);
            
        }
        
        //temp point for storing the node locations
        PointPhysicalT x;
        
        
        ///Stage 2 : Create the nodes
        //Now loop over all the nodes and calculate the coordinates for rach DIMension (this makes the algorithm independent of DIMension
        for (unsigned int nodeIndex=0; nodeIndex<totalNumOfNodes; ++nodeIndex)
        {
            unsigned int nodeIndexRemain=nodeIndex;
            
            
            
            for (int iDIM=DIM-1;iDIM>-1;--iDIM)
            {
                x[iDIM] = BottomLeft[iDIM] + (nodeIndexRemain/numOfNodesInEachSubspace[iDIM]*delta_x[iDIM]);
                nodeIndexRemain %=numOfNodesInEachSubspace[iDIM];
            }
        
            //actally add the point
            points_.push_back(x);

        }
            
            
            
        //Stage 3 : Create the elements
           
        VectorOfElementPtrT       tempElementVector(totalNumOfElements);
        
        std::vector<unsigned int> elementNdId(DIM), vertexNdId(DIM), globalVertexID(verticesPerElement);
        
        //This will store a two array, which goes elementNumber and the n-DIMensional coordinate of that element. This is only used for the face creation (step 4).
        //std::vector<std::vector<unsigned int> > allElementNdId;
        //allElementNdId.resize(totalNumOfElements);
        
      
        //elementNdId is DIM coordinate of the bottom left node i.e. in two (0,0), (1,0) ,(2,0) ... etc are the first three (if at least three elements in x)
        for (unsigned int elementIndex=0; elementIndex<totalNumOfElements; ++elementIndex)
        {
            unsigned int numElementsRemaining=elementIndex;
            
            for (int iDIM=DIM-1; iDIM>-1;--iDIM)
            {
                elementNdId[iDIM]=numElementsRemaining/numOfElementsInEachSubspace[iDIM];
                numElementsRemaining %= numOfElementsInEachSubspace[iDIM];
            }

            
            // vertexNdId are the DIM coordinate of each vertex in the element with vertexNdId[0] being the bottom left
            for (unsigned int i=0; i<verticesPerElement; ++i)
            {
                powerOf2 = 1;
                for (unsigned int iDIM=0; iDIM<DIM; ++iDIM)
                {
                    vertexNdId[iDIM] = elementNdId[iDIM] + ((i & powerOf2) !=0);
                    powerOf2 *=2;
                }
                globalVertexID[i]=vertexNdId[0];
                    
                //Now map to the one DIMensional global ID
                for (unsigned int iDIM=1;iDIM<DIM;++iDIM)
                {
                    globalVertexID[i] += vertexNdId[iDIM]*numOfNodesInEachSubspace[iDIM];
                }
            }
        
            tempElementVector[elementIndex]=addElement(globalVertexID);
        }
        
        
    //Stage 4 : Create the faces
      
        switch (DIM)
        {
            case 1:
                rectangularCreateFaces1D(tempElementVector, linearNoElements);
                break;
                
            case 2:
                rectangularCreateFaces2D(tempElementVector, linearNoElements);
                break;
                
            case 3:
                rectangularCreateFaces3D(tempElementVector, linearNoElements);
                break;
           
            default:
                throw("Face generator not implemented in this DIMension");
                
        }//end case
        
       
    }


    template<unsigned int DIM>
    void MeshManipulator<DIM>::rectangularCreateFaces1D(VectorOfElementPtrT& tempElementVector, const VectorOfPointIndicesT& linearNoElements)
    {
        
        //First the internal faces
        for (int i=0;i<linearNoElements[0]-1;i++)
        {
            addFace(tempElementVector[i],1,tempElementVector[i+1],0);
        }

        if (periodicX_==1)
            
        {
            addFace(tempElementVector[0],0,tempElementVector[linearNoElements[0]-1],1);
        }   
        else 
        {
            addFace(tempElementVector[0],0,NULL,0);
            addFace(tempElementVector[linearNoElements[0]-1],1,NULL,0);
        }
        
    }


    template<unsigned int DIM>
    void MeshManipulator<DIM>::rectangularCreateFaces2D(VectorOfElementPtrT& tempElementVector, const VectorOfPointIndicesT& linearNoElements)
    {
        
        //first do the faces with normals pointing in the x-direction
        Geometry::FaceType xFace = Geometry::WALL_BC;
        unsigned int index;
        
        for (int j=0; j<linearNoElements[1]; j++)
        {
            for (int i=0; i<linearNoElements[0]-1;i++)
            {
                
                index=i+j*linearNoElements[0];
                addFace(tempElementVector[index],2,tempElementVector[index+1],1);
                
            }
            
            
            index=j*linearNoElements[0];
            if (periodicX_==1)
            {
                addFace(tempElementVector[index],1,tempElementVector[index+linearNoElements[0]-1],2);
            }
            else 
            {
                //Left bounday face
                cout<<"index1="<<index<<endl;
                addFace(tempElementVector[index],1,NULL,0);
                
                //Right boundary face
                index =index+linearNoElements[0]-1;
                cout<<"index2="<<index<<endl;
                addFace(tempElementVector[index],2,NULL,0);
            }
        }
        
        //now do the faces with normals pointing in the y-direction
        Geometry::FaceType yFace=Geometry::WALL_BC;
        for (int j=0; j<linearNoElements[0]; j++)
        {
            for (int i=0; i<linearNoElements[1]-1;i++)
            {
                index=j+i*linearNoElements[0];
                //cout<<"index3="<<index<<endl;
                //cout<<"index3.5="<<index+linearNoElements[0]<<endl;
                
                addFace(tempElementVector[index],0,tempElementVector[index+linearNoElements[0]],3);
                
            }
            
            index=j;
            
            if (periodicY_==1)
            {
                
                addFace(tempElementVector[index],0,tempElementVector[index+(linearNoElements[1]-1)*linearNoElements[0]],3);
                
            }
            else 
            {
                
                //Bottom boundary face
                
               // cout<<"index4="<<index<<endl;
                addFace(tempElementVector[index],0,NULL,0);
                
                //Top boundary face
                index =index+(linearNoElements[1]-1)*linearNoElements[0];
                
                //cout<<"index5="<<index<<endl;
                addFace(tempElementVector[index],3,NULL,0);
            }
            
        }
        
        
    }

    template<unsigned int DIM>
    void MeshManipulator<DIM>::rectangularCreateFaces3D(VectorOfElementPtrT& tempElementVector, const VectorOfPointIndicesT& linearNoElements)
    {
        
        unsigned int index;
        //first do the faces in x-direction
        //counter in z
        for(int k=0;k<linearNoElements[2];k++)
        {
            //counter in y
            for (int j=0; j<linearNoElements[1]; j++)
            {
                //counter in x
                for (int i=0; i<linearNoElements[0]-1;i++)
                {
                
                    index=i+j*linearNoElements[0]+k*linearNoElements[0]*linearNoElements[1];
                    addFace(tempElementVector[index],3,tempElementVector[index+1],2);
                
                }//end loop over x
            
            
                index=j*linearNoElements[0]+k*linearNoElements[0]*linearNoElements[1];
                if (periodicX_==1)
                {
                    addFace(tempElementVector[index],2,tempElementVector[index+linearNoElements[0]-1],3);
                }
                else 
                {
                
                    //Left boundary
                
                   // cout<<"index1="<<index<<endl;
                    addFace(tempElementVector[index],2,NULL,0);
                
                    //Right boundary face
                    index =index+linearNoElements[0]-1;
                   // cout<<"index2="<<index<<endl;
                    addFace(tempElementVector[index],3,NULL,0);
                } // end boundary cases
                
            } // end loop over y
        } //end loop over z
        
        
        //Now do the faces pointing in the y-direction
        //count in z
        for(int k=0;k<linearNoElements[2];k++)
        {
            //counter in x
            for (int j=0; j<linearNoElements[0]; j++)
            {
                //counter in y
                for (int i=0; i<linearNoElements[1]-1;i++)
                {
                    
                    index=j+i*linearNoElements[0]+k*linearNoElements[0]*linearNoElements[1];
                    addFace(tempElementVector[index],4,tempElementVector[index+linearNoElements[0]],1);
                    
                }//end loop over x
                
                
                index=j+k*linearNoElements[0]*linearNoElements[1];
                if (periodicY_==1)
                {
                 //   cout << index << " , " << index+(linearNoElements[1]-1)*linearNoElements[0] << endl;
                    addFace(tempElementVector[index],1,tempElementVector[(index+(linearNoElements[1]-1)*linearNoElements[0])],4);
                }
                else 
                {
                    
                    //front bounday face
                    
                   // cout<<"index1="<<index<<endl;
                    addFace(tempElementVector[index],1,NULL,0);
                    
                    //Back boundary face
                    index =index+(linearNoElements[1]-1)*linearNoElements[0];
                 //   cout<<"index2="<<index<<endl;
                    addFace(tempElementVector[index],4,NULL,0);
                } // end boundary cases
                
            } // end loop over y
        } //end loop over z
        
        //Now finally do the face in the z-direction
        
        //count in x direction 
        for(int k=0;k<linearNoElements[0];k++)
        {
            //counter in y
            for (int j=0; j<linearNoElements[1]; j++)
            {
                //counter in z
                for (int i=0; i<linearNoElements[2]-1;i++)
                {
                    
                    index=k+j*linearNoElements[0]+i*linearNoElements[0]*linearNoElements[1];
                    addFace(tempElementVector[index],5,tempElementVector[index+linearNoElements[0]*linearNoElements[1]],0);
                    
                }//end loop over x
                
                index=k+j*linearNoElements[0];
                if (periodicZ_==1)
                {
                    addFace(tempElementVector[index],0,tempElementVector[index+(linearNoElements[2]-1)*linearNoElements[1]*linearNoElements[0]],5);
                }
                else 
                {
                    
                    //bottom boundary
                    //cout<<"index1="<<index<<endl;
                    addFace(tempElementVector[index],0,NULL,0);
                    
                    //Top boundary face
                    index =index+(linearNoElements[2]-1)*linearNoElements[1]*linearNoElements[0];
                    //cout<<"index2="<<index<<endl;
                    addFace(tempElementVector[index],5,NULL,0);
                } // end boundary cases
                
            } // end loop over y
        } //end loop over z
        
    }
 
//createTrianglularMesh follows the same structure as createRectangularMesh. Where createRectangularMesh makes rectangular elements, createTrianglularMesh splits the elements into a partition of triangles.
template<unsigned int DIM>
void MeshManipulator<DIM>::createTriangularMesh(PointPhysicalT BottomLeft, PointPhysicalT TopRight, const VectorOfPointIndicesT& linearNoElements)
{
    //Stage 0 : Check for required requirements
  
    if (linearNoElements.size() != DIM)
    {
	cout << "The number of Linear Intervals has to map the size of the problem and current it does not" << endl;
	throw(10);
    }
  
    if(DIM==3 && periodicX_==1 && linearNoElements[0]%2==1)
    {
	throw "The 3D triangular grid generator can't handle an odd amount of elements in the periodic dimension X";
    }
  
    if(DIM==3 && periodicY_==1 && linearNoElements[1]%2==1)
    {
	throw "The 3D triangular grid generator can't handle an odd amount of elements in the periodic dimension Y";
    }
  
    if(DIM==3 && periodicZ_==1 && linearNoElements[2]%2==1)
    {
	throw "The 3D triangular grid generator can't handle an odd amount of elements in the periodic dimension Z";
    }
 
    //Stage 1 : Precompute some required values
  
    PointPhysicalT delta_x(DIM);
  
    for (int i = 0; i < DIM; ++i)
    {
	delta_x[i] = (TopRight[i] - BottomLeft[i]) / (linearNoElements[i]);
    }
  
    std::vector<int> numOfNodesInEachSubspace(DIM), numOfElementsInEachSubspace(DIM);
  
    numOfNodesInEachSubspace[0] = 1;
    numOfElementsInEachSubspace[0] = 1;
  
    unsigned int totalNumOfNodes, totalNumOfElements, verticesPerElement,
    verticesPerGroup, trianglesPerRectangle;
  
    totalNumOfNodes = (linearNoElements[0] + 1);
    //'elements' in this counter denote groups of trianglesPerRectangle elements
    totalNumOfElements = (linearNoElements[0]);
    verticesPerElement = 2;
    verticesPerGroup = 2;
    trianglesPerRectangle = 1;
    int powerOf2;
  
    for (int idim = 1; idim < DIM; ++idim)
    {
	totalNumOfNodes *= (linearNoElements[idim] + 1);
	totalNumOfElements *= (linearNoElements[idim]);
	verticesPerElement += 1;
	verticesPerGroup *= 2;
	trianglesPerRectangle += (2 * idim - 1);
	numOfElementsInEachSubspace[idim] = numOfElementsInEachSubspace[idim - 1] * (linearNoElements[idim - 1]);
	numOfNodesInEachSubspace[idim] = numOfNodesInEachSubspace[idim - 1] * (linearNoElements[idim - 1] + 1);
    }
 
    //'elements' in this counter denote groups of trianglesPerRectangle elements
    //totalNumOfElements*=(trianglesPerRectangle);
    //for(int idim=0;idim<DIM;++idim)
    //{
    // numOfElementsInEachSubspace[idim]*=(trianglesPerRectangle);
    //}
  
    PointPhysicalT x;
  
    //Stage 2 : Create the nodes
  
    for (int nodeIndex = 0; nodeIndex < totalNumOfNodes; ++nodeIndex)
    {
	int nodeIndexRemain = nodeIndex;
	for (int idim = DIM - 1; idim > -1; --idim)
	{
	    x[idim] = BottomLeft[idim] + (nodeIndexRemain / numOfNodesInEachSubspace[idim] * delta_x[idim]);
	    nodeIndexRemain %= numOfNodesInEachSubspace[idim];
	}
	points_.push_back(x);
    }
 
    //Stage 3 : Create the elements
  
  
    //artifact from hpGEM 1 (?)
//     void* referenceGeometryVoidPtr;
//     Geometry::ReferenceGeometry < DIM > *referenceGeometry;
//   
//     if (DIM == 1)
//     {
//     //The rectangular mesh generator will be better suited to create line elements
//     referenceGeometryVoidPtr = &Geometry::ReferenceLine::Instance();
//     }else if (DIM == 2)
//     {
//     referenceGeometryVoidPtr = &Geometry::ReferenceTriangle::Instance();
//     }else if (DIM == 3)
//     {
//     referenceGeometryVoidPtr = &Geometry::ReferenceTetrahedron::Instance();
//     }
//     referenceGeometry = static_cast<Geometry::ReferenceGeometry<DIM>*>(referenceGeometryVoidPtr);
//   
    std::vector<Base::Element<DIM>*> tempElementVector(trianglesPerRectangle * totalNumOfElements);
    std::vector<unsigned int> elementNdId(DIM), vertexNdId(DIM);
    std::vector < std::vector<unsigned int> > globalVertexID(trianglesPerRectangle);
 
    //for(int i=0;i<trianglesPerRectangle;++i){globalVertexID[i].resize(verticesPerElement);}
  
    for (int elementGroupIndex = 0; elementGroupIndex < totalNumOfElements;++elementGroupIndex)
    {
	//indicates if the element has to be rotated to make connecting faces; rotates the element 90 degrees along the y-axis if needed
	int rotate = 0;
      
	for(int i=0;i<trianglesPerRectangle;++i)
	{
	    globalVertexID[i].clear();
	}
	int elementIndexRemainder = elementGroupIndex;
      
	for (int idim = DIM - 1; idim > -1; --idim)
	{
	    elementNdId[idim] = elementIndexRemainder / numOfElementsInEachSubspace[idim];
	    elementIndexRemainder %= numOfElementsInEachSubspace[idim];
	    rotate = (elementNdId[idim] + rotate) % 2;
	}
      
	for (int i = 0; i < verticesPerGroup; ++i)
	{
	    if (rotate == 0)
	    {
		powerOf2 = 1;
		for (int idim = 0; idim < DIM; ++idim)
		{
		    vertexNdId[idim] = elementNdId[idim] + ((i & powerOf2) != 0);
		    powerOf2 *= 2;
		}
	    }else
	    {
		powerOf2 = verticesPerGroup;
		for (int idim = 0; idim < DIM; ++idim)
		{
		    powerOf2 /= 2;
		    vertexNdId[idim] = elementNdId[idim] + (((i ^ rotate) & powerOf2) != 0);
		}
	    }
 
	    //first map to the one dimensional global ID
	    int vertexIndex = vertexNdId[0];
	    for (int idim = 1; idim < DIM; ++idim)
	    {
		vertexIndex += vertexNdId[idim] * numOfNodesInEachSubspace[idim];
	    }
 
	    //then cherrypick the element(s) this vertex should connect to (probably not the cleanest implementation; doesn't work if DIM>3)
	    switch (i)
	    {
	    case 0:
		globalVertexID[0].push_back(vertexIndex);
		break;
	    case 3:
		globalVertexID[1].insert(++(globalVertexID[1].begin()),vertexIndex);
		break;	  
	    case 5:
		globalVertexID[2].push_back(vertexIndex);
		break;	  
	    case 6:
		globalVertexID[3].insert(++(globalVertexID[3].begin()),vertexIndex);
		break;	  
	    case 1:
		for (int i = 0; i < trianglesPerRectangle; ++i)
		{
		    if (i != 3)
		    {
			globalVertexID[i].push_back(vertexIndex);
		    }
		}
		break;	  
	    case 2:
		for (int i = 0; i < trianglesPerRectangle; ++i)
		{
		    if (i != 2)
		    {
			globalVertexID[i].push_back(vertexIndex);
		    }
		}
		break;	  
	    case 4:
		for (int i = 0; i < trianglesPerRectangle; ++i)
		{
		    if (i != 1)
		    {
			globalVertexID[i].push_back(vertexIndex);
		    }
		}
		break;	  
	    case 7:
		for (int i = 0; i < trianglesPerRectangle; ++i)
		{
		    if (i != 0)
		    {
			globalVertexID[i].push_back(vertexIndex);
		    }
		}
		break;
	    } //switch
	} //for all vertices of the rectangle
 
	for (int i = 0; i < trianglesPerRectangle; ++i)
	{
	    tempElementVector[trianglesPerRectangle * elementGroupIndex + i] = addElement(globalVertexID[i]);
	}
    } //for all rectangles
 
  //Stage 4 : Create the faces
 
    switch (DIM)
    {
    case 1:
	//only provided for completeness, it is probably always better to just use rectangularMeshGenerator for line elements
	triangularCreateFaces1D(tempElementVector, linearNoElements);
	break;  
    case 2:
	triangularCreateFaces2D(tempElementVector, linearNoElements);
	break;  
    case 3:
	triangularCreateFaces3D(tempElementVector, linearNoElements);
	break;  
    }
}

using Geometry::WALL_BC;
 
template<unsigned int DIM>
void MeshManipulator<DIM>::triangularCreateFaces1D(std::vector<Base::Element<DIM>*>& tempElementVector,const std::vector<unsigned int>& linearNoElements)
{
 
    //First the internal faces
    for (int i = 0; i+1 < linearNoElements[0]; i++)
    {
	addFace(tempElementVector[i], 1 - i % 2, tempElementVector[i + 1],1 - i % 2);
    }
  
    if (periodicX_ == 1)
    {
	addFace(tempElementVector[0], 0, tempElementVector[linearNoElements[0] - 1], linearNoElements[0] % 2);
    }else
    {
	addFace(tempElementVector[0], 0, NULL,0, WALL_BC);
	addFace(tempElementVector[linearNoElements[0] - 1],linearNoElements[0] % 2, NULL,0, WALL_BC);
    }
 
}
 
template<unsigned int DIM>
void MeshManipulator<DIM>::triangularCreateFaces2D(std::vector<Base::Element<DIM>*>& tempElementVector,const std::vector<unsigned int>& linearNoElements)
{
    //this face creator assumes the grid has been divided in rectangles. The rectangles are then sub-divided into two triangles, which are alternatively rotated and not rotated, in a checkerboard pattern. The rectangle at DIM index (0,0,0) is not rotated. There is an ordering of the vertexes assumed that looks like the following:
    // 2 21 02 2
    // 01 0 1 01
    //not rotated rotated
    // 0 1 0 1 (element numbers within the rectangle)
  
    //first do the faces with normals pointing in the x-direction
    Geometry::FaceType xFace = Geometry::WALL_BC;
    unsigned int index;
    unsigned int rotate;
    for (int j = 0; j < linearNoElements[1]; ++j)
    {
	for (int i = 0; i +1< linearNoElements[0]; ++i)
	{
	    //hard-coded number of triangles per rectangle, being able to change a constant is not going to make this easier to change
	    index = 2 * (i + j * linearNoElements[0]);
	    rotate = (i + j) % 2;
	    addFace(tempElementVector[index + 1], 2 * rotate, tempElementVector[index + 2], rotate);
	}
      
	index = 2 * j * linearNoElements[0];
	rotate = (j + linearNoElements[0] - 1) % 2;
	if (periodicX_ == 1)
	{
	    addFace(tempElementVector[index], 1 - j % 2,
	    tempElementVector[index + 2 * linearNoElements[0] - 1],
	    2 * rotate);
	} else
	{
	    //Left bounday face
	  
    // 	cout << "index1=" << index << endl;
	    addFace(tempElementVector[index], 1 - j % 2, NULL, 0,WALL_BC);
	  
	    //Right boundary face
	    index = index + 2 * linearNoElements[0] - 1;
    // 	cout << "index2=" << index << endl;
	    addFace(tempElementVector[index], 2 * rotate, NULL, 0,WALL_BC);
	}
    }
 
    //now do the faces with normals pointing in the y-direction
    //changing loop ordering is risky+makes the code a bit less efficient (?)
    Geometry::FaceType yFace = Geometry::WALL_BC;
    for (int j = 0; j < linearNoElements[0]; ++j)
    {
	for (int i = 0; i+1 < linearNoElements[1]; ++i)
	{
	    index = 2 * (j + i * linearNoElements[0]);
	    rotate = (i + j) % 2;
// 	    cout << "index3=" << index << endl;
// 	    cout << "index3.5=" << index + 2 * linearNoElements[0] << endl;
	    addFace(tempElementVector[index + 1 - rotate], 2 - rotate,tempElementVector[index + 2 * linearNoElements[0] + 1 - rotate],0);  
	}
	  
	index = 2 * j;
	rotate = (j + linearNoElements[1] - 1) % 2;
	if (periodicY_ == 1)
	{
	    addFace(tempElementVector[index + j % 2], 0,tempElementVector[index + 2 * (linearNoElements[1] - 1) * linearNoElements[0] + 1 - rotate],2 - rotate);      
	}else
	{	  
	    //Bottom boundary face	  
// 	    cout << "index4=" << index + j % 2 << endl;
	    addFace(tempElementVector[index + j % 2], 0, NULL, 0,WALL_BC);	  
	    //Top boundary face
	    index = index + 2 * (linearNoElements[1] - 1) * linearNoElements[0] + 1 - rotate;	  
// 	    cout << "index5=" << index << endl;
	    addFace(tempElementVector[index], 2 - rotate, NULL, 0,WALL_BC);
	}
    }
 
    //now do the diagonal faces
    for (int j = 0; j < linearNoElements[1]; ++j)
    {
	for (int i = 0; i < linearNoElements[0]; ++i)
	{
	    index = 2 * (i + j * linearNoElements[0]);
	    addFace(tempElementVector[index], 2, tempElementVector[index + 1],1);      
	//diagonal faces are never boudary faces
	}
    } 
}
 
template<unsigned int DIM>
void MeshManipulator<DIM>::triangularCreateFaces3D(std::vector<Base::Element<DIM>*>& tempElementVector,const std::vector<unsigned int>& linearNoElements)
{
    //my ascii-art skill are sadly not good enough to show how the rectangular element has been devided
    unsigned int index;
    unsigned int rotate;
    //counter in z
    for (int k = 0; k < linearNoElements[2]; k++)
    {
	//counter in y
	for (int j = 0; j < linearNoElements[1]; j++)
	{
	    //counter in x
	    for (int i = 0; i+1 < linearNoElements[0]; i++)
	    {
// 	        cout<<"("<<i<<","<<j<<","<<k<<")"<<endl;
	        //hard-coded number of triangles per rectangle, being able to change a constant is not going to make this easier to change
		index = 5 * (i + j * linearNoElements[0] + k * linearNoElements[0] * linearNoElements[1]);
		rotate = (i + j + k) % 2;
		addFace(tempElementVector[index + 2], 3 * rotate,tempElementVector[index + 5], 2 - 2 * rotate);
		addFace(tempElementVector[index + 1 + 2 * rotate], 1 + 2 * rotate,tempElementVector[index + 6 + 2 * rotate], 2);	
	    } //end loop over x
      
	    index = 5 * (j * linearNoElements[0] + k * linearNoElements[0] * linearNoElements[1]);
	    rotate = (j + k + linearNoElements[0] - 1) % 2;
	    if (periodicX_ == 1)
	    {
		addFace(tempElementVector[index], 2 * ((k + j) % 2),
		tempElementVector[index + 5 * linearNoElements[0] - 3],3 * rotate);
		addFace(tempElementVector[index + 3 - 2 * ((k + j) % 2)], 2,
		tempElementVector[index + 5 * linearNoElements[0] - 4 + 2 * rotate], 1 + 2 * rotate);
	    }else
	    {	
		//Left boundary	
// 	        cout << "index1=" << index << endl;
		addFace(tempElementVector[index], 2 * ((k + j) % 2), NULL,0, WALL_BC);
		addFace(tempElementVector[index + 3 - 2 * ((k + j) % 2)], 2, NULL,0,WALL_BC );
		//Right boundary face
		index = index + 5 * linearNoElements[0] - 5;
//              cout << "index2=" << index << endl;
		addFace(tempElementVector[index + 2], 3 * rotate, NULL,0,WALL_BC);
		addFace(tempElementVector[index + 1 + 2 * rotate], 1 + 2 * rotate, NULL, 0,WALL_BC);
	    } // end boundary cases    
	} // end loop over y
    } //end loop over z
    
    //Now do the faces pointing in the y-direction
    //count in z
    for (int k = 0; k < linearNoElements[2]; k++)
    {
	//counter in x
	for (int j = 0; j < linearNoElements[0]; j++)
	{
	    //counter in y
	    for (int i = 0; i+1 < linearNoElements[1]; i++)
	    {
// 		cout<<"("<<i<<","<<j<<","<<k<<")"<<endl;	      
		index = 5 * (j + i * linearNoElements[0] + k * linearNoElements[0] * linearNoElements[1]);
		rotate = (i + j + k) % 2;
		addFace(tempElementVector[index + 1], 3, tempElementVector[index + 5 * linearNoElements[0] + 2 - 2 * rotate], 2 - rotate);
		addFace(tempElementVector[index + 3], 1, tempElementVector[index + 5 * linearNoElements[0] + 2 * rotate], 1 + rotate);
	    } //end loop over x
	  
	    index = 5 * (j + k * linearNoElements[0] * linearNoElements[1]);
	    rotate = (j + k) % 2;
	    if (periodicY_ == 1)
	    {
		//cout << "Hmmm this is the problem " << endl;
// 		cout << index << " , " << index + 5 * (linearNoElements[1] - 1) * linearNoElements[0] << endl;
		addFace(tempElementVector[index + 2 * rotate], 1 + rotate, tempElementVector[(index + 5 * (linearNoElements[1] - 1) * linearNoElements[0]) + 1], 3);
		addFace(tempElementVector[index + 2 - 2 * rotate], 2 - rotate, tempElementVector[(index + 5 * (linearNoElements[1] - 1) * linearNoElements[0]) + 3], 1);
	    }else
	    {	  
		//front bounday face	  
// 		cout << "index1=" << index << endl;
		addFace(tempElementVector[index + 2 - 2 * rotate], 2 - rotate, NULL,0, WALL_BC);
		addFace(tempElementVector[index + 2 * rotate], 1 + rotate, NULL,0, WALL_BC);	  
		//Back boundary face
		index = index + 5 * (linearNoElements[1] - 1) * linearNoElements[0];
// 		cout << "index2=" << index << endl;
		addFace(tempElementVector[index + 1], 3, NULL, 0,WALL_BC);
		addFace(tempElementVector[index + 3], 1, NULL, 0,WALL_BC);
	    } // end boundary cases	  
	} // end loop over y
    } //end loop over z
  
    //Now do the face in the z-direction
    //count in x direction
    for (int k = 0; k < linearNoElements[0]; k++)
    {
	//counter in y
	for (int j = 0; j < linearNoElements[1]; j++)
	{
	    //counter in z
	    for (int i = 0; i +1< linearNoElements[2]; i++)
	    {
// 		cout<<"("<<i<<","<<j<<","<<k<<")"<<endl;	      
		index = 5 * (k + j * linearNoElements[0] + i * linearNoElements[0] * linearNoElements[1]);
		rotate = (i + j + k) % 2;
		addFace(tempElementVector[index + 2 - 2 * rotate], 3 - 3 * rotate, tempElementVector[index + 5*linearNoElements[0] * linearNoElements[1] + 2 - 2 * rotate], 2 * rotate);
		addFace(tempElementVector[index + 3], 3 - rotate, tempElementVector[index + 5*linearNoElements[0] * linearNoElements[1] + 1], 1 + rotate);	  
	    } //end loop over x
	  
	    index = 5 * (k + j * linearNoElements[0]);
	    rotate = (k + j + linearNoElements[2] - 1) % 2;
	    if (periodicZ_ == 1)
	    {
// 		cout << "This is the final problem " << endl;
// 		cout << index << " , ";
// 		cout << index + 5 * (linearNoElements[2] - 1) * linearNoElements[1] * linearNoElements[0] << endl;
		addFace(tempElementVector[index + 1 + ((j + k) % 2)], 2-2*((j + k) % 2),tempElementVector[index + 5 * (linearNoElements[2] - 1) * linearNoElements[1] * linearNoElements[0] + 2 + rotate], 3 - rotate);
		addFace(tempElementVector[index + (j + k) % 2], 2 - (j+k)%2 ,tempElementVector[index + 5 * (linearNoElements[2] - 1) * linearNoElements[1] * linearNoElements[0] + 3-3*rotate], 3-3*rotate);
	    }else
	    {	  
		//bottom boundary
// 		cout << "index1=" << index << endl;
		addFace(tempElementVector[index+2*((j+k)%2)], 2 - 2*((j+k)%2), NULL,0,WALL_BC );
		addFace(tempElementVector[index + 1], 2-(j+k)%2, NULL,0, WALL_BC);	  
		//Top boundary face
		index = index + 5 * (linearNoElements[2] - 1) * linearNoElements[1] * linearNoElements[0];
// 		cout << "index2=" << index << endl;
		addFace(tempElementVector[index + 2 - 2 * rotate], 3 - 3 * rotate, NULL, 0,WALL_BC);
		addFace(tempElementVector[index + 3], 3 - rotate, NULL, 0,WALL_BC);
	    } // end boundary cases      
	} // end loop over y
    } //end loop over z
    //Now do the diagonal faces
    //counter in z
    for (int k = 0; k < linearNoElements[2]; k++)
    {
	//counter in y
	for (int j = 0; j < linearNoElements[1]; j++)
	{
	    //counter in x
	    for (int i = 0; i < linearNoElements[0]; i++)
	    {
// 		cout<<"("<<i<<","<<j<<","<<k<<")"<<endl;	      
		index = 5 * (i + j * linearNoElements[0] + k * linearNoElements[0] * linearNoElements[1]);
		addFace(tempElementVector[index + 4], 0,tempElementVector[index + 2], 1);
		addFace(tempElementVector[index + 4], 1,tempElementVector[index + 1], 0);
		addFace(tempElementVector[index + 4], 2,tempElementVector[index], 3);
		addFace(tempElementVector[index + 4], 3,tempElementVector[index + 3], 0);	  
	    } //end loop over x
	} //end loop over y
    } //end loop over z  
}

template<unsigned int DIM>
void 
MeshManipulator<DIM>::readCentaurMesh(const std::string& filename)
{

    //First open the file
    std::ifstream centaurFile;
    
    centaurFile.open(filename.c_str(), std::ios::binary);
	if (!centaurFile.is_open())
	{
	    throw("Cannot open Centaur meshfile.");
	}
    
    switch (DIM)
    {
	case 2: 
	    readCentaurMesh2D(centaurFile);
	    break;
	case 3:
	    readCentaurMesh3D(centaurFile);
	    break;
	default:
	    std::cerr<<"Centaur mesh reader has not been implemented in this DIMension" << std::endl;
    }
    
    //Finally close the file
    centaurFile.close();
}

    template<unsigned int DIM>
    void MeshManipulator<DIM>::readCentaurMesh2D(std::ifstream& centaurFile)
    {
      
        
        //These are used to check the length of the read lines to check for read errors
        int sizeOfLine;	   
        
        //This first value in the centaur file is the size of each line in the file;
        centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
        
        // Version number of the Centaur mesh file 0.1 if using Tito's matlab generator
        float version;		      
        centaurFile.read(reinterpret_cast<char*>(&version), sizeof(version));
        std::cout << "This read mesh is in Centaur version " << version << " format" <<std::endl;
        
        // Centaur File Type <0 is two DIMensional and >0 is three DIMensional
        int centaurFileType;			
        centaurFile.read(reinterpret_cast<char*>(&centaurFileType),sizeof(centaurFileType));
        
        
        
        if (centaurFileType<0)
        {
            std::cout << "Reading a two DIMensional centaur mesh" <<std::endl;
            
            //The rest of the first line is junk
            char junk[1024];
            
            int checkInt;
            centaurFile.read(&junk[0], sizeOfLine-sizeof(version)-sizeof(centaurFileType));
            
            //Check the first line was read correctly : each line in centaur start and end with the line size as a check
            centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
            if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
            
            //Start the second line
            centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
            
            //Next read the total number of nodes
            int numberOfNodes;
            centaurFile.read(reinterpret_cast<char*>(&numberOfNodes),sizeof(numberOfNodes));
            std::cout << "File contains " << numberOfNodes <<" nodes" <<std::endl;
            
            //Check the second line was read correctly : each line in centaur start and end with the line size as a check
            centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
            if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
            
            
            //Now we will read in all the nodes
            centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
            double nodeCoord[2];
            PointPhysicalT nodeCoordPointFormat;
            for (int i=0; i<numberOfNodes; i++)
            {
                // Reads the x and y coordinates of each node.
                centaurFile.read(reinterpret_cast<char*>(&nodeCoord[0]), sizeof(nodeCoord));
                // pass the node to the nodelist.
                
                //Covert from *double to hpGEM PointPhysical format
                nodeCoordPointFormat[0]=nodeCoord[0];
                nodeCoordPointFormat[1]=nodeCoord[1];
                points_.push_back(nodeCoordPointFormat);
               
            }
            //Now check the node line was read correctly : each line in centaur start and end with the line size as a check
            centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
            if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
            
            
            
            //Now check how many triangle in the file
            centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
            // Number of triangular elements
            int numberOfTriangles;		
            centaurFile.read(reinterpret_cast<char*>(&numberOfTriangles),sizeof(numberOfTriangles));
            std::cout << "File contains " <<numberOfTriangles<< " triangle(s)" << std::endl;
            
            //Check the line was read correctly : each line in centaur start and end with the line size as a check
            centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
            if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
            
            centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
            /// \bug Need to add a triangle reader
            if (numberOfTriangles>0)
            {
                unsigned int triGlobalNodeIndexes[3];
                std::vector<unsigned int> globalNodeIndexes(3);
                Geometry::ReferenceGeometry<2>* referenceTriangle = &Geometry::ReferenceTriangle::Instance();
             
                for (int i=0; i<numberOfTriangles; i++)
                {
                 
                     centaurFile.read(reinterpret_cast<char*>(&triGlobalNodeIndexes[0]), sizeof(triGlobalNodeIndexes));
                    for (int j=0;j<3;j++){globalNodeIndexes[j]=triGlobalNodeIndexes[j]-1;}
                    
                    addElement(globalNodeIndexes);
                    
                }
                
                
            }
            centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
            if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
                        
            //Now check the number of quaduratiles in the file
            centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
            int numberOfQuads;			
            centaurFile.read(reinterpret_cast<char*>(&numberOfQuads),sizeof(numberOfQuads));
            std::cout << "File contains " <<numberOfQuads<<" quaduratile(s)" <<std::endl;
	    
	    ///\bug There is a read(checkInt missing here
	    
            if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
            
            //Now read the quaduritles in
            centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
            ///\bug Code works but we do not understand why the following integer is throwen away
            centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
            if (numberOfQuads>0)
            {
                unsigned int quadGlobalNodeIndexes[4];
                unsigned int temp;
                std::vector<unsigned int> globalNodeIndexes(4);
                Geometry::ReferenceGeometry<2>* referenceSquare = &Geometry::ReferenceSquare::Instance();
                for (int i=0; i<numberOfQuads; i++)
                    {
                        //Reading the vertex indices of each quadrilateral.
                        centaurFile.read(reinterpret_cast<char*>(&quadGlobalNodeIndexes[0]), sizeof(quadGlobalNodeIndexes));
                
                        // renumbering of the vertices to match the ordering assumed by
                        // hpGem:
                        
                        temp=quadGlobalNodeIndexes[2];
                        quadGlobalNodeIndexes[2]=quadGlobalNodeIndexes[3];
                        quadGlobalNodeIndexes[3]=temp;
                
                        // renumber them from 1..N to 0..N-1.
                        for (int j=0; j < 4; j++)
                            globalNodeIndexes[j] = quadGlobalNodeIndexes[j]-1; 
          
                        addElement(globalNodeIndexes);
                    }

            }
            //Check the line was read correctly : each line in centaur start and end with the line size as a check
            centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
            if (checkInt!=sizeOfLine)
            {
                std::cerr << "Error in centaur file " << std::endl; 
                return;
            }
            
         
            faceFactory();
            
            
        }
        else 
        {
            std::cerr << "Incorrect mesh file. This mesh appears to contain three DIMensional data" << std::endl;
            
        }        
    }
    







template<unsigned int DIM>
void MeshManipulator<DIM>::readCentaurMesh3D(std::ifstream& centaurFile)
{
  
    
    //These are used to check the length of the read lines to check for read errors
    int sizeOfLine;	   
    
    //This first value in the centaur file is the size of each line in the file;
    centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
    
    // Version number of the Centaur mesh file 0.1 if using Tito's matlab generator
    float version;		      
	centaurFile.read(reinterpret_cast<char*>(&version), sizeof(version));
    std::cout << "This read mesh is in Centaur version " << version << " format" <<std::endl;
    
    // Centaur File Type <0 is two DIMensional and >0 is three DIMensional
    int centaurFileType;			
	centaurFile.read(reinterpret_cast<char*>(&centaurFileType),sizeof(centaurFileType));
    
    
    
    if (centaurFileType>0)
    {
        std::cout << "Reading a three DIMensional centaur mesh" <<std::endl;
        
        //The rest of the first line is junk
        char junk[1024];
        
        int checkInt;
        centaurFile.read(&junk[0], sizeOfLine-sizeof(version)-sizeof(centaurFileType));
        
        //Check the first line was read correctly : each line in centaur start and end with the line size as a check
        centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
        if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
        
        //Start the second line
        centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
        
        //Next read the total number of nodes
        int numberOfNodes;
	    centaurFile.read(reinterpret_cast<char*>(&numberOfNodes),sizeof(numberOfNodes));
        std::cout << "File contains " << numberOfNodes <<" nodes" <<std::endl;
        
        //new centaur versions support splitting this list over multiple lines
        int numberOfNodesPerLine(numberOfNodes);
        if(centaurFileType>4){
	    centaurFile.read(reinterpret_cast<char*>(&numberOfNodesPerLine),sizeof(numberOfNodesPerLine));
            std::cout << "One line in the file contains at most " << numberOfNodesPerLine <<" nodes" <<std::endl;
	}
        
        //Check the second line was read correctly : each line in centaur start and end with the line size as a check
        centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
        if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
        
        //Now we will read in all the nodes
        centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
        double nodeCoord[3];
        PointPhysicalT nodeCoordPointFormat;
        for (int i=0; i<numberOfNodes; i++)
        {
	    if(i>0&&i%numberOfNodesPerLine==0){
	        //If all the nodes on a line are read end the line and start a new one
	        centaurFile.read(reinterpret_cast<char*>(&checkInt),sizeof(checkInt));
		if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
		centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	    }
	  
            // Reads the x, y and z coordinates of each node.
            centaurFile.read(reinterpret_cast<char*>(&nodeCoord[0]), sizeof(nodeCoord));
            // pass the node to the nodelist.
            
            //Covert from *double to hpGEM PointPhysical format
            nodeCoordPointFormat[0]=nodeCoord[0];
            nodeCoordPointFormat[1]=nodeCoord[1];
	    nodeCoordPointFormat[2]=nodeCoord[2];
	    cout<<nodeCoordPointFormat<<endl;
            points_.push_back(nodeCoordPointFormat);
           
	}
        //Now check the node line was read correctly : each line in centaur start and end with the line size as a check
        centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
        if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
        
        //Keep track of the node->Element connectivity to ease face creation later on
        std::vector<std::list<int> > listOfElementsForEachNode(numberOfNodes);   
	std::vector<Element<DIM>* > tempElementVector;
        
        //file version 1 has no lines about hexahedra
        if(centaurFileType>1){
	    //Now check how many hexahedra in the file
	    centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	    // Number of hexahedral elements
	    int numberOfHexahedra;		
	    centaurFile.read(reinterpret_cast<char*>(&numberOfHexahedra),sizeof(numberOfHexahedra));
	    std::cout << "File contains " <<numberOfHexahedra<< " hexahedron(s)" << std::endl;
	    
	    //new centaur versions support splitting this list over multiple lines
	    int numberOfHexahedraPerLine(numberOfHexahedra);
	    if(centaurFileType>4){
		centaurFile.read(reinterpret_cast<char*>(&numberOfHexahedraPerLine),sizeof(numberOfHexahedraPerLine));
		std::cout << "One line in the file contains at most " << numberOfHexahedraPerLine <<" hexahedra" <<std::endl;
	    }
	  
	    centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	    if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	  
	  
	    unsigned int hexahedralGlobalNodeIndexes[8];
            unsigned int temp;
            std::vector<unsigned int> globalNodeIndexes(8);
	    centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));	    
            for (int i=0; i<numberOfHexahedra; i++)
            {
		if(i>0&&i%numberOfHexahedraPerLine==0){
		    //If all the nodes on a line are read end the line and start a new one
		    centaurFile.read(reinterpret_cast<char*>(&checkInt),sizeof(checkInt));
		    if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
		    centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
		}
		  
		//Reading the vertex indices of each hexahedron.
		centaurFile.read(reinterpret_cast<char*>(&hexahedralGlobalNodeIndexes[0]), sizeof(hexahedralGlobalNodeIndexes));
	
		// renumbering of the vertices to match the ordering assumed by
		// hpGem: (based on the numbering in hpGEM 1)
		
		temp=hexahedralGlobalNodeIndexes[2];
		hexahedralGlobalNodeIndexes[2]=hexahedralGlobalNodeIndexes[3];
		hexahedralGlobalNodeIndexes[3]=temp;
			
		temp=hexahedralGlobalNodeIndexes[6];
		hexahedralGlobalNodeIndexes[6]=hexahedralGlobalNodeIndexes[7];
		hexahedralGlobalNodeIndexes[7]=temp;
		
		// renumber them from 1..N to 0..N-1.
		for (int j=0; j < 8; j++)
		    globalNodeIndexes[j] = hexahedralGlobalNodeIndexes[j]-1; 
  
		Base::Element<DIM>* newElement=addElement(globalNodeIndexes);
		tempElementVector.push_back(newElement);
		
		for(int j=0;j<8;++j){
		    listOfElementsForEachNode[globalNodeIndexes[j] ].push_back(newElement->getID()-1);
		}
	    }
	    centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	    if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	}
        
        
        //Now check how many triangular prisms in the file
	centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	// Number of prismatic elements
	int numberOfPrisms;		
	centaurFile.read(reinterpret_cast<char*>(&numberOfPrisms),sizeof(numberOfPrisms));
	std::cout << "File contains " <<numberOfPrisms<< " triangular prism(s)" << std::endl;
	
	//new centaur versions support splitting this list over multiple lines
	int numberOfPrismsPerLine(numberOfPrisms);
	if(centaurFileType>4){
	    centaurFile.read(reinterpret_cast<char*>(&numberOfPrismsPerLine),sizeof(numberOfPrismsPerLine));
	    std::cout << "One line in the file contains at most " << numberOfPrismsPerLine <<" prisms" <<std::endl;
	}
      
	centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
      
      
	unsigned int prismaticGlobalNodeIndexes[6];
	std::vector<unsigned int> globalNodeIndexes(6);
	centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));	    
	for (int i=0; i<numberOfPrisms; i++)
	{
	    if(i>0&&i%numberOfPrismsPerLine==0){
		//If all the nodes on a line are read end the line and start a new one
		centaurFile.read(reinterpret_cast<char*>(&checkInt),sizeof(checkInt));
		if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
		centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	    }
	      
	    //Reading the vertex indices of each hexahedron.
	    centaurFile.read(reinterpret_cast<char*>(&prismaticGlobalNodeIndexes[0]), sizeof(prismaticGlobalNodeIndexes));
	    
	    // renumber them from 1..N to 0..N-1.
	    for (int j=0; j < 6; j++)
		globalNodeIndexes[j] = prismaticGlobalNodeIndexes[j]-1; 

		Base::Element<DIM>* newElement=addElement(globalNodeIndexes);
		tempElementVector.push_back(newElement);
		
		for(int j=0;j<6;++j){
		    listOfElementsForEachNode[globalNodeIndexes[j] ].push_back(newElement->getID()-1);
		}
	}
	centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
        
        //file version 1 has no lines about pyramids
        if(centaurFileType>1){
	    //Now check how many pyramids in the file
	    centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	    // Number of pyramids elements
	    int numberOfPyraminds;		
	    centaurFile.read(reinterpret_cast<char*>(&numberOfPyraminds),sizeof(numberOfPyraminds));
	    std::cout << "File contains " <<numberOfPyraminds<< " pyramid(s)" << std::endl;
	    
	    //new centaur versions support splitting this list over multiple lines
	    int numberOfPyramindsPerLine(numberOfPyraminds);
	    if(centaurFileType>4){
		centaurFile.read(reinterpret_cast<char*>(&numberOfPyramindsPerLine),sizeof(numberOfPyramindsPerLine));
		std::cout << "One line in the file contains at most " << numberOfPyramindsPerLine <<" pyramids" <<std::endl;
	    }
	  
	    centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	    if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	  
	  
	    unsigned int pyramidGlobalNodeIndexes[5];
            unsigned int temp;
            globalNodeIndexes.resize(5);
	    centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));	    
            for (int i=0; i<numberOfPyraminds; i++)
            {
		if(i>0&&i%numberOfPyramindsPerLine==0){
		    //If all the nodes on a line are read end the line and start a new one
		    centaurFile.read(reinterpret_cast<char*>(&checkInt),sizeof(checkInt));
		    if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
		    centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
		}
		  
		//Reading the vertex indices of each pyramid.
		centaurFile.read(reinterpret_cast<char*>(&pyramidGlobalNodeIndexes[0]), sizeof(pyramidGlobalNodeIndexes));
	
		//and then the renumbering fun begins
		//first make sure we always use the same numbering sceme even when reading from old centaur files
		//     For centaurfiletypes <= 3, the pyramids will have the
		//     orientation:
		//     1-2-3-4 points away from 5
		//     
		//     For centaurfiletypes >  3, the pyramids will have the 
		//     orientation:
		//     1-2-3-4 points towards 5
		//     
		//     mirror the pyramid in the centaurFileType <4 case
		//     to always get the orientation: 1-2-3-4 points towards 5
		
		
		if(centaurFileType<4){
		    temp=pyramidGlobalNodeIndexes[0];
		    pyramidGlobalNodeIndexes[0]=pyramidGlobalNodeIndexes[1];
		    pyramidGlobalNodeIndexes[1]=temp;
		    
		    temp=pyramidGlobalNodeIndexes[2];
		    pyramidGlobalNodeIndexes[2]=pyramidGlobalNodeIndexes[3];
		    pyramidGlobalNodeIndexes[3]=temp;
		}
		
		//now renumber the ordered vertices to the expected numbering in hpGEM
		//for the moment we have the following corresponcence:
		// Centaur | hpGEM 1
		// -----------------
		//  0      | 1
		//  1      | 2
		//  2      | 4
		//  3      | 3
		//  4      | 0
		
		temp=pyramidGlobalNodeIndexes[0];
		pyramidGlobalNodeIndexes[0]=pyramidGlobalNodeIndexes[4];
		pyramidGlobalNodeIndexes[4]=pyramidGlobalNodeIndexes[2];
		pyramidGlobalNodeIndexes[2]=pyramidGlobalNodeIndexes[1];
		pyramidGlobalNodeIndexes[1]=temp;		
		
		// renumber them from 1..N to 0..N-1.
		for (int j=0; j < 5; j++)
		    globalNodeIndexes[j] = pyramidGlobalNodeIndexes[j]-1; 
  
		Base::Element<DIM>* newElement=addElement(globalNodeIndexes);
		tempElementVector.push_back(newElement);
		
		for(int j=0;j<5;++j){
		    listOfElementsForEachNode[globalNodeIndexes[j] ].push_back(newElement->getID()-1);
		}
	    }
	    centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	    if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	}
        
        //Now check how many tetrahedra in the file
	centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	// Number of tetrahedral elements
	int numberOfTetrahedra;		
	centaurFile.read(reinterpret_cast<char*>(&numberOfTetrahedra),sizeof(numberOfTetrahedra));
	std::cout << "File contains " <<numberOfTetrahedra<< " tetrahedron(s)" << std::endl;
	
	//new centaur versions support splitting this list over multiple lines
	int numberOfTetrahedraPerLine(numberOfTetrahedra);
	if(centaurFileType>4){
	    centaurFile.read(reinterpret_cast<char*>(&numberOfTetrahedraPerLine),sizeof(numberOfTetrahedraPerLine));
	    std::cout << "One line in the file contains at most " << numberOfTetrahedraPerLine <<" tetrahedra" <<std::endl;
	}
      
	centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
      
      
	unsigned int tetrahedralGlobalNodeIndexes[4];
	globalNodeIndexes.resize(4);
	centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));	    
	for (int i=0; i<numberOfTetrahedra; i++)
	{
	    if(i>0&&i%numberOfTetrahedraPerLine==0){
		//If all the tetrahedra on a line are read end the line and start a new one
		centaurFile.read(reinterpret_cast<char*>(&checkInt),sizeof(checkInt));
		if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
		centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	    }
	      
	    //Reading the vertex indices of each tetrahedron.
	    centaurFile.read(reinterpret_cast<char*>(&tetrahedralGlobalNodeIndexes[0]), sizeof(tetrahedralGlobalNodeIndexes));
	    
	    // renumber them from 1..N to 0..N-1.
	    for (int j=0; j < 4; j++)
		globalNodeIndexes[j] = tetrahedralGlobalNodeIndexes[j]-1; 

	    Base::Element<DIM>* newElement=addElement(globalNodeIndexes);
	    tempElementVector.push_back(newElement);
	    
	    for(int j=0;j<4;++j){
		listOfElementsForEachNode[globalNodeIndexes[j] ].push_back(newElement->getID()-1);
	    }
	}
	centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
		
	//faceFactory();
	
	
//    Read number of boundary faces and face to node
//    
//    A triangular boundary face can belong to a tetrahedron, prism 
//    or a pyramid
//    A quadrilateral boundary face belongs to a hexahedron,
//    prism or a pyramid
//    
//    The storage in the ibfnt array is as follows:
//                            ibfnt:  1  2  3  4  5  6  7  8
//    quadrilateral hexahedral face   x  x  x  x  x  x  x  x
//    triangular prism face           x  x  x  0  x  x  x  0
//    quadrilateral prism face        x  x  x  x  x  x  0  0
//    triangular pyramidal face       x  x  x  0  x  x  0  0
//    quadrilateral pyramidal face    x  x  x  x  x  0  0  0
//    tetrahedral face                x  x  x  0  x  0  0  0 
//    
//    In each case, the node numbers in the first 4 slots are the those
//    that belong to the face and the node numbers after those are the
//    rest of the nodes belonging to the cell associated with the face.
//    For hybfiletypes 3 and before, the quadrilateral were given
//    with the orientation 1-2-4-3, that is 3 above 1 and 4 above 2.
//    For hybfiletypes 4 and after, the quadrilateral faces have been
//    changed to have a more standard 1-2-3-4 ordering. The code below
//    will convert the old ordering to the new ordering.
//    
//    For hybfiletypes 3 and before, for each cell type, the 8th slot
//    in the ibfnt array is reserved for the panel number (pan)
//    to which the face belongs. The code below will convert this old
//    scheme into the new scheme described next.
//
//    For hybfiletypes 4 and after, there is another array, ibfacpan,
//    which stores the panel number allowing the eighth spot of the
//    ibfnt array to only be used for hexahedral quadrilateral boundary
//    faces.
//       
//    This panel number is then related to the boundary condition.

        
        
	centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	
	//number of boundary faces
	unsigned int numberOfBoundaryFaces;
        centaurFile.read(reinterpret_cast<char*>(&numberOfBoundaryFaces), sizeof(numberOfBoundaryFaces));
	std::cout << "File contains " <<numberOfBoundaryFaces<< " boundaryFace(s)" << std::endl;
	
	unsigned int boundaryFacesPerLine(numberOfBoundaryFaces);
	if(centaurFileType>4){
	    centaurFile.read(reinterpret_cast<char*>(&boundaryFacesPerLine),sizeof(boundaryFacesPerLine));
	    std::cout << "One line in the file contains at most " << boundaryFacesPerLine <<" tetrahedra" <<std::endl;
	}
	
	HalfFaceDescription boundarFaces[numberOfBoundaryFaces];
	
	std::vector<std::vector<int> > facesForEachCentaurPanel(0);
	//old centaur files use 7 entries to describe the face
	int nodalDescriptionOfTheFace[centaurFileType>3?8:7];
	int panelNumber,ijunk;
	centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	
	//first read the information about the faces
        centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
        for(int i=0;i<numberOfBoundaryFaces;++i){
	  
	    if(i>0&&i%numberOfBoundaryFaces==0){
		//If all the tetrahedra on a line are read end the line and start a new one
		centaurFile.read(reinterpret_cast<char*>(&checkInt),sizeof(checkInt));
		if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
		centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	    }
	    
	    centaurFile.read(reinterpret_cast<char*>(&nodalDescriptionOfTheFace[0]), sizeof(nodalDescriptionOfTheFace));
	    
	    findElementNumber(listOfElementsForEachNode[nodalDescriptionOfTheFace[0]-1],listOfElementsForEachNode[nodalDescriptionOfTheFace[1]-1],
	                      listOfElementsForEachNode[nodalDescriptionOfTheFace[2]-1],
		              nodalDescriptionOfTheFace[0]-1,nodalDescriptionOfTheFace[1]-1,nodalDescriptionOfTheFace[2]-1,listOfElementsForEachNode[nodalDescriptionOfTheFace[4]-1],
		              boundarFaces[i],tempElementVector);
	    //If someone can be bothered to implement and test it, there is some automated validation possible at this point using the rest of the nodalDescriptionOfTheFace data
	    
	    //then read the information about the panel numbers
	    if(centaurFileType<4){
		centaurFile.read(reinterpret_cast<char*>(&panelNumber), sizeof(panelNumber));	
		if(centaurFileType>1){
		    centaurFile.read(reinterpret_cast<char*>(&ijunk), sizeof(ijunk));
		}
		if(panelNumber>facesForEachCentaurPanel.size()){
		    facesForEachCentaurPanel.resize(panelNumber);
		}
		facesForEachCentaurPanel[panelNumber].push_back(i);
	    }
	}
	
	centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	
	//modern centaur file version store panel information separately
	if(centaurFileType>3){
	    centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	    for(int i=0;i<numberOfBoundaryFaces;++i){	  
		if(i>0&&i%numberOfBoundaryFaces==0){
		    //If all the tetrahedra on a line are read end the line and start a new one
		    centaurFile.read(reinterpret_cast<char*>(&checkInt),sizeof(checkInt));
		    if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
		    centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
		}		
		centaurFile.read(reinterpret_cast<char*>(&panelNumber), sizeof(panelNumber));	
		if(panelNumber>facesForEachCentaurPanel.size()){
		    facesForEachCentaurPanel.resize(panelNumber);
		}
		facesForEachCentaurPanel[panelNumber-1].push_back(i);
	    }
	    centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	    if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	}

	//put the centaur panels in their boudary group
	std::vector<std::vector<int> > facesForEachBoundaryGroup(0);
	int groupOfPanelNumber;
	
	//this bit of information is a little late
	int numberOfPanels;
	
        centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
        centaurFile.read(reinterpret_cast<char*>(&numberOfPanels), sizeof(numberOfPanels));
	assert(numberOfPanels==facesForEachCentaurPanel.size());
	centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	
	//then read the panel to group connections
        centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
        for(int i=0;i<numberOfPanels;++i){
	    centaurFile.read(reinterpret_cast<char*>(&groupOfPanelNumber), sizeof(groupOfPanelNumber));
	    if(groupOfPanelNumber>facesForEachBoundaryGroup.size()){
	        facesForEachBoundaryGroup.resize(groupOfPanelNumber);
	    }
	    for(int j=0;j<facesForEachCentaurPanel[i].size();++j){
		facesForEachBoundaryGroup[groupOfPanelNumber-1].push_back(facesForEachCentaurPanel[i][j]);
	    }
	}
	centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	
	int centaurBCType;
	char nameOfBoundaryCondition[80];
	
	//this bit of information is again a little late
	int numberOfBoundaryGroups;
	
        centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
        centaurFile.read(reinterpret_cast<char*>(&numberOfBoundaryGroups), sizeof(numberOfBoundaryGroups));
	assert(numberOfBoundaryGroups==facesForEachBoundaryGroup.size());
	centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	
	//now set the boundary conditions for each group
        centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	for(int i=0;i<numberOfBoundaryGroups;++i){
	    centaurFile.read(reinterpret_cast<char*>(&centaurBCType), sizeof(centaurBCType));
	    if(centaurBCType<1001){
	       cout<<"Viscous Wall boundary for group "<<i<<" assigned as WALL_BC"<<endl;
	       for(int j=0;j<facesForEachBoundaryGroup[i].size();++j){
		   //big assumption on the nature of elementIDs here...
                   addFace(tempElementVector[boundarFaces[facesForEachBoundaryGroup[i][j]].elementNum],boundarFaces[facesForEachBoundaryGroup[i][j]].localFaceIndex,NULL,0,Geometry::WALL_BC);
	       }
	    }else if(centaurBCType<2001){
	       cout<<"Inviscid Wall boundary for group "<<i<<" assigned as WALL_BC"<<endl;
	       for(int j=0;j<facesForEachBoundaryGroup[i].size();++j){
		   //big assumption on the nature of elementIDs here...
                   addFace(tempElementVector[boundarFaces[facesForEachBoundaryGroup[i][j]].elementNum],boundarFaces[facesForEachBoundaryGroup[i][j]].localFaceIndex,NULL,0,Geometry::WALL_BC);
	       }
	    }else if(centaurBCType<3001){
	       cout<<"symmetry plane boundary for group "<<i<<" assigned as WALL_BC"<<endl;
	       for(int j=0;j<facesForEachBoundaryGroup[i].size();++j){
		   //big assumption on the nature of elementIDs here...
                   addFace(tempElementVector[boundarFaces[facesForEachBoundaryGroup[i][j]].elementNum],boundarFaces[facesForEachBoundaryGroup[i][j]].localFaceIndex,NULL,0,Geometry::WALL_BC);
	       }
	    }else if(centaurBCType<4001){
	       cout<<"inlet pipe boundary for group "<<i<<" assigned as OPEN_BC"<<endl;
	       for(int j=0;j<facesForEachBoundaryGroup[i].size();++j){
		   //big assumption on the nature of elementIDs here...
                   addFace(tempElementVector[boundarFaces[facesForEachBoundaryGroup[i][j]].elementNum],boundarFaces[facesForEachBoundaryGroup[i][j]].localFaceIndex,NULL,0,Geometry::OPEN_BC);
	       }
	    }else if(centaurBCType<5001){
	       cout<<"outlet pipe boundary for group "<<i<<" assigned as OPEN_BC"<<endl;
	       for(int j=0;j<facesForEachBoundaryGroup[i].size();++j){
		   //big assumption on the nature of elementIDs here...
                   addFace(tempElementVector[boundarFaces[facesForEachBoundaryGroup[i][j]].elementNum],boundarFaces[facesForEachBoundaryGroup[i][j]].localFaceIndex,NULL,0,Geometry::OPEN_BC);
	       }
	    }else if(centaurBCType<6001){
	       cout<<"farfield boundary for group "<<i<<" assigned as OPEN_BC"<<endl;
	       for(int j=0;j<facesForEachBoundaryGroup[i].size();++j){
		   //big assumption on the nature of elementIDs here...
                   addFace(tempElementVector[boundarFaces[facesForEachBoundaryGroup[i][j]].elementNum],boundarFaces[facesForEachBoundaryGroup[i][j]].localFaceIndex,NULL,0,Geometry::OPEN_BC);
	       }
	    }else if(centaurBCType<7001){
	       cout<<"periodic boundary for group "<<i<<" ignored for being internal; node connections will be assigned later"<<endl;
	    }else if(centaurBCType<8001){
	       cout<<"shadow boundary for group "<<i<<" ignored for being internal; node connections will be assigned later"<<endl;
	    }else if(centaurBCType<8501){
	       cout<<"interface boundary for group "<<i<<" ignored for being internal"<<endl;
	    }else if(centaurBCType<9001){
	       cout<<"wake boundary for group "<<i<<" assigned as OPEN_BC"<<endl;
	       for(int j=0;j<facesForEachBoundaryGroup[i].size();++j){
		   //big assumption on the nature of elementIDs here...
                   addFace(tempElementVector[boundarFaces[facesForEachBoundaryGroup[i][j]].elementNum],boundarFaces[facesForEachBoundaryGroup[i][j]].localFaceIndex,NULL,0,Geometry::OPEN_BC);
	       }
	    }else if(centaurBCType<10001){
	       cout<<"moving wall boundary for group "<<i<<" assigned as WALL_BC"<<endl;
	       for(int j=0;j<facesForEachBoundaryGroup[i].size();++j){
		   //big assumption on the nature of elementIDs here...
                   addFace(tempElementVector[boundarFaces[facesForEachBoundaryGroup[i][j]].elementNum],boundarFaces[facesForEachBoundaryGroup[i][j]].localFaceIndex,NULL,0,Geometry::WALL_BC);
	       }
	    }else{
	       cout<<"alternative boundary condition for group "<<i<<" assigned as WALL_BC"<<endl;
	       for(int j=0;j<facesForEachBoundaryGroup[i].size();++j){
		   //big assumption on the nature of elementIDs here...
                   addFace(tempElementVector[boundarFaces[facesForEachBoundaryGroup[i][j]].elementNum],boundarFaces[facesForEachBoundaryGroup[i][j]].localFaceIndex,NULL,0,Geometry::WALL_BC);
	       }
	    }
	    cout<<"total number of boundary faces: "<<faces_.size()<<endl;
	}
	centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	
	//This is where centaur tells the names of all the boundary groups
        centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	for(int i=0;i<numberOfBoundaryGroups;++i){
	    centaurFile.read(reinterpret_cast<char*>(&nameOfBoundaryCondition[0]), sizeof(nameOfBoundaryCondition));
	    cout<<"boundary condition "<<i<<" is called "<<nameOfBoundaryCondition<<endl;
	}
	centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	
	//Then comes periodic boundary information
	//file versions 3 and greater store some extra information that hpGEM will be constructing itself
	//this extra information mangles the reading of the usefull information a bit
	double transformationData[16];
	int matchingNodes[2];
	int numberOfPeriodicNodes;
	
	if(centaurFileType>3){
	    int numberOfPeriodicTransformations;
	    centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
	    centaurFile.read(reinterpret_cast<char*>(&numberOfPeriodicTransformations), sizeof(numberOfPeriodicTransformations));
	    cout<<"There are "<<numberOfPeriodicTransformations<<" periodic boundary -> shadow boundary transformation(s)"<<endl;
	    centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
	    if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	    for(int i=0;i<numberOfPeriodicTransformations;++i){
	        //information on how to do the transformation can be computed later so just throw it away now
		centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
		centaurFile.read(reinterpret_cast<char*>(&ijunk), sizeof(ijunk));
		centaurFile.read(reinterpret_cast<char*>(&transformationData[0]), sizeof(transformationData));
		centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
		if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
		centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
		centaurFile.read(reinterpret_cast<char*>(&ijunk), sizeof(ijunk));
		centaurFile.read(reinterpret_cast<char*>(&transformationData[0]), sizeof(transformationData));
		centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
		if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	        
	        
	        
	        
	        //now read the amount of periodic nodes
		centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
		centaurFile.read(reinterpret_cast<char*>(&numberOfPeriodicNodes), sizeof(numberOfPeriodicNodes));
		cout<<"transformation group "<<i<<" contains "<<numberOfPeriodicNodes<<" node -> node matching(s)"<<endl;
		centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
		if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
		
		//and the actual pairing information
		centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
		for(int j=0;j<numberOfPeriodicNodes;j++){
		    centaurFile.read(reinterpret_cast<char*>(&matchingNodes[0]), sizeof(matchingNodes));
		    
		    ///\bug for EXTREMELY coarse meshes this will destroy the distinction between faces on the boundary of the domain. Workaround: use at least 3 nodes per direction on each face.
		    listOfElementsForEachNode[matchingNodes[0]-1 ].merge(listOfElementsForEachNode[matchingNodes[1]-1 ]);
		    listOfElementsForEachNode[matchingNodes[0]-1].unique();
		    listOfElementsForEachNode[matchingNodes[1]-1 ]=listOfElementsForEachNode[matchingNodes[0]-1 ];		    
		}
		centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
		if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	    }	    
	}else{
	        //now read the amount of periodic nodes
		centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
		centaurFile.read(reinterpret_cast<char*>(&numberOfPeriodicNodes), sizeof(numberOfPeriodicNodes));
		cout<<"the transformation group contains "<<numberOfPeriodicNodes<<" node -> node matching(s)"<<endl;
		centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
		if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
		
		//and the actual pairing information
		centaurFile.read(reinterpret_cast<char*>(&sizeOfLine), sizeof(sizeOfLine));
		for(int j=0;j<numberOfPeriodicNodes;j++){
		    centaurFile.read(reinterpret_cast<char*>(&matchingNodes[0]), sizeof(matchingNodes));
		    
		    listOfElementsForEachNode[matchingNodes[0] ].merge(listOfElementsForEachNode[matchingNodes[1] ]);
		    listOfElementsForEachNode[matchingNodes[1] ]=listOfElementsForEachNode[matchingNodes[0] ];		    
		}
		centaurFile.read(reinterpret_cast<char*>(&checkInt), sizeof(checkInt));
		if (checkInt!=sizeOfLine) {std::cerr << "Error in centaur file " << std::endl; return;}
	}
	
	cout<<"begin constructing internal faces"<<endl;
	constructInternalFaces(listOfElementsForEachNode,tempElementVector);	
        
	
    }else{
      
      std::cerr << "Incorrect mesh file. This mesh appears to contain two DIMensional data" << std::endl;
        
    }
}

template<unsigned int DIM>
void MeshManipulator<DIM>::findElementNumber(std::list<int>& a, std::list<int>& b, std::list<int>& c, int aNumber, int bNumber, int cNumber, std::list<int>& notOnFace, HalfFaceDescription& face, std::vector<Element<DIM>*>& vectorOfElements)
{
    //step 1: the element should be connected to all of the given nodes
        
    std::list<int>::iterator aEntry(a.begin()), bEntry(b.begin()), cEntry(c.begin()), otherEntry(notOnFace.begin());
    
    //assumes the lists are already sorted
    while(!(*aEntry==*bEntry && *aEntry==*cEntry && *aEntry==*otherEntry)){
        if(*aEntry    <*bEntry    ){aEntry++;}
        if(*bEntry    <*cEntry    ){bEntry++;}
        if(*cEntry    <*otherEntry){cEntry++;}
        if(*otherEntry<*aEntry    ){otherEntry++;}
        assert(aEntry!=a.end());
	assert(bEntry!=b.end());
	assert(cEntry!=c.end());
	assert(otherEntry!=notOnFace.end());
    }
    
    face.elementNum=*aEntry;
    
    //step 2: find the actual face
    std::vector<unsigned int> facenumbers;
    for(int i=0;i<vectorOfElements[*aEntry]->getPhysicalGeometry()->getNrOfFaces();++i){
	bool aFound(false),bFound(false),cFound(false);
	vectorOfElements[*aEntry]->getPhysicalGeometry()->getGlobalFaceNodeIndices(i,facenumbers);
	for(int j=0;j<facenumbers.size();++j){
	    aFound|=(facenumbers[j]==aNumber);
	    bFound|=(facenumbers[j]==bNumber);
	    cFound|=(facenumbers[j]==cNumber);
	}
	if(aFound&&bFound&&cFound){
	    face.localFaceIndex=i;
	    return;
	}
    }
    throw "The centaur mesh file contains a face with wrong bounding nodes!";
}

//in principle this will also work for 2D but fixing DIM makes the code a lot easier to read
template<unsigned int DIM>
void MeshManipulator<DIM>::constructInternalFaces(std::vector<std::list<int> >& listOfElementsForEachNode,std::vector<Element<DIM>*>& vectorOfElements)
{
    for(typename std::list<Element<DIM>* >::iterator currentElement=elements_.begin();currentElement!=elements_.end();++currentElement){
        for(int currentFace=0;currentFace<(*currentElement)->getPhysicalGeometry()->getNrOfFaces();++currentFace){
	  
	    //step 1: find the other element
	    std::list<int>::iterator elementListEntry[3];
	    std::vector<unsigned int> faceNode;
	    (*currentElement)->getPhysicalGeometry()->getGlobalFaceNodeIndices(currentFace,faceNode);
	    for(int i=0;i<3;++i){
	        elementListEntry[i]=listOfElementsForEachNode[faceNode[i]].begin();
	    }
	    while(!(*elementListEntry[0]==*elementListEntry[1] && *elementListEntry[0]==*elementListEntry[2])){
	        if(*elementListEntry[0]<*elementListEntry[1]){elementListEntry[0]++;}
	        if(*elementListEntry[1]<*elementListEntry[2]){elementListEntry[1]++;}
	        if(*elementListEntry[2]<*elementListEntry[0]){elementListEntry[2]++;}
	    }
	    if(*elementListEntry[0]!=(*currentElement)->getID()-1){
		//cout<<"found candidate matching "<<*elementListEntry[0]<<"->"<<(*currentElement)->getID()-1<<endl;
	        
	        //step 2: find the other face
	        //the trick used for the boundary faces wont work because of periodic 'internal' faces
	        for(int otherFace=0;otherFace<vectorOfElements[*elementListEntry[0]]->getPhysicalGeometry()->getNrOfFaces();++otherFace){
		    
		    //but we can just find the original element back
		    std::list<int>::iterator otherElementListEntry[3];
		    std::vector<unsigned int> otherFaceNode;
		    vectorOfElements[*elementListEntry[0]]->getPhysicalGeometry()->getGlobalFaceNodeIndices(otherFace,otherFaceNode);
		    for(int i=0;i<3;++i){
			otherElementListEntry[i]=listOfElementsForEachNode[otherFaceNode[i]].begin();
		    }
		    while(!(*otherElementListEntry[0]==*otherElementListEntry[1] && *otherElementListEntry[0]==*otherElementListEntry[2])){
			if(*otherElementListEntry[0]<*otherElementListEntry[1]){otherElementListEntry[0]++;}
			if(*otherElementListEntry[1]<*otherElementListEntry[2]){otherElementListEntry[1]++;}
			if(*otherElementListEntry[2]<*otherElementListEntry[0]){otherElementListEntry[2]++;}
		    }
		    //cout<<"finding the other element again("<<*otherElementListEntry[0]<<")"<<endl;
		    //assert(*otherElementListEntry[0]==*elementListEntry[0]);//for other faces this element may not be the first match found
		    if(*otherElementListEntry[0]==(*currentElement)->getID()-1 && *otherElementListEntry[0]==*otherElementListEntry[1] && *otherElementListEntry[0]==*otherElementListEntry[2]){
			//cout<<"found the fist element("<<*otherElementListEntry[0]<<")"<<endl;
		        addFace(*currentElement,currentFace,vectorOfElements[*elementListEntry[0]],otherFace);
		    }else{
			otherElementListEntry[0]++;
			while(!(*otherElementListEntry[0]==*otherElementListEntry[1] && *otherElementListEntry[0]==*otherElementListEntry[2]) &&
				otherElementListEntry[0]!=listOfElementsForEachNode[otherFaceNode[0]].end() &&
				otherElementListEntry[1]!=listOfElementsForEachNode[otherFaceNode[1]].end() &&
				otherElementListEntry[2]!=listOfElementsForEachNode[otherFaceNode[2]].end() ){
			    if(*otherElementListEntry[0]<*otherElementListEntry[1]){otherElementListEntry[0]++;}
			    if(*otherElementListEntry[1]<*otherElementListEntry[2]){otherElementListEntry[1]++;}
			    if(*otherElementListEntry[2]<*otherElementListEntry[0]){otherElementListEntry[2]++;}
			}
			if(*otherElementListEntry[0]==(*currentElement)->getID()-1 && *otherElementListEntry[0]==*otherElementListEntry[1] && *otherElementListEntry[0]==*otherElementListEntry[2]){
			    //cout<<"found the fist element("<<*otherElementListEntry[0]<<")"<<endl;
			    addFace(*currentElement,currentFace,vectorOfElements[*elementListEntry[0]],otherFace);
			}
		    }
		}
	      
	    }else{
	        //this face was already treated or it is a boundary face
	    }
	}
    }
    cout<<"Total number of Faces: "<<faces_.size()<<endl;
}


/// \bug does not do the bc flags yet or periodic mesh reads
/// \bug this routine implicitly requires 0 based global element numbering and will only work if list::iterator iterates the element list in the same order as the ordering of the element IDs
template<unsigned int DIM>
void MeshManipulator<DIM>::faceFactory()
{

    //This will store the number of faces.
    unsigned int numOfFaces;
    
    //Half face holds global node number of both nodes, the element number and the local face number in that element.
    HalfFaceDescription halfFace;
    
    //List to hold the half faces. List is used for the quick sorting of the halfFaces
    std::list<HalfFaceDescription> halfFaceList;
    VectorOfElementPtrT tempElementVector(elements_.size());
    
    VectorOfPointIndicesT globalFaceIndexes;
    int insertposition=0;
        
        //first loop over the elements reading in all the data of the half faces
        unsigned int elementID=0;
        for (typename ListOfElementsT::iterator it=elements_.begin(); it !=elements_.end(); ++it)
        {
            const Geometry::PhysicalGeometry<DIM>* const myPhysicalGeometry = (*it)->getPhysicalGeometry();
            const Geometry::ReferenceGeometry<DIM>* const myReferenceGeometry = (*it)->getReferenceGeometry();
            tempElementVector[elementID]=&(**it);
            
            numOfFaces = myReferenceGeometry->getNrOfCodim1Entities();
            
            //Loop over the faces on the element
            for (int i=0; i<numOfFaces; i++)
            {
	        halfFace.nodeList.clear();
		halfFace.nodeList.resize(DIM);
                myPhysicalGeometry->getGlobalFaceNodeIndices(i,globalFaceIndexes);
                
                halfFace.elementNum=elementID;
                halfFace.localFaceIndex=i;
                //make sure the node numbers in the face description are sorted
		for(int j=0; j<globalFaceIndexes.size();++j){
		    while(insertposition<DIM&&halfFace.nodeList[insertposition]<globalFaceIndexes[j]){
			halfFace.nodeList[insertposition]=halfFace.nodeList[insertposition+1];
			++insertposition;
		    }
		    if(insertposition<=DIM){
			halfFace.nodeList[insertposition-1]=globalFaceIndexes[j];
		    }
		    insertposition=0;
		}	
                //Make sure the firstNode number is the smaller of the two global node number, required for the sorting.
//                 if (globalFaceIndexes[0]<globalFaceIndexes[1])
//                 {
//                     halfFace.firstNode=globalFaceIndexes[0];
//                     halfFace.secondNode=globalFaceIndexes[1];
//                 }
//                 else 
//                 {
//                     halfFace.firstNode=globalFaceIndexes[1];
//                     halfFace.secondNode=globalFaceIndexes[0];
//                 }
                
                //Add the halfFace to the list
                halfFaceList.push_front(halfFace);

            }
                
            elementID++;
        }
        
        //Now sort the list on the two value (firstNode, secondNode) so it order and the two pair internal halfFaces are next to each other
        halfFaceList.sort(compareHalfFace);
        
        //Writing out for testing
        for (typename std::list<HalfFaceDescription>::const_iterator cit=halfFaceList.begin(); cit !=halfFaceList.end(); ++cit)
        {
            //std::cout << (*cit).elementNum << " " << (*cit).localFaceIndex<< " "<< (*cit).nodeList[0]<<" " << (*cit).nodeList[1]<<" "<<  std::endl;
        }
        
        //Now create the faces
        HalfFaceDescription current;
        HalfFaceDescription next;
        
        //Loop over all the half faces
         for (typename std::list<HalfFaceDescription>::const_iterator cit=halfFaceList.begin(); cit !=halfFaceList.end(); ++cit)
         {
             //For the first halfFace just read it in, as we cannot tell if is an internal or external yet.
             if (cit==halfFaceList.begin())
             {
                 current=*cit;
             }
             else 
             {
                 next=*cit;
                 
                 //This is an interal face
                 if ((current.nodeList[0]==next.nodeList[0]) && (current.nodeList[1]==next.nodeList[1]) && ( (DIM<3) || (current.nodeList[2]==next.nodeList[2]) )) 
                 {
                     
                     addFace(tempElementVector[current.elementNum],current.localFaceIndex,tempElementVector[next.elementNum],next.localFaceIndex);
                     //jump a face
                     ++cit;
                     current=*cit;
                     
                     //There was only one face left, so it now the end of the list
                     if (cit==halfFaceList.end())
                     {
                         break;
                     }
                 }
                 else //it is a boundary face 
                 {
                     addFace(tempElementVector[current.elementNum],current.localFaceIndex,NULL,0,Geometry::OPEN_BC);
                     current=next;
                 }

             }
         }
    cout<<"Total number of Faces: "<<faces_.size()<<endl;
    }

      //---------------------------------------------------------------------
    template<unsigned int DIM>
    int MeshManipulator<DIM>::getNumberOfMeshes() const 
    { 
        return vecOfElementTree_.size();
    }

    //! Create a new (empty) mesh-tree.
    template<unsigned int DIM>
    void MeshManipulator<DIM>::createNewMeshTree()
    {
      vecOfElementTree_.push_back(new ElementLevelTreeT);
      vecOfFaceTree_.push_back(new FaceLevelTreeT);
      ++numMeshTree_;
      setActiveMeshTree(numMeshTree_ - 1);
    }

    //! Get the element container of a specific mesh-tree.
    template<unsigned int DIM>
    typename MeshManipulator<DIM>::ElementLevelTreeT*
    MeshManipulator<DIM>::ElCont(int meshTreeIdx) const
    {
        int onIndex;
        if ((meshTreeIdx >= 0) && (meshTreeIdx < numMeshTree_))
        {
          onIndex = meshTreeIdx;
        }
        else if ((meshTreeIdx == -1) && (activeMeshTree_ >= 0))
        {
          onIndex = activeMeshTree_;
        }
        else
          throw "MeshManipulator::ElCont(): invalid mesh-tree index or no active mesh-tree.";
        
        return vecOfElementTree_[onIndex];
    }

    //! Get the face container of a specific mesh-tree.
    template<unsigned int DIM>
    typename MeshManipulator<DIM>::FaceLevelTreeT* 
    MeshManipulator<DIM>::FaCont(int meshTreeIdx) const
    {
        int onIndex;
        if ((meshTreeIdx >= 0) && (meshTreeIdx < numMeshTree_))
        {
          onIndex = meshTreeIdx;
        }
        else if ((meshTreeIdx == -1) && (activeMeshTree_ >= 0))
        {
          onIndex = activeMeshTree_;
        }
        else
          throw "MeshManipulator::FaCont(): invalid mesh-tree index or no active mesh-tree.";
        
        return vecOfFaceTree_[onIndex];
    }

    //! Some mesh generator: centaur / rectangular / triangle / tetrahedra / triangular-prism.
    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::someMeshGenerator(int meshTreeIdx)
    {
        int onIndex;
        if ((meshTreeIdx >= 0) && (meshTreeIdx < numMeshTree_))
        {
          onIndex = meshTreeIdx;
        }
        else if ((meshTreeIdx == -1) && (activeMeshTree_ >= 0))
        {
          onIndex = activeMeshTree_;
        }
        else
          throw "MeshManipulator::someMeshGenerator(): invalid mesh-tree index or no active mesh-tree.";
          
        const int numberOfElement = 1 + (rand() % 10);
        const int startElementId = (onIndex+1)*1000;
        for (int id=startElementId; id<startElementId+numberOfElement; ++id)
        {
          ElementT el(id);
          vecOfElementTree_[onIndex]->addEntry(el);
        }

        const int numberOfFace = 1 + (rand() % 10);
        const int startFaceId = (onIndex+1)*1000;
        for (int id=startFaceId; id<startFaceId+numberOfFace; ++id)
        {
          FaceT fa(id);
          vecOfFaceTree_[onIndex]->addEntry(fa);
        }
    }

    //! Set active mesh-tree.
    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::setActiveMeshTree(unsigned int meshTreeIdx)
    {
        if (meshTreeIdx < numMeshTree_)
            activeMeshTree_ = meshTreeIdx;
        else
            throw "MeshManipulator<DIM>::setActiveMeshTree(): invalid mesh-tree index.\n";
    }

    //! Get active mesh-tree index.
    template<unsigned int DIM>
    int
    MeshManipulator<DIM>::getActiveMeshTree() const
    {
        return activeMeshTree_;
    }

    //! Reset active mesh-tree.
    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::resetActiveMeshTree()
    {
        activeMeshTree_ = -1;
    }

    //! Get maximum h-level of a specific mesh-tree.
    template<unsigned int DIM>
    unsigned int 
    MeshManipulator<DIM>::getMaxLevel(int meshTreeIdx) const
    {
        int onIndex;
        if ((meshTreeIdx >= 0) && (meshTreeIdx < numMeshTree_))
        {
          onIndex = meshTreeIdx;
        }
        else if ((meshTreeIdx == -1) && (activeMeshTree_ >= 0))
        {
          onIndex = activeMeshTree_;
        }
        else
          throw "MeshManipulator::getMaxLevel(): invalid mesh-tree index or no active mesh-tree.";
        
        return vecOfElementTree_[onIndex].getMaxLevel();
    }

    //! Set active level of a specific mesh-tree.
    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::setActiveLevel(unsigned int meshTreeIdx, int level)
    {
        int onIndex;
        if ((meshTreeIdx >= 0) && (meshTreeIdx < numMeshTree_))
        {
          onIndex = meshTreeIdx;
        }
        else if ((meshTreeIdx == -1) && (activeMeshTree_ >= 0))
        {
          onIndex = activeMeshTree_;
        }
        else
          throw "MeshManipulator::setActiveLevel(): invalid mesh-tree index or no active mesh-tree.";

        if ((level >= 0) && (level <= vecOfElementTree_[onIndex].getMaxLevel()))
        {
          vecOfElementTree_[onIndex].setActiveLevel(level);
          vecOfFaceTree_[onIndex].setActiveLevel(level);
        }
        else
          throw "MeshManipulator::setActiveLevel(): invalid level.";
        
    }

    //! Get active level of a specific mesh-tree.
    template<unsigned int DIM>
    int
    MeshManipulator<DIM>::getActiveLevel(int meshTreeIdx) const
    {
        int onIndex;
        if ((meshTreeIdx >= 0) && (meshTreeIdx < numMeshTree_))
        {
          onIndex = meshTreeIdx;
        }
        else if ((meshTreeIdx == -1) && (activeMeshTree_ >= 0))
        {
          onIndex = activeMeshTree_;
        }
        else
          throw "MeshManipulator::getActiveLevel(): invalid mesh-tree index or no active mesh-tree.";

        return vecOfElementTree_[onIndex].getActiveLevel();
    }

    //! Reset active level of a specific mesh-tree.
    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::resetActiveLevel(int meshTreeIdx)
    {
        int onIndex;
        if ((meshTreeIdx >= 0) && (meshTreeIdx < numMeshTree_))
        {
          onIndex = meshTreeIdx;
        }
        else if ((meshTreeIdx == -1) && (activeMeshTree_ >= 0))
        {
          onIndex = activeMeshTree_;
        }
        else
          throw "MeshManipulator::resetActiveLevel(): invalid mesh-tree index or no active mesh-tree.";

        vecOfElementTree_[onIndex].resetActiveLevel();
        vecOfFaceTree_[onIndex].resetActiveLevel();
    }

    //! Duplicate mesh contents including all refined meshes.
    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::duplicate(unsigned int fromMeshTreeIdx, unsigned int toMeshTreeIdx, unsigned int upToLevel)
    {
        
    }

    //! Refine a specific mesh-tree.
    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::doRefinement(unsigned int meshTreeIdx, int refinementType)
    {
        int level = getMaxLevel(meshTreeIdx);
        setActiveLevel(meshTreeIdx,level);

        if (refinementType == -1)  
        {
            // elements should have been flagged before calling this
            std::cout << "MeshManipulator<" << DIM << ">::doRefinement(Mesh(" << meshTreeIdx << "))\n";
            doElementRefinement(meshTreeIdx);
            doFaceRefinement(meshTreeIdx);
            for (ElementIteratorT it=ElCont(meshTreeIdx).beginLevel(level); it != ElCont(meshTreeIdx).end(); ++it)
            {
                // reset element's marking for refinement
    //           it->unsetRefineType();
    //           it->setBeingRefinedOff();
            }
        }
        else
        {
            // apply the uniform mesh refinement
            for (ElementIteratorT it=ElCont(meshTreeIdx).beginLevel(level); it != ElCont(meshTreeIdx).end(); ++it)
            {
                // mark element for refinement
                it->setRefineType(refinementType);
            }

            std::cout << "MeshManipulator<" << DIM << ">::doRefinement(" << meshTreeIdx << "," << refinementType << ")\n";
            doElementRefinement(meshTreeIdx);
            doFaceRefinement(meshTreeIdx);
            for (ElementIteratorT it=ElCont(meshTreeIdx).beginLevel(level); it != ElCont(meshTreeIdx).end(); ++it)
            {
                // reset element's marking for refinement
    //           it->unsetRefineType();
    //           it->setBeingRefinedOff();
            }
        }
        
        level = getMaxLevel(meshTreeIdx);
        setActiveLevel(meshTreeIdx,level);
    }

    //! Do refinement on the elements.
    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::doElementRefinement(unsigned int meshTreeIdx)
    {
        unsigned int needDummyFaceOnLevel = 0;
        std::vector<ElementT*> vecElementsToRefined;  // list of unrefined elements
        for (ElementIteratorT el=ElCont(meshTreeIdx).begin(); el != ElCont(meshTreeIdx).end(); ++el)
        {
            Geometry::RefinementGeometry<DIM>* RG = el->getRefinementGeometry();
            
            int refineType;
    //         refineType = el->getRefineType();   // TODO: add this to Element

    /*        

            if (refineType  < 0)   // not refined, just duplicate the element
            {
                vecElementsToRefined.push_back(&(*el));
                needDummyFaceOnLevel = el->getLevel()+1;
                continue;
            }

            // ********* Add new nodes to the container
            //----------------------------------------
            RG->getAllNodes(refineType, VectorOfPointPhysicalsT& nodes);
            unsigned int nrNewNodes = nrOfNewNodes(refineType);
            unsigned int nrAllNodes = nodes.size();
            unsigned int nrOldNodes = nrAllNodes - nrNewNodes;
            
            // get physical nodes of this elements
            VectorOfPhysicalPointsT nodesPhys;    // vector of physical points
            VectorOfPointIndexesT   nodesIdx;     // vector of global indices
            for (PointIndexT j=0; j<nrVertices; ++j)
            {
                PointPhysicalT p;       // a physical node
                PointIndexT pIdx;       // a global index
                PG->getVertexPoint (j, p);
                pIdx = PG->getVertexIndex (j);
                nodesPhys.push_back(p);
                nodesIdx.push_back(pIdx);
            }

            // get new physical nodes due to the refinement, and add them
            // up to the nodes collection of this element
            PG->NewPhysicalNodes(refineType, nodesPhys);

            // add the new physical nodes into the nodes container
            // and get their global indices
            for (LocalPointIndexT j=nrVertices; j<nodesPhys.size(); ++j)
            {
                int pIdx = PCptr->getGlobalIndex(nodesPhys[j]);
                if (pIdx >= 0)
                {
                    // it's already exist
                    nodesIdx.push_back(pIdx);
                }
                else
                {
                    // it's not exist yet.  Add it to the node container
                    MeshBase<DIM>::AllNodes.addRoot(nodesPhys[j]);
                    nodesIdx.push_back(PCptr->size()-1);
                }
            }
            // Now we already have all nodes: being used by this element and
            // to be used by the new sub-elements.

            // ********* Add sub-elements to the container
            //----------------------------------------
            unsigned int nrNewElements = PG->nrOfSubElements(refineType);
            std::vector<ElementBase*> vecSubElements;
            for (unsigned int j=0; j<nrNewElements; ++j)
            {
                VectorOfPointIndexesT localNodeIndexes;
                PG->SubElementNodeIndexes(refineType, j, localNodeIndexes);
                ElementDescriptor elDescriptor(localNodeIndexes.size());
                for (VectorOfPointIndexesT k=0; k<localNodeIndexes.size(); ++k)
                {
                    elDescriptor.addNode(nodesIdx[localNodeIndexes[k]]);
                }

    //             ElementFactory<DIM> elFactory(*this);
    //             ElementT *elem = elFactory.makeElement(&elDescriptor);
    //             elem->setRefinementType(refineType);
    //             vecSubElements.push_back(elem);
            }
            
            // Add the sub-elements as the children of this element
            ElementIteratorT elIt = ElCont(meshTreeIdx).addChildren(el, vecSubElements);
            
            // Clear up the storage
            nodesPhys.clear(); // clear vector of physical points
            nodesIdx.clear();  // clear vector of global index

            
            // ********* Add sub-Internal Faces to the container
            //----------------------------------------
            VectorOfPointIndexesT elementIdx1;
            VectorOfPointIndexesT elementIdx2;
            VectorOfPointIndexesT localFaceIdx1;
            VectorOfPointIndexesT localFaceIdx2;
            PG->AdjacentSubElementsPairs(refineType, elementIdx1,localFaceIdx1, elementIdx2,localFaceIdx2);
            unsigned int nrOfNewFaces = elementIdx1.size();

            std::vector<FaceT*> vecSubFaces;
            for (unsigned int j=0; j<nrOfNewFaces; ++j)
            {
                // Create the face, connecting the two elements
                ElementT* el1 = vecSubElements[elementIdx1[j]];
                ElementT* el2 = vecSubElements[elementIdx2[j]];
                FaceT* iFace = new FaceT( el1, localFaceIdx1[j], el2, localFaceIdx2[j]);
                vecSubFaces.push_back(iFace);
            }
            // Add them to the container as the children of a dummy face
            FaceIteratorT fa = FaCont(meshTreeIdx).getDummyFace(el->getLevel());

            FaCont(meshTreeIdx).addChildren(fa, vecSubFaces);
            vecSubFaces.clear();
            
            // Mark that this element was just refined
            el->setJustRefined();
            vecSubElements.clear();
    */
        } // end of loop over elements

        if (needDummyFaceOnLevel)
        {
            FaCont(meshTreeIdx).getDummyFace(needDummyFaceOnLevel);
        }
        
        std::vector<ElementT*> vecEmpty;  // empty list of new elements
        while(!vecElementsToRefined.empty()) 
        {
            ElementT *elem = vecElementsToRefined.back();
            ElementIteratorT elIt = ElCont(meshTreeIdx).addChildren(elem->getIterator(), vecEmpty);
            vecElementsToRefined.pop_back();
        }
    }

    //! Do refinement on the faces.
    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::doFaceRefinement(unsigned int meshTreeIdx)
    {
        
    }

    //! Check whether the two elements may be connected by a face or not.
    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::pairingCheck(const ElementIteratorT elL, unsigned int locFaceNrL,
                                       const ElementIteratorT elR, unsigned int locFaceNrR,
                                       int& pairingValue, bool& sizeOrder)
    // pairingValue: 0=not match, 1=partial match, 2=perfect match
    // sizeOrder: true = LR,  false = RL
    {
        // get node numbers from left (and right) sides
        std::vector<PointIndexT> globNodesL;
        std::vector<PointIndexT> globNodesR;
        
        const Geometry::PhysicalGeometry<DIM>* const leftPG = elL->getPhysicalGeometry();
        Geometry::PhysicalGeometry<DIM>* rightPG;
        
        leftPG->getFaceNodeIndices(locFaceNrL,globNodesL);
        if (&*elR != 0)
        {
            rightPG = elR->getPhysicalGeometry();
            rightPG->getFaceNodeIndices(locFaceNrR,globNodesR);
        }

        // store them as sets
        std::set<PointIndexT> setL(globNodesL.data(), globNodesL.data() + globNodesL.size());
        std::set<PointIndexT> setR(globNodesR.data(), globNodesR.data() + globNodesR.size());

        if (setL == setR)
        {
            // nodes of the faces are match
            pairingValue = 2;
            sizeOrder = true;
            return;
        }
        
        std::set<PointIndexT> commNodes;
        std::set_intersection( setL.begin(), setL.end(), setR.begin(), setR.end(), std::inserter(commNodes, commNodes.begin()) );

        unsigned int nrNodesL = globNodesL.size();
        unsigned int nrNodesR = globNodesR.size();
        if (nrNodesL != nrNodesR)
        {
            pairingValue = 0;
            return;
        }
        
        std::set<PointIndexT> diffNodesL;
        std::set_difference( setL.begin(), setL.end(), commNodes.begin(), commNodes.end(), std::inserter(diffNodesL, diffNodesL.end()) );

        std::set<PointIndexT> diffNodesR;
        std::set_difference( setR.begin(), setR.end(), commNodes.begin(), commNodes.end(), std::inserter(diffNodesR, diffNodesR.end()) );

        typedef std::set<PointIndexT>::iterator SetIterType;

        // do collinear test for all possible pairs
        pairingValue = 1;
        
        // order of face sizes: true = LR;  false = RL
        sizeOrder = true;
        
        // loop until empty or until the faces proved not collinear
        while (!diffNodesL.empty())
        {
            SetIterType itDiffL=diffNodesL.begin();
            PointPhysicalT ppL;
            leftPG->getGlobalNodeCoordinates(*itDiffL, ppL);

            bool foundCollinear(false);
            SetIterType itDiffL2;
            SetIterType itDiffR;
            SetIterType itDiffR2;
            for (itDiffR=diffNodesR.begin(); itDiffR!=diffNodesR.end(); ++itDiffR)
            {
              PointPhysicalT ppR;
              rightPG->getGlobalNodeCoordinates(*itDiffR, ppR);

              if (commNodes.size() > 0)
              {
                for (SetIterType itCommon=commNodes.begin(); itCommon!=commNodes.end(); ++itCommon)
                {
                  PointPhysicalT pp0;
                  leftPG->getGlobalNodeCoordinates(*itCommon, pp0);

                  //-------------
                  // perform 3-nodes collinear test
                  PointPhysicalT dL(ppL-pp0);
                  PointPhysicalT dR(ppR-pp0);

                  double ratio = 0.;
                  unsigned int d;
                  for (d=0; d<DIM; ++d)
                  {
                    if (std::abs(dR[d])>Geometry::SmallerDoubleThanMinimalSizeOfTheMesh)
                    {
                      ratio = dL[d]/dR[d];
                      break;
                    }
                  }

                  if (ratio < 0.)
                  {
                    pairingValue = 0;
                    return;
                  }
                  
                  foundCollinear = true;
                  for (;d<DIM; ++d)
                  {
                    if (std::abs(dR[d])>Geometry::SmallerDoubleThanMinimalSizeOfTheMesh)
                    {
                      if (dL[d]/dR[d] < 0.)
                      {
                        foundCollinear = false;
                        break;
                      }

                      if (std::abs(dL[d]/dR[d] - ratio) > Geometry::SmallerDoubleThanMinimalSizeOfTheMesh)
                      {
                        foundCollinear = false;
                        break;
                      }
                    }
                  }

                  // order of face sizes: true = LR;  false = RL
                  sizeOrder = (sizeOrder && (ratio <= 1.0));
                  
                  // found a collinear nodes pair
                  if (foundCollinear) break;
                }  // for itCommon
              } // if (commNodes.size() > 0)
              else
              // no common nodes
              {
                for (itDiffL2=diffNodesL.begin(); itDiffL2!=diffNodesL.end(); ++itDiffL2)
                {
                  if ((itDiffL2 == itDiffL) || (itDiffL2 == itDiffR)) 
                      continue;
                  
                  PointPhysicalT ppL2;
                  leftPG->getGlobalNodeCoordinates(*itDiffL2, ppL2);

                  for (itDiffR2=diffNodesR.begin(); itDiffR2!=diffNodesR.end(); ++itDiffR2)
                  {
                    if ((*itDiffR2 == *itDiffR) || (*itDiffR2 == *itDiffL) || (*itDiffR2 == *itDiffL2)) 
                        continue;
                    
                    PointPhysicalT ppR2;
                    rightPG->getGlobalNodeCoordinates(*itDiffR2, ppR2);
                  
                    //-------------
                    // perform two 3-nodes pairs collinear test 
                    PointPhysicalT dL1(ppL-ppR);
                    PointPhysicalT dR1(ppL2-ppR);
                    
                    PointPhysicalT dL2(ppL2-ppR);
                    PointPhysicalT dR2(ppR2-ppR);
                    
                    PointPhysicalT dL3(ppL-ppR2);
                    PointPhysicalT dR3(ppR-ppR2);
                    
                    double ratio1 = 0.;
                    double ratio2 = 0.;
                    double ratio3 = 0.;
                    unsigned int d1;
                    unsigned int d2;
                    unsigned int d3;
                    for (d1=0; d1<DIM; ++d1)
                    {
                      if (std::abs(dR1[d1])>Geometry::SmallerDoubleThanMinimalSizeOfTheMesh)
                      {
                        ratio1 = dL1[d1]/dR1[d1];
                        break;
                      }
                    }

                    for (d2=0; d2<DIM; ++d2)
                    {
                      if (std::abs(dR2[d2])>Geometry::SmallerDoubleThanMinimalSizeOfTheMesh)
                      {
                        ratio2 = dL2[d2]/dR2[d2];
                        break;
                      }
                    }
                    
                    for (d3=0; d3<DIM; ++d3)
                    {
                      if (std::abs(dR3[d3])>Geometry::SmallerDoubleThanMinimalSizeOfTheMesh)
                      {
                        ratio3 = dL3[d3]/dR3[d3];
                        break;
                      }
                    }
                    
                    if ((ratio1 < 0) || (ratio2 < 0) || (ratio3 < 0))
                    {
                      pairingValue = 0;
                      return;
                    }

                    foundCollinear = true;
                    for (;d1<DIM; ++d1)
                    {
                      if (std::abs(dR1[d1])>Geometry::SmallerDoubleThanMinimalSizeOfTheMesh)
                      {
                        if (dL1[d1]/dR1[d1] < 0.)
                        {
                          pairingValue = 0;
                          return;
                        }

                        if (std::abs(dL1[d1]/dR1[d1] - ratio1) > Geometry::SmallerDoubleThanMinimalSizeOfTheMesh)
                        {
                          foundCollinear = false;
                          break;
                        }
                      }
                    }
                    
                    for (;(d2<DIM) && foundCollinear; ++d2)
                    {
                      if (std::abs(dR2[d2])>Geometry::SmallerDoubleThanMinimalSizeOfTheMesh)
                      {
                        if (dL2[d2]/dR2[d2] < 0.)
                        {
                          pairingValue = 0;
                          return;
                        }

                        if (std::abs(dL2[d2]/dR2[d2] - ratio2) > Geometry::SmallerDoubleThanMinimalSizeOfTheMesh)
                        {
                          foundCollinear = false;
                          break;
                        }
                      }
                    }
                    
                    for (;(d3<DIM) && foundCollinear; ++d3)
                    {
                      if (std::abs(dR3[d3])>Geometry::SmallerDoubleThanMinimalSizeOfTheMesh)
                      {
                        if (dL3[d3]/dR3[d3] < 0.)
                        {
                          pairingValue = 0;
                          return;
                        }

                        if (std::abs(dL3[d3]/dR3[d3] - ratio3) > Geometry::SmallerDoubleThanMinimalSizeOfTheMesh)
                        {
                          foundCollinear = false;
                          break;
                        }
                      }
                    }
                    
                    // found a collinear nodes pair
                    if (foundCollinear) break;     // itDiffR2 will be deleted
                  }
                  // found a collinear nodes pair
                  if (foundCollinear) break;     // itDiffL2 will be deleted
                }  // for itDiffL2
              }  //  else, no common nodes
             
              // found a collinear nodes pair
              if (foundCollinear) break;  // itDiffR will be deleted
            }  // for itDiffR

            if (foundCollinear) 
            // found a collinear nodes pair
            {
              if (commNodes.size() > 0)
              {
                diffNodesR.erase(itDiffR);
              }
              else
              {
                diffNodesL.erase(itDiffL2);
                diffNodesR.erase(itDiffR);
                diffNodesR.erase(itDiffR2);
              }
            }
            else
            // the faces are not collinear
            {
              pairingValue = 0;
              break;
            }
            
            diffNodesL.erase(itDiffL);
        }  // end while-loop
        
        if (!diffNodesR.empty())
          pairingValue = 0;

    }  // end pairingCheck

    //! Check whether the two elements may be connected by a face or not in periodic face case.
    template<unsigned int DIM>
    void
    MeshManipulator<DIM>::periodicPairingCheck(const FaceIteratorT fa, 
                                               const ElementIteratorT elL, unsigned int localFaceNrL,
                                               const ElementIteratorT elR, unsigned int localFaceNrR,
                                               int& pairingValue, bool& sizeOrder)
    {
        
    }
      //---------------------------------------------------------------------
}

#endif