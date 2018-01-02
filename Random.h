#ifndef __random_h__
#define __random_h__

#include <stdlib.h>
#include <cstring>
#pragma hdrstop
#include <vector>
#include <map>
#include <utility>
#include <function.h>

using namespace std;

typedef vector<int> NumList;
typedef vector<NumList> NumLists;
typedef vector<string> CatIndex;

class Random
{
friend class RandomDialog;
public:
   Random( const string filename );
   int getRandom( int probability, const string& category );
private:
	CatIndex catIndex;
   NumLists numLists;
};
#endif