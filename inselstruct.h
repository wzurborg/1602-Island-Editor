#ifndef _inselstruct_h_
#define _inselstruct_h_

   struct TERZBERG
   {
      unsigned char   ident;
      uint8  posx;
      uint8  posy;
      uint8  bytes[3];
      uint16 summe;
   };

   struct TINSEL3
   {
	uint8      inselnr;
	uint8      felderx;
	uint8      feldery;
 	uint8      strtduerrflg:1;
	uint8      nofixflg:1;
	uint8      vulkanflg:1;
   uint16     posx;
	uint16     posy;
	uint16     hirschreviercnt;
	uint16     speedcnt;
   uint8		  unknown[28];
   };

   struct TINSEL5
   {
	uint8      inselnr;
	uint8      felderx;
	uint8      feldery;
 	uint8      strtduerrflg:1;
	uint8      nofixflg:1;
	uint8      vulkanflg:1;
   uint16     posx;
	uint16     posy;
	uint16     hirschreviercnt;
	uint16     speedcnt;
	uint8      stadtplayernr[11];
	uint8      vulkancnt;
	uint8      schatzflg;
	uint8      rohstanz;
	uint8      eisencnt;
	uint8      playerflags;
	TERZBERG   eisenberg[4];
	TERZBERG   vulkanberg[4];
	uint       rohstflags;
	uint16     filenr;
	uint16     sizenr;
	uint8      klimanr;
	uint8      orginalflg;
	uint8      duerrproz;
	uint8      rotier;
	uint       seeplayerflags;
	uint       duerrcnt;
	uint       leer3;
	};

#endif
   
