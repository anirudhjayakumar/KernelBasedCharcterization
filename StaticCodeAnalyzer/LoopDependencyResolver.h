/*
 * LoopDependencyResolver.h
 *
 *  Created on: 09-Aug-2013
 *      Author: anirudhj
 */

#ifndef LOOPDEPENDENCYRESOLVER_H_
#define LOOPDEPENDENCYRESOLVER_H_
#include "SACommon.h"
#include "FrameworkCommon.h"
#include <map>
#include <vector>
class SgNode;
class SgProject;

namespace StaticAnlysis {

struct SLoopDependencyDesc
{

	bool bDependent;
	VecCriticalVars vecDepndtVars;
};

class CLoopDependencyResolver {
public:
	CLoopDependencyResolver();
	virtual ~CLoopDependencyResolver();
	ReturnCode Initialize(SgProject *pProject, const VecCriticalVars &vecVars);
	ReturnCode ResolveDependency(SgNode *node, SLoopDependencyDesc &loopDepDesc);
private:
	SgProject *m_pProject;
	VecCriticalVars m_vecVars;
private:
	ReturnCode 		GetReferenceVarList(SgNode *node, VecCriticalVars &refVars);
	VecCriticalVars MatchRefListToCriticalVars(VecCriticalVars &refVars, VecCriticalVars &criticalVars);

};

} /* namespace StaticAnlysis */
#endif /* LOOPDEPENDENCYRESOLVER_H_ */
