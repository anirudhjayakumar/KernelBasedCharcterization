/*
 * VariableDependencyFinder.cpp
 *
 *  Created on: 12-Aug-2013
 *      Author: anirudhj
 */

#include "VariableDependencyFinder.h"
#include "rose.h"
#include "LoopDependencyResolver.h"
#include "DefUseAnalysis.h"
#include <boost/lexical_cast.hpp>
#include <set>
using namespace std;

#define TYPE_LEAF 		0
#define TYPE_BINOP 		1


#define MAX_DEPENDENCY_VARS 5000

namespace StaticAnlysis
{



class CAdditionalVariableDependencyFinder: public AstSimpleProcessing
{
private:
    VecCriticalVars m_NewCritVarList;
    VecCriticalVars m_OriginalList;
    SgProject*      m_pProject;
    CLoopDependencyResolver oLoopDep;
    int       m_nIDCounter;
    NodeQuerySynthesizedAttributeType m_allFunCalls;
public:
    ReturnCode Init(SgProject *pProject, const VecCriticalVars &critList, int nIDCounter)
    {
        m_pProject  = pProject;
        m_OriginalList = critList;
        oLoopDep.Initialize(m_pProject,m_OriginalList);
        m_nIDCounter = nIDCounter;
        m_allFunCalls = NodeQuery::querySubTree(m_pProject,V_SgFunctionCallExp);
    }

