/*
 * SACommon.cpp
 *
 *  Created on: 21-Aug-2013
 *      Author: anirudhj
 */
#include "SACommon.h"
using namespace std;
#include <vector>
#include <map>
#include <boost/lexical_cast.hpp>
#include "tinyxml.h"
#include <algorithm>
namespace StaticAnlysis
{

void PrintVecCriticalVar(VecCriticalVars &vars)
{
    cout << endl << " printing critical variable list" << endl;

    for ( auto itr = vars.begin(); itr != vars.end(); ++itr)
        {
            cout << (*itr)->sVariableName << " " << (*itr)->nID << " " <<  (*itr)->pOrignal->sVariableName << " " << (*itr)->nDefinitionLine << " " << (*itr)->sDefinitionFile << " " << (*itr)->sExprStr << endl;
        }
    cout << " end printing critical variable list" << endl;
}


bool Contains(const VecCriticalVars &vars, const SCriticalVariableDefPtr &ptr, int &index)
{
    index = 0;
    for ( auto itr = vars.begin(); itr != vars.end(); ++itr)
        {
            if(*(*itr) == *ptr)
                {
                    return true;
                }
            index++;
        }
    return false;
}



bool FindFunctionCallPtrInVector(SFunctionCallDefPtr ptr,VecSFunctionCallDef &vecDefs)
{
    for ( auto itr = vecDefs.begin(); itr != vecDefs.end(); ++itr)
        {
            if ( *(*itr) == *ptr)
                return true;
        }
    return false;
}

bool CStaticAnalyzeCommon::instanceFlag = false;

CStaticAnalyzeCommon* CStaticAnalyzeCommon::instance = NULL;

CStaticAnalyzeCommon* CStaticAnalyzeCommon::getInstance()
{
    if(! instanceFlag)
        {
            instance = new CStaticAnalyzeCommon();
            instanceFlag = true;
            return instance;
        }
    else
        {
            return instance;
        }
}

ReturnCode CStaticAnalyzeCommon::Init(const std::string &file)
{
    TiXmlDocument XmlDoc(file.c_str());
    if( !XmlDoc.LoadFile())
        {
            cout << "Loading xml failed" << endl;
            return FAIL;
        }
    TiXmlElement *pRootXml = XmlDoc.RootElement();
    m_sProgramName = pRootXml->Attribute("program");
    string sType = pRootXml->Attribute("type");
    bool found = false;
    for( int index = 0 ; index < e_LANG_SUPPORT_COUNT; ++index)
        {
            if( mapLanguageStrings[index] == sType)
                {
                    m_eLanguage = (ELanguage)index;
                    found = true;
                    break;
                }
        }

    if( !found )
        {
            cout << "Unable to resolve language type" << endl;
            return FAIL;
        }

    TiXmlElement *pFortranCommonBlocks = pRootXml->FirstChildElement("FortranCommonBlocks");
    if(pFortranCommonBlocks)
        {
            TiXmlElement *pBlock = pFortranCommonBlocks->FirstChildElement("Block");
            if(pBlock)
                {
                    for( ; pBlock; pBlock=pBlock->NextSiblingElement("Block"))
                        {
                            int key;
                            vector<int> mapLines;
                            pBlock->Attribute("keyLine", &key);
                            string sMapingLines = pBlock->Attribute("mapingLines");
                            vector<string> tokens = GARLUTILS::Tokenize(sMapingLines,",");
                            for(auto itr = tokens.begin(); itr != tokens.end(); ++itr)
                                {
                                    mapLines.push_back(boost::lexical_cast<int>(*itr));
                                }
                            string sFile = pBlock->Attribute("file");
                            m_FortranCommonBlocks.insert( pair<IntStringPair, vector<int> >(IntStringPair(key,sFile), mapLines));

                        }
                }
        }
    return SUCCESS;
}

int        CStaticAnalyzeCommon::FortranCommonBlockLineNo(const int nLineNo, const std::string &file)
{
    for ( auto mapItr = m_FortranCommonBlocks.begin(); mapItr != m_FortranCommonBlocks.end(); ++mapItr)
        {
            if( mapItr->first.second == file)
                {
                    for (auto vecItr = mapItr->second.begin(); vecItr != mapItr->second.end(); ++vecItr)
                        {
                            if ( (*vecItr) == nLineNo)
                                return mapItr->first.first;
                        }
                }
        }
    return nLineNo;
}

bool isMPICall(std::string sName)
{
    sName.erase (remove(sName.begin(), sName.end(), ':'), sName.end());
    transform(sName.begin(), sName.end(), sName.begin(), ::tolower);
    if (sName == strMPI_ALLREDUCE || sName == strMPI_ALLTOALL || sName == strMPI_BCAST || \
            sName == strMPI_REDUCE || sName == strMPI_SEND)
        {
            return true;
        }
    return false;
}

}
