/*
 * RInstance.h
 *
 *  Created on: Apr 10, 2014
 *      Author: anirudhj
 */

#ifndef RINSTANCE_H_
#define RINSTANCE_H_

#define FIND_X 'X'
#define FIND_Y 'Y'

#include <string>
class RInside;
float localRegression(const std::string &sFile,float var, char find, RInside *r);

#endif /* RINSTANCE_H_ */
