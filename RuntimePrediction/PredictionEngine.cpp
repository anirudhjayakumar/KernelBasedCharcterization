/*
 * PredictionEngine.cpp
 *
 *  Created on: 28-Jun-2013
 *      Author: anirudh
 */

#include "PredictionEngine.h"
#include "MPIBenchmarkAnalysis.h"
#include "FrameworkCommon.h"
#include "tinyxml.h"
#include <iostream>
#include <cmath>
#include <map>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <utility>
#include <set>
#include "PredictionHelper.h"
#include <boost/lexical_cast.hpp>
#include <stack>
#include <RInside.h>
using namespace std;

#define ELEMENT_PROGRAM 			"Program"
#define ELEMENT_PROBLEM_SIZE    	"ProblemSize"
#define ELEMENT_VARIABLE 			"Variable"
#define ELEMENT_LOOPS				"Loops"
#define ELEMENT_LOOP				"Loop"
#define ELEMENT_MPI                 "MPI"
#define ELEMENT_MPICALL             "MPICall"
#define ELEMENT_PROCEDURES          "Procedures"
#define ELEMENT_CRITVAR        		"CriticalVar"
#define ELEMENT_VAR           		"Var"
#define ELEMENT_PROGBLOCKS          "ProgramBlocks"
#define ELEMENT_BLOCK               "Block"

#define ATTRIB_NAME					"name"
#define ATTRIB_DEBUG_PROC			"debugProc"
#define ATTRIB_PRED_PROC			"predProc"
#define ATTRIB_DEBUG_VAL			"debugVal"
#define ATTRIB_PRED_VAL  			"predVal"
#define ATTRIB_TYPE     			"type"
#define ATTRIB_TIMESEC              "timeSec"
#define ATTRIB_DEPVAR               "depVar"
#define ATTRIB_ID                   "id"
#define ATTRIB_DEPTH                "depth"
#define ATTRIB_VARCOMB              "varComb"
#define ATTRIB_FILE                 "file"
#define ATTRIB_LOADBAL              "lb"
#define ATTRIB_CONSTTIME            "constTime"
#define ATTRIB_SIZEDEPVARS          "sizeDepVars"
#define ATTRIB_LOOPDEPVARS          "loopDepVars"
#define ATTRIB_PFEXPR          		"postfixExpr"
#define ATTRIB_ORIGINAL        		"org"
#define ATTRIB_LINE                 "line"
#define ATTRIB_VARLIST              "varList"
#define ATTRIB_PREDMAX              "predMax"
#define ATTRIB_PREDMIN              "predMin"
#define ATTRIB_KERNELTIME           "kernelTime"
#define ATTRIB_NCALLS               "nCalls"

