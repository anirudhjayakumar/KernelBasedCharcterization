/*
 * SAOutputAccessor.h
 *
 *  Created on: 09-Aug-2013
 *      Author: Anirudh Jayakumar (jayakumar.anirudh@gmail.com)
 */

#ifndef SAOUTPUTACCESSOR_H_
#define SAOUTPUTACCESSOR_H_
#include "SACommon.h"
#include "FrameworkCommon.h"
#include <string>
#include <boost/shared_ptr.hpp>
class TiXmlDocument;
namespace StaticAnlysis {

class CSAOutputAccessor {
private:
	boost::shared_ptr<TiXmlDocument> m_pDoc;
public:
	CSAOutputAccessor();
	virtual ~CSAOutputAccessor();
	ReturnCode Initialize();
	ReturnCode AddLoopNode(const SLoopNestingDef &loopDef, const VecVecCriticalVars &vars);
	bool       SaveFile( const std::string &filePath);
	ReturnCode AddCriticalVars(const VecCriticalVars &vars);
	ReturnCode AddMPICallDescs(const std::vector<SMPICallDesc> &mpiCalls);
private:
	std::string RemoveRedundantVarCombs(const std::string &str);
};

} /* namespace StaticAnlysis */
#endif /* SAOUTPUTACCESSOR_H_ */