    VecCriticalVars GetAdditionalDependencyList()
    {
        return m_NewCritVarList;
    }
    virtual void visit(SgNode* n);
};

void CAdditionalVariableDependencyFinder::visit(SgNode* node)
{

    switch (node->variantT())
        {
        case V_SgDoWhileStmt:
        case V_SgForStatement:
        case V_SgWhileStmt:
        case V_SgFortranDo:
        {
            SLoopDependencyDesc loopDepDesc;
            oLoopDep.ResolveDependency(node, loopDepDesc);
            if( loopDepDesc.bDependent )
                {
                    SgStatement *pLoopBody = NULL;

                    switch (node->variantT())
                        {
                        case V_SgDoWhileStmt:
                        {
                            pLoopBody = isSgDoWhileStmt(node)->get_body	();
                            break;
                        }
                        case V_SgForStatement:
                        {
                            pLoopBody = isSgForStatement(node)->get_loop_body () ;
                            break;
                        }
                        case V_SgWhileStmt:
                        {
                            pLoopBody = isSgWhileStmt(node)->get_body();
                            break;
                        }
                        case V_SgFortranDo:
                        {
                            pLoopBody = isSgFortranDo(node)->get_body();
                            break;
                        }
                        }
                    SgBasicBlock *loopBlock = isSgBasicBlock(pLoopBody);
                    if(loopBlock)
                        {
                            SgStatementPtrList &listStatements = loopBlock -> get_statements ();
                            if( listStatements.size() > 0 )
                                {
                                    for( auto stmtItr = listStatements.begin(); stmtItr != listStatements.end(); ++stmtItr)
                                        {
                                            SgStatement *pStmt = *stmtItr;
                                            auto etype = pStmt->variantT ();
                                            switch ( etype )
                                                {
                                                case V_SgExprStatement:
                                                {

                                                    SgExpression *pExpr = isSgExprStatement(pStmt)->get_expression ();
                                                    auto eExprType = pExpr->variantT ();
                                                    //cout << "Found ExprStatement: " << pExpr->unparseToString() << endl;
                                                    if ( eExprType == V_SgAssignOp)
                                                        {
                                                            ////cout<< pRefStatement->unparseToString() << endl;
                                                            SgAssignOp *pAssignOp = isSgAssignOp(pExpr);
                                                            SgExpression *pExprlhs = pAssignOp->get_lhs_operand ();
                                                            SgVarRefExp *vRef = isSgVarRefExp(pExprlhs);
                                                            if ( !vRef ) //see if it is an array reference
                                                                {
                                                                    SgPntrArrRefExp *exp = isSgPntrArrRefExp(pExprlhs);
                                                                    SgExpression *  sgExp = exp->get_lhs_operand ();
                                                                    vRef = isSgVarRefExp(sgExp);
                                                                }
                                                            if( !vRef ) continue;
                                                            SCriticalVariableDefPtr leftRef(new SCriticalVariableDef());
                                                            SgInitializedName* initName = isSgInitializedName(vRef->get_symbol()->get_declaration());
                                                            string strVRef = initName->get_qualified_name().str();
                                                            leftRef->sVariableName    = strVRef;
                                                            leftRef->sDefinitionFile  = vRef->get_symbol()->get_declaration()->get_file_info()->get_filename();
                                                            int lineNo = vRef->get_symbol()->get_declaration()->get_file_info()->get_line();
                                                            leftRef->nDefinitionLine  = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
                                                                                        leftRef->sDefinitionFile);
                                                            cout << pStmt -> unparseToString() << endl;
                                                            int index = 0; //not used
                                                            if( Contains(m_OriginalList,leftRef,index) || Contains(m_NewCritVarList,leftRef,index))
                                                                continue;
                                                            SgExpression *pExprRhs = pAssignOp->get_rhs_operand ();
                                                            NodeQuerySynthesizedAttributeType refExp = NodeQuery::querySubTree(pExprRhs,V_SgVarRefExp);
                                                            for( auto refItr = refExp.begin(); refItr != refExp.end(); ++refItr)
                                                                {
                                                                    cout << pStmt -> unparseToString() << endl;
                                                                    SgVarRefExp *vRightRef = isSgVarRefExp(*refItr);
                                                                    SCriticalVariableDefPtr rightRef(new SCriticalVariableDef());
                                                                    SgInitializedName* initName = isSgInitializedName(vRightRef->get_symbol()->get_declaration());
                                                                    string strVRef = initName->get_qualified_name().str();
                                                                    rightRef->sVariableName    = strVRef;
                                                                    rightRef->sDefinitionFile  = vRightRef->get_symbol()->get_declaration()->get_file_info()->get_filename();
                                                                    int lineNo = vRightRef->get_symbol()->get_declaration()->get_file_info()->get_line();
                                                                    rightRef->nDefinitionLine  = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
                                                                                                 rightRef->sDefinitionFile);
                                                                    if( *leftRef == *rightRef)
                                                                        {
                                                                            leftRef->nID = m_nIDCounter++;
                                                                            leftRef->sExprStr = "";
                                                                            for ( auto varItr = loopDepDesc.vecDepndtVars.begin(); varItr !=  \
                                                                                    loopDepDesc.vecDepndtVars.end(); ++varItr)
                                                                                {
                                                                                    leftRef->sExprStr += boost::lexical_cast<string>((*varItr)->nID) + ",";

                                                                                }
                                                                            leftRef->pOrignal = loopDepDesc.vecDepndtVars[0]->pOrignal;
                                                                            m_NewCritVarList.push_back(leftRef);
                                                                            set<SgVarRefExp *> associatedVars = CVariableDependencyFinder::GetAssociatedCriticalVarRefs(pStmt, m_allFunCalls, leftRef);




                                                                            for(auto itr = associatedVars.begin(); itr != associatedVars.end(); ++itr)
                                                                                {
                                                                                    SgVarRefExp * assVarRef = isSgVarRefExp(*itr);
                                                                                    SgInitializedName* initName = isSgInitializedName(assVarRef->get_symbol()->get_declaration());
                                                                                    string strVRef = initName->get_qualified_name().str();
                                                                                    SCriticalVariableDefPtr pVRef(new SCriticalVariableDef()) ;
                                                                                    pVRef->sVariableName    = strVRef;
                                                                                    pVRef->sDefinitionFile  = assVarRef->get_symbol()->get_declaration()->get_file_info()->get_filename();
                                                                                    int lineNo = assVarRef->get_symbol()->get_declaration()->get_file_info()->get_line();
                                                                                    pVRef->nDefinitionLine  = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
                                                                                                              pVRef->sDefinitionFile);
                                                                                    pVRef->pOrignal   = leftRef->pOrignal;
                                                                                    pVRef->sExprStr   = leftRef->sExprStr;
                                                                                    SCriticalVariableDefPtr tmp;
                                                                                    int index; //not used
                                                                                    if ( !(Contains(m_OriginalList,pVRef,index) || Contains(m_NewCritVarList,pVRef,index) ))
                                                                                        {
                                                                                            pVRef->nID    = m_nIDCounter++;
                                                                                            m_NewCritVarList.push_back(pVRef);
                                                                                        }

                                                                                }




                                                                            break;
                                                                        }
                                                                }
                                                        }
                                                }
                                                }
                                        }
                                }
                        }
                }
            break;
        }
        }
}



CVariableDependencyFinder::CVariableDependencyFinder()
{
    // TODO Auto-generated constructor stub

}

