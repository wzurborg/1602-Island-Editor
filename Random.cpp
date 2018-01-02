#include <fstream.h>
#include "Random.h"
#include "AnnoEdit.h"

Random::Random( const string filename )
{
   // open random file
	ifstream file( filename.c_str() );
   file.setf( ios::skipws );
   if( !file.good() )
   {
      msgBox( 0, IDS_RANDOMNOTFOUND, IDS_ERROR, MB_OK );
   	return;
   }

   // find field type
  	string word;
   NumList* currentList = 0;
   while( true )
   {
   	file >> word;
      if( !file.good() ) break;

      if( word[0] == '[' )
      {
      	// new category found
         word.remove( 0, 1 );
         char c = word[ word.length() - 1 ];
         while( c != ']' )
         {
         	file.unsetf( ios::skipws );
				file >> c;
            file.setf( ios::skipws );
            if( !file.good() ) break;
            word += c;
         }
        	word.remove( word.length() - 1 );
         numLists.push_back( NumList() );
         catIndex.push_back( word );
         currentList = &numLists.back();
      }
      else
      {
      	// add number to current numlist
         if( currentList != 0 )
         {
         	int n = atoi( word.c_str() );
            if( n > 0 )
	         	currentList->push_back( n );
         }
      }
   }
}

int Random::getRandom( int probability, const string& category )
{
	if( random( 100 ) >= probability )
   	return -1;

   for( int n=0; n<catIndex.size(); n++ )
   {
   	if( catIndex[n] == category )
      {
			const NumList& cat = numLists[n];
         if( cat.size() > 0 )
			   return cat[ random( cat.size() - 1 ) ];
      }
   }
   return -1;
}
