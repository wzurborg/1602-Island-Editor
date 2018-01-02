#ifndef _inselfeld_h_
#define _inselfeld_h_

#include <iostream.h>
#include <windef.h>
#include <owl/dc.h>

#include <vector>

using namespace std;

class FldDialog;

class InselFeld
{
friend ostream& operator <<( ostream&, const InselFeld& );
friend class ReplaceFieldId;
friend class InselDoc;
friend class UnmarkField;
friend class MarkIfTouches;
friend class MarkIfId;
friend class MarkIfContains;
friend class FlipIfTouches;
public:
	InselFeld( uint inselnr = 0 );		// creates dummy field
	InselFeld( unsigned char*& inAddr );

   // set only selected elements from other field
   void setSelectedFrom( const InselFeld& other );

   static unsigned unitSize();
   static void setUnitSize( unsigned unitSize );
   static unsigned fldSize( int id ); // number of units

   void write( unsigned char*& outAddr ) const;

   void draw( TDC& tdc ) const;
   void drawMini( TDC& tdc, const TPoint& offset, int factor ) const;
   
   TRect rect() const;	   // returns rectangle on display (pixels)
   TRect fldRect() const;  // returns rectangle on display (fields)

   bool operator ==( const InselFeld& ) const;
   bool operator !=( const InselFeld& ) const;
   bool operator < (const InselFeld& ) const;

   bool isSelected() const;
   void setSelected( bool sel );
   bool isDummy() const;
   int id() const;
   int ori() const;
   string debugInfo();
   TPoint pos() const;  	// returns (x|y) position
   vector<TPoint> allPos() const; // returns all (x|y) points

   const TSize& size() const;
   void setId( int idLow );
   void setOri( int ori );
   void setPos( const TPoint& pos );
   void takeValuesFrom( FldDialog& d );
	void setSize();

private:

	TColor pixelColor() const;
   
	struct
   {
		uint16  _id;
      uint8   _posX;
      uint8   _posY;
     	uint    _ori      : 2;
      uint    _animcnt  : 4;
      uint    _inselnr  : 8;
      uint    _stadtnr  : 3;
      uint    _randnr   : 5;
      uint    _playernr : 4;
   } myStruct;

   bool _selected;
	TSize _size;	// in units of fields, normally (1,1)

   static unsigned _unitSize;
};

#endif