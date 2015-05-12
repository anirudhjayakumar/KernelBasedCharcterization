/*
 * FrameworkCommon.cpp
 *
 *  Created on: 29-Aug-2013
 *      Author: anirudhj
 */
#include "FrameworkCommon.h"
#include <boost/filesystem.hpp>
using namespace std;

namespace GARLUTILS
{
vector<string> Tokenize(string sText, string sToken)
{
    vector<string> vecTokens;
    boost::char_separator<char> sep(sToken.c_str());
    boost::tokenizer< boost::char_separator<char> > tokens(sText, sep);
    BOOST_FOREACH (const string& t, tokens)
    {
        vecTokens.push_back(t);
    }
    return vecTokens;
}

std::string GetFileNameFromPath(const std::string &strFileName)
{
    boost::filesystem::path const p(strFileName	);
    return p.filename().c_str();
}

bool  IsAllTrue(const std::vector<bool> &vecs)
{
    for (auto itr = vecs.begin(); itr != vecs.end(); ++itr)
        if (*itr == false)
            return false;
    return true;
}

bool  IsAllFalse(const std::vector<bool> &vecs)
{
    for (auto itr = vecs.begin(); itr != vecs.end(); ++itr)
        if (*itr == true)
            return false;
    return true;
}

void  NextPermutation(std::vector< int >  &vecblockIndex,
                      const std::vector< int >  &vecblockSize)
{
    int sizeVec = vecblockIndex.size();
    for ( int index_ = sizeVec -1; index_ >= 0; --index_)
        {
            if( ++vecblockIndex[index_] == vecblockSize[index_])
                {
                    vecblockIndex[index_] = 0;
                }
            else
                break;
        }
}

} // namespace GARLUTILS