CVariableDependencyFinder::~CVariableDependencyFinder()
{
    // TODO Auto-generated destructor stub
}

bool CVariableDependencyFinder::isVariableAlreadyConsidered(SCriticalVariableDefPtr pVar, SCriticalVariableDefPtr &ptr)
{
    bool bFound_ = false;
    for ( auto itr = searchQueue.begin(); itr != searchQueue.end(); ++itr )
        {
            if ( *(*itr) == *pVar)   // == overload
                {
                    ptr = *itr;
                    bFound_ = true;
                    break;
                }
        }
    if( !bFound_)
        {
            for ( auto itr_ = m_dependencyList.begin(); itr_ != m_dependencyList.end(); ++itr_ )
                {
                    if ( *(*itr_) == *pVar)   //== overload
                        {
                            ptr = *itr_;
                            bFound_ = true;
                            break;
                        }
                }
        }
    return bFound_;
}

ReturnCode   CVariableDependencyFinder::Initialize(SgProject *project, const VecCriticalVars & vars)
{
    m_pProject = project;
    //generateDOT(*m_pProject);

    while ( !searchQueue.empty() )
        searchQueue.pop(); //clear queue
    m_dependencyList.clear();
    for (auto itr = vars.begin(); itr != vars.end(); ++itr)
        {
            searchQueue.push(*itr);
        }
    m_nIDCounter = vars.size();
    return SUCCESS;

}



