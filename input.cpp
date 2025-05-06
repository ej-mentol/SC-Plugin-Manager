//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "input.h"
#include "message.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"

//---------------------------------------------------------------------------
__fastcall TInputForm::TInputForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TInputForm::OkButtonClick(TObject *Sender)
{
	OkButton->ModalResult = mrNone;
    if (NameEdit->Text.IsEmpty())
		{
			ErrorMessage( L"This field cannot be empty" );
			return;
		}
	OkButton->ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TInputForm::CancelButtonClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TInputForm::FormShow(TObject *Sender)
{
    NameEdit->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TInputForm::NameEditKeyDown(TObject *Sender, WORD &Key, System::WideChar &KeyChar,
          TShiftState Shift)
{
    if( Key == VK_RETURN ) OkButton->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TInputForm::NameEditChangeTracking(TObject *Sender)
{
 OkButton->Enabled = ( !NameEdit->Text.IsEmpty() );
}
//---------------------------------------------------------------------------

