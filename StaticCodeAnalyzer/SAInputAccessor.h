/*
 * SAInputAccessor.h
 *
 *  Created on: 09-Aug-2013
 *      Author: Anirudh Jayakumar (jayakumar.anirudh@gmail.com)
 */

#ifndef SAINPUTACCESSOR_H_
#define SAINPUTACCESSOR_H_

#include <string>
#include <vector>
#include "FrameworkCommon.h"
#include "SACommon.h"
namespace StaticAnlysis {



class CSAInputAccessor {
private:
	VecCriticalVars m_vecCriticalVars;
public:
	CSAInputAccessor();
	virtual ~CSAInputAccessor();
	ReturnCode  Initialize(const std::string &sConfigFile);

	VecCriticalVars  EnumerateCriticalVariables();

};

} /* namespace StaticAnlysis */
#endif /* SAINPUTACCESSOR_H_ */
