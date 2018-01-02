#ifndef _flddialog_h_
#define _flddialog_h_

#include <owl/treewind.h>
#include <owl/dialog.h>
#include <owl/edit.h>
#include <owl/checkbox.h>
#include <owl/radiobut.h>
#include "inselview.h"

class FldDialog : public TDialog
{
public:
   FldDialog( TWindow* parent, TResId resId );
   virtual void SetupWindow();

   void CmAccept();
   void CmAcceptEnable(TCommandEnabler&);
   void CmUndo();
   void CmUndoEnable(TCommandEnabler&);
   void CmTurn();
   void CmTurnEnable(TCommandEnabler&);
   void CmKeepOri();
   void CmFieldSelect(TTwNotify&);
   void CmDetails();
   void CeDetails(TCommandEnabler&);
   void CmLadenSetzen(WPARAM);
   void CmMarkId();
   void CeMarkId(TCommandEnabler&);
   void EvSetFocus(HWND);
   void EvKillFocus(HWND);
   void EvActivate(uint, bool, HWND);

   void updateWindow();
   void clearAll();
   unsigned int id() const;
   unsigned int ori() const;
   bool keepOri() const;
   void setId( unsigned int id );
   void setOri( unsigned int ori );
   void setView( InselView* view );
   bool loading() const;	// true = loading, false = setting

private:

   TEdit* editId;
   TEdit* editOri;
   TEdit* editDescr;
   TEdit* editExt;
   TStatic* bitmapFrame;
   TCheckBox* cbKeepOri;
   TTreeWindow* treeWindow;
   TBitmap* fieldBg;
   InselView* currentView;

   TRadioButton* feldLaden;
   TRadioButton* feldSetzen;

   int _id;
   int _ori;
   int _id_sav;
   int _ori_sav;

   DECLARE_RESPONSE_TABLE( FldDialog );
};

#endif
