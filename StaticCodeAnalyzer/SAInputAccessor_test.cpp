/*
 * CSAInputAccessor_test.cpp
 *
 *  Created on: 09-Aug-2013
 *      Author: Anirudh Jayakumar (jayakumar.anirudh@gmail.com)
 */

#include "SAInputAccessor.h"
#include "SAOutputAccessor.h"
#include "LoopNestingAnalyzer.h"
#include "rose.h"
#include "VariableDependencyFinder.h"
#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
using namespace std;
using namespace StaticAnlysis;

int main(int argc, char * argv[])
{
	ReturnCode rc = 0;
	CStaticAnalyzeCommon *comInst = CStaticAnalyzeCommon::getInstance();
	comInst->Init("/home/anirudhj/workspace/StaticCodeAnalyzer/CommonConfigs.xml");
	CSAInputAccessor iaccessor;
	iaccessor.Initialize("/home/anirudhj/workspace/StaticCodeAnalyzer/StaticAnalysisInput.xml");
	VecCriticalVars vars = iaccessor.EnumerateCriticalVariables();
	vector<string> argvList(argv, argv + argc);
	SgProject* pSgProject = frontend(argvList);
	generateDOT(*pSgProject);


	CVariableDependencyFinder finder;
	rc = finder.Initialize(pSgProject,vars);

	rc = finder.FindDependencies();
	VecCriticalVars depvars;
	finder.Result(depvars);
	PrintVecCriticalVar(depvars);
	CLoopNestingAnalyzer anlyse;
	anlyse.StartAnalysis(pSgProject,depvars);
	VecLoopNestVarDep vec;
	vector<SMPICallDesc> mpiCalls;
	anlyse.GetResults(vec,mpiCalls);
	cout << "---------MPI Logs------------" << endl;
	for( auto itr = mpiCalls.begin(); itr != mpiCalls.end(); ++itr)
	{
		cout << itr->nLine << " ";
		string sDep("");
		for ( auto sitr = itr->sizeDep.begin(); sitr != itr->sizeDep.end(); ++sitr)
			sDep+= boost::lexical_cast<string>((*sitr)->nID) + ",";
		cout << sDep << endl;
	}
	cout << "---------MPI Logs------------" << endl;
	CSAOutputAccessor oaccessor;
	oaccessor.Initialize();
	for(auto itr = vec.begin(); itr != vec.end(); ++itr)
		oaccessor.AddLoopNode(itr->first, itr->second);
	oaccessor.AddCriticalVars(depvars);
	oaccessor.AddMPICallDescs(mpiCalls);
	oaccessor.SaveFile("/home/anirudhj/workspace/StaticCodeAnalyzer/StaticAnalysisOutput.xml");

	return 0;

}
