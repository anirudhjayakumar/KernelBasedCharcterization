/*
 * LoopNestingAnalyzer.cpp
 *
 *  Created on: 09-Aug-2013
 *      Author: Anirudh Jayakumar (jayakumar.anirudh@gmail.com)
 */

#include "LoopNestingAnalyzer.h"
#include "LoopDependencyResolver.h"
#include "rose.h"
#include <string>
#include "SACommon.h"
#include <map>
#include <utility>
#include <set>
#include <boost/lexical_cast.hpp>
using namespace std;
typedef unsigned int NestingLevel;
typedef unsigned int NestingDepth;

#define MPI_SIZE_INDEX 1


namespace StaticAnlysis {

struct InhNestingAnnotations
{
	NestingLevel m_level;
	SgNode *m_pParentNode;
	InhNestingAnnotations()
	{
		m_level = 0;
		m_pParentNode = NULL;
	}
	InhNestingAnnotations(NestingLevel level, SgNode *pParentNode)
	{
		m_level = level;
		m_pParentNode = pParentNode;
	}
};

class SynNestingAnnotations
{
private:
	NestingDepth 	 			m_depth;
	set<SgNode*> 			    m_loopNodes;
	VecSFunctionCallDef 		m_funcCalls;
	set<SgFunctionCallExp*>     m_MPICalls;
public:
	SynNestingAnnotations() { m_depth = 0; m_loopNodes.clear(); }
	NestingDepth         GetNestingDepth() { return m_depth; }
	set<SgNode*>	     GetLoopNodes() { return m_loopNodes; }
	VecSFunctionCallDef	 GetFunctionCallDefs() { return m_funcCalls; }
	set<SgFunctionCallExp*> GetMPICalls() { return m_MPICalls; }
	void     		 SetNestDepth(NestingDepth depth) { m_depth = depth; }
	void             SetLoopNode(SgNode *node) {
		m_loopNodes.insert(node);
	}
	void             SetLoopNodes(set<SgNode*> nodes)
	{
		for ( auto itr = nodes.begin(); itr != nodes.end(); ++ itr)
			m_loopNodes.insert(*itr);
	}
	void             SetFunctionCallDef(SFunctionCallDefPtr def) { m_funcCalls.push_back(def); }
	void             SetFunctionCallDefs(VecSFunctionCallDef defs)
	{
		for ( auto itr = defs.begin(); itr != defs.end(); ++ itr)
			m_funcCalls.push_back(*itr);
	}
	void             SetMPICallDef(SgFunctionCallExp* def) { m_MPICalls.insert(def); }
	void             SetMPICallDefs(set<SgFunctionCallExp*> defs)
	{
		for ( auto itr = defs.begin(); itr != defs.end(); ++ itr)
			m_MPICalls.insert(*itr);
	}


	void             ClearLoopNodes() { m_loopNodes.clear(); }

	bool   operator> (const SynNestingAnnotations &loopAnn)
	{
		return  this -> m_depth > loopAnn.m_depth;
	}
	bool  operator< (const SynNestingAnnotations &loopAnn)
	{
		return this -> m_depth < loopAnn.m_depth;
	}
	bool operator== (const SynNestingAnnotations &loopAnn)
	{
		return this -> m_depth == loopAnn.m_depth;
	}
};



class LoopNodeAnnotation: public AstAttribute {
public:
	LoopNodeAnnotation(){
		m_depVars.clear();m_nestedLoops.clear();m_FuncCalls.clear();
	}
	NestingLevel GetNestingLevel() {
		return m_nestingLevel;
	}
	NestingDepth GetNestingDepth() {
		return m_nestingDepth;
	}
	string GetFileName() {
		return m_file;
	}
	int GetLineNo() {
		return m_line;
	}
	VecCriticalVars & GetDependentVar() {
		return m_depVars;
	}
	vector<SgNode*> & GetNestedLoops()  {
		return m_nestedLoops;
	}
	VecSFunctionCallDef & GetFuncCallDefs()  {
		return m_FuncCalls;
	}

	set<SgFunctionCallExp*> & GetMPICallDefs()  {
		return m_mpiCalls;
	}

	SgNode * GetParentNode() 	{
		return m_ParentLoopNode;
	}
	void SetNestingLevel(NestingLevel level) {
		m_nestingLevel = level;
	}
	void SetNestingDepth(NestingDepth depth) {
		m_nestingDepth = depth;
	}
	void SetFileName(string file) {
		m_file = file;
	}
	void SetLineNo(int lineNo) {
		m_line = lineNo;
	}
	void SetDependentVar(SCriticalVariableDefPtr var) {
		m_depVars.push_back(var);
	}
	void SetNestedLoops(SgNode* loop)  {
		m_nestedLoops.push_back(loop);
	}
	void SetFuncCallDef(SFunctionCallDefPtr funDef)  {
		m_FuncCalls.push_back(funDef);
	}
	void SetMPICallDef(SgFunctionCallExp* funDef)  {
		m_mpiCalls.insert(funDef);
	}
	void SetParentNode(SgNode *node) {
		m_ParentLoopNode = node;
	}
private:
	NestingLevel 		m_nestingLevel;
	NestingDepth 		m_nestingDepth;
	string 		 		m_file;
	int 		 		m_line;
	VecCriticalVars 	m_depVars;
	vector<SgNode*> 	m_nestedLoops;
	VecSFunctionCallDef m_FuncCalls;
	SgNode*             m_ParentLoopNode; //NULL for level 1
	set<SgFunctionCallExp*> m_mpiCalls;
};

class CLoopNestingAnalyserImpl: public AstTopDownBottomUpProcessing<
	InhNestingAnnotations, SynNestingAnnotations> {
public:
	CLoopNestingAnalyserImpl() {
		m_pProject = NULL;
		m_mapFunDeftoLoops.clear();m_mapLoopNodetoAnnotation.clear();
	}

	ReturnCode Initialize(SgProject *pProject, const VecCriticalVars &vecVars)
	{
		m_pProject = pProject;
		m_pLoopDep = boost::shared_ptr<CLoopDependencyResolver> ( new CLoopDependencyResolver() );
		m_pLoopDep->Initialize(pProject, vecVars);
		m_vecCriticalVars = vecVars;
		/* initialize m_mapFunDeftoLoops with all function declarations*/
		NodeQuerySynthesizedAttributeType funCalls = NodeQuery::querySubTree(pProject,V_SgFunctionCallExp);
		for (auto funcCallItr = funCalls.begin(); funcCallItr != funCalls.end(); ++funcCallItr )
		{
			SgFunctionCallExp * funCall = isSgFunctionCallExp(*funcCallItr);
			SgFunctionDeclaration *pFunDec = funCall -> getAssociatedFunctionDeclaration();
			if(pFunDec)
			{
				vector<SgNode *> nodes;
				m_mapFunDeftoLoops.insert(pair< SgFunctionDeclaration *, vector< SgNode* > >(pFunDec,nodes));
			}
		}

		return SUCCESS;
	}

	InhNestingAnnotations evaluateInheritedAttribute(SgNode* node,
			InhNestingAnnotations inhAnn) {

		InhNestingAnnotations retAnn(inhAnn.m_level,inhAnn.m_pParentNode);

		switch (node->variantT()) {
		case V_SgGotoStatement:
			//cout	<< "WARNING: Goto statement found. We do not consider goto loops.\n";
			return retAnn;
			break;
		case V_SgDoWhileStmt:
		case V_SgForStatement:
		case V_SgWhileStmt:
		case V_SgFortranDo:
		{
			SLoopDependencyDesc loopDepDesc;
			m_pLoopDep->ResolveDependency(node, loopDepDesc);
			if( loopDepDesc.bDependent )
			{
				LoopNodeAnnotation *pAnnotations = new LoopNodeAnnotation();
				Sg_File_Info* fileInfo  = node->get_file_info();
				pAnnotations->SetFileName(fileInfo->get_filenameString());
				pAnnotations->SetLineNo(fileInfo->get_line());
				pAnnotations->SetNestingLevel(++retAnn.m_level); //nesting level incremented here
				pAnnotations->SetParentNode(inhAnn.m_pParentNode);
				retAnn.m_pParentNode = node;

				string depVarIds("");
				for ( auto itr = loopDepDesc.vecDepndtVars.begin(); itr != loopDepDesc.vecDepndtVars.end(); ++itr)
				{
					if(depVarIds != "")
						depVarIds+=",";
					pAnnotations->SetDependentVar(*itr);
					depVarIds+=boost::lexical_cast<string>((*itr)->nID);
				}
				//print the info to stdout

				cout << " Loop: " << " " << fileInfo->get_line() << " " << depVarIds << endl;

				auto insRet = m_mapLoopNodetoAnnotation.insert(pair<SgNode*, LoopNodeAnnotation*>(node, pAnnotations));
				if ( retAnn.m_level == 1 )
				{
					SgFunctionDefinition *pFuncDef = SageInterface::getEnclosingFunctionDefinition(node);
					if (pFuncDef)
					{
						SgFunctionDeclaration * pFunDec = pFuncDef->get_declaration ();
						auto mapItr = m_mapFunDeftoLoops.find(pFunDec);
						if ( mapItr != m_mapFunDeftoLoops.end())
						{
							mapItr->second.push_back(node);
						}
						else
						{
							vector<SgNode*> vecNode;
							vecNode.push_back(node);
							m_mapFunDeftoLoops.insert(pair <SgFunctionDeclaration*,vector<SgNode*> >(pFunDec,vecNode));
						}
					}
				}
				return retAnn;
			}
			else
			{
				return retAnn;
			}
		}
		default:
			return retAnn;
		}
	}

	SynNestingAnnotations defaultSynthesizedAttribute(InhNestingAnnotations inh) {
		/*! we do not need the inherited attribute here
		 as default value for synthesized attribute we set 0, representing nesting depth 0.
		 */
		SynNestingAnnotations def;
		return def;
	}

	SynNestingAnnotations evaluateSynthesizedAttribute(SgNode* node,
			InhNestingAnnotations inhAnn, SynthesizedAttributesList l) {


		vector<SynNestingAnnotations> maxDepthList;
		// compute maximum nesting depth of synthesized attributes
		SynNestingAnnotations retSyn; //depth = 0
		NestingDepth newDepth = 0;
		if( l.size() > 1)
		{
				SynNestingAnnotations &refAnn = *(l.begin());
				maxDepthList.push_back(refAnn);
				for (SynthesizedAttributesList::iterator i = l.begin() + 1; i != l.end();
						i++) {
					if (*i > refAnn)
					{
						maxDepthList.clear();
						maxDepthList.push_back(*i);
						refAnn = *i;
					}
					else if ( *i == refAnn  && i->GetNestingDepth() > 0)
					{
						maxDepthList.push_back(*i);
					}

				}
			newDepth = maxDepthList[0].GetNestingDepth();
			retSyn.SetNestDepth(newDepth);
			//the depth is the max depth, but we do carry forward all the loops not just the max dept ones
			for ( auto itr = l.begin(); itr != l.end(); ++itr )
			{
				if ( itr -> GetNestingDepth() > 0) {
					retSyn.SetFunctionCallDefs(itr->GetFunctionCallDefs());
					retSyn.SetLoopNodes(itr->GetLoopNodes() );
					retSyn.SetMPICallDefs(itr->GetMPICalls());
				}
			}
		}
		else if( l.size() == 1)
		{
			retSyn = l[0];
			newDepth = retSyn.GetNestingDepth();
		}
		switch (node->variantT()) {
		case V_SgGotoStatement:
			//cout	<< "WARNING: Goto statement found. We do not consider goto loops.\n";
			break;
		case V_SgDoWhileStmt:
		case V_SgForStatement:
		case V_SgWhileStmt:
		case V_SgFortranDo: {
			//int line = node->get_file_info()->get_line();
			//cout << "Loop Statement Line--: " << line << endl ;
			auto itrLoopAnn = m_mapLoopNodetoAnnotation.find(node);
			if ( itrLoopAnn != m_mapLoopNodetoAnnotation.end() )
			{
				newDepth++;
				itrLoopAnn->second->SetNestingDepth(newDepth);
				set<SgNode*> nodes = retSyn.GetLoopNodes();
				for ( auto itr = nodes.begin(); itr != nodes.end(); ++itr)
					itrLoopAnn->second->SetNestedLoops(*itr);

				// get all function calls at this level
				NodeQuerySynthesizedAttributeType funRefs = NodeQuery::querySubTree(node,V_SgFunctionCallExp);
				for ( auto itr = funRefs.begin(); itr != funRefs.end(); ++itr)
				{
					SgFunctionCallExp * funCall = isSgFunctionCallExp(*itr);
					SgFunctionDeclaration *funcDef = funCall->getAssociatedFunctionDeclaration();

					SFunctionCallDefPtr funDesc(new SFunctionCallDesc);
					funDesc->defPtr = funcDef;
					funDesc->nLine = funCall->get_file_info()->get_line();
					funDesc->sFile = funCall->get_file_info()->get_filename();

					// find the ones not called by the lower loops
					VecSFunctionCallDef defs = retSyn.GetFunctionCallDefs();

					// associate those function to this node
					if (!FindFunctionCallPtrInVector(funDesc,defs))
					{
						itrLoopAnn->second->SetFuncCallDef(funDesc);
						retSyn.SetFunctionCallDef(funDesc);
					}
				}
				set<SgFunctionCallExp*> mpiCalls =  GetMPICallList(node);
				for ( auto mpiItr = mpiCalls.begin(); mpiItr != mpiCalls.end(); ++mpiItr)
				{
					set<SgFunctionCallExp*> lowerDepthCalls = retSyn.GetMPICalls();
					if(lowerDepthCalls.find(*mpiItr) == lowerDepthCalls.end())
					{
						itrLoopAnn->second->SetMPICallDef(*mpiItr);
						retSyn.SetMPICallDef(*mpiItr);
						m_LoopDepMPICalls.insert(*mpiItr);
					}
				}

				retSyn.ClearLoopNodes();
				retSyn.SetLoopNode(node);
				auto sdf= retSyn.GetLoopNodes();

				retSyn.SetNestDepth(newDepth);
			}
			break;
		}

		default: {
		}
		}
		//cout << endl << "Depth: " << retSyn.GetNestingDepth() << endl;
		return retSyn;
	}

	void FillMPICallsWithFunctionAssociations()
	{
		set<SgFunctionCallExp*> projMPIList = GetMPICallList(m_pProject);
		for ( auto funItr = projMPIList.begin(); funItr != projMPIList.end(); ++funItr)
		{
			if( m_LoopDepMPICalls.find(*funItr) == m_LoopDepMPICalls.end())
			{
				SgFunctionDefinition * funcDef 	= SageInterface::getEnclosingFunctionDefinition(*funItr);
				SgFunctionDeclaration * pFunDec = funcDef->get_declaration ();
				if(m_mapFunDeftoMPI.find(pFunDec) != m_mapFunDeftoMPI.end())
				{
					m_mapFunDeftoMPI[pFunDec].insert(*funItr);
				}
				else
				{
					set< SgFunctionCallExp* > setMpi;
					setMpi.insert(*funItr);
					m_mapFunDeftoMPI.insert(pair<SgFunctionDeclaration *, set< SgFunctionCallExp* > >(pFunDec,setMpi));
				}
			}
		}

	}

	void SetScalingParamsForMPICalls(SgNode* node, vector<SgNode*> depNodes)
	{
		LoopNodeAnnotation *loopAnn = m_mapLoopNodetoAnnotation[node];
		depNodes.push_back(node);
		VecSgNodes &nestedLoops = loopAnn->GetNestedLoops();
		for ( auto loopItr = nestedLoops.begin(); loopItr != nestedLoops.end(); ++loopItr)
		{
			SetScalingParamsForMPICalls(*loopItr,depNodes);
		}
		VecSFunctionCallDef &funDefs = loopAnn->GetFuncCallDefs();
		for(auto funItrs = funDefs.begin(); funItrs != funDefs.end(); ++funItrs)
		{
			SgFunctionDeclaration *pDef = (*funItrs)->defPtr;
			if( m_mapFunDeftoMPI.find(pDef) != m_mapFunDeftoMPI.end())
			{
				set< SgFunctionCallExp* > mpiCalls = m_mapFunDeftoMPI[pDef];
				for( auto mpiItr = mpiCalls.begin(); mpiItr != mpiCalls.end(); ++mpiItr )
				{
					SgFunctionCallExp *mpiCall = *mpiItr;
					AddMPICallInfoToMap(mpiCall,depNodes);
				}
			}

			if(m_mapFunDeftoLoops.find(pDef) != m_mapFunDeftoLoops.end())
			{
				vector<SgNode*> &nodes = m_mapFunDeftoLoops[pDef];
				for( auto itrLoops = nodes.begin(); itrLoops != nodes.end(); ++itrLoops)
				{
					SetScalingParamsForMPICalls(*itrLoops,depNodes);
				}
			}

		}

		set<SgFunctionCallExp*> & loopMPICalls = loopAnn -> GetMPICallDefs();
		for( auto mpiItr = loopMPICalls.begin(); mpiItr != loopMPICalls.end(); ++mpiItr)
		{
			AddMPICallInfoToMap(*mpiItr,depNodes);
		}
	}

	void AddMPICallInfoToMap(SgFunctionCallExp *mpiCall, vector<SgNode*> &depNodes)
	{
		if ( m_MPICallToDepLoops.find(mpiCall) != m_MPICallToDepLoops.end())
		{
			m_MPICallToDepLoops[mpiCall].push_back(depNodes);
		}
		else
		{
			VecVecSgNodes nodes;
			nodes.push_back(depNodes);
			m_MPICallToDepLoops.insert(pair<SgFunctionCallExp*,VecVecSgNodes> (mpiCall,nodes));
		}
	}

	VecCriticalVars GetSizeDepVars(SgFunctionCallExp *mpiCall)
	{
		VecCriticalVars vecVars;
		SgExprListExp* actualArguments = mpiCall->get_args();
		SgExpressionPtrList& actualArgList = actualArguments->get_expressions();
		if( actualArgList.size() > 2)
		{
			NodeQuerySynthesizedAttributeType refInExpression = NodeQuery::querySubTree(actualArgList[MPI_SIZE_INDEX], \
																									V_SgVarRefExp);
			for( auto refItr = refInExpression.begin(); refItr != refInExpression.end(); ++refItr)
			{
				SgVarRefExp *pRef = isSgVarRefExp(*refItr);
				SgInitializedName *initName = isSgInitializedName(pRef->get_symbol()->get_declaration());
				SCriticalVariableDefPtr pVarRef(new SCriticalVariableDef());
				pVarRef->sVariableName     = initName-> unparseToString();
				pVarRef->sDefinitionFile   = initName-> get_file_info()->get_filename ();
				int lineNo = initName-> get_file_info()->get_line();
				pVarRef->nDefinitionLine   = CStaticAnalyzeCommon::getInstance()->FortranCommonBlockLineNo(lineNo, \
																						pVarRef->sDefinitionFile);
				int index = 0;
				if ( Contains(m_vecCriticalVars,pVarRef,index) )
				{
					vecVars.push_back(m_vecCriticalVars[index]);
				}
			}
		}
		return vecVars;
	}

	void FindMPICallSizeDepVars()
	{
		//for loop associated mpicalls

		for( auto mpicallItr = m_LoopDepMPICalls.begin(); mpicallItr != m_LoopDepMPICalls.end(); ++mpicallItr)
		{
			m_MPICallToSizeDepVar.insert(pair<SgFunctionCallExp*,VecCriticalVars>(*mpicallItr,GetSizeDepVars(*mpicallItr)));
		}

		//for func associated mpicalls
		for ( auto mapItr = m_mapFunDeftoMPI.begin(); mapItr != m_mapFunDeftoMPI.end(); ++mapItr)
		{
			set< SgFunctionCallExp* > &mpiCalls = mapItr->second;
			for (auto mpiItr = mpiCalls.begin(); mpiItr != mpiCalls.end(); ++mpiItr)
			{
				m_MPICallToSizeDepVar.insert(pair<SgFunctionCallExp*,VecCriticalVars>(*mpiItr,GetSizeDepVars(*mpiItr)));
			}
		}
	}

	void FillMPICallLoopDepVars()
	{

		for ( auto mpiItr = m_MPICallToDepLoops.begin(); mpiItr != m_MPICallToDepLoops.end(); ++mpiItr)
		{
			VecVecSgNodes &vvSgNodes = mpiItr->second;
			VecVecCriticalVars vvVars;
			for( auto vvItr = vvSgNodes.begin(); vvItr != vvSgNodes.end(); ++vvItr)
			{
				VecSgNodes &nodes = *vvItr;
				vector< int >  vecblockIndex;
				vector< int >  vecblockSize;
				for( auto nodeItr = nodes.begin(); nodeItr != nodes.end(); ++nodeItr)
				{
					LoopNodeAnnotation *loopAnn = m_mapLoopNodetoAnnotation[*nodeItr];
					vecblockSize.push_back(loopAnn->GetDependentVar().size());
					vecblockIndex.push_back(0);
				}
				int nTotalPermutations = 1;
				int nPermuationIndex = 0;
				for ( auto intItr = vecblockSize.begin(); intItr != vecblockSize.end(); ++intItr)
					nTotalPermutations*=*intItr;
				VecCriticalVars vars;
				do
				{
					int index_ = 0;
					for( auto nodeItr = nodes.begin(); nodeItr != nodes.end(); ++nodeItr)
					{
						LoopNodeAnnotation *loopAnn = m_mapLoopNodetoAnnotation[*nodeItr];
						VecCriticalVars &depVar = loopAnn->GetDependentVar();
						vars.push_back(depVar[vecblockIndex[index_]]);
						index_++;
					}
					GARLUTILS::NextPermutation(vecblockIndex,vecblockSize);
				} while (++nPermuationIndex < nTotalPermutations);
				vvVars.push_back(vars);
			}
			m_MPICallToDepLoopVars.insert(pair<SgFunctionCallExp*, VecVecCriticalVars>(mpiItr->first,vvVars));
		}
	}

	void StartAnalysis()
	{
		traverseInputFiles(m_pProject, InhNestingAnnotations());
		FillMPICallsWithFunctionAssociations();
		set<SgFunctionDeclaration *> setFuncDec;
		for ( auto itr = m_mapFunDeftoMPI.begin(); itr != m_mapFunDeftoMPI.end(); ++itr)
		{
			ReOrderMPIFunctionAssociations(itr->first,setFuncDec);
		}
		for ( auto mapItr = m_mapLoopNodetoAnnotation.begin(); mapItr != m_mapLoopNodetoAnnotation.end(); ++mapItr)
		{
			if (mapItr->second->GetNestingLevel() == 1)
			{
				vector<SgNode*> nodes;
				SetScalingParamsForMPICalls(mapItr->first, nodes);
			}
		}
		FindMPICallSizeDepVars();
		FillMPICallLoopDepVars();
	}

	string GetMPIStringType(SgFunctionCallExp *call)
	{
		SgFunctionCallExp * funCall = isSgFunctionCallExp(call);
		SgFunctionDeclaration *pFunDec = funCall -> getAssociatedFunctionDeclaration();
		string sName = pFunDec -> get_qualified_name();
		sName.erase (remove(sName.begin(), sName.end(), ':'), sName.end());
		transform(sName.begin(), sName.end(), sName.begin(), ::tolower);
		if (sName == strMPI_ALLREDUCE)
		{
			return MPI_ALLREDUCE;
		}
		else if (sName == strMPI_ALLTOALL)
		{
			return MPI_ALLTOALL;
		}
		else if(sName == strMPI_BCAST)
		{
			return MPI_BROADCAST;
		}
		else if( sName == strMPI_REDUCE )
		{
			return MPI_REDUCE;
		}
		else if (sName == strMPI_SEND)
		{
			return MPI_LATENCY;
		}
		return "Unknown";
	}

	void GetMPIResults(vector<SMPICallDesc> &mpiDescs)
	{
		for ( auto mapItr = m_MPICallToSizeDepVar.begin(); mapItr != m_MPICallToSizeDepVar.end(); ++mapItr)
		{

			SMPICallDesc callDesc;
			callDesc.sFile = mapItr->first->get_file_info()->get_filename();
			callDesc.nLine = mapItr->first->get_file_info()->get_line();
			callDesc.sType = GetMPIStringType(mapItr->first);
			callDesc.sizeDep = mapItr->second;
			if(m_MPICallToDepLoopVars.find(mapItr->first) != m_MPICallToDepLoopVars.end())
				callDesc.loopDep = m_MPICallToDepLoopVars[mapItr->first];
			mpiDescs.push_back(callDesc);

		}
	}

public:
	map < SgFunctionDeclaration *, vector< SgNode* > > 		m_mapFunDeftoLoops; //level 1 loop
	map < SgNode *, LoopNodeAnnotation *> 					m_mapLoopNodetoAnnotation;
	boost::shared_ptr<CLoopDependencyResolver>  			m_pLoopDep;
	SgProject *												m_pProject;
	set<SgFunctionCallExp*> 								m_LoopDepMPICalls;
	map < SgFunctionDeclaration *, set< SgFunctionCallExp* > > 		m_mapFunDeftoMPI;
	map < SgFunctionCallExp*,VecVecSgNodes >                m_MPICallToDepLoops;
	map < SgFunctionCallExp*,VecCriticalVars >              m_MPICallToSizeDepVar;
	VecCriticalVars                                         m_vecCriticalVars;
	map < SgFunctionCallExp*, VecVecCriticalVars>           m_MPICallToDepLoopVars;
public:

	void ReOrderMPIFunctionAssociations(SgFunctionDeclaration *pDec, set<SgFunctionDeclaration *> &vecResolvedFunc)
	{
		//get all the function call made from the function
		SgFunctionDefinition *pDef = ((const SgFunctionDeclaration*)pDec)->get_definition ();
		if(pDef)
		{
			NodeQuerySynthesizedAttributeType funCalls = NodeQuery::querySubTree(pDef,V_SgFunctionCallExp);

			for (auto funcCallItr = funCalls.begin(); funcCallItr != funCalls.end(); ++funcCallItr )
			{
				SgFunctionCallExp * funCall = isSgFunctionCallExp(*funcCallItr);
				SgFunctionDeclaration *pFunDec = funCall -> getAssociatedFunctionDeclaration();
				auto retPair = vecResolvedFunc.insert(pFunDec);
				if (retPair.second == true)
					ReOrderMPIFunctionAssociations(pFunDec,vecResolvedFunc);
				if(m_mapFunDeftoMPI.find(pFunDec) != m_mapFunDeftoMPI.end())
				{
					set< SgFunctionCallExp* > &mpiCalls = m_mapFunDeftoMPI[pFunDec];
					if( !mpiCalls.empty())
					{
						auto setItr = m_mapFunDeftoMPI.find(pDec);
						if(setItr != m_mapFunDeftoMPI.end())
						{
							for ( auto mpiItr = mpiCalls.begin(); mpiItr != mpiCalls.end(); ++mpiItr)
							{
								setItr->second.insert(*mpiItr);
							}
						}
						else
						{
							m_mapFunDeftoMPI.insert(pair<SgFunctionDeclaration*,set< SgFunctionCallExp* >>(pDec,mpiCalls));
						}

					}
				}
			}
		}
	}

	void ReOrderFunctionAssociations(SgFunctionDeclaration *pDec, set<SgFunctionDeclaration *> &vecResolvedFunc)
	{
		//get all the function call made from the function
		SgFunctionDefinition *pDef = ((const SgFunctionDeclaration*)pDec)->get_definition ();
		if(pDef)
		{
			NodeQuerySynthesizedAttributeType funCalls = NodeQuery::querySubTree(pDef,V_SgFunctionCallExp);

			/* for each of these function calls, get the function defention and call ReOrderFunctionAssociations()
			 * also check if the function is already resolved before making the call to ReOrderFunctionAssociations
			 * mark pDec as resolved before making the call
			 *
			 * get all loops for the fun and add it to pDec
			 */

			for (auto funcCallItr = funCalls.begin(); funcCallItr != funCalls.end(); ++funcCallItr )
			{
				SgFunctionCallExp * funCall = isSgFunctionCallExp(*funcCallItr);
				SgFunctionDeclaration *pFunDec = funCall -> getAssociatedFunctionDeclaration();
				auto retPair = vecResolvedFunc.insert(pFunDec);
				if (retPair.second == true)
					ReOrderFunctionAssociations(pFunDec,vecResolvedFunc);
				vector< SgNode* > loopNodes = m_mapFunDeftoLoops[pFunDec];
				if( !loopNodes.empty())
				{
					auto vecItr = m_mapFunDeftoLoops.find(pDec);
					for ( auto loopItr = loopNodes.begin(); loopItr != loopNodes.end(); ++loopItr)
					{
						vecItr->second.push_back(*loopItr);
					}

				}
			}
		}
	}



	void ReOrderLoopAnnotations(SgNode *node)
	{
	    //int line = node->get_file_info()->get_line();
		//cout << "Loop Statement Line: " << line << endl ;
		LoopNodeAnnotation *ann = m_mapLoopNodetoAnnotation[node];
		if(ann->GetNestingDepth() != 0)
		{
			auto vecLoops = ann->GetNestedLoops();
			for (auto loopIter = vecLoops.begin(); loopIter != vecLoops.end(); ++loopIter)
				ReOrderLoopAnnotations(*loopIter);
		}
		VecSFunctionCallDef & funcCalls = ann -> GetFuncCallDefs();
		if (funcCalls.size() > 0 )
		{
			vector<SgNode*> nodes = GetMaxDepthLoopInFunc(funcCalls);
			vector<SgNode*> allNodes = GetAllLoopInFunc(funcCalls);
			if ( !nodes.empty() ) {
				LoopNodeAnnotation *newAnn = m_mapLoopNodetoAnnotation[nodes[0]];
				NestingDepth depthInc = newAnn->GetNestingDepth();
				// find max depth of children
				vector<SgNode*> &lnodes = ann->GetNestedLoops();
				NestingDepth maxDepth = 0;
				if( !lnodes.empty())
					maxDepth = m_mapLoopNodetoAnnotation[lnodes[0]]->GetNestingDepth();
				for ( auto itrNode = lnodes.begin(); itrNode != lnodes.end(); ++itrNode)
				{
					if ( maxDepth < m_mapLoopNodetoAnnotation[*itrNode]->GetNestingDepth())
						maxDepth = m_mapLoopNodetoAnnotation[*itrNode]->GetNestingDepth();
				}



				NestingLevel levelInc = ann->GetNestingLevel();
				for (auto itr = allNodes.begin(); itr != allNodes.end(); ++itr)
				{
					IncLoopLevel(levelInc,*itr);
					ann->SetNestedLoops(*itr);
					m_mapLoopNodetoAnnotation[(*itr)]->SetParentNode(node);
				}
				SgNode *tmpNode = node;
				if(depthInc > maxDepth )
				{
					for(;tmpNode;tmpNode = m_mapLoopNodetoAnnotation[tmpNode]->GetParentNode() )
					{
						LoopNodeAnnotation *editAnn = m_mapLoopNodetoAnnotation[tmpNode];
						editAnn->SetNestingDepth( editAnn->GetNestingDepth() + (depthInc - maxDepth) );
					}
				}
			}

		}
		return;

	}

	void IncLoopLevel(NestingLevel level, SgNode *node)
	{
		LoopNodeAnnotation *ann = m_mapLoopNodetoAnnotation[node];
		ann->SetNestingLevel( ann->GetNestingLevel() + level);
		vector<SgNode*> &nodes = ann->GetNestedLoops();
		for ( auto itr = nodes.begin(); itr != nodes.end(); ++itr)
		{
			IncLoopLevel(level,*itr);
		}
		return;
	}

	vector<SgNode*>  GetAllLoopInFunc(VecSFunctionCallDef & funcCalls )
	{
		vector<SgNode*> nodes;
		nodes.clear();
		for ( auto itr = funcCalls.begin(); itr != funcCalls.end(); ++itr)
		{
			SgFunctionDeclaration *pDec = (*itr)->defPtr;
			vector<SgNode*> &loopNodes = m_mapFunDeftoLoops[pDec];
			for ( auto nodeItr = loopNodes.begin(); nodeItr != loopNodes.end(); ++nodeItr)
				nodes.push_back(*nodeItr);
		}
		return nodes;
	}

	vector<SgNode*> GetMaxDepthLoopInFunc(VecSFunctionCallDef & funcCalls )
	{
		vector<SgNode*> nodes;
		nodes.clear();
		for ( auto itr = funcCalls.begin(); itr != funcCalls.end(); ++itr)
		{
			SgFunctionDeclaration *pDec = (*itr)->defPtr;
			vector<SgNode*> &loopNodes = m_mapFunDeftoLoops[pDec];
			for ( auto nodeItr = loopNodes.begin(); nodeItr != loopNodes.end(); ++nodeItr)
			{
				if ( nodes.empty())
					nodes.push_back(*nodeItr);
				LoopNodeAnnotation *ann = m_mapLoopNodetoAnnotation[*nodeItr];
				LoopNodeAnnotation *refAnn = m_mapLoopNodetoAnnotation[nodes[0]];
				if ( ann->GetNestingDepth() > refAnn->GetNestingDepth())
				{
					nodes.clear();
					nodes.push_back(*nodeItr);
				}
				else if(ann->GetNestingDepth() == refAnn->GetNestingDepth() && ann != refAnn)
				{
					nodes.push_back(*nodeItr);
				}
			}
		}
		return nodes;

	}


	VecVecCriticalVars ResolveNestedLoopVarDependency(SgNode* node)
	{
		VecVecCriticalVars vars;
		LoopNodeAnnotation *ann = m_mapLoopNodetoAnnotation[node];
		vector<SgNode*> &nestedLoop = ann->GetNestedLoops();
		for ( auto itr = nestedLoop.begin(); itr != nestedLoop.end() ; ++itr)
		{
			const VecVecCriticalVars &nestVars = ResolveNestedLoopVarDependency(*itr);
			for ( auto itrVar = nestVars.begin(); itrVar != nestVars.end(); ++itrVar)
				vars.push_back(*itrVar);
		}
		VecCriticalVars & currDepVar = ann->GetDependentVar();
		if( vars.empty())
		{
			for ( auto itr = currDepVar.begin(); itr != currDepVar.end(); ++itr )
			{
				VecCriticalVars var;
				var.push_back(*itr);
				vars.push_back(var);
			}
			return vars;
		}
		else
		{
			VecVecCriticalVars retVars;
			for ( auto itrvv = vars.begin(); itrvv != vars.end(); ++itrvv)
			{
				for (auto itrv = currDepVar.begin(); itrv != currDepVar.end(); ++itrv)
				{
					VecCriticalVars &varvv = *itrvv;
					VecCriticalVars newCVar;
					for ( auto itr = varvv.begin(); itr != varvv.end(); ++itr)
						newCVar.push_back(*itr);
					newCVar.push_back(*itrv);
					retVars.push_back(newCVar);
				}
			}
			return retVars;
		}
	}

	set<SgFunctionCallExp*> GetMPICallList(SgNode *node)
	{
		set<SgFunctionCallExp*> funcList;
		NodeQuerySynthesizedAttributeType funCalls = NodeQuery::querySubTree(node,V_SgFunctionCallExp);

		for ( auto itr = funCalls.begin(); itr != funCalls.end(); ++itr)
		{
			SgFunctionCallExp * funCall = isSgFunctionCallExp(*itr);
			SgFunctionDeclaration *pFunDec = funCall -> getAssociatedFunctionDeclaration();
			if(pFunDec)
			{
				string sFuncName = pFunDec -> get_qualified_name();
				//cout << funCall -> unparseToString() << "|" << sFuncName << "|" << funCall->get_file_info()->get_line() << "|" \
						<< funCall->get_file_info()->get_filename() << endl;
				if (isMPICall(sFuncName))
					funcList.insert(funCall);
			}
		}
		return funcList;
	}

//	set
};

CLoopNestingAnalyzer::CLoopNestingAnalyzer() {
	m_pImpl_ = ImplPtr(new CLoopNestingAnalyserImpl());

}

CLoopNestingAnalyzer::~CLoopNestingAnalyzer() {
	// TODO Auto-generated destructor stub
}

ReturnCode 	CLoopNestingAnalyzer::StartAnalysis(SgProject *pProject, VecCriticalVars &vecVars)
{
	m_pImpl_->Initialize(pProject,vecVars);
	m_pImpl_->StartAnalysis();
	return SUCCESS;
}

ReturnCode   CLoopNestingAnalyzer::GetResults(VecLoopNestVarDep &vecResult, std::vector<SMPICallDesc> &mpiDescs)
{
	set<SgFunctionDeclaration *> setFuncDec;
	for ( auto itr = m_pImpl_->m_mapFunDeftoLoops.begin(); itr != \
		                                   m_pImpl_->m_mapFunDeftoLoops.end(); ++itr)
	{
		m_pImpl_->ReOrderFunctionAssociations(itr->first,setFuncDec);
	}


	VecLoopNestVarDep vecLoopDesc;
	vector<SgNode*> vecLevel_1_Loops;
	for ( auto itr = m_pImpl_->m_mapLoopNodetoAnnotation.begin(); itr != \
	                                   m_pImpl_->m_mapLoopNodetoAnnotation.end(); ++itr)
	{
		if ( itr->second->GetNestingLevel() == 1 )
		{
			//cout << itr->second->GetLineNo()  << endl;
			m_pImpl_->ReOrderLoopAnnotations(itr->first);
			//print all annotations
		}
	}

	for ( auto itr = m_pImpl_->m_mapLoopNodetoAnnotation.begin(); itr != \
		                                   m_pImpl_->m_mapLoopNodetoAnnotation.end(); ++itr)
	{
		if ( itr->second->GetNestingLevel() == 1 )
		{

			SLoopNestingDef loopDef;
			loopDef.nNestDepth = itr->second->GetNestingDepth();
			loopDef.sFile      = itr->second->GetFileName();
			loopDef.nLine      = itr->second->GetLineNo();
			VecVecCriticalVars vars;
			vars = m_pImpl_->ResolveNestedLoopVarDependency(itr->first);
			vecResult.push_back(pair <SLoopNestingDef, VecVecCriticalVars> (loopDef, vars));

		}
	}

	m_pImpl_->GetMPIResults(mpiDescs);

	return SUCCESS;
}


} /* namespace StaticAnlysis */