ReturnCode   CVariableDependencyFinder::FindDependencies()
{
    map<SCriticalVariableDefPtr,vector<SCriticalVariableDefPtr> > mapVariableAssociation;
    try
        {
            NodeQuerySynthesizedAttributeType refExp = NodeQuery::querySubTree(m_pProject,V_SgVarRefExp);
            NodeQuerySynthesizedAttributeType allFunCalls = NodeQuery::querySubTree(m_pProject,V_SgFunctionCallExp);
            int outerloop = 0;
            bool bAdditionalVars = false;
            while ( !searchQueue.empty())
                {
                    outerloop++;
                    if ( m_dependencyList.size() == MAX_DEPENDENCY_VARS )
                        break;

                    SCriticalVariableDefPtr currVar = searchQueue.front();

                    bool bFound = false;
                    for ( auto itr = m_dependencyList.begin(); itr != m_dependencyList.end(); ++itr )
                        {
                            if ( *(*itr) == *currVar)   //== overload
                                {
                                    bFound = true;
                                    break;
                                }
                        }
                    if ( !bFound )
                        m_dependencyList.push_back(currVar);

                    int innerloop = 0;
                    for (auto refvarIter = refExp.begin(); refvarIter != refExp.end(); ++refvarIter)
                        {
                            innerloop++;
                            SgVarRefExp * varRef = isSgVarRefExp(*refvarIter);
                            SgInitializedName* initName = isSgInitializedName(varRef->get_symbol()->get_declaration());
                            string strVRef = initName->get_qualified_name().str();
                            SCriticalVariableDefPtr pVRef(new SCriticalVariableDef()) ;
                            pVRef->sVariableName    = strVRef;
                            pVRef->sDefinitionFile  = varRef->get_symbol()->get_declaration()->get_file_info()->get_filename();
                            int lineNo = varRef->get_symbol()->get_declaration()->get_file_info()->get_line();
                            pVRef->nDefinitionLine  = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
                                                      pVRef->sDefinitionFile);

                            if (!(*pVRef == *currVar))
                                continue;

                            SgStatement * pRefStatement = SageInterface::getEnclosingStatement(*refvarIter);
                            auto etype = pRefStatement->variantT ();

                            switch ( etype )
                                {
                                case V_SgExprStatement:
                                {

                                    SgExpression *pExpr = isSgExprStatement(pRefStatement)->get_expression ();
                                    auto eExprType = pExpr->variantT ();
                                    //cout << "Found ExprStatement: " << pExpr->unparseToString() << endl;
                                    if ( eExprType == V_SgAssignOp)
                                        {
                                            ////cout<< pRefStatement->unparseToString() << endl;
                                            SgAssignOp *pAssignOp = isSgAssignOp(pExpr);
                                            SgExpression *pExprlhs = pAssignOp->get_lhs_operand ();
                                            SgVarRefExp *vRef = isSgVarRefExp(pExprlhs);
                                            if ( !vRef ) //see if it is an array reference
                                                {
                                                    SgPntrArrRefExp *exp = isSgPntrArrRefExp(pExprlhs);
                                                    SgExpression *  sgExp = exp->get_lhs_operand ();
                                                    vRef = isSgVarRefExp(sgExp);
                                                }
                                            if( !vRef ) continue;
                                            SgInitializedName* initName = isSgInitializedName(vRef->get_symbol()->get_declaration());
                                            string strRefVar = initName->get_qualified_name().str();
                                            if ( currVar->sVariableName == strRefVar ) continue; // for ref var in lhs

                                            SCriticalVariableDefPtr pVarRef(new SCriticalVariableDef()) ;
                                            pVarRef->sVariableName    = strRefVar;
                                            pVarRef->sDefinitionFile  = vRef->get_symbol()->get_declaration()->get_file_info()->get_filename();
                                            int lineNo = vRef->get_symbol()->get_declaration()->get_file_info()->get_line();
                                            pVarRef->nDefinitionLine  = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
                                                                        pVarRef->sDefinitionFile);
                                            pVarRef->pOrignal         = currVar->pOrignal;

                                            //Expr Eval
                                            cout << "Insert: " << pRefStatement ->unparseToString() << endl;
                                            m_vecCriticalStmts.insert(pRefStatement);
                                            SCriticalVariableDefPtr tmp;
                                            if ( !isVariableAlreadyConsidered(pVarRef, tmp) )
                                                {
                                                    pVarRef->nID              = m_nIDCounter++;
                                                    searchQueue.push(pVarRef);

                                                    // Call by ref: get associated definitions from other part of the source code.
                                                    // Add them to critical var list
                                                    // create a map from currVar to associated Vars.
                                                    // When the expressions are evaluated for currVar, the expression gets added to
                                                    // associated vars too
                                                    if ( pVarRef->sVariableName == "rowstr")
                                                        {
                                                            cout  << "break here" << endl;
                                                        }
                                                    set<SgVarRefExp *> associatedVars = GetAssociatedCriticalVarRefs(pRefStatement, allFunCalls, pVarRef);


                                                    vector<SCriticalVariableDefPtr> assVarsDefs;
                                                    //Step 5: Create a map from currCriticalVar to all the unique vars
                                                    //Step 6: add unique vars to searchQueue

                                                    for(auto itr = associatedVars.begin(); itr != associatedVars.end(); ++itr)
                                                        {
                                                            SgVarRefExp * assVarRef = isSgVarRefExp(*itr);
                                                            SgInitializedName* initName = isSgInitializedName(assVarRef->get_symbol()->get_declaration());
                                                            string strVRef = initName->get_qualified_name().str();
                                                            SCriticalVariableDefPtr pVRef(new SCriticalVariableDef()) ;
                                                            pVRef->sVariableName    = strVRef;
                                                            pVRef->sDefinitionFile  = assVarRef->get_symbol()->get_declaration()->get_file_info()->get_filename();
                                                            int lineNo = assVarRef->get_symbol()->get_declaration()->get_file_info()->get_line();
                                                            pVRef->nDefinitionLine  = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
                                                                                      pVRef->sDefinitionFile);
                                                            pVRef->pOrignal   = pVarRef->pOrignal;
                                                            SCriticalVariableDefPtr tmp;
                                                            int index; //not used
                                                            if ( !isVariableAlreadyConsidered(pVRef,tmp) )
                                                                {
                                                                    pVRef->nID              = m_nIDCounter++;
                                                                    searchQueue.push(pVRef);

                                                                    if(! Contains(assVarsDefs,pVRef,index))
                                                                        assVarsDefs.push_back(pVRef);
                                                                }
                                                            else
                                                                {
                                                                    if(! Contains(assVarsDefs,tmp,index))
                                                                        assVarsDefs.push_back(tmp);
                                                                }
                                                        }
                                                    if(assVarsDefs.size() > 0)
                                                        mapVariableAssociation.insert(pair<SCriticalVariableDefPtr,vector<SCriticalVariableDefPtr>> (pVarRef,assVarsDefs));
                                                }


                                        }
                                    else  if (V_SgFunctionCallExp == eExprType)
                                        {
                                            // we look for the varRef in each of the argument expression. Once we
                                            // find it, we take the corresponding indexed formal parameter and assign it
                                            // to critical variable
                                            SgExprListExp* actualArguments = NULL;
                                            SgFunctionCallExp *pFunctionalCallExp = isSgFunctionCallExp(pExpr);
                                            actualArguments = pFunctionalCallExp->get_args();
                                            SgExpressionPtrList& actualArgList = actualArguments->get_expressions();
                                            unsigned index = 0;
                                            bool found_ = false;
                                            for ( auto itr = actualArgList.begin(); itr != actualArgList.end(); ++itr)
                                                {
                                                    NodeQuerySynthesizedAttributeType refInExpression = NodeQuery::querySubTree(*itr,V_SgVarRefExp);
                                                    for( auto refItr = refInExpression.begin(); refItr != refInExpression.end(); ++refItr)
                                                        {
                                                            SgVarRefExp * varRefExp = isSgVarRefExp(*refItr);
                                                            if ( varRef == varRefExp )
                                                                {
                                                                    found_ = true;
                                                                    break;
                                                                }
                                                        }
                                                    if ( found_ ) break;
                                                    index++;
                                                }

                                            //Get the formal arguments.
                                            SgFunctionDeclaration *pFuncDec = 	pFunctionalCallExp->getAssociatedFunctionDeclaration ();
                                            SgInitializedNamePtrList formalArgList = pFuncDec->get_args();
                                            //cout << strVRef << " "<< pFuncDec -> get_qualified_name() << endl;
                                            if ( index  < formalArgList.size() )
                                                {
                                                    SgInitializedName *initName = formalArgList[index];
                                                    SCriticalVariableDefPtr pVarRef(new SCriticalVariableDef());
                                                    pVarRef->sVariableName     = initName-> unparseToString();
                                                    pVarRef->sDefinitionFile   = initName-> get_file_info()->get_filename ();
                                                    int lineNo = initName-> get_file_info()->get_line();
                                                    pVarRef->nDefinitionLine   = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
                                                                                 pVarRef->sDefinitionFile);
                                                    pVarRef->pOrignal      	   = currVar->pOrignal;

                                                    // Expr Eval
                                                    // expressions are not resolved for function parameter assignments
                                                    // we are taking about calls like func( a + b,c)
                                                    // We assign the ExprStr as the id of the currVar which in the above
                                                    // case will either be a or b
                                                    pVarRef->sExprStr          = boost::lexical_cast<string>(currVar->nID);

                                                    SCriticalVariableDefPtr tmp;
                                                    if ( !isVariableAlreadyConsidered(pVarRef,tmp) )
                                                        {
                                                            pVarRef->nID               = m_nIDCounter++;
                                                            searchQueue.push(pVarRef);
                                                        }
                                                }

                                        }
                                    break;
                                }
                                case V_SgVariableDeclaration:
                                {
                                    SgVariableDeclaration *pVarDeclare = isSgVariableDeclaration(pRefStatement);
                                    //cout << "Found VariableDeclaration: " << pVarDeclare->unparseToString() << endl;
                                    SgInitializedNamePtrList &lstVars =	pVarDeclare->get_variables ();

                                    /*NOTE: Assume that the each SgVariableDeclaration will have only one
                                     * variable that it initialises. This is the how the AST is represented in the
                                     * current rose version. If this changes in future rose versions the below assumption
                                     * of taking index 0 as the declared variable will break
                                    */

                                    SgInitializedName *pDeclaredVariable = lstVars[0];
                                    SCriticalVariableDefPtr pVarRef(new SCriticalVariableDef());
                                    pVarRef->sVariableName     = pDeclaredVariable-> get_qualified_name().str();
                                    pVarRef->sDefinitionFile   = pRefStatement-> get_file_info()->get_filename ();
                                    int lineNo = pRefStatement-> get_file_info()->get_line();
                                    pVarRef->nDefinitionLine   = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
                                                                 pVarRef->sDefinitionFile);
                                    pVarRef->pOrignal      	   = currVar->pOrignal;
                                    SCriticalVariableDefPtr tmp;
                                    if ( !isVariableAlreadyConsidered(pVarRef,tmp) )
                                        {
                                            pVarRef->nID           = m_nIDCounter++;
                                            searchQueue.push(pVarRef);
                                        }

                                    //Expr Eval
                                    cout << "Insert: " << pRefStatement ->unparseToString() << endl;
                                    m_vecCriticalStmts.insert(pRefStatement);

                                    break;
                                }
                                default:
                                {
                                    // nothing to do.
                                }
                                }
                        }
                    searchQueue.pop();
                    if ( searchQueue.empty() && bAdditionalVars  == false)
                        {
                            CAdditionalVariableDependencyFinder oAddVarFinder;
                            oAddVarFinder.Init(m_pProject,m_dependencyList,m_nIDCounter);
                            oAddVarFinder.traverseInputFiles(m_pProject,preorder);
                            VecCriticalVars vecAddVars = oAddVarFinder.GetAdditionalDependencyList();
                            for ( auto varItr = vecAddVars.begin(); varItr != vecAddVars.end(); ++varItr)
                                searchQueue.push(*varItr);
                            bAdditionalVars = true;
                            m_nIDCounter+=vecAddVars.size();
                        }
                    //cout << "List Size: " << searchQueue.size() << endl;
                }
        }
    catch ( ... )
        {
            return FAIL;
        }

    //Expr Eval

    for ( auto itrStmts = m_vecCriticalStmts.begin(); itrStmts != m_vecCriticalStmts.end(); ++itrStmts )
        {
            SgStatement * pRefStatement = *itrStmts;

            cout << "Eval: " << pRefStatement ->unparseToString() << endl;
            SCriticalVariableDefPtr critVar;
            SgExpression *pExpr = NULL;
            auto etype = pRefStatement->variantT ();
            switch ( etype )
                {
                case V_SgExprStatement: // assignment exp: a = b + c;
                {
                    SgExpression *pRefExpr = isSgExprStatement(pRefStatement)->get_expression ();
                    SgAssignOp *pAssignOp = isSgAssignOp(pRefExpr);
                    SgExpression *pExprlhs = pAssignOp->get_lhs_operand ();
                    SgVarRefExp *vRef = isSgVarRefExp(pExprlhs);
                    if ( !vRef ) //see if it is an array reference
                        {
                            SgPntrArrRefExp *exp = isSgPntrArrRefExp(pExprlhs);
                            SgExpression *  sgExp = exp->get_lhs_operand ();
                            vRef = isSgVarRefExp(sgExp);
                        }
                    SgInitializedName* initName = isSgInitializedName(vRef->get_symbol()->get_declaration());
                    string strRefVar = initName->get_qualified_name().str();
                    SCriticalVariableDefPtr pVarRef(new SCriticalVariableDef()) ;
                    pVarRef->sVariableName    = strRefVar;
                    pVarRef->sDefinitionFile  = vRef->get_symbol()->get_declaration()->get_file_info()->get_filename();
                    int lineNo = vRef->get_symbol()->get_declaration()->get_file_info()->get_line();
                    pVarRef->nDefinitionLine  = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
                                                pVarRef->sDefinitionFile);

                    bool bFound = false;

                    for ( auto itr_ = m_dependencyList.begin(); itr_ != m_dependencyList.end(); ++itr_ )
                        {
                            if ( *(*itr_) == *pVarRef)   //== overload
                                {
                                    critVar = (*itr_);
                                    bFound = true;
                                    break;
                                }
                        }

                    if ( !bFound) continue;
                    pExpr = pAssignOp -> get_rhs_operand ();
                    break;

                }
                case V_SgVariableDeclaration: // int a = b+ c
                {
                    SgVariableDeclaration *pVarDeclare = isSgVariableDeclaration(pRefStatement);
                    SgInitializedNamePtrList &lstVars =	pVarDeclare->get_variables ();

                    SgInitializedName *pDeclaredVariable = lstVars[0];
                    SCriticalVariableDefPtr pVarRef(new SCriticalVariableDef());
                    pVarRef->sVariableName     = pDeclaredVariable-> get_qualified_name().str();
                    pVarRef->sDefinitionFile   = pRefStatement-> get_file_info()->get_filename ();
                    int lineNo = pRefStatement-> get_file_info()->get_line();
                    pVarRef->nDefinitionLine   = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
                                                 pVarRef->sDefinitionFile);
                    bool bFound = false;
                    for ( auto itr_ = m_dependencyList.begin(); itr_ != m_dependencyList.end(); ++itr_ )
                        {
                            if ( *(*itr_) == *pVarRef)   //== overload
                                {
                                    critVar = (*itr_);
                                    bFound = true;
                                    break;
                                }
                        }
                    if ( !bFound) continue;
                    NodeQuerySynthesizedAttributeType assignExp = NodeQuery::querySubTree(pDeclaredVariable,V_SgAssignInitializer);
                    if (assignExp.empty()) continue;
                    SgAssignInitializer *pAssigninit = isSgAssignInitializer(assignExp[0]);
                    if(!pAssigninit) continue;
                    pExpr = pAssigninit->get_operand ();
                    break;

                }
                }

            string sExVal = ConvertToPostFix((SgNode * )pExpr);
            if ( sExVal == NOOP && critVar->sExprStr == "")
                critVar->sExprStr = boost::lexical_cast<string>(critVar->pOrignal->nID);
            else if ( critVar->sExprStr == "" )
                critVar->sExprStr = sExVal;
            else if ( sExVal != NOOP)
                critVar->sExprStr+= SEMI_DELIM + sExVal;

            if ( mapVariableAssociation.find(critVar) != mapVariableAssociation.end() )
                {
                    vector<SCriticalVariableDefPtr> assCritVars = mapVariableAssociation[critVar];
                    for ( auto itr = assCritVars.begin(); itr != assCritVars.end(); ++itr )
                        {
                            if ( sExVal == NOOP && (*itr)->sExprStr == "")
                                (*itr)->sExprStr = boost::lexical_cast<string>((*itr)->pOrignal->nID);
                            else if ( (*itr)->sExprStr == "" )
                                (*itr)->sExprStr = sExVal;
                            else if ( sExVal != NOOP)
                                (*itr)->sExprStr+= SEMI_DELIM + sExVal;
                        }
                }

        }


    return SUCCESS;
}



