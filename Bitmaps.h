#ifndef __bitmaps_h__
#define __bitmaps_h__

#include <cstring.h>
#include <owl/gdiobjec.h>
#include <map>

struct AnnotatedBitmap
{
	AnnotatedBitmap() {bitmap=0;}
	TBitmap** bitmap;
   string descr;
};

using namespace std;
class Bitmaps : private map < int, AnnotatedBitmap, less<int> >
{
public:
	Bitmaps();
   TBitmap* getBitmap( const TDC& dc, int key, int ori );
   const string& getDescr( int key );

private:
   TBitmap* turnBitmap( const TBitmap& bm, int ori );
};

#endif