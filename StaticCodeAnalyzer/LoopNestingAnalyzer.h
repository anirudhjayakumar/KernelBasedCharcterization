/*
 * LoopNestingAnalyzer.h
 *
 *  Created on: 09-Aug-2013
 *      Author: Anirudh Jayakumar (jayakumar.anirudh@gmail.com)
 */

#ifndef LOOPNESTINGANALYZER_H_
#define LOOPNESTINGANALYZER_H_

#include <boost/shared_ptr.hpp>
#include "SACommon.h"
#include "FrameworkCommon.h"
#include <utility>
#include <map>
class SgProject;



namespace StaticAnlysis {
class CLoopNestingAnalyserImpl;
typedef boost::shared_ptr<CLoopNestingAnalyserImpl> ImplPtr;

class CLoopNestingAnalyzer {
public:
	CLoopNestingAnalyzer();
	virtual ~CLoopNestingAnalyzer();
	ReturnCode 			StartAnalysis(SgProject *pProject, VecCriticalVars &vecVars);
	ReturnCode          GetResults(VecLoopNestVarDep &vecResults,std::vector<SMPICallDesc> &mpiDescs);

private:
	ImplPtr m_pImpl_;

};

} /* namespace StaticAnlysis */
#endif /* LOOPNESTINGANALYZER_H_ */