std::string CVariableDependencyFinder::ConvertToPostFix(SgNode * node)
{
    int type;
    if (CheckCondition(node, type) == false) return NOOP;
    if(type == TYPE_LEAF)
        {
            SgVarRefExp * vRef = isSgVarRefExp(node);
            SgInitializedName* initName = isSgInitializedName(vRef->get_symbol()->get_declaration());
            string strRefVar = initName->get_qualified_name().str();
            SCriticalVariableDefPtr pVarRef(new SCriticalVariableDef()) ;
            pVarRef->sVariableName    = strRefVar;
            pVarRef->sDefinitionFile  = vRef->get_symbol()->get_declaration()->get_file_info()->get_filename();
            int lineNo = vRef->get_symbol()->get_declaration()->get_file_info()->get_line();
            pVarRef->nDefinitionLine  = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
                                        pVarRef->sDefinitionFile);
            bool bFound = false;
            SCriticalVariableDefPtr pCritVar;
            for ( auto itr_ = m_dependencyList.begin(); itr_ != m_dependencyList.end(); ++itr_ )
                {
                    if ( *(*itr_) == *pVarRef)   //== overload;
                        {
                            pCritVar = *itr_;
                            bFound = true;
                            break;
                        }
                }
            if ( !bFound) return NOOP;
            string sRet = boost::lexical_cast<string>(pCritVar->nID) + ",";
            return sRet;

        }
    else if ( type == TYPE_BINOP )
        {
            SgBinaryOp *pBinOp = isSgBinaryOp(node);
            auto eType = pBinOp -> variantT ();
            string opStr;
            switch(eType)
                {
                case V_SgAddOp:
                {
                    opStr = OP_ADD;
                    break;
                }
                case V_SgMultiplyOp:
                {
                    opStr = OP_MUL;
                    break;
                }
//		case V_SgDivideOp:
//		{
//			opStr = OP_DIV;
//			break;
//		}
//		case V_SgSubtractOp:
//		{
//			opStr = OP_SUB;
//			break;
//		}
                }
            string sLHS = ConvertToPostFix( pBinOp -> get_lhs_operand());
            string sRHS = ConvertToPostFix( pBinOp -> get_rhs_operand());

            if( sLHS == NOOP && sRHS == NOOP)
                {
                    return NOOP;
                }
            else if (sLHS != NOOP && sRHS != NOOP)
                {
                    return sLHS + sRHS + opStr + ",";
                }
            else if (sLHS != NOOP && sRHS == NOOP)
                {
                    return sLHS;
                }
            else if (sLHS == NOOP && sRHS != NOOP)
                {
                    if( opStr == OP_ADD || opStr == OP_MUL)
                        {
                            return sRHS;
                        }
//			else if( opStr == OP_DIV)
//			{
//				return ONE + "," + sRHS + opStr + ",";
//			}
//			else if( opStr == OP_SUB)
//			{
//				return ZERO + "," + sRHS + opStr + ",";
//			}
                }
            else
                {
                    cout << endl << "Oops shouldn't be here!!!" << endl;
                    return NOOP;
                }


        }
    else //should come here
        {
            cout << endl << "Oops shouldn't be here!!!" << endl;
            return NOOP;
        }
}

