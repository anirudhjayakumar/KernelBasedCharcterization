/*
 * FrameworkCommon.h
 *
 *  Created on: 09-Aug-2013
 *      Author: Anirudh Jayakumar (jayakumar.anirudh@gmail.com)
 */

#ifndef FRAMEWORKCOMMON_H_
#define FRAMEWORKCOMMON_H_

#define SUCCESS 0
#define FAIL    1


typedef int ReturnCode;
#include <queue>
#include <vector>
#include <string>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>


// for expression evaluation
const std::string NOOP("NOOP");
const std::string ZERO("ZERO");
const std::string ONE("ONE");
const std::string  OP_MUL("MUL");
const std::string  OP_ADD("ADD");
const std::string  OP_SUB("SUB");
const std::string  OP_DIV("DIV");

// MPI calls
const std::string strMPI_SEND("mpi_send");
const std::string strMPI_REDUCE("mpi_reduce");
const std::string strMPI_BCAST("mpi_bcast");
const std::string strMPI_ALLREDUCE("mpi_allreduce");
const std::string strMPI_ALLTOALL("mpi_alltoall");

#define MPI_ALLTOALL 		"MPI_ALLTOALL"
#define MPI_BROADCAST 		"MPI_BROADCAST"
#define MPI_LATENCY 		"MPI_LATENCY"
#define MPI_REDUCE  		"MPI_REDUCE"
#define MPI_ALLREDUCE 		"MPI_ALLREDUCE"
#define MPI_ASYNCRECV       "MPI_ASYNCRECV"

#define SEMI_DELIM ";"


typedef std::vector<std::string> VecStrings;
typedef std::vector<VecStrings>  VecVecStrings;
typedef std::vector<int> VecInts;
typedef std::vector<VecInts>  VecVecInts;


namespace GARLUTILS
{
template<typename T, typename Container=std::deque<T> >
class iterable_queue : public std::queue<T,Container>
{
public:
    typedef typename Container::iterator iterator;
    typedef typename Container::const_iterator const_iterator;

    iterator begin()
    {
        return this->c.begin();
    }
    iterator end()
    {
        return this->c.end();
    }
    const_iterator begin() const
    {
        return this->c.begin();
    }
    const_iterator end() const
    {
        return this->c.end();
    }
};


std::vector<std::string> Tokenize(std::string sText, std::string sToken);
std::string GetFileNameFromPath(const std::string &strFileName);
bool  IsAllTrue(const std::vector<bool> &vecs);
bool  IsAllFalse(const std::vector<bool> &vecs);
void  NextPermutation(std::vector< int >  &vecblockIndex,
                      const std::vector< int >  &vecblockSize);
}

#endif /* FRAMEWORKCOMMON_H_ */
