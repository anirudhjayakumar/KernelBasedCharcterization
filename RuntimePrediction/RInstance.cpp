/*
 * RInstance.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: anirudhj
 */




#include <RInside.h>
#include "RInstance.h"
#include <boost/lexical_cast.hpp>
using namespace std;
// for the embedded R via RInside
float localRegression(const string &sFile, float var,char find,RInside* R)
{
    //RInside R;                      // create an embedded R instance
    std::string str;
    if( find == FIND_Y)
    {
    	str =
        "library(\"locfit\");"
        "mpi <- read.csv(\"" + sFile +"\",head=TRUE,sep=\",\");"
        "fit <- locfit(t~lp(s),mpi);"
        "ret <- predict(fit," + boost::lexical_cast<string>(var) + ");"
        "ret";                     // returns Z
    }
    else if( find == FIND_X)
    {
    	str =
		"library(\"locfit\");"
		"mpi <- read.csv(\"" + sFile +"\",head=TRUE,sep=\",\");"
		"fit <- locfit(s~lp(t),mpi);"
		"ret <- predict(fit," + boost::lexical_cast<string>(var) + ");"
		"ret";
    }

    cout << str << endl;
    float out = Rcpp::as<float>(R->parseEval(str));   // eval string, Z then assigned to num. vec
    return out;
}