namespace Prediction {


void SResultStruct::Print()
{

	cout << "*******************************************************"  << endl;
	cout << "Consolidated Results" << endl;
	cout << "Max: " <<  pairMinMaxRuntimes.second << endl;
	cout << "Min: " <<  pairMinMaxRuntimes.first << endl;
	cout << "Avg: " <<  nAvgRuntime << endl;
	cout << "Median: " << nMedianRuntime << endl;
	cout << "MidQuartileAvg: " << nMidQuartileAvg << endl;
	cout << "*******************************************************"  << endl;
	return;
}

PredictionEngine::PredictionEngine() {
	m_bDivWithProc = false;

}

PredictionEngine::~PredictionEngine() {

}

std::string PredictionEngine::CleanXML(const std::string &sProgramConfigs)
{
	TiXmlDocument m_XmlDoc(sProgramConfigs.c_str());
	if( !m_XmlDoc.LoadFile())
	{
		cout << "Loading xml failed" << endl;
		return "";
	}
	TiXmlElement *m_pRootXml = m_XmlDoc.RootElement();
	m_pRootXml->Attribute(ATTRIB_DEBUG_PROC,&m_nDebugProc);

	TiXmlElement *pProgBlock = m_pRootXml->FirstChildElement(ELEMENT_PROGBLOCKS);
	TiXmlElement *pBlock = pProgBlock->FirstChildElement(ELEMENT_BLOCK);

	int counter = 0;

	if(pBlock)
	{
		for (;pBlock;pBlock = pBlock->NextSiblingElement(ELEMENT_BLOCK))
		{
			BlockEdit(pBlock,counter);
		}
	}

	string file = sProgramConfigs + ".Edit";
	m_XmlDoc.SaveFile(file.c_str());
	return file;
}

void PredictionEngine::BlockEdit(TiXmlElement *pElement, int &counter)
{
	if(pElement)
	{
		pElement->SetAttribute(ATTRIB_ID,counter++);
		double time;
		pElement->Attribute(ATTRIB_TIMESEC,&time);
		double normTime = time/m_nDebugProc;
		string sTime  = boost::lexical_cast<string>(normTime);
		pElement->SetAttribute(ATTRIB_TIMESEC,sTime.c_str());
		TiXmlElement *pChild = pElement->FirstChildElement(ELEMENT_BLOCK);
		if(pChild)
		{
			for (;pChild;pChild = pChild->NextSiblingElement(ELEMENT_BLOCK))
			{
				BlockEdit(pChild,counter);
			}
		}
	}
}
int PredictionEngine::Initialize(const string &sProgramConfigs)
{
	TiXmlDocument m_XmlDoc(sProgramConfigs.c_str());
    if( !m_XmlDoc.LoadFile())
    {
    	cout << "Loading xml failed" << endl;
    	return FAIL;
    }
    TiXmlElement *m_pRootXml = m_XmlDoc.RootElement();

    m_pRootXml->Attribute(ATTRIB_PRED_PROC,&m_nPredProc);
    m_pRootXml->Attribute(ATTRIB_CONSTTIME,&m_nConstTime);
    m_pRootXml->Attribute(ATTRIB_DEBUG_PROC,&m_nDebugProc);
    int divByProc;
    m_pRootXml->Attribute("divProc",&divByProc);
    if(divByProc == 1) m_bDivWithProc = true;
    m_sProgramName = m_pRootXml->Attribute(ATTRIB_NAME);
    string sMPIBenchFile =  m_pRootXml->Attribute(ATTRIB_FILE);
    m_oMPIAnalysis.Initialize(sMPIBenchFile);
    //read problem size
    m_mapVarNameToDesc.clear();
    TiXmlElement *pProblemSize = m_pRootXml->FirstChildElement(ELEMENT_PROBLEM_SIZE);
    if(pProblemSize)
    {
    	TiXmlElement *pVariable = pProblemSize->FirstChildElement(ELEMENT_VARIABLE);
    	if (pVariable)
    	{
    		for(;pVariable;pVariable = pVariable->NextSiblingElement(ELEMENT_VARIABLE))
			{
    			string sName = pVariable->Attribute(ATTRIB_NAME);
    			SProblemSizeDefPtr ptr ( new SProblemSizeDef());
    			ptr->sVarName = sName;
    			pVariable->Attribute(ATTRIB_DEBUG_VAL,&(ptr->dDebugVal));
    			pVariable->Attribute(ATTRIB_PRED_VAL,&(ptr->dPredVal));
    			pVariable->Attribute(ATTRIB_ID,&(ptr->id));
    			string sLB = pVariable->Attribute(ATTRIB_LOADBAL);
    			if( sLB.compare("True") == 0 || sLB.compare("true") == 0)
    				ptr->bLoadBal = true;
    			else
    				ptr->bLoadBal = false;

//    			if ( ptr->bLoadBal )
//    			{
//    				ptr->dDebugVal = ptr->dDebugVal / m_nDebugProc;
//    				ptr->dPredVal  = ptr->dPredVal  / m_nPredProc;
//    			}
    			m_mapVarNameToDesc.insert(pair<string,SProblemSizeDefPtr>(sName, ptr));
			}
    	}
    }



	TiXmlElement *pProgBlock = m_pRootXml->FirstChildElement(ELEMENT_PROGBLOCKS);
	if( pProgBlock )
	{
		TiXmlElement *pChildBlock = pProgBlock->FirstChildElement(ELEMENT_BLOCK);
		if(pChildBlock)
		{
			for(;pChildBlock;pChildBlock = pChildBlock->NextSiblingElement(ELEMENT_BLOCK))
			{
				SBasicBlockPtr childBlock(new SBasicBlock());
				ReadBasicBlocks(pChildBlock,childBlock);
				m_vecTopLevelBasicBlocks.push_back(childBlock);
			}
		}
	}



	map<int,string> varToOrg;
	//parse critical vars
	TiXmlElement *pCV = m_pRootXml->FirstChildElement(ELEMENT_CRITVAR);
	if(pCV)
	{
		TiXmlElement *pVar = pCV->FirstChildElement(ELEMENT_VAR);
		if(pVar)
		{
			for(;pVar;pVar=pVar->NextSiblingElement(ELEMENT_VAR))
			{
				int id;
				string sExpr;
				pVar->Attribute(ATTRIB_ID,&id);
				sExpr = pVar->Attribute(ATTRIB_PFEXPR);
				vector<string> sExprs = GARLUTILS::Tokenize(sExpr,SEMI_DELIM);
				m_mapIDtoPostfixExprs.insert(pair<int,vector<string>>(id,sExprs));
				varToOrg.insert(pair<int,string>(id,pVar->Attribute(ATTRIB_ORIGINAL)));
			}
		}
	}

	// init debug and prod values
	for( auto varIter = m_mapIDtoPostfixExprs.begin(); varIter != m_mapIDtoPostfixExprs.end(); ++varIter)
	{
		m_mapIDtoDebugVals.insert(pair<int,double>(varIter->first,0.0));
		m_mapIDtoProdVals.insert(pair<int,double>(varIter->first,0.0));
	}
	//calculate debug and production values for each of the critical variable
	bool bChanged = true;

	//initialize mapIDtoSatus to false
	map<int,vector<bool>> mapIDtoStatus;
	for( auto varIter = m_mapIDtoPostfixExprs.begin(); varIter != m_mapIDtoPostfixExprs.end(); ++varIter)
	{
		vector<bool> vecStats;
		unsigned size = varIter->second.size();
		for ( unsigned index = 0; index < size; ++index)
			vecStats.push_back(false);
		mapIDtoStatus.insert(pair<int,vector<bool>>(varIter->first,vecStats));
	}

	for(auto itr = m_mapVarNameToDesc.begin(); itr != m_mapVarNameToDesc.end(); ++itr)
	{
		int id = itr->second->id;
		double dDebugVal,dProdVal;
		if( itr->second->bLoadBal )
		{
			dDebugVal = itr->second->dDebugVal / m_nDebugProc;
			dProdVal  = itr->second->dPredVal  / m_nPredProc;
		}
		else
		{
			dDebugVal = itr->second->dDebugVal ;
			dProdVal  = itr->second->dPredVal  ;
		}

		(mapIDtoStatus[id])[0] = true;

		m_mapIDtoDebugVals[id] = dDebugVal;
		m_mapIDtoProdVals[id]  = dProdVal;

	}



	double dDebugVal,dProdVal;
	bool bProgress = true;
	while(bProgress)
	{
		bProgress = false;
		while(bChanged)
		{
			bChanged = false;
			for( auto varIter = m_mapIDtoPostfixExprs.begin(); varIter != m_mapIDtoPostfixExprs.end(); ++varIter)
			{
				for (unsigned index = 0; index < varIter->second.size(); ++index)
				{
					if((mapIDtoStatus[varIter->first])[index] == false)
					{
						if(CanExprBeEvaluated(varIter->second[index],mapIDtoStatus))
						{

							ResolvePostFixExpression(varIter,dProdVal,dDebugVal,index);

							if ( m_mapIDtoDebugVals[varIter->first] < dDebugVal)
								 m_mapIDtoDebugVals[varIter->first] = dDebugVal;
							if ( m_mapIDtoProdVals[varIter->first] < dProdVal)
								m_mapIDtoProdVals[varIter->first] = dProdVal;
							(mapIDtoStatus[varIter->first])[index] = true;
							bChanged = bProgress = true;
						}

					}
				}
			}
		}
		bChanged = true;
		cout << "--------------- RESOLVE STATUS------------------" << endl;
		int index = 0;
		for( auto itr = mapIDtoStatus.begin(); itr != mapIDtoStatus.end(); ++itr)
		{
			cout << index << ":" << itr->first << " -> " << GARLUTILS::IsAllTrue(itr->second) << endl;
			index++;
		}
		cout << "--------------- RESOLVE STATUS------------------" << endl;

		if(!bProgress) break;
		//look for all partially true critical vars and remove all unresolved expressions
		for( auto itr = mapIDtoStatus.begin(); itr != mapIDtoStatus.end(); ++itr)
		{
			if ( !GARLUTILS::IsAllFalse(itr->second) && !GARLUTILS::IsAllTrue(itr->second))
			{
				for ( auto boolItr = itr->second.begin(); boolItr != itr->second.end(); ++boolItr)
				{
					*boolItr = true;
				}
			}
		}
	}


	for( auto itr = mapIDtoStatus.begin(); itr != mapIDtoStatus.end(); ++itr)
	{
		int id = itr->first;
		if ( !GARLUTILS::IsAllFalse(itr->second) && !GARLUTILS::IsAllTrue(itr->second))
		{
			for ( auto boolItr = itr->second.begin(); boolItr != itr->second.end(); ++boolItr)
			{
				*boolItr = true;
			}
		}

		if ( GARLUTILS::IsAllFalse(itr->second) )
		{
			double dDebugVal,dProdVal;
			string sOrg = varToOrg[itr->first];
			if(m_mapVarNameToDesc[sOrg]->bLoadBal)
			{
				dDebugVal = m_mapVarNameToDesc[sOrg]->dDebugVal / m_nDebugProc;
				dProdVal = m_mapVarNameToDesc[sOrg]->dPredVal / m_nPredProc;
			}
			else
			{
				dDebugVal = m_mapVarNameToDesc[sOrg]->dDebugVal;
				dProdVal  = m_mapVarNameToDesc[sOrg]->dPredVal;
			}
			m_mapIDtoDebugVals[itr->first] = dDebugVal;
			m_mapIDtoProdVals[itr->first] = dProdVal;
			for ( auto boolItr = itr->second.begin(); boolItr != itr->second.end(); ++boolItr)
			{
				*boolItr = true;
			}
		}
	}
//
	cout << "--------------- RESOLVE STATUS------------------" << endl;
	int index = 0;
	for( auto itr = mapIDtoStatus.begin(); itr != mapIDtoStatus.end(); ++itr)
	{
		cout << index << ":" << itr->first << " -> " << GARLUTILS::IsAllTrue(itr->second) << endl;
		index++;
	}
	cout << "--------------- RESOLVE STATUS------------------" << endl;


	cout << "--------------- Production Values ------------------" << endl;
	for( auto itr = m_mapIDtoProdVals.begin(); itr != m_mapIDtoProdVals.end(); ++itr)
	{
		cout << itr->first << " -> " << itr->second << endl;
	}
	cout << "--------------- Production Values ------------------" << endl;

	cout << "--------------- Debug Values ------------------" << endl;
	for( auto itr = m_mapIDtoDebugVals.begin(); itr != m_mapIDtoDebugVals.end(); ++itr)
	{
		cout << itr->first << " -> " << itr->second << endl;
	}
	cout << "--------------- Debug Values ------------------" << endl;

    return SUCCESS;
}

bool       PredictionEngine::CanExprBeEvaluated(const std::string &sExpr, map<int,vector<bool>> &mapIDtoStatus )
{
	VecStrings exprElements = GARLUTILS::Tokenize(sExpr,",");
	for ( auto strItr = exprElements.begin(); strItr != exprElements.end(); ++strItr)
	{
		if(*strItr != ZERO && *strItr != ONE && *strItr != OP_MUL && *strItr != OP_ADD && \
				*strItr != OP_SUB && *strItr != OP_DIV )
		{
			int var = boost::lexical_cast<int>(*strItr);
			if(!GARLUTILS::IsAllTrue((mapIDtoStatus[var])))
				return false;
		}
	}
	return true;
}


ReturnCode PredictionEngine::ResolvePostFixExpression(std::map<int,vector<string>>::iterator itr, double &prod, double &debug, unsigned index)
{
    stack<double> prodVals;
    stack<double> debugVal;

    double operand1,operand2;
    double result;

	VecStrings exprElements = GARLUTILS::Tokenize(itr->second[index],",");
	if(exprElements.size() == 1)
	{
		int var = boost::lexical_cast<int>(exprElements[0]);
		prod = m_mapIDtoProdVals[var];
		debug = m_mapIDtoDebugVals[var];
		return SUCCESS;
	}
	for ( auto strItr = exprElements.begin(); strItr != exprElements.end(); ++strItr)
	{
		if(*strItr == OP_MUL)
		{
			operand1 = prodVals.top();prodVals.pop();
			operand2 = prodVals.top();prodVals.pop();
			result = operand1 * operand2;
			prodVals.push(result);
			operand1 = debugVal.top();debugVal.pop();
			operand2 = debugVal.top();debugVal.pop();
			result = operand1 * operand2;
			debugVal.push(result);

		}
		else if(*strItr == OP_ADD)
		{
			operand1 = prodVals.top();prodVals.pop();
			operand2 = prodVals.top();prodVals.pop();
			result = operand1 + operand2;
			prodVals.push(result);
			operand1 = debugVal.top();debugVal.pop();
			operand2 = debugVal.top();debugVal.pop();
			result = operand1 + operand2;
			debugVal.push(result);
		}
		else if(*strItr == OP_SUB)
		{
			operand1 = prodVals.top();prodVals.pop();
			operand2 = prodVals.top();prodVals.pop();
			result = operand2 - operand1;
			prodVals.push(result);
			operand1 = debugVal.top();debugVal.pop();
			operand2 = debugVal.top();debugVal.pop();
			result = operand2 - operand1;
			debugVal.push(result);
		}
		else if( *strItr == OP_DIV)
		{
			operand1 = prodVals.top();prodVals.pop();
			operand2 = prodVals.top();prodVals.pop();
			result = operand2 / operand1;
			prodVals.push(result);
			operand1 = debugVal.top();debugVal.pop();
			operand2 = debugVal.top();debugVal.pop();
			result = operand2 / operand1;
			debugVal.push(result);
		}
		else if ( *strItr == ZERO)
		{
			prodVals.push(0.0);
			debugVal.push(0.0);
		}
		else if( *strItr == ONE)
		{
			prodVals.push(1.0);
			debugVal.push(1.0);
		}
		else
		{
			int var = boost::lexical_cast<int>(*strItr);
			prodVals.push(m_mapIDtoProdVals[var]);
			debugVal.push(m_mapIDtoDebugVals[var]);
		}
	}

	prod = prodVals.top();
	debug = debugVal.top();
	return SUCCESS;
}


ReturnCode PredictionEngine::ComputeRuntime(SResultStruct &sResult)
{

	double minVal(0), maxVal(0);
	for ( auto itr = m_vecTopLevelBasicBlocks.begin(); itr != m_vecTopLevelBasicBlocks.end(); ++itr)
	{
		auto ret = EvaluateBasicBlock(*itr);
		minVal+=ret.first;
		maxVal+=ret.second;
	}
	sResult.pairMinMaxRuntimes.first = (int)minVal;
	sResult.pairMinMaxRuntimes.second = (int)maxVal;
//	map<int, vector< RuntimeInfo > >      maploopRunTimes;
//    map<int, vector<double> >             mapMPIRunTimes;
//
//    //find predictions for loops
//    for ( auto itr = m_vecLoopDesc.begin(); itr != m_vecLoopDesc.end(); ++itr)
//    {
//    	maploopRunTimes.insert(pair<int,vector< RuntimeInfo > >(itr->id,CalculatePredictionsForLoop(*itr)));
//    }
//    double procScale = m_nPredProc / m_nDebugProc;
//    if(m_bDivWithProc)
//    {
//    	for( auto itr_ = maploopRunTimes.begin(); itr_ != maploopRunTimes.end(); ++itr_)
//    	{
//    		for( auto itr = itr_->second.begin(); itr != itr_->second.end(); ++itr)
//    		{
//    			get<1>(*itr) = get<1>(*itr) / procScale;
//    		}
//    	}
//    }
//
//   //find prediction for MPI calls
//    for ( auto itr = m_vecMPICalls.begin(); itr != m_vecMPICalls.end(); ++itr)
//	{
//    	mapMPIRunTimes.insert(pair<int,vector< double > >(itr->id,CalculatePredictionsForMPI(*itr)));
//	}
//
//    //fill in a vector of begin itrs of block desc and another vector with thier sizes
//	typedef vector<RuntimeInfo >::const_iterator IterRuntimeinfo;
//	vector< int >  vecblockIndex;
//	vector< int >  vecblockSize;
//
//	for ( auto itr = maploopRunTimes.begin(); itr != maploopRunTimes.end(); ++itr)
//	{
//		vecblockIndex.push_back(0);
//		vecblockSize.push_back(itr->second.size());
//	}
//
//	for ( auto itr = mapMPIRunTimes.begin(); itr != mapMPIRunTimes.end(); ++itr)
//	{
//		vecblockIndex.push_back(0);
//		vecblockSize.push_back(itr->second.size());
//	}
//
//	int nPermuationIndex = 0;
//	int nTotalPermutations = 1;
//
//	for ( auto intItr = vecblockSize.begin(); intItr != vecblockSize.end(); ++intItr)
//		nTotalPermutations*=*intItr;
//
//	vector<double> dApplicationRuntimes;
//
//	do
//	{
//
//		double dPermutaionRuntime = 0.;
//		cout << "Permutation " << nPermuationIndex << endl;
//		int index = 0;
//		for ( auto itr = maploopRunTimes.begin(); itr != maploopRunTimes.end(); ++itr)
//		{
//			const RuntimeInfo &runTimeInfo = itr->second[ vecblockIndex[index] ];
//			cout << "  Loop:" << itr->first << " Model-" << mapPredictionModelName[get<0>(runTimeInfo)] << " RunTime:" << get<1>(runTimeInfo) << endl;
//			dPermutaionRuntime+=get<1>(runTimeInfo);
//			++index;
//		}
//
//		for ( auto itr = mapMPIRunTimes.begin(); itr != mapMPIRunTimes.end(); ++itr)
//		{
//			cout << "  MPI:" << itr->first << " RunTime:" << itr->second[ vecblockIndex[index] ] << endl;
//			dPermutaionRuntime+=itr->second[ vecblockIndex[index] ];
//			++index;
//		}
//
//
//		dApplicationRuntimes.push_back(dPermutaionRuntime);
//
//		GARLUTILS::NextPermutation(vecblockIndex,vecblockSize);
//
//	} while ( ++nPermuationIndex < nTotalPermutations);
//
//	sort(dApplicationRuntimes.begin(),dApplicationRuntimes.end());
//
//	//add const time to all the runtime vals
//
//
//	for( auto itr = dApplicationRuntimes.begin(); itr != dApplicationRuntimes.end(); ++itr)
//	{
//		*itr+=m_nConstTime;
//
//
//	}
//
//	cout << "Total Range: " << dApplicationRuntimes.size() << endl;
//	cout << "]" << endl;
//
//	sResult.nAvgRuntime 			  		= accumulate( dApplicationRuntimes.begin(), \
//												dApplicationRuntimes.end(), 0.0 )/ dApplicationRuntimes.size();
//
//
//	sResult.nMedianRuntime 			  		= dApplicationRuntimes[dApplicationRuntimes.size()/2];
//
//	int sizeVec = dApplicationRuntimes.size() / 4;
//	sResult.nMidQuartileAvg                 = accumulate( dApplicationRuntimes.begin() + sizeVec, \
//												dApplicationRuntimes.end() - sizeVec, 0.0 )/ ( dApplicationRuntimes.size() - (2*sizeVec));
//	sResult.pairMinMaxRuntimes              = pair<int,int>(dApplicationRuntimes[0],\
//														dApplicationRuntimes[dApplicationRuntimes.size() - 1]);
//
//
//	return 0;
}



vector< double >  PredictionEngine::CalculatePredictionsForMPI(const SMPIProcedureCall &mpiDesc)
{
	vector< double > predTimes;
	double dDebugDataSize;
	vector<double> vecProdDataSize;
	// scale for the loop variables
	vector<double> vecProdTimeScaledForLoop;
	switch(mpiDesc.eType)
	{
	case 	e_AlltoAll:
	case	e_Broadcast:
	case	e_Reduce:
	case	e_AllReduce:
	case	e_Latency:
	case    e_AsyncRecv:
	{
		dDebugDataSize = m_oMPIAnalysis.GetDataSizeInBytes(mpiDesc.dTime,m_nDebugProc,mpiDesc.eType);
		for( auto itr = mpiDesc.vecSizeDepVars.begin(); itr != mpiDesc.vecSizeDepVars.end(); ++itr)
		{
			double debugVal, prodVal;

			debugVal = m_mapIDtoDebugVals[boost::lexical_cast<int>(*itr)];
			prodVal  = m_mapIDtoProdVals[boost::lexical_cast<int>(*itr)] ;

			double prodSize = (prodVal)/(debugVal);
			prodSize*=dDebugDataSize;
			vecProdDataSize.push_back(prodSize);
		}

		double procScale = m_nPredProc / m_nDebugProc;
		if( vecProdDataSize.empty() ) // use debug size if no dependenat var
			vecProdDataSize.push_back(dDebugDataSize);
		//else
		//{
			//if(m_bDivWithProc)
			//{
				//for( auto itr = vecProdDataSize.begin(); itr != vecProdDataSize.end(); ++itr)
				//{
				//	*itr = *itr / procScale;
				//}
			//}
		//}
		for(auto itr = vecProdDataSize.begin(); itr != vecProdDataSize.end(); ++itr)
			predTimes.push_back(m_oMPIAnalysis.GetTimeInSecs(*itr,m_nPredProc,mpiDesc.eType));


		for(auto itrPredTimes = predTimes.begin(); itrPredTimes != predTimes.end(); ++itrPredTimes)
		{
			for (auto itr = mpiDesc.loopDepVars.begin(); itr != mpiDesc.loopDepVars.end(); ++itr)
			{
				double dScaleFactor = 1;
				double dDebugScale(1), dPredScale(1);
				VecStrings vStr = *itr;
				for (auto itrStr = vStr.begin(); itrStr != vStr.end(); ++itrStr)
				{
					double debugVal, prodVal;
					debugVal = m_mapIDtoDebugVals[boost::lexical_cast<int>(*itrStr)];
					prodVal  = m_mapIDtoProdVals[boost::lexical_cast<int>(*itrStr)] ;
					dDebugScale*=debugVal;
					dPredScale*=prodVal;
				}
				dScaleFactor = dPredScale/dDebugScale;
				vecProdTimeScaledForLoop.push_back((*itrPredTimes)*dScaleFactor);
			}
		}
		break;
	}


	}
	return vecProdTimeScaledForLoop;
}

vector< RuntimeInfo >  PredictionEngine::CalculatePredictionsForLoop(const SLoopDesc &loopDesc)
{
	vector< RuntimeInfo > runTimeInfo;

	// this list will be useful for e_CompleteRange and e_EliminateHigherOrder
#define X(a,b,c,d,e) e,
	LoopScaleMethod mapPredictionModelFunctions[] = { PREDICTION_MODEL_TABLE };
#undef X

	for ( auto varListIter = loopDesc.vDepVars.begin(); varListIter != loopDesc.vDepVars.end(); ++varListIter)
	{
		VecInts  varList = *varListIter;
		int order = (int)varList.size();
		for( int index = 0; index < e_ModelCount; ++index)
		{
			if( mapPredictionModelOrder[index] == order)
			{
				LoopScaleMethod func = mapPredictionModelFunctions[index];
				func(runTimeInfo,loopDesc.dTimeSec,varList,m_nDebugProc,m_nPredProc,m_mapIDtoDebugVals, \
						                                              m_mapIDtoProdVals);
			}

		}
	}
	return runTimeInfo;
}

ReturnCode      PredictionEngine::ReadBasicBlocks(TiXmlElement *pBasicBlock, SBasicBlockPtr blockPtr)
{
	pBasicBlock->Attribute(ATTRIB_ID,&(blockPtr->id));
	pBasicBlock->Attribute(ATTRIB_TIMESEC,&(blockPtr->timeSec));
	pBasicBlock->Attribute(ATTRIB_LINE,&(blockPtr->line));
	if( pBasicBlock->Attribute(ATTRIB_KERNELTIME) )
		pBasicBlock->Attribute(ATTRIB_KERNELTIME,&(blockPtr->dKernelTime));
	else
		blockPtr->dKernelTime = 0.0;

	if( pBasicBlock->Attribute(ATTRIB_NCALLS) )
			pBasicBlock->Attribute(ATTRIB_NCALLS,&(blockPtr->nCalls));
		else
			blockPtr->nCalls = 0;

	string sVarList("");
	sVarList = pBasicBlock->Attribute(ATTRIB_VARLIST);
	if(sVarList != NO_VAR )
	{
		vector<string> vars = GARLUTILS::Tokenize(sVarList,",");
		for ( auto itr = vars.begin(); itr != vars.end(); ++itr)
		{
			int varId = boost::lexical_cast<int>(*itr);
			blockPtr->varList.push_back(varId);
		}
	}
	string sType = pBasicBlock->Attribute(ATTRIB_TYPE);
	bool bFound = false;
	int index;
	for ( index = 0; index < e_BlockCount; ++index)
	{
		if ( sType == mapBlockTypeName[index] )
		{
			bFound = true;
			break;
		}
	}
	assert(bFound);
	blockPtr->eType = (EBlockType)index;
	TiXmlElement *pChildBlock = pBasicBlock->FirstChildElement(ELEMENT_BLOCK);
	if(pChildBlock)
	{
		for(;pChildBlock;pChildBlock = pChildBlock->NextSiblingElement(ELEMENT_BLOCK))
		{
			SBasicBlockPtr childBlock(new SBasicBlock());
			ReadBasicBlocks(pChildBlock,childBlock);
			childBlock->parent = blockPtr;
			blockPtr->childBlocks.push_back(childBlock);
		}
	}
	return SUCCESS;
}

std::pair<double, double>       PredictionEngine::EvaluateBasicBlock(SBasicBlockPtr blockPtr)
{
	double exclBlockTime = blockPtr->timeSec;
	// get exclusive time of each block
	for( auto itr = blockPtr->childBlocks.begin(); itr != blockPtr->childBlocks.end(); ++itr)
	{
		exclBlockTime-=(*itr)->timeSec;
	}

	if( exclBlockTime < 0)
		exclBlockTime = 0;
	double scalingVal, logScalingVal;
	vector<double> scalingVals;
	std::pair<double, double> retVals;
	double minScal(1.0), maxScal(1.0), minVal(0.0), maxVal(0.0), minSum(0.0), maxSum(0.0);
	double dDebugDataSize;
	vector<double> vecProdDataSize;
	switch(blockPtr->eType)
	{
	case e_BlockFunc:
	case e_BlockLoop:
    {
    	if( blockPtr->dKernelTime < 1)
    	{
			for( auto itr = blockPtr->varList.begin(); itr != blockPtr->varList.end(); ++itr)
			{
				scalingVal = m_mapIDtoProdVals[*itr] / m_mapIDtoDebugVals[*itr];
				logScalingVal = log(m_mapIDtoProdVals[*itr]) / log(m_mapIDtoDebugVals[*itr]);
				scalingVals.push_back(scalingVal);
				scalingVals.push_back(logScalingVal);
			}
			sort(scalingVals.begin(),scalingVals.end());
			if(scalingVals.size() > 0)
			{
				minScal = scalingVals[0];
				maxScal = scalingVals[scalingVals.size() - 1];
			}
			for( auto itr = blockPtr->childBlocks.begin(); itr != blockPtr->childBlocks.end(); ++itr)
			{
				retVals = EvaluateBasicBlock(*itr);
				minSum += retVals.first;
				maxSum += retVals.second;
			}

			minVal = (minScal*exclBlockTime) + (minScal*minSum);
			maxVal = (maxScal*exclBlockTime) + (maxScal*maxSum);


			break;
    	}
    	else
    	{
    		minVal = maxVal = blockPtr->dKernelTime;
    		break;
    	}
    }
	case e_BlockMPI_AlltoAll:
	case e_BlockMPI_AllReduce:
	case e_BlockMPI_Broadcast:
	case e_BlockMPI_Latency:
	case e_BlockMPI_Reduce:
	case e_BlockMPI_AsyncRecv:
	{

		double mpi_iter = blockPtr->nCalls;
		SBasicBlockPtr iterParent = blockPtr->parent;
		/*while(iterParent)
		{
			if( iterParent->varList.size() > 0)
			{
				double max = m_mapIDtoDebugVals[iterParent->varList[0]];
				for( auto itr = iterParent->varList.begin() + 1; itr != iterParent->varList.end(); ++itr)
				{
					if( m_mapIDtoDebugVals[*itr] > max) max = m_mapIDtoDebugVals[*itr];
				}
				mpi_iter*=max;
			}
			iterParent = iterParent->parent;
		}*/
		exclBlockTime = exclBlockTime / mpi_iter;
		dDebugDataSize = m_oMPIAnalysis.GetDataSizeInBytes(exclBlockTime,m_nDebugProc,BasicBlockToMPIEnum(blockPtr->eType));
		for( auto itr = blockPtr->varList.begin(); itr != blockPtr->varList.end(); ++itr)
		{
			scalingVal = m_mapIDtoProdVals[*itr] / m_mapIDtoDebugVals[*itr];
			vecProdDataSize.push_back(dDebugDataSize*scalingVal);
		}


		if( vecProdDataSize.empty() ) // use debug size if no dependenat var
			vecProdDataSize.push_back(dDebugDataSize);

		sort(vecProdDataSize.begin(),vecProdDataSize.end());

		minScal = vecProdDataSize[0];
		maxScal = vecProdDataSize[vecProdDataSize.size() - 1];

		minVal = m_oMPIAnalysis.GetTimeInSecs(minScal,m_nPredProc,BasicBlockToMPIEnum(blockPtr->eType));
		maxVal = m_oMPIAnalysis.GetTimeInSecs(maxScal,m_nPredProc,BasicBlockToMPIEnum(blockPtr->eType));

		minVal = minVal * mpi_iter;
		maxVal = maxVal * mpi_iter;
		break;
	}
	}
	retVals.first = minVal;
	retVals.second = maxVal;
	blockPtr->predTimeMax = maxVal;
	blockPtr->predTimeMin = minVal;
	cout << "block:" << blockPtr->id << " Min:" << minVal << " Max:" << maxVal << endl;
	return retVals;
}

void PredictionEngine::PrintBlock(const std::string &file)
{
	TiXmlDocument m_Doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	m_Doc.LinkEndChild(decl);
	TiXmlElement *root = new TiXmlElement(ELEMENT_PROGBLOCKS);
	m_Doc.LinkEndChild(root);
	for ( auto itr = m_vecTopLevelBasicBlocks.begin(); itr != m_vecTopLevelBasicBlocks.end(); ++itr)
	{
		CreateBlockXml(root,*itr);

	}
	m_Doc.SaveFile(file.c_str());

}
void PredictionEngine::CreateBlockXml(TiXmlElement *pElement, SBasicBlockPtr ptr)
{
	TiXmlElement *block = new TiXmlElement(ELEMENT_BLOCK);
	pElement->LinkEndChild(block);
	block->SetAttribute(ATTRIB_ID,ptr->id);
	block->SetAttribute(ATTRIB_TIMESEC,boost::lexical_cast<string>(ptr->timeSec).substr(0,5).c_str());
	block->SetAttribute(ATTRIB_PREDMAX,boost::lexical_cast<string>(ptr->predTimeMax).substr(0,5).c_str());
	block->SetAttribute(ATTRIB_PREDMIN,boost::lexical_cast<string>(ptr->predTimeMin).substr(0,5).c_str());
	block->SetAttribute(ATTRIB_LINE,ptr->line);


	string sVarList("");
	for ( auto itr = ptr->varList.begin(); itr != ptr->varList.end(); ++itr)
	{
		sVarList += boost::lexical_cast<string>(*itr) + ",";
	}

	block->SetAttribute(ATTRIB_VARLIST,sVarList.c_str());

	string sType = mapBlockTypeName[ptr->eType];
	block->SetAttribute(ATTRIB_TYPE,sType.c_str());
	for( auto itr = ptr->childBlocks.begin(); itr != ptr->childBlocks.end(); ++itr)
	{
		CreateBlockXml(block, *itr);
	}

}

EMPIBenchmark PredictionEngine::BasicBlockToMPIEnum(EBlockType eType)
{
	switch(eType)
	{
	case e_BlockMPI_AlltoAll:
		return e_AlltoAll;
	case e_BlockMPI_AllReduce:
		return e_AllReduce;
	case e_BlockMPI_Broadcast:
		return e_Broadcast;
	case e_BlockMPI_Latency:
		return e_Latency;
	case e_BlockMPI_Reduce:
		return e_Reduce;
	case e_BlockMPI_AsyncRecv:
			return e_AsyncRecv;
	default:
		return e_Invalid;
	}
}

} // end namespace Pediction
