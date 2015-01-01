/*
 * LoopDependencyResolver.cpp
 *
 *  Created on: 09-Aug-2013
 *      Author: anirudhj
 */

#include "LoopDependencyResolver.h"
#include "SACommon.h"
#include "rose.h"
using namespace std;
#include <iostream>
namespace StaticAnlysis {

CLoopDependencyResolver::CLoopDependencyResolver() {
	// TODO Auto-generated constructor stub

}

CLoopDependencyResolver::~CLoopDependencyResolver() {
	// TODO Auto-generated destructor stub
}

ReturnCode CLoopDependencyResolver::Initialize(SgProject *pProject,const VecCriticalVars &vecVars)
{
	m_pProject = pProject;
	m_vecVars = vecVars;
	return SUCCESS;
}

ReturnCode CLoopDependencyResolver::ResolveDependency(SgNode *node, SLoopDependencyDesc &loopDepDesc)
{
	VecCriticalVars refVars;
	int bFound = true;
	//cout << "Loop Statement Line: " << node->get_file_info()->get_line() << endl;
	switch (node->variantT())
		{

		case V_SgDoWhileStmt: {
			//cout << "Do While\n";
			SgDoWhileStmt *pSgWhileStmt = isSgDoWhileStmt(node);
			SgStatement *condStmt =  	pSgWhileStmt->get_condition ();
			GetReferenceVarList(condStmt,refVars);
			//cout << "Condition Statement: " <<  condStmt -> unparseToString() << endl;
			//cout << " End Do While" << endl;
			break;
		}
		case V_SgForStatement: {
			//cout << "For statement\n";
			SgForStatement *pForStmt = isSgForStatement(node);

			SgStatementPtrList & initstmt = pForStmt->get_init_stmt ();
			//cout << "Init List:" ;
			for ( auto itr = initstmt.begin(); itr != initstmt.end(); ++itr)
			{
				GetReferenceVarList(*itr,refVars);
				//cout << (*itr)-> unparseToString() << ", ";
			}
			//cout << endl;

			SgStatement* testStmt = pForStmt->get_test();
			GetReferenceVarList(testStmt,refVars);
			//cout << "Test Statement: " << testStmt-> unparseToString() << endl;
			//cout << "End For" << endl;


			break;
		}
		case V_SgWhileStmt: {
			//cout << "While \n";
			SgWhileStmt *pSgWhileStmt = isSgWhileStmt(node);

			SgStatement *condStmt =  	pSgWhileStmt->get_condition ();
			GetReferenceVarList(condStmt,refVars);
			//cout << "Cond Statement: " << condStmt-> unparseToString() << endl;
			//cout << " End while" << endl;
			break;
		}
		case V_SgFortranDo:
		{
			//cout << "Fortran Do\n";
			SgFortranDo * pSgFortranDo = isSgFortranDo(node);

			SgExpression * 	initExpr = pSgFortranDo -> get_initialization ();
			if( initExpr)
			{
				GetReferenceVarList(initExpr,refVars);
				//cout << "Init Expr: " << initExpr->unparseToString() << endl;
			}
			SgExpression * 	boundExpr = pSgFortranDo -> get_bound ();
			if( initExpr)
			{
				GetReferenceVarList(boundExpr,refVars);
				//cout << "bound Expr: " << boundExpr->unparseToString() << endl;
			}
			//cout << " End fortran do" << endl;
			break;
		}
		default:
		{
			bFound = false;
			break;
		}


	}
	if( bFound)
	{
		VecCriticalVars vars = MatchRefListToCriticalVars(refVars, m_vecVars);
		if( vars.empty() )
			loopDepDesc.bDependent = false;
		else
		{
			loopDepDesc.bDependent = true;
			loopDepDesc.vecDepndtVars = vars;
		}
	}
	else
		loopDepDesc.bDependent = false;
	return SUCCESS;
}

ReturnCode 		CLoopDependencyResolver::GetReferenceVarList(SgNode *node, VecCriticalVars &refVars)
{
	NodeQuerySynthesizedAttributeType refExp = NodeQuery::querySubTree(node, V_SgVarRefExp);
	for (auto refvarIter = refExp.begin(); refvarIter != refExp.end(); ++refvarIter)
	{
		SgVarRefExp * varRef = isSgVarRefExp(*refvarIter);
		SgInitializedName* initName = isSgInitializedName(varRef->get_symbol()->get_declaration());
		string strRefVar = initName->get_qualified_name().str();
		SCriticalVariableDefPtr pVarRef( new SCriticalVariableDef());
		pVarRef->sVariableName    = strRefVar;
		pVarRef->sDefinitionFile  = varRef->get_symbol()->get_declaration()->get_file_info()->get_filename();
		int lineNo = varRef->get_symbol()->get_declaration()->get_file_info()->get_line();
		pVarRef->nDefinitionLine  = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
				                                                                    pVarRef->sDefinitionFile);
		refVars.push_back(pVarRef);
	}
	return SUCCESS;
}

VecCriticalVars CLoopDependencyResolver::MatchRefListToCriticalVars(VecCriticalVars &refVars, VecCriticalVars &criticalVars)
{
	VecCriticalVars matchVars;
	for ( auto itrRef = refVars.begin(); itrRef != refVars.end(); ++itrRef)
	{
		for ( auto itrCritical = criticalVars.begin(); itrCritical != criticalVars.end(); ++itrCritical)
		{
			if( **itrRef == **itrCritical) //== operator
			{
				if(find(matchVars.begin(), matchVars.end(), *itrCritical) == matchVars.end())
					matchVars.push_back(*itrCritical); //imp to push the critical val as they have the id and other params
				//break;
			}
		}
	}

	return matchVars;
}



} /* namespace StaticAnlysis */
