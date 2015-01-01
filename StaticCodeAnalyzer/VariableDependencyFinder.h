/*
 * VariableDependencyFinder.h
 *
 *  Created on: 12-Aug-2013
 *      Author: anirudhj
 */

#ifndef VARIABLEDEPENDENCYFINDER_H_
#define VARIABLEDEPENDENCYFINDER_H_
#include "SACommon.h"
#include "FrameworkCommon.h"
#include <map>
#include <vector>
#include <set>
#include <string>
class SgProject;
class DFAnalysis;
class SgNode;
class SgStatement;
class SgVarRefExp;
typedef std::vector<SgNode*> NodeQuerySynthesizedAttributeType;
namespace StaticAnlysis {



class CVariableDependencyFinder {
public:
	CVariableDependencyFinder();
	virtual ~CVariableDependencyFinder();
	ReturnCode   Initialize(SgProject *project, const VecCriticalVars & vars);
	ReturnCode   FindDependencies();
	void         Result(VecCriticalVars &vars);
	static std::set<SgVarRefExp *> GetAssociatedCriticalVarRefs(SgStatement * pRefStatement, \
			NodeQuerySynthesizedAttributeType &allFunCalls, SCriticalVariableDefPtr &pVarRef);
private:
	VecCriticalVars m_dependencyList;
	GARLUTILS::iterable_queue<SCriticalVariableDefPtr> searchQueue;
	std::set<SgStatement*> m_vecCriticalStmts;
	SgProject *m_pProject;
	int       m_nIDCounter;
private:
	// looks into dependency list and queue for the presence of pVar
	bool isVariableAlreadyConsidered(SCriticalVariableDefPtr pVar, SCriticalVariableDefPtr &ptr);
	bool CheckCondition(SgNode *node, int &type);
	std::string ConvertToPostFix(SgNode * node);

};

} /* namespace StaticAnlysis */
#endif /* VARIABLEDEPENDENCYFINDER_H_ */
