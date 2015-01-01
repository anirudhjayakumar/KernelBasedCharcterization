/*
 * SAInputAccessor.cpp
 *
 *  Created on: 09-Aug-2013
 *      Author: Anirudh Jayakumar (jayakumar.anirudh@gmail.com)
 */

#include "SAInputAccessor.h"
#include "tinyxml.h"
using namespace std;
#include <iostream>
#include <boost/lexical_cast.hpp>

#define ELEMENT_STATICANALYSIS "StaticAnalysis"
#define ELEMENT_CRITICALVAR    "CriticalVar"
#define ELEMENT_VAR            "Var"
#define ATTRIB_NAME			   "name"
#define ATTRIB_FILE			   "file"
#define ATTRIB_LINE			   "line"


namespace StaticAnlysis {

CSAInputAccessor::CSAInputAccessor() {
	// TODO Auto-generated constructor stub

}

CSAInputAccessor::~CSAInputAccessor() {
	// TODO Auto-generated destructor stub
}

ReturnCode  CSAInputAccessor::Initialize(const std::string &sConfigFile)
{
	TiXmlDocument XmlDoc(sConfigFile.c_str());
	if( !XmlDoc.LoadFile())
	{
		cout << "Loading xml failed" << endl;
		return FAIL;
	}
	TiXmlElement *pRootXml = XmlDoc.RootElement();
	TiXmlElement *pCriticalVar = pRootXml->FirstChildElement(ELEMENT_CRITICALVAR);
	if(pCriticalVar)
	{
		TiXmlElement *pVars = pCriticalVar->FirstChildElement(ELEMENT_VAR);
		int index = 0;
		if(pVars)
		{
			for( ;pVars;pVars=pVars->NextSiblingElement(ELEMENT_VAR))
			{
				SCriticalVariableDefPtr var(new SCriticalVariableDef);
				var->sVariableName    = pVars->Attribute(ATTRIB_NAME);
				var->sDefinitionFile  = pVars->Attribute(ATTRIB_FILE);
				pVars->Attribute(ATTRIB_LINE,&(var->nDefinitionLine));
				var->pOrignal = var; //these are the original size variables
				var->nID = index;
				var->sExprStr = boost::lexical_cast<string>(index);
				m_vecCriticalVars.push_back(var);
				index++;
			}
		}
	}


	return SUCCESS;
}

VecCriticalVars  CSAInputAccessor::EnumerateCriticalVariables()
{
	return 	m_vecCriticalVars;
}

} /* namespace StaticAnlysis */

