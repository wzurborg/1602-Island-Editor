#ifndef _szen_doc_h_
#define _szen_doc_h_

#include <classlib/file.h>
#include <owl/filedoc.h>
#include <owl/dc.h>
#include "inselstruct.h"
#include "inseldoc.h"

class SzenView;
class SzenDoc;

//using namespace std;
class SzenDoc : public TFileDocument
{
public:
	SzenDoc( TDocument* parent = 0 );
   virtual bool Open( int mode, const char far* path=0 );
   virtual bool InitDoc();
   virtual bool IsOpen();
   virtual bool Close();
   virtual bool Commit( bool force = false );
   virtual bool Revert( bool clear = false );

   void draw( TDC& tdc, TRect& r );
   void setView( SzenView* view );
   virtual ~SzenDoc();

private:

	void readInsel( unsigned char* buffer, uint buflength,
   					InselDoc::FileType type );
	void showInsel( TDC& tdc );
   SzenView* myView;
   TFile myFile;
   unsigned char* myBuffer;
   unsigned long bufLength;
   CONTAINER<InselDoc*>* myInseln;
};
#endif
