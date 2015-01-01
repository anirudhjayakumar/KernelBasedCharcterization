/*
 * PredictionEngine.h
 *
 *  Created on: 28-Jun-2013
 *      Author: anirudh
 */

#ifndef PREDICTIONENGINE_H_
#define PREDICTIONENGINE_H_

#include <string>
#include <vector>
#include <map>
#include "MPIBenchmarkAnalysis.h"
#include "FrameworkCommon.h"
#include <boost/shared_ptr.hpp>
#include <tuple>

#define MPI_NOSIZE "NOOP"
#define MPI_NOLOOP "NOOP"
#define NO_VAR     "NOOP"
class TiXmlDocument;
class TiXmlElement;

namespace Prediction {


// EPredictionModels are models based on problem sizes. The input problem size for
// predictions using these models will be TotalSize/Procs - per core size. The expectation/assumption
// is that the workload will be balanced with change in processor size


#define PREDICTION_MODEL_TABLE \
	X(e_PSM_LogN, 		  	"LogN", 			1,  1, LSM_LogN) \
	X(e_PSM_N_LogN, 		"NLog_N", 			2,  2, LSM_NLog_N) \
	X(e_PSM_LogN_LogN, 		"LogN_LogN", 		2,  1, LSM_LogN_LogN) \
	X(e_PSM_LogN_LogN_LogN, "LogN_LogN_LogN", 	3,  1, LSM_LogN_LogN_LogN) \
	X(e_PSM_N, 				"N", 				1,  1, LSM_N) \
	X(e_PSM_NN, 			"NN", 				2,  1, LSM_NN) \
	X(e_PSM_NNN, 			"NNN", 				3,  1, LSM_NNN) \
	X(e_PSM_logN_NN, 		"LogN_NN",  		3,  3, LSM_LogN_NN) \
	X(e_PSM_GTOrder3, 		"GTOrder3",  		4,  2, LSM_GTOrder3)

enum EPredictionModels {
#define X(a,b,c,d,e) a,
	PREDICTION_MODEL_TABLE
#undef X
	e_ModelCount
};

#define X(a,b,c,d,e) b,
static const char * mapPredictionModelName[] = { PREDICTION_MODEL_TABLE };
#undef X

#define X(a,b,c,d,e) c,
const int mapPredictionModelOrder[] = { PREDICTION_MODEL_TABLE };
#undef X

#define X(a,b,c,d,e) d,
const int mapPredictionModelPermuatations[] = { PREDICTION_MODEL_TABLE };
#undef X


struct SMPIProcedureCall
{
	EMPIBenchmark eType;
	int           id;
	double        dTime;
	VecStrings    vecSizeDepVars;
	VecVecStrings loopDepVars;

};



#define BLOCK_TYPE_TABLE \
	X(e_BlockLoop, 		  	"loop") \
	X(e_BlockFunc, 		"func") \
	X(e_BlockMPI_AlltoAll, 		"MPI_ALLTOALL") \
	X(e_BlockMPI_Broadcast, "MPI_BROADCAST") \
	X(e_BlockMPI_Latency, 				"MPI_LATENCY") \
	X(e_BlockMPI_Reduce, 			"MPI_REDUCE") \
	X(e_BlockMPI_AllReduce, 			"MPI_ALLREDUCE") \
	X(e_BlockMPI_AsyncRecv, 			"MPI_ASYNCRECV")

enum EBlockType {
#define X(a,b) a,
	BLOCK_TYPE_TABLE
#undef X
	e_BlockCount
};

#define X(a,b) b,
static const char * mapBlockTypeName[] = { BLOCK_TYPE_TABLE };
#undef X


struct SBasicBlock;
typedef boost::shared_ptr<SBasicBlock>  SBasicBlockPtr;
typedef std::vector<SBasicBlockPtr> VecSBasicBlock;
struct SBasicBlock
{
	EBlockType eType;
	double timeSec;
	double predTimeMax;
	double predTimeMin;
	int line;
	std::string sFile;
	int id;
	std::vector<int> varList;
	VecSBasicBlock childBlocks;
	SBasicBlockPtr parent;
	double dKernelTime;
	int nCalls;
};

struct SProblemSizeDef
{
	std::string sVarName;
	double      dDebugVal;
	double      dPredVal;
	bool        bLoadBal;
	int         id;
};

typedef boost::shared_ptr<SProblemSizeDef>  SProblemSizeDefPtr;
typedef std::vector<SProblemSizeDefPtr> VecProblemSizeDef;
typedef std::vector<VecProblemSizeDef> VecVecProblemSizeDef;
typedef std::vector<SMPIProcedureCall> VecMPICalls;
typedef boost::shared_ptr<SProblemSizeDef>  SProblemSizeDefPtr;

typedef std::tuple<EPredictionModels,double> RuntimeInfo;// model, predicted exec time



struct SLoopDesc
{
	double dTimeSec;
	int    id;
	int    nNestingDepth;
	VecVecInts vDepVars;
};

typedef std::vector<SLoopDesc> VecLoopDesc;

typedef void (*LoopScaleMethod)(std::vector<RuntimeInfo> &runtimeInfo, double ,  \
		                const VecInts &, int, int, std::map<int,double>&,std::map<int,double>&);


struct SResultStruct
{
	std::pair<int,int> pairMinMaxRuntimes;
	int 			   nAvgRuntime;
	int 			   nMedianRuntime;
	int 			   nMidQuartileAvg;
	void Print();
};

class PredictionEngine
{
private:
	std::string   					      		m_sProgramName;
	std::map<std::string,SProblemSizeDefPtr> 	m_mapVarNameToDesc;
	int 								  		m_nDebugProc;
	int                                   		m_nPredProc;
	CMPIBenchmarkAnalysis 						m_oMPIAnalysis;
	double                                      m_nConstTime;
	std::map<int,std::vector<std::string > >    m_mapIDtoPostfixExprs;
	std::map<int,double >         				m_mapIDtoDebugVals;
	std::map<int,double >         				m_mapIDtoProdVals;
	bool                                        m_bDivWithProc;
	VecSBasicBlock                              m_vecTopLevelBasicBlocks;
    RInside                                    *m_RInstance;


public:
	PredictionEngine();
	virtual ~PredictionEngine();
	ReturnCode 						Initialize		(const std::string &sProgramConfigs);
	ReturnCode                    	ComputeRuntime	(SResultStruct &sResult);
	static EMPIBenchmark BasicBlockToMPIEnum(EBlockType eType);
	void PrintBlock(const std::string &file);
	std::string CleanXML(const std::string &sProgramConfigs);
private:
	std::string						PerformanceModelsString(EPredictionModels eModel);
	std::vector< RuntimeInfo >      CalculatePredictionsForLoop(const SLoopDesc &loopDesc);
	std::vector< double >           CalculatePredictionsForMPI(const SMPIProcedureCall &mpiDesc);
	ReturnCode                      ResolvePostFixExpression(std::map<int,std::vector<std::string>>::iterator itr \
																	, double &prod, double &debug, unsigned index);
	bool                            CanExprBeEvaluated(const std::string &sExpr,std::map<int,std::vector<bool>> \
																				&mapIDtoStatus);
	ReturnCode                      ReadBasicBlocks(TiXmlElement *pBasicBlock, SBasicBlockPtr blockPtr);
	std::pair<double, double>       EvaluateBasicBlock(SBasicBlockPtr blockPtr);
	void CreateBlockXml(TiXmlElement *pElement, SBasicBlockPtr ptr);
	void BlockEdit(TiXmlElement *pElement, int &counter);
};
}
#endif /* PREDICTIONENGINE_H_ */
