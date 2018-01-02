#ifndef __InselDialog__h__
#define __InselDialog__h__

#include <owl/dialog.h>
#include <owl/edit.h>
#include <owl/radiobut.h>
#include <owl/checkbox.h>
#include <owl/groupbox.h>
#include "inselview.h"

class InselDialog : public TDialog
{
public:
	InselDialog( InselView* parent, TResId resId );
   virtual void SetupWindow();
   virtual bool CanClose();

private:
	InselDoc& inselDoc;

	TGroupBox* klimaBox;   
	TRadioButton* noerdlich;
   TRadioButton* suedlich;
   TCheckBox* treeAdjust;

   TEdit* breite;
   TEdit* hoehe;
   TEdit* posx;
   TEdit* posy;
};
#endif