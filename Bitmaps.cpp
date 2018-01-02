#include <owl/decframe.h>
#include <classlib/file.h>
#include "annoedit.h"
#include "bitmaps.h"
#include <stdio.h>

Bitmaps::Bitmaps()
{
	int key;
   string descr;
   char c;
	ifstream file( descrFile.c_str() );
   if( !file.good() )
   {
      msgBox( 0, IDS_DESCRNOTFOUND, IDS_ERROR, MB_OK );
   	return;
   }
   file.setf( ios::skipws );
   while( true )
   {
   	file >> c;
      if( isdigit(c) )
      {
      	file.putback( c );
	   	file >> key;
   	   descr.read_line( file );
      	if( !file.good() ) break;
	      (*this)[key].descr = descr;
      }
      else file.ignore( 999, '\n' );
   }
}

TBitmap* Bitmaps::getBitmap( const TDC& dc, int key, int ori )
{
   TBitmap**& bm_pp = (*this)[key].bitmap;
   if( bm_pp == 0 )
   {
      bm_pp = new TBitmap*[4];
      for( int i=0; i<4; i++ ) bm_pp[i] = 0;
   }

   if( bm_pp[ori] == 0 )
   {
      if( bm_pp[0] == 0 )
      {
			char buffer[10];
         sprintf( buffer, "%d", key );
			string bmName = bitmapsDir + "\\";
         bmName.append( buffer );
         bmName.append( ".bmp" );
         TFile bmf( bmName.c_str(), TFile::ReadOnly, TFile::PermRead );
         TFileStatus s;
         if( bmf.GetStatus( s ) != 0 )
         {
         	TDib dib( bmf );
            if( dib.NumColors() == 256 )
	         	bm_pp[0] = new TBitmap( dc, dib );
			}
      }

      if( bm_pp[0] != 0 )
      {
      	if( ori != 0 )
         {
         	bm_pp[ori] = turnBitmap( *bm_pp[0], ori );
         }
      }
   }

	return bm_pp[ori];
}

TBitmap* Bitmaps::turnBitmap( const TBitmap& bm, int ori )
{
	if( ori == 0 )
   	return new TBitmap( bm );

	BITMAP bms;
   bm.GetObject( bms );

	uint32 numPixel = bms.bmWidth * bms.bmHeight;
   uint32 numBytes = bms.bmWidthBytes * bms.bmHeight;
   uint32 bytesPerPixel = bms.bmBitsPixel / 8;

   uchar* oldBits = new uchar[ numBytes ];
	bm.GetBitmapBits( numBytes, oldBits );

   uchar* newBits = new uchar[ numBytes ];
   int i;
   for( i=0; i<numPixel; i++ )
   {
   	int x1 = i%bms.bmWidth;
      int y1 = i/bms.bmWidth;
      int x2, y2;
   	switch( ori%4 )
      {
      	case 1:
         	x2 = y1;
            y2 = bms.bmWidth - 1 - x1;
         	break;
         case 2:
         	x2 = bms.bmWidth - 1 - x1;
         	y2 = bms.bmHeight - 1 - y1;
            break;
         case 3:
         	x2 = bms.bmHeight - 1 - y1;
            y2 = x1;
            break;
         default:
         	x2 = x1;
            y2 = y1;
            break;
      }
      int i1 = bms.bmWidthBytes * y1 + x1 * bytesPerPixel;
      int j1 = bms.bmWidthBytes * y2 + x2 * bytesPerPixel;
      for( int ii = 0; ii<bytesPerPixel; ii++ )
	      newBits[i1+ii] = oldBits[j1+ii];
   }

   TBitmap* newBm = new TBitmap( bm );
	newBm->SetBitmapBits( numBytes, newBits );
	delete [] oldBits;
   delete [] newBits;
   return newBm;
}

const string& Bitmaps::getDescr( int key )
{
   return (*this)[key].descr;
}

