/*
 * PredictionHelper.h
 *
 *  Created on: 30-Aug-2013
 *      Author: anirudhj
 */

#ifndef PREDICTIONHELPER_H_
#define PREDICTIONHELPER_H_

#include "PredictionEngine.h"

namespace Prediction {

static void LSM_LogN(std::vector<RuntimeInfo> &runtimeInfo, double dTime, \
		const VecInts & vars, int debuProcs, int predProcs,std::map<int,double>&debVals , \
		                             std::map<int,double>& prodVals)
{
	double timeSec;

	int varA = vars[0];
	double dPredValA,dDebugValA;
	dPredValA = prodVals[varA];
	dDebugValA = debVals[varA];


//	if( varA->bLoadBal )
//	{
//		dPredValA = varA->dPredVal / predProcs;
//		dDebugValA = varA->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValA = varA->dPredVal;
//		dDebugValA = varA->dDebugVal;
//	}
	timeSec = (log(dPredValA)/log(dDebugValA)) * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_LogN,timeSec));

}

static void LSM_NLog_N(std::vector<RuntimeInfo> &runtimeInfo, double dTime, \
		                 const VecInts & vars, int debuProcs, int predProcs ,\
		                 std::map<int,double>&debVals ,std::map<int,double>& prodVals)
{
	double timeSec;

	int varA = vars[0];
	int varB = vars[1];

	double dPredValA,dDebugValA;
	double dPredValB,dDebugValB;
	dPredValA   = prodVals[varA];
	dDebugValA  = debVals[varA];
	dPredValB   = prodVals[varB];
	dDebugValB  = debVals[varB];
//	if( varA->bLoadBal )
//	{
//		dPredValA = varA->dPredVal / predProcs;
//		dDebugValA = varA->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValA = varA->dPredVal ;
//		dDebugValA = varA->dDebugVal;
//	}
//
//	if( varB->bLoadBal )
//	{
//		dPredValB = varB->dPredVal / predProcs;
//		dDebugValB = varB->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValB = varB->dPredVal ;
//		dDebugValB = varB->dDebugVal ;
//	}

	double bScalingFactor = (dPredValA * log(dPredValB)) /
								(dDebugValA * log(dDebugValB)); //alogb
	timeSec = bScalingFactor * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_N_LogN,timeSec));
	bScalingFactor = (dPredValB * log(dPredValA)) /
							(dDebugValB * log(dDebugValA)); //alogb
	timeSec = bScalingFactor * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_N_LogN,timeSec));

}

static void LSM_LogN_LogN(std::vector<RuntimeInfo> &runtimeInfo, double dTime, \
		                   const VecInts & vars, int debuProcs, int predProcs, \
		                   std::map<int,double>&debVals , std::map<int,double>& prodVals)
{
	double timeSec;

	int varA = vars[0];
	int varB = vars[1];

	double dPredValA,dDebugValA;
	double dPredValB,dDebugValB;

	dPredValA   = prodVals[varA];
	dDebugValA  = debVals[varA];
	dPredValB   = prodVals[varB];
	dDebugValB  = debVals[varB];

//	if( varA->bLoadBal )
//	{
//		dPredValA = varA->dPredVal / predProcs;
//		dDebugValA = varA->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValA = varA->dPredVal ;
//		dDebugValA = varA->dDebugVal ;
//	}
//
//	if( varB->bLoadBal )
//	{
//		dPredValB = varB->dPredVal / predProcs;
//		dDebugValB = varB->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValB = varB->dPredVal ;
//		dDebugValB = varB->dDebugVal ;
//	}

	double bScalingFactor = (log(dPredValA) * log(dPredValB)) /
								(log(dDebugValA) * log(dDebugValB));
	timeSec = bScalingFactor * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_LogN_LogN,timeSec));


}

static void LSM_LogN_LogN_LogN(std::vector<RuntimeInfo> &runtimeInfo, double dTime, \
						const VecInts & vars, int debuProcs, int predProcs, \
		                   std::map<int,double>&debVals , std::map<int,double>& prodVals )
{
	double timeSec;

	int varA = vars[0];
	int varB = vars[1];
	int varC = vars[2];

	double dPredValA,dDebugValA;
	double dPredValB,dDebugValB;
	double dPredValC,dDebugValC;

	dPredValA   = prodVals[varA];
	dDebugValA  = debVals[varA];
	dPredValB   = prodVals[varB];
	dDebugValB  = debVals[varB];
	dPredValC   = prodVals[varC];
	dDebugValC  = debVals[varC];

//	if( varA->bLoadBal )
//	{
//		dPredValA = varA->dPredVal / predProcs;
//		dDebugValA = varA->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValA = varA->dPredVal ;
//		dDebugValA = varA->dDebugVal ;
//	}
//
//	if( varB->bLoadBal )
//	{
//		dPredValB = varB->dPredVal / predProcs;
//		dDebugValB = varB->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValB = varB->dPredVal ;
//		dDebugValB = varB->dDebugVal;
//	}
//
//	if( varC->bLoadBal )
//	{
//		dPredValC = varC->dPredVal / predProcs;
//		dDebugValC = varC->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValC = varC->dPredVal ;
//		dDebugValC = varC->dDebugVal ;
//	}

	double bScalingFactor = (log(dPredValA) * log(dPredValB) * log(dPredValC)) /
							(log(dDebugValA) * log(dDebugValB) * log(dDebugValC));
	timeSec = bScalingFactor * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_LogN_LogN_LogN,timeSec));

}

static void LSM_N(std::vector<RuntimeInfo> &runtimeInfo, double dTime, \
		                const VecInts & vars, int debuProcs, int predProcs, \
		                std::map<int,double>&debVals , std::map<int,double>& prodVals )
{
	double timeSec;

	int varA = vars[0];
	double dPredValA,dDebugValA;
	dPredValA = prodVals[varA];
	dDebugValA = debVals[varA];

//	if( varA->bLoadBal )
//	{
//		dPredValA = varA->dPredVal / predProcs;
//		dDebugValA = varA->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValA = varA->dPredVal ;
//		dDebugValA = varA->dDebugVal ;
//	}


	timeSec = (dPredValA/dDebugValA) * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_N,timeSec));

}

static void LSM_NN(std::vector<RuntimeInfo> &runtimeInfo, double dTime, \
		                    const VecInts & vars, int debuProcs, int predProcs, \
			                std::map<int,double>&debVals , std::map<int,double>& prodVals )
{
	double timeSec;
	int varA = vars[0];
	int varB = vars[1];

	double dPredValA,dDebugValA;
	double dPredValB,dDebugValB;

	dPredValA   = prodVals[varA];
	dDebugValA  = debVals[varA];
	dPredValB   = prodVals[varB];
	dDebugValB  = debVals[varB];

//	if( varA->bLoadBal )
//	{
//		dPredValA = varA->dPredVal / predProcs;
//		dDebugValA = varA->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValA = varA->dPredVal ;
//		dDebugValA = varA->dDebugVal ;
//	}
//
//	if( varB->bLoadBal )
//	{
//		dPredValB = varB->dPredVal / predProcs;
//		dDebugValB = varB->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValB = varB->dPredVal ;
//		dDebugValB = varB->dDebugVal ;
//	}

	double bScalingFactor = (dPredValA * dPredValB) /
								(dDebugValA * dDebugValB);
	timeSec = bScalingFactor * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_NN,timeSec));

}

static void LSM_NNN(std::vector<RuntimeInfo> &runtimeInfo, double dTime, \
		                       const VecInts & vars, int debuProcs, int predProcs, \
			                   std::map<int,double>&debVals , std::map<int,double>& prodVals )
{
	double timeSec;

	int varA = vars[0];
	int varB = vars[1];
	int varC = vars[2];

	double dPredValA,dDebugValA;
	double dPredValB,dDebugValB;
	double dPredValC,dDebugValC;

	dPredValA   = prodVals[varA];
	dDebugValA  = debVals[varA];
	dPredValB   = prodVals[varB];
	dDebugValB  = debVals[varB];
	dPredValC   = prodVals[varC];
	dDebugValC  = debVals[varC];


//	if( varA->bLoadBal )
//	{
//		dPredValA = varA->dPredVal / predProcs;
//		dDebugValA = varA->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValA = varA->dPredVal ;
//		dDebugValA = varA->dDebugVal ;
//	}
//
//	if( varB->bLoadBal )
//	{
//		dPredValB = varB->dPredVal / predProcs;
//		dDebugValB = varB->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValB = varB->dPredVal ;
//		dDebugValB = varB->dDebugVal ;
//	}
//
//	if( varC->bLoadBal )
//	{
//		dPredValC = varC->dPredVal / predProcs;
//		dDebugValC = varC->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValC = varC->dPredVal ;
//		dDebugValC = varC->dDebugVal ;
//	}

	double bScalingFactor = (dPredValA * dPredValB * dPredValC) /
							(dDebugValA * dDebugValB * dDebugValC);
	timeSec = bScalingFactor * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_NNN,timeSec));

}