bool CVariableDependencyFinder::CheckCondition(SgNode *node, int &type)
{
    if(node == NULL) return false;
    SgBinaryOp *binOp = isSgBinaryOp(node);
    if(binOp != NULL)
        {
            auto eType = binOp -> variantT ();
            switch(eType)
                {
                case V_SgAddOp:
                case V_SgMultiplyOp:
//		case V_SgDivideOp:
//		case V_SgSubtractOp:
                {
                    type = TYPE_BINOP;
                    return true;
                    break;
                }
                default:
                {
                    return false;
                }

                }
        }
    SgVarRefExp * vRef = isSgVarRefExp(node);
    if(vRef)
        {
            type = TYPE_LEAF;
            return true; //critical var check will be done by the caller
        }
    return false;
}

void         CVariableDependencyFinder::Result(VecCriticalVars &vars)
{
    vars = m_dependencyList;
}

std::set<SgVarRefExp *> CVariableDependencyFinder::GetAssociatedCriticalVarRefs(SgStatement * pRefStatement, \
        NodeQuerySynthesizedAttributeType &allFunCalls, SCriticalVariableDefPtr &pVarRef)
{
    std::set<SgVarRefExp *> associatedVars;
    //step 1: see if enclosing function is not the original program
    SgFunctionDefinition * funcDef 	= SageInterface::getEnclosingFunctionDefinition(pRefStatement);
    SgFunctionDeclaration * pFunDec = funcDef->get_declaration ();
    string funcName = pFunDec->get_qualified_name();
    funcName.erase (remove(funcName.begin(), funcName.end(), ':'), funcName.end());
    if( funcName != CStaticAnalyzeCommon::getInstance()->GetProgramName())
        {
            //step 2: see if criticalVar is a function argument. if yes find index
            SgInitializedNamePtrList formalArgList = pFunDec->get_args();
            int index = 0;
            bool isArgument = false;
            while(index < formalArgList.size())
                {
                    SgInitializedName *initName = formalArgList[index];
                    if( initName-> unparseToString() == pVarRef->sVariableName)
                        {
                            isArgument = true;
                            break;
                        }
                    index++;
                }
            if(isArgument)
                {
                    //step 3: Get all functions calls for the func decl

                    for (auto funcCallItr = allFunCalls.begin(); funcCallItr != allFunCalls.end(); ++funcCallItr )
                        {
                            SgFunctionCallExp * funCall = isSgFunctionCallExp(*funcCallItr);
                            SgFunctionDeclaration *def = funCall->getAssociatedFunctionDeclaration ();
                            //cout << def -> get_qualified_name() << "," << pFunDec -> get_qualified_name() << endl;
                            if ( def != pFunDec) continue;
                            SgExprListExp* actualArguments = funCall->get_args();
                            if(actualArguments)
                                {
                                    SgExpressionPtrList& actualArgList = actualArguments->get_expressions();
                                    if( actualArgList.size() > index)
                                        {
                                            SgExpression *pExpr = actualArgList[index];
                                            if(isSgVarRefExp(pExpr))
                                                {
                                                    //Step 4: Get all the unique variables for each of these function calls
                                                    SgVarRefExp *assVar = isSgVarRefExp(pExpr);
                                                    SCriticalVariableDefPtr pVar(new SCriticalVariableDef()) ;
                                                    SgInitializedName* initName = isSgInitializedName(assVar->get_symbol()->get_declaration());
                                                    string strRefVar = initName->get_qualified_name().str();
                                                    pVar->sVariableName    = strRefVar;
                                                    pVar->sDefinitionFile  = assVar->get_symbol()->get_declaration()->get_file_info()->get_filename();
                                                    int lineNo = assVar->get_symbol()->get_declaration()->get_file_info()->get_line();
                                                    pVar->nDefinitionLine  = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
                                                                             pVar->sDefinitionFile);
                                                    std::set<SgVarRefExp *> associVars = GetAssociatedCriticalVarRefs(SageInterface::getEnclosingStatement(pExpr),allFunCalls,pVar);
                                                    for ( auto itrSet = associVars.begin(); itrSet != associVars.end(); ++itrSet)
                                                        associatedVars.insert(*itrSet);
                                                    associatedVars.insert(assVar);
                                                }
                                        }
                                }
                        }
                }
        }
    return associatedVars;
}


} /* namespace StaticAnlysis */
