/*
 * SACommon.h
 *
 *  Created on: 09-Aug-2013
 *      Author: anirudhj
 */

#ifndef SACOMMON_H_
#define SACOMMON_H_
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include "FrameworkCommon.h"
class SgFunctionDeclaration;
class SgNode;

namespace StaticAnlysis
{
struct SCriticalVariableDef;

typedef boost::shared_ptr<SCriticalVariableDef>  SCriticalVariableDefPtr;
typedef std::vector<SCriticalVariableDefPtr> VecCriticalVars;
typedef std::vector<VecCriticalVars>      VecVecCriticalVars;

struct SCriticalVariableDef
{
    std::string sVariableName;
    std::string sDefinitionFile; // the file where the variable is defined
    int		    nDefinitionLine; // the line where the variable is defined
    int         nID; //unique identifier
    std::string sExprStr; //the postfix representation of the expression
    SCriticalVariableDefPtr pOrignal; //used for dependent vars to point to the original vars
    bool operator==( const SCriticalVariableDef &othr) const
    {
        if (sVariableName == othr.sVariableName && sDefinitionFile == othr.sDefinitionFile \
                &&  nDefinitionLine == othr.nDefinitionLine)
            return true;
        else
            return false;
    }
    bool operator<( const SCriticalVariableDef &otr) const
    {
        if (this->sVariableName != otr.sVariableName)
            return this->sVariableName < otr.sVariableName;
        else if (this->sDefinitionFile != otr.sDefinitionFile)
            return this->sDefinitionFile < otr.sDefinitionFile;
        else if (this->nDefinitionLine != otr.nDefinitionLine)
            return this->nDefinitionLine < otr.nDefinitionLine;
        else
            return false;
    }
    void Print()
    {
        std::cout << "Name: " << sVariableName <<  std::endl;
        std::cout << "File: " << sDefinitionFile <<  std::endl;
        std::cout << "Line: " << nDefinitionLine <<  std::endl;

    }
    SCriticalVariableDef()
    {
        sVariableName = sDefinitionFile = sExprStr = "";
    }
};

void PrintVecCriticalVar(VecCriticalVars &vars);
bool Contains(const VecCriticalVars &vars, const SCriticalVariableDefPtr &ptr, int &index);

struct SLoopNestingDef
{
    int         nNestDepth;
    std::string sFile;
    int         nLine;
};

typedef std::vector<SLoopNestingDef> VecLoopNestingDef;

typedef std::vector< std::pair<SLoopNestingDef, VecVecCriticalVars > > VecLoopNestVarDep;

bool isMPICall(std::string sName);


struct SFunctionCallDesc
{
    int         nLine;
    std::string sFile;
    SgFunctionDeclaration *defPtr;
    bool operator==( const SFunctionCallDesc &othr) const
    {
        if (nLine == othr.nLine && sFile == othr.sFile \
                &&  defPtr == othr.defPtr)
            return true;
        else
            return false;
    }
};

typedef boost::shared_ptr<SFunctionCallDesc>  SFunctionCallDefPtr;

typedef std::vector<SFunctionCallDefPtr> VecSFunctionCallDef;

bool FindFunctionCallPtrInVector(SFunctionCallDefPtr ptr,VecSFunctionCallDef &vecDefs);


typedef std::vector<SgNode*> VecSgNodes;
typedef std::vector<VecSgNodes> VecVecSgNodes;

struct SMPICallDesc
{
    int					nLine;
    std::string     	sFile;
    VecVecCriticalVars  loopDep;
    VecCriticalVars 	sizeDep;
    std::string         sType;
};



#define LANGUAGE_SUPPORT_TABLE \
	X(e_C99, 		  	"C99") \
	X(e_FORTRAN77, 		"FORTRAN77") \

enum ELanguage
{
#define X(a,b) a,
    LANGUAGE_SUPPORT_TABLE
#undef X
    e_LANG_SUPPORT_COUNT
};

#define X(a,b) b,
static const char * mapLanguageStrings[] = { LANGUAGE_SUPPORT_TABLE };
#undef X

typedef std::pair<int,std::string> IntStringPair;
class CStaticAnalyzeCommon
{
private:
    static bool instanceFlag;
    static CStaticAnalyzeCommon *instance;
    CStaticAnalyzeCommon()
    {
        //private constructor
    }
    std::map<IntStringPair,std::vector<int> > m_FortranCommonBlocks;
    std::string  m_sProgramName;
    ELanguage    m_eLanguage;
public:
    static CStaticAnalyzeCommon* getInstance();
    ~CStaticAnalyzeCommon()
    {
        instanceFlag = false;
    }
    ReturnCode Init(const std::string &file);
    int        FortranCommonBlockLineNo(const int nLineNo, const std::string &file);
    std::string GetProgramName()
    {
        return m_sProgramName;
    }
    ELanguage  GetLanguageType()
    {
        return m_eLanguage;
    }
};



}
#endif /* SACOMMON_H_ */
