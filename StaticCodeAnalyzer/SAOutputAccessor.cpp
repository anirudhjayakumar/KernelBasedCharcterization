/*
 * SAOutputAccessor.cpp
 *
 *  Created on: 09-Aug-2013
 *      Author: Anirudh Jayakumar (jayakumar.anirudh@gmail.com)
 */
#include <string>
#include "SAOutputAccessor.h"
#include "tinyxml.h"
#include <boost/lexical_cast.hpp>
#include <set>
#define ELEMENT_STATICANALYSIS "StaticAnalysis"
#define ELEMENT_LOOPNESTING    "LoopNesting"
#define ELEMENT_LOOP           "Loop"
#define ELEMENT_CRITVAR        "CriticalVar"
#define ELEMENT_VAR            "Var"
#define ELEMENT_MPICALL        "MPICalls"
#define ELEMENT_CALL           "Call"

#define ATTRIB_FILE 		   "file"
#define ATTRIB_LINE 		   "line"
#define ATTRIB_DEPTH           "depth"
#define ATTRIB_VAR_COMB        "varComb"
#define ATTRIB_PFEXPR          "postfixExpr"
#define ATTRIB_ID              "id"
#define ATTRIB_ORIGINAL        "org"
#define ATTRIB_SIZEDEPVARS     "sizeDepVars"
#define ATTRIB_LOOPDEPVARS     "loopDepVars"
#define ATTRIB_TYPE            "type"

