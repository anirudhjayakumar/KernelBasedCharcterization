/*
 * MPIBenchmarkAnalysis.h
 *
 *  Created on: 22-Aug-2013
 *      Author: anirudhj
 */

#ifndef MPIBENCHMARKANALYSIS_H_
#define MPIBENCHMARKANALYSIS_H_

#include <string>
#include <map>
#include <utility>
#include "FrameworkCommon.h"
class TiXmlElement;
class RInside;


namespace Prediction
{

enum EMPIBenchmark
{
    e_Invalid = -1,
    e_AlltoAll,
    e_Broadcast,
    e_Latency,
    e_Reduce,
    e_AllReduce,
    e_AsyncRecv
};

class CMPIBenchmarkAnalysis
{
private:
    std::string m_sFolder;
    std::map<int,std::pair<double,double> > m_mapNodeAllToAll;
    std::map<int,std::pair<double,double> > m_mapNodeBroadcast;
    std::map<int,std::pair<double,double> > m_mapNodeLatency;
    std::map<int,std::pair<double,double> > m_mapNodeReduce;
    std::map<int,std::pair<double,double> > m_mapNodeAllReduce;
    std::map<int,std::pair<double,double> > m_mapNodeAsyncRecv;
    RInside *m_RInstance;
public:
    CMPIBenchmarkAnalysis() {}
    virtual ~CMPIBenchmarkAnalysis() {}
    ReturnCode 		Initialize(const std::string &xmlFile);
    double     		GetTimeInSecs(const double buffSizeInBytes, int nprocs, const EMPIBenchmark eType);
    double     		GetDataSizeInBytes(const double timeInSecs, int nprocs, const EMPIBenchmark eType);
    static std::string GetString(EMPIBenchmark eType);
    static EMPIBenchmark GetMPIEnum(std::string sType);

private:
    void FillMap(std::map<int,std::pair<double,double> > &map_, TiXmlElement *pNode);
};

} /* namespace StaticAnlysis */
#endif /* MPIBENCHMARKANALYSIS_H_ */
