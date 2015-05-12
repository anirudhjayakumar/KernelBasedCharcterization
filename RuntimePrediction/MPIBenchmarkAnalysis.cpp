/*
 * MPIBenchmarkAnalysis.cpp
 *
 *  Created on: 22-Aug-2013
 *      Author: anirudhj
 */

#include "MPIBenchmarkAnalysis.h"
#include "tinyxml.h"
#include <iostream>
#include "FrameworkCommon.h"
#include "RInstance.h"
#include <boost/lexical_cast.hpp>
#include <RInside.h>
#define ELEMENT_MPIBENCHMARK "MPIBenchMark"
#define ELEMENT_ALLTOALL     "AlltoAll"
#define ELEMENT_PROC         "Proc"
#define ELEMENT_BROADCAST    "Broadcast"
#define ELEMENT_LATENCY      "Latency"
#define ELEMENT_REDUCE       "Reduce"
#define ELEMENT_ALLREDUCE    "AllReduce"
#define ELEMENT_2DLATENCY    "BiDirLatency"
#define ELEMENT_ASYNCRECV    "AsyncRecv"
#define ATTRIB_N			 "n"
#define ATTRIB_M			 "m"
#define ATTRIB_C			 "c"


using namespace std;

namespace Prediction
{

int CMPIBenchmarkAnalysis::Initialize(const std::string &xmlFile)
{
    m_sFolder = "/home/anirudhj/WORK-IISC/AppCharacterization/MPICharacterization/tyrone/";
    TiXmlDocument XmlDoc(xmlFile.c_str());
    if( !XmlDoc.LoadFile())
        {
            cout << "Loading xml failed" << endl;
            return FAIL;
        }
    TiXmlElement *pRootXml 		= XmlDoc.RootElement();
    TiXmlElement *pAllToAll 	= pRootXml->FirstChildElement(ELEMENT_ALLTOALL);
    TiXmlElement *pBrdcast 		= pRootXml->FirstChildElement(ELEMENT_BROADCAST);
    TiXmlElement *pLatency 		= pRootXml->FirstChildElement(ELEMENT_LATENCY);
    TiXmlElement *pReduce 		= pRootXml->FirstChildElement(ELEMENT_REDUCE);
    TiXmlElement *pAllReduce 	= pRootXml->FirstChildElement(ELEMENT_ALLREDUCE);
    TiXmlElement *pAsyncRecv 	= pRootXml->FirstChildElement(ELEMENT_ASYNCRECV);
    FillMap(m_mapNodeAllToAll, pAllToAll);
    FillMap(m_mapNodeBroadcast, pBrdcast);
    FillMap(m_mapNodeLatency, pLatency);
    FillMap(m_mapNodeReduce, pReduce);
    FillMap(m_mapNodeAllReduce, pAllReduce);
    FillMap(m_mapNodeAsyncRecv,pAsyncRecv);
    m_RInstance =  new RInside();
    return SUCCESS;
}

EMPIBenchmark CMPIBenchmarkAnalysis::GetMPIEnum(string sType)
{
    if ( sType.compare(MPI_ALLREDUCE) == 0)
        return e_AllReduce;
    else if ( sType.compare(MPI_BROADCAST) == 0)
        return e_Broadcast;
    else if ( sType.compare(MPI_LATENCY) == 0)
        return e_Latency;
    else if ( sType.compare(MPI_REDUCE) == 0)
        return e_Reduce;
    else if ( sType.compare(MPI_ALLREDUCE) == 0)
        return e_AllReduce;
    else if ( sType.compare(MPI_ASYNCRECV) == 0)
        return e_AsyncRecv;
    else
        return e_Invalid;
}

string CMPIBenchmarkAnalysis::GetString(EMPIBenchmark eType)
{
    switch (eType)
        {
        case e_AlltoAll:
        {
            return MPI_ALLTOALL;
            break;
        }
        case e_Broadcast:
        {
            return MPI_BROADCAST;
            break;
        }
        case e_Latency:
        {
            return MPI_LATENCY;
            break;
        }
        case e_Reduce:
        {
            return MPI_REDUCE;
            break;
        }
        case e_AllReduce:
        {
            return MPI_ALLREDUCE;
            break;
        }
        case e_AsyncRecv:
        {
            return MPI_ASYNCRECV;
            break;
        }
        default:
        {
            return "Unknown";
        }
        }
}

double CMPIBenchmarkAnalysis::GetTimeInSecs(const double buffSizeInBytes, int nprocs, const EMPIBenchmark eType)
{
    double m,c;
    double y;
    double x = buffSizeInBytes;
    switch(eType)
        {
        case e_AllReduce:
        {
            m = m_mapNodeAllReduce[nprocs].first;
            c = m_mapNodeAllReduce[nprocs].second;
            break;
        }
        case e_AlltoAll:
        {
            m = m_mapNodeAllToAll[nprocs].first;
            c = m_mapNodeAllToAll[nprocs].second;
            break;
        }
        case e_Broadcast:
        {
            m = m_mapNodeBroadcast[nprocs].first;
            c = m_mapNodeBroadcast[nprocs].second;
            break;
        }
        case e_Latency:
        {
            m = m_mapNodeLatency[nprocs].first;
            c = m_mapNodeLatency[nprocs].second;
            break;
        }
        case e_Reduce:
        {
            m = m_mapNodeReduce[nprocs].first;
            c = m_mapNodeReduce[nprocs].second;
            break;
        }
        case e_AsyncRecv:
        {
            m = m_mapNodeAsyncRecv[nprocs].first;
            c = m_mapNodeAsyncRecv[nprocs].second;
            break;
        }
        }

    y = (m * x) + c;
    return y;

    /*

    	double m,c;
    	double y;
    	double x = buffSizeInBytes;
    	string sFile("");
    	switch(eType)
    	{
    	case e_AllReduce:
    		{
    			sFile = m_sFolder + "allreduce/bench_" + boost::lexical_cast<string>(nprocs);
    			break;
    		}
    	case e_AlltoAll:
    		{
    			sFile = m_sFolder + "alltoall/bench_" + boost::lexical_cast<string>(nprocs);
    			break;
    		}
    	case e_Broadcast:
    		{
    			sFile = m_sFolder + "allbcast/bench_" + boost::lexical_cast<string>(nprocs);
    			break;
    		}
    	case e_Latency:
    		{
    			sFile = m_sFolder + "latency/bench";
    			break;
    		}
    	case e_Reduce:
    		{
    			sFile = m_sFolder + "reduce/bench_" + boost::lexical_cast<string>(nprocs);
    			break;
    		}
    	case e_AsyncRecv:
    		{
    			sFile = m_sFolder + "irecv/bench_" + boost::lexical_cast<string>(nprocs);
    			break;
    		}
    	}

    	y = localRegression(sFile,x,FIND_Y,m_RInstance);
    	y = y / 1000000;
    	return y;*/
}

double CMPIBenchmarkAnalysis::GetDataSizeInBytes(const double timeInSecs, int nprocs, const EMPIBenchmark eType)
{
    /*
    double m,c;
    	double y = timeInSecs;
    	double x;
    	switch(eType)
    	{
    	case e_AllReduce:
    		{
    			m = m_mapNodeAllReduce[nprocs].first;
    			c = m_mapNodeAllReduce[nprocs].second;
    			break;
    		}
    	case e_AlltoAll:
    		{
    			m = m_mapNodeAllToAll[nprocs].first;
    			c = m_mapNodeAllToAll[nprocs].second;
    			break;
    		}
    	case e_Broadcast:
    		{
    			m = m_mapNodeBroadcast[nprocs].first;
    			c = m_mapNodeBroadcast[nprocs].second;
    			break;
    		}
    	case e_Latency:
    		{
    			m = m_mapNodeLatency[nprocs].first;
    			c = m_mapNodeLatency[nprocs].second;
    			break;
    		}
    	case e_Reduce:
    		{
    			m = m_mapNodeReduce[nprocs].first;
    			c = m_mapNodeReduce[nprocs].second;
    			break;
    		}
    	case e_AsyncRecv:
    		{
    			m = m_mapNodeAsyncRecv[nprocs].first;
    			c = m_mapNodeAsyncRecv[nprocs].second;
    			break;
    		}
    	}

    	x = (y - c) / m;
    	return x;*/
    double m,c;
    double y = timeInSecs;
    double x;
    string sFile("");
    switch(eType)
        {
        case e_AllReduce:
        {
            sFile = m_sFolder + "allreduce/bench_" + boost::lexical_cast<string>(nprocs);
            break;
        }
        case e_AlltoAll:
        {
            sFile = m_sFolder + "alltoall/bench_" + boost::lexical_cast<string>(nprocs);
            break;
        }
        case e_Broadcast:
        {
            sFile = m_sFolder + "allbcast/bench_" + boost::lexical_cast<string>(nprocs);
            break;
        }
        case e_Latency:
        {
            sFile = m_sFolder + "latency/bench";
            break;
        }
        case e_Reduce:
        {
            sFile = m_sFolder + "reduce/bench_" + boost::lexical_cast<string>(nprocs);
            break;
        }
        case e_AsyncRecv:
        {
            sFile = m_sFolder + "irecv/bench_" + boost::lexical_cast<string>(nprocs);
            break;
        }
        }

    x = localRegression(sFile,y*1000000,FIND_X,m_RInstance);
    return x;

}

void CMPIBenchmarkAnalysis::FillMap(std::map<int,std::pair<double,double> > &map_, TiXmlElement *pNode)
{
    TiXmlElement *pProc = pNode->FirstChildElement(ELEMENT_PROC);
    if(pProc)
        {
            for( ; pProc; pProc=pProc->NextSiblingElement(ELEMENT_PROC))
                {
                    int nproc;
                    pair<double,double> coef;
                    pProc->Attribute(ATTRIB_N,&nproc);
                    pProc->Attribute(ATTRIB_M,&(coef.first));
                    pProc->Attribute(ATTRIB_C,&(coef.second));
                    map_.insert(pair<int,pair<double,double> > (nproc,coef));

                }
        }
}


} /* namespace StaticAnlysis */
