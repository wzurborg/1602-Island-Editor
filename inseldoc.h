#ifndef _insel_doc_h_
#define _insel_doc_h_

#include <classlib/file.h>
#include <classlib/thread.h>
#include <owl/filedoc.h>
#include <owl/dc.h>

#include "inselfeld.h"
#include "inselstruct.h"

#define CONTAINER deque
#include <deque.h>

class FldDialog;
class InselView;
class InselDoc;
class NewInsel;

class InselThread : public TThread
{
public:
	InselThread( InselDoc& doc, int command );

private:
	virtual int Run();
   InselDoc& _doc;
   int _command;
};


using namespace std;
class InselDoc : public TFileDocument
{
friend class InselThread;
public:
	enum Klimatyp
	{
   	UNKNOWN,
		NORD,
   	SUED
	};

   enum MirrorOperation
   {
      HORIZONTAL,
      VERTICAL
   };

   enum FileType
   {
   	SZS,
      GAM,
      SCP
   };

	InselDoc( TDocument* parent = 0 );
   InselDoc( unsigned char* buffer, uint bufl,
   		    InselDoc::FileType type, TDocument* parent = 0 );
   virtual bool Open( int mode, const char far* path=0 );
   virtual bool InitDoc();
   virtual bool IsOpen();
   virtual bool Close();
   virtual bool Commit( bool force = false );
   virtual bool Revert( bool clear = false );

   void draw( TDC& tdc, TRect& r );
   TRect getSelection( int& numMarked, InselFeld*& firstMarked ) const;
	InselFeld* fieldFromPoint( const TPoint& p );
   InselFeld* markOne( const TPoint& p );
   InselFeld* flipOne( const TPoint& p );
   void unmarkAll();
   bool canUndo() const;
   void undo();
   void markAllTouching( const TRect& r );
   void flipAllTouching( const TRect& r );
   void setAllMarkedFrom( FldDialog& d, bool saveUndo = true );
   void setOneFromDialog( InselFeld& fld, FldDialog& d, bool saveUndo = true );
   void setRandomValues( int probability, const string& category );
	void setRandom( bool saveUndo = true );
   void selectId( int id );	// selects all fields with given id
   void turnFields( int numTurns );    // numTurns=1..3
   void turnArea( int numTurns );    // numTurns=1..3
   void mirrorArea( MirrorOperation op );
   void copyToClipboard();
   void pasteFromClipboard();
   bool clipboardEmpty() const;
   int numMarked() const;
   int numDummy() const;
   void makeDummies();
   const TSize& size() const;
   void setSize( const TSize& size );
   const TPoint& pos() const;
   void setPos( const TPoint& pos );
   Klimatyp klima() const;
   void setKlima( Klimatyp type );
   void adjustTrees( bool saveUndo = true );
   void setView( InselView* view );
   void processCommand( int command );
   void commandTerminated();
   bool isBusy() const;

   // Functions for Szenario Views
   bool readFromBuffer();
   void drawMini( TDC& tdc ) const;
      
   virtual ~InselDoc();

private:
	void threadCommand( int command );

   void saveUndoInfo();
   bool createNewInsel();

	struct Clipboard
   {
	   TPoint cbTopLeft;		// top left field of clipboard
	   CONTAINER<InselFeld> cbData;
   };
   static Clipboard clipboard;

   CONTAINER<InselFeld>* myInsel_p;

   struct
   {
   	CONTAINER<InselFeld>* insel_p;
      TSize size;
   } myUndo;
   
//   InselFeld** feldIndex;
	int* feldIndex;

   uint myInseltyp;		// 3 or 5
   uint myInselNr;		// steht in jedem Feld
   union
   {
   	TINSEL5* insel5;
      TINSEL3* insel3;
   } header;

   InselView* myView;
   InselThread* myThread;
   TFile myFile;
   FileType myFileType;
   TSize mySize;
   TPoint myPos;
   Klimatyp myKlima;
   int myProbability;
   string myCategory;	// for random operation
   unsigned char* myBuffer;
   unsigned long bufLength;
   int index1stField;	// buffer index of first field
   bool deleteBuffer;
};
#endif