using namespace std;
namespace StaticAnlysis
{

CSAOutputAccessor::CSAOutputAccessor()
{
    // TODO Auto-generated constructor stub

}

CSAOutputAccessor::~CSAOutputAccessor()
{
    // TODO Auto-generated destructor stub
}

ReturnCode CSAOutputAccessor::Initialize()
{
    m_pDoc = boost::shared_ptr<TiXmlDocument>( new TiXmlDocument() );
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    m_pDoc->LinkEndChild(decl);
    TiXmlElement *root = new TiXmlElement(ELEMENT_STATICANALYSIS);
    m_pDoc->LinkEndChild(root);
    TiXmlElement *loopNesting = new TiXmlElement(ELEMENT_LOOPNESTING);
    root->LinkEndChild(loopNesting);
    TiXmlElement *critVar = new TiXmlElement(ELEMENT_CRITVAR);
    root->LinkEndChild(critVar);
    TiXmlElement *mpiCall = new TiXmlElement(ELEMENT_MPICALL);
    root->LinkEndChild(mpiCall);
    return SUCCESS;
}

ReturnCode CSAOutputAccessor::AddCriticalVars(const VecCriticalVars &vars)
{
    TiXmlElement *critVars = m_pDoc->RootElement()->FirstChildElement(ELEMENT_CRITVAR);
    for( auto itr = vars.begin(); itr != vars.end(); ++itr)
        {
            TiXmlElement *pVar = new TiXmlElement(ELEMENT_VAR);
            pVar->SetAttribute(ATTRIB_ID,(*itr)->nID);
            pVar->SetAttribute(ATTRIB_PFEXPR,(*itr)->sExprStr.c_str());
            pVar->SetAttribute(ATTRIB_ORIGINAL,(*itr)->pOrignal->sVariableName.c_str());
            critVars->LinkEndChild(pVar);
        }
}

ReturnCode CSAOutputAccessor::AddLoopNode(const SLoopNestingDef &loopDef, const VecVecCriticalVars &vars)
{
    TiXmlElement *loopNesting = m_pDoc->RootElement()->FirstChildElement(ELEMENT_LOOPNESTING);
    TiXmlElement *loop = new TiXmlElement(ELEMENT_LOOP);
    loop->SetAttribute(ATTRIB_FILE,loopDef.sFile.c_str());
    loop->SetAttribute(ATTRIB_LINE,boost::lexical_cast<string>(loopDef.nLine).c_str());
    loop->SetAttribute(ATTRIB_DEPTH,boost::lexical_cast<string>(loopDef.nNestDepth).c_str());
    string varCombfinal("");
    for(auto itr = vars.begin(); itr != vars.end(); ++itr)
        {
            string varComb("");
            const VecCriticalVars &criVar = *itr;
            for ( auto itrvar = criVar.begin(); itrvar != criVar.end(); ++itrvar)
                {
                    varComb += boost::lexical_cast<string>((*itrvar)->nID);
                    if( itrvar + 1 != criVar.end())
                        varComb += ",";
                }
            varCombfinal += varComb;
            if( itr + 1 != vars.end())
                varCombfinal += ":";
        }


    loop->SetAttribute(ATTRIB_VAR_COMB,RemoveRedundantVarCombs(varCombfinal).c_str());
    loopNesting->LinkEndChild(loop);
    return SUCCESS;
}

string CSAOutputAccessor::RemoveRedundantVarCombs(const string &str)
{
    set< vector<string> > varCombList;
    vector<string> varCombs = GARLUTILS::Tokenize(str,":");
    for ( auto varCombItr = varCombs.begin(); varCombItr != varCombs.end(); ++varCombItr)
        {
            string svarComb = *varCombItr;
            vector<string> vars = GARLUTILS::Tokenize(svarComb,",");
            sort(vars.begin(), vars.end());
            varCombList.insert(vars);
        }

    string sRet;
    for ( auto itr = varCombList.begin(); itr != varCombList.end(); ++itr)
        {
            if ( itr != varCombList.begin()) sRet+=":";
            string varComb;
            vector<string> vars = *itr;
            for ( auto varItr = vars.begin(); varItr != vars.end(); ++varItr)
                {
                    if ( varItr != vars.begin()) varComb+=",";
                    varComb+=*varItr;
                }
            sRet+=varComb;
        }
    return sRet;
}

ReturnCode CSAOutputAccessor::AddMPICallDescs(const std::vector<SMPICallDesc> &mpiCalls)
{
    TiXmlElement *pMPI = m_pDoc->RootElement()->FirstChildElement(ELEMENT_MPICALL);
    for(auto callItr = mpiCalls.begin(); callItr != mpiCalls.end(); ++callItr)
        {
            TiXmlElement *mpi = new TiXmlElement(ELEMENT_CALL);
            mpi->SetAttribute(ATTRIB_FILE,callItr->sFile.c_str());
            mpi->SetAttribute(ATTRIB_LINE,callItr->nLine);
            VecCriticalVars vars = callItr->sizeDep;
            string sizeVars("");
            for(auto varItr = vars.begin(); varItr != vars.end(); ++varItr)
                {
                    if(varItr != vars.begin()) sizeVars+=",";
                    sizeVars += boost::lexical_cast<string>((*varItr)->nID);
                }
            mpi->SetAttribute(ATTRIB_SIZEDEPVARS,sizeVars.c_str());
            mpi->SetAttribute(ATTRIB_TYPE,callItr->sType.c_str());
            VecVecCriticalVars vvVars = callItr->loopDep;
            string varCombfinal("");
            for(auto itr = vvVars.begin(); itr != vvVars.end(); ++itr)
                {
                    string varComb("");
                    const VecCriticalVars &criVar = *itr;
                    for ( auto itrvar = criVar.begin(); itrvar != criVar.end(); ++itrvar)
                        {
                            varComb += boost::lexical_cast<string>((*itrvar)->nID);
                            if( itrvar + 1 != criVar.end())
                                varComb += ",";
                        }
                    varCombfinal += varComb;
                    if( itr + 1 != vvVars.end())
                        varCombfinal += ":";
                }
            mpi->SetAttribute(ATTRIB_LOOPDEPVARS,RemoveRedundantVarCombs(varCombfinal).c_str());
            pMPI->LinkEndChild(mpi);
        }

}

bool CSAOutputAccessor::SaveFile( const std::string &filePath)
{
    return m_pDoc->SaveFile(filePath.c_str());
}

} /* namespace StaticAnlysis */
