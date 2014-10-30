/* 
 * File:   MpiContainer.hpp
 * Author: brinkf
 *
 * Created on October 30, 2014, 4:31 PM
 */

#ifndef MPICONTAINER_HPP
#define	MPICONTAINER_HPP

class MPIContainer {
public:
    MPIContainer& Instance(){
        static MPIContainer theInstance();
        return theInstance;
    }
    
    int getProcessorID();
    int getCommSize();
#if HPGEM_USE_MPI
    MPI::Intracomm& getComm();
    
    template<class T>
    void broadcast(T&);
    
    template<class T>
    void send(T&);
#endif
    
private:
    MPIContainer();
    MPIContainer(const MPIContainer& orig)=delete;
    virtual ~MPIContainer();

};

#endif	/* MPICONTAINER_HPP */

