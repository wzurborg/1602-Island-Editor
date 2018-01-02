#ifndef __newinsel__h__
#define __newinsel__h__

#include <owl/dialog.h>
#include <owl/edit.h>
#include <owl/radiobut.h>
#include "InselDoc.h"

class NewInsel : public TDialog
{
public:
	NewInsel( TWindow* parent );

   virtual void SetupWindow();
   virtual bool CanClose();

   void CmTypeChanged();
   void CmSizeChanged();

   TSize size() const;
   InselDoc::Klimatyp klima() const;


private:
	TRadioButton* sizeLar;
   TRadioButton* sizeBig;
   TRadioButton* sizeMed;
   TRadioButton* sizeMit;
   TRadioButton* sizeLit;

   TEdit* sizeX;
   TEdit* sizeY;

   TRadioButton* klimaNord;
   TRadioButton* klimaSued;

   InselDoc::Klimatyp myKlima;
   TSize mySize;

   DECLARE_RESPONSE_TABLE( NewInsel );
};

#endif


