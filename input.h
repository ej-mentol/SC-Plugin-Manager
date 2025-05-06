//---------------------------------------------------------------------------

#ifndef inputH
#define inputH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Edit.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>

#include <FMX.Layouts.hpp>
//---------------------------------------------------------------------------
class TInputForm : public TForm
{
__published:	// IDE-managed Components
	TEdit *NameEdit;
	TButton *OkButton;
	TButton *CancelButton;
	TLayout *TopLayout;
	void __fastcall OkButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall NameEditKeyDown(TObject *Sender, WORD &Key, System::WideChar &KeyChar,
          TShiftState Shift);
	void __fastcall NameEditChangeTracking(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TInputForm(TComponent* Owner);
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif
