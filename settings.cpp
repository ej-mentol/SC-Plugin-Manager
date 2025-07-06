//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "settings.h"
#include "message.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"

namespace Ioutils = System::Ioutils;

//---------------------------------------------------------------------------
__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
	DefaultProfileEdit->OnChangeTracking = this->OnChangedTracking;
	PluginsPathEdit->OnChangeTracking = this->OnChangedTracking;
	ProfilesPathEdit->OnChangeTracking = this->OnChangedTracking;
}
//---------------------------------------------------------------------------
void __fastcall TSettingsForm::OkButtonClick(TObject *Sender)
{
    OkButton->ModalResult = mrNone;

    struct TCheckItem
    {
        TEdit* Edit;
        bool IsFile;
    };

    TCheckItem checks[] = {
		{ DefaultProfileEdit, true },
		{ PluginsPathEdit, false },
		{ ProfilesPathEdit, false }
    };

    for (auto &check : checks)
    {
        bool exists = check.IsFile ? TFile::Exists(check.Edit->Text) : TDirectory::Exists(check.Edit->Text);
        if (!exists)
        {
            ErrorMessage(L"'%s' could not be found", check.Edit->Text);
            return;
        }
    }

    OkButton->ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::CancelButtonClick(TObject *Sender)
{
 Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::EllipsesEditButton2Click(TObject *Sender)
{
	UnicodeString dir = L"";
	if( Dialogs::SelectDirectory("Select 'plugins' Directory", PluginsPathEdit->Text, dir ) ) PluginsPathEdit->Text = dir;
}

//---------------------------------------------------------------------------
void __fastcall TSettingsForm::OnChangedTracking( TObject *Sender )
{
OkButton->Enabled =
	!DefaultProfileEdit->Text.IsEmpty() &&
	!PluginsPathEdit->Text.IsEmpty() &&
	!ProfilesPathEdit->Text.IsEmpty();
}
void __fastcall TSettingsForm::EllipsesEditButton1Click(TObject *Sender)
{
	if( OpenDialog->Execute() ) DefaultProfileEdit->Text = OpenDialog->FileName;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::EllipsesEditButton3Click(TObject *Sender)
{
	UnicodeString dir = L"";
	if( Dialogs::SelectDirectory("Select Profiles Directory", ProfilesPathEdit->Text, dir ) ) ProfilesPathEdit->Text = dir;
}
//---------------------------------------------------------------------------