static void LSM_LogN_NN(std::vector<RuntimeInfo> &runtimeInfo, double dTime, \
		                        const VecInts & vars, int debuProcs, int predProcs, \
				                std::map<int,double>&debVals , std::map<int,double>& prodVals )
{
	double timeSec;

	int varA = vars[0];
	int varB = vars[1];
	int varC = vars[2];

	double dPredValA,dDebugValA;
	double dPredValB,dDebugValB;
	double dPredValC,dDebugValC;

	dPredValA   = prodVals[varA];
	dDebugValA  = debVals[varA];
	dPredValB   = prodVals[varB];
	dDebugValB  = debVals[varB];
	dPredValC   = prodVals[varC];
	dDebugValC  = debVals[varC];

//	if( varA->bLoadBal )
//	{
//		dPredValA = varA->dPredVal / predProcs;
//		dDebugValA = varA->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValC = varC->dPredVal ;
//		dDebugValC = varC->dDebugVal;
//	}
//
//	if( varB->bLoadBal )
//	{
//		dPredValB = varB->dPredVal / predProcs;
//		dDebugValB = varB->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValB = varB->dPredVal ;
//		dDebugValB = varB->dDebugVal ;
//	}
//
//	if( varC->bLoadBal )
//	{
//		dPredValC = varC->dPredVal / predProcs;
//		dDebugValC = varC->dDebugVal / debuProcs;
//	}
//	else
//	{
//		dPredValC = varC->dPredVal ;
//		dDebugValC = varC->dDebugVal ;
//	}

	double bScalingFactor = (log(dPredValA) * dPredValB * dPredValC) /
							(log(dDebugValA) * dDebugValB * dDebugValC);
	timeSec = bScalingFactor * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_logN_NN,timeSec));

	bScalingFactor = (dPredValA * log(dPredValB) * dPredValC) /
						(dDebugValA * log(dDebugValB) * dDebugValC);
	timeSec = bScalingFactor * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_logN_NN,timeSec));

	bScalingFactor = (dPredValA * dPredValB * log(dPredValC)) /
						(dDebugValA * dDebugValB * log(dDebugValC));
	timeSec = bScalingFactor * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_logN_NN,timeSec));

}

static void LSM_GTOrder3(std::vector<RuntimeInfo> &runtimeInfo, double dTime,\
					const VecInts & vars, int debuProcs, int predProcs, \
	                   std::map<int,double>&debVals , std::map<int,double>& prodVals )
{
	double dPredVal(1), dLogPredVal(1);
	double dDebugVal(1), dLogDebugVal(1);
	double bScalingFactor,timeSec;

	for (auto varItr = vars.begin(); varItr != vars.end(); ++varItr)
	{
		double dDeVal, dPrVal;

		dDeVal = debVals[(*varItr)];
		dPrVal = prodVals[(*varItr)];

		dPredVal*= dPrVal;
		dLogPredVal*=log(dPrVal);
		dDebugVal*= dDeVal;
		dLogDebugVal*=log(dDeVal);
	}
	bScalingFactor = dPredVal/dDebugVal;
	timeSec = bScalingFactor * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_GTOrder3,timeSec));
	bScalingFactor = dLogPredVal/dLogDebugVal;
	timeSec = bScalingFactor * dTime;
	runtimeInfo.push_back(std::make_tuple (e_PSM_GTOrder3,timeSec));

	dPredVal = dLogPredVal = dDebugVal = dLogDebugVal = 1;
}

}



#endif /* PREDICTIONHELPER_H_ */
