#ifndef __RandomDialog_h__
#define __RandomDialog_h__

#include <owl/dialog.h>
#include <owl/combobox.h>
#include "inselview.h"

class RandomDialog : public TDialog
{
public:
	RandomDialog( InselView* parent, TResId resId );
   virtual void SetupWindow();
   virtual bool CanClose();

   int getProbability() const;
   const string& getCategory() const;

private:
	TComboBox* percentSel;
   TComboBox* fldSel;

   int _probability;
   string _category;
};
#endif