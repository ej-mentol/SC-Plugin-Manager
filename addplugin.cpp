#include <fmx.h>
#pragma hdrstop

#include "addplugin.h"
#include "message.h"

#pragma package(smart_init)
#pragma resource "*.fmx"

__fastcall TAddPluginForm::TAddPluginForm(TComponent* Owner) : TForm(Owner)
{
    FPluginsList = std::make_unique<TStringList>();
    FFilteredPlugins = std::make_unique<TStringList>();

    FilterPopup = new TPopup(this);
    FilterPopup->Parent = this;
    FilterPopup->Visible = false;
    FilterPopup->Placement = TPlacement::Bottom;
    FilterPopup->PlacementTarget = ScriptEdit;
    FilterPopup->StyleLookup = "popupstyle";

    FilterListBox = new TListBox(FilterPopup);
    FilterListBox->Parent = FilterPopup;
    FilterListBox->Align = TAlignLayout::Client;
    FilterListBox->ShowScrollBars = false;
    FilterListBox->OnItemClick = FilterListBoxItemClick;

    ScriptEdit->OnChangeTracking = ScriptEditChangeTracking;
    ScriptEdit->OnKeyDown = ScriptEditKeyDown;

    OnCreate = FormCreate;
    OnShow = FormShow;
}

void __fastcall TAddPluginForm::FormCreate(TObject* Sender)
{
    //
}

bool TAddPluginForm::ValidateForm() const
{
    if (NameEdit->Text.Trim().IsEmpty())
        return false;

    if (ScriptEdit->Text.Trim().IsEmpty())
        return false;
    String scriptPath = ScriptEdit->Text.Trim();
    for (int i = 0; i < FPluginsList->Count; i++) {
        String fullPath = FPluginsList->Strings[i];
        String relPath = GetRelativePath(fullPath);

        if (relPath.CompareIC(scriptPath) == 0)
            return true;
    }

    return false;
}

void __fastcall TAddPluginForm::SetFileList(
    const TStringDynArray &plugins, const String &basePath)
{
    FBasePath = basePath.Trim();
    while (!FBasePath.IsEmpty() && (FBasePath[FBasePath.Length()] == L'\\' ||
                                       FBasePath[FBasePath.Length()] == L'/'))
        FBasePath.SetLength(FBasePath.Length() - 1);

    FPluginsList->Clear();

    for (const auto &fullPath : plugins) {
        String path = fullPath.Trim();
        FPluginsList->Add(path);
    }

    UpdateFilteredList("");
}

String TAddPluginForm::GetRelativePath(const String &fullPath) const
{
    if (fullPath.IsEmpty())
        return String();

    String base = ExcludeTrailingPathDelimiter(FBasePath.Trim());
    String path = ExcludeTrailingPathDelimiter(fullPath.Trim());

    if (path.Pos(base) == 1 && path.Length() > base.Length()) {
        String rel =
            path.SubString(base.Length() + 1, path.Length() - base.Length());
        if (!rel.IsEmpty() && (rel[1] == L'\\' || rel[1] == L'/'))
            rel = rel.SubString(2, rel.Length() - 1);

        rel = StringReplace(
            rel, L"\\", L"/", TReplaceFlags() << TReplaceFlag::rfReplaceAll);

        String extension = System::Ioutils::TPath::GetExtension(rel);
        if (extension.CompareIC(".as") == 0)
			rel = rel.SubString(1, rel.Length() - extension.Length());

		return rel;
	}

	String filename = System::Ioutils::TPath::GetFileName(path);
	String extension = System::Ioutils::TPath::GetExtension(filename);
	if (extension.CompareIC(".as") == 0)
		return filename.SubString(1, filename.Length() - extension.Length());

	return filename;
}

// Old

//String TAddPluginForm::GetRelativePath(const String& fullPath) const
//{
//    if (fullPath.IsEmpty())
//        return String();
//
//	String base = FBasePath.Trim();
//	while (!base.IsEmpty() && (base[base.Length()] == L'\\' || base[base.Length()] == L'/'))
//		base.SetLength(base.Length() - 1);
//
//	String path = fullPath.Trim();
//	if (path.Pos(base) == 1)
//	{
//		String rel = path.SubString(base.Length() + 1, path.Length() - base.Length());
//		if (!rel.IsEmpty() && (rel[1] == L'\\' || rel[1] == L'/'))
//			rel = rel.SubString(2, rel.Length() - 1);
//
//		rel = StringReplace(rel, L"\\", L"/", TReplaceFlags() << TReplaceFlag::rfReplaceAll);
//
//		String extension = System::Ioutils::TPath::GetExtension(rel);
//		if (extension.CompareIC(".as") == 0)
//			rel = rel.SubString(1, rel.Length() - extension.Length());
//
//		return rel;
//	}
//
//	String filename = System::Ioutils::TPath::GetFileName(path);
//	String extension = System::Ioutils::TPath::GetExtension(filename);
//    if (extension.CompareIC(".as") == 0)
//        return filename.SubString(1, filename.Length() - extension.Length());
//
//    return filename;
//}

void TAddPluginForm::UpdateFilteredList(const String &filterText)
{
    FilterListBox->Items->Clear();
    FFilteredPlugins->Clear();

    if (filterText.IsEmpty()) {
        const int MaxItemsWithoutFilter = 20;

        for (int i = 0;
             i < std::min(FPluginsList->Count, MaxItemsWithoutFilter); i++)
        {
            String fullPath = FPluginsList->Strings[i];
            String relPath = GetRelativePath(fullPath);

            FFilteredPlugins->Add(fullPath);

            TListBoxItem* item = new TListBoxItem(FilterListBox);
            item->Text = relPath;
            item->StyledSettings = item->StyledSettings >> TStyledSetting::Size;
            item->TextSettings->Font->Size = 14;
            item->Height = 30;

            FilterListBox->AddObject(item);
        }

        if (FFilteredPlugins->Count > 0)
            ShowFilterPopup();
        else
            HideFilterPopup();

        return;
    }

    String filterLower = filterText.LowerCase();

    for (int i = 0; i < FPluginsList->Count; i++) {
        String fullPath = FPluginsList->Strings[i];
        String relPath = GetRelativePath(fullPath);

        if (relPath.LowerCase().Pos(filterLower) > 0) {
            FFilteredPlugins->Add(fullPath);

            TListBoxItem* item = new TListBoxItem(FilterListBox);
            item->Text = relPath;
            item->StyledSettings = item->StyledSettings >> TStyledSetting::Size;
            item->TextSettings->Font->Size = 14;
            item->Height = 30;

            FilterListBox->AddObject(item);
        }
    }

    if (FFilteredPlugins->Count > 0)
        ShowFilterPopup();
    else
        HideFilterPopup();
}

void TAddPluginForm::ShowFilterPopup()
{
    const int MaxVisibleItems = 8;
    int itemCount = std::min(FFilteredPlugins->Count, MaxVisibleItems);
    int itemHeight = 30;
    int popupHeight = itemCount * itemHeight + 4;

    FilterPopup->Width = ScriptEdit->Width;
    FilterPopup->Height = popupHeight;

    if (!FilterPopup->IsOpen)
        FilterPopup->IsOpen = true;

    if (FilterListBox->Count > 0 && FilterListBox->ItemIndex < 0)
        FilterListBox->ItemIndex = 0;
}

void TAddPluginForm::HideFilterPopup()
{
    if (FilterPopup->IsOpen)
        FilterPopup->IsOpen = false;
}

void __fastcall TAddPluginForm::FilterListBoxItemClick(
    TCustomListBox* const Sender, TListBoxItem* const Item)
{
    int index = Item->Index;
    if (index >= 0 && index < FFilteredPlugins->Count) {
        String fullPath = FFilteredPlugins->Strings[index];
        String relPath = GetRelativePath(fullPath);

        ScriptEdit->Text = relPath;
        HideFilterPopup();

        if (NameEdit->Text.Trim().IsEmpty()) {
            String suggestedName =
                System::Ioutils::TPath::GetFileNameWithoutExtension(fullPath);
            NameEdit->Text = suggestedName;
        }

        ScriptEdit->SetFocus();
    }
}

void __fastcall TAddPluginForm::ScriptEditChangeTracking(TObject* Sender)
{
    UpdateFilteredList(ScriptEdit->Text.Trim());
    OkButton->Enabled =
        (!ScriptEdit->Text.IsEmpty() && !NameEdit->Text.IsEmpty());
}

void __fastcall TAddPluginForm::ScriptEditKeyDown(
    TObject* Sender, WORD &Key, System::WideChar &KeyChar, TShiftState Shift)
{
    if (!FilterPopup->IsOpen)
        return;

    switch (Key) {
        case vkDown:
            if (FilterListBox->ItemIndex < FilterListBox->Count - 1)
                FilterListBox->ItemIndex++;
            Key = 0;
            break;

        case vkUp:
            if (FilterListBox->ItemIndex > 0)
                FilterListBox->ItemIndex--;
            Key = 0;
            break;

        case vkReturn:
        case vkTab:
            if (FilterListBox->ItemIndex >= 0) {
                TListBoxItem* selectedItem = dynamic_cast<TListBoxItem*>(
                    FilterListBox->ListItems[FilterListBox->ItemIndex]);
                if (selectedItem)
                    FilterListBoxItemClick(FilterListBox, selectedItem);
            }
            Key = 0;
            break;

        case vkEscape:
            HideFilterPopup();
            Key = 0;
            break;
    }
}

String TAddPluginForm::GetSelectedPlugin() const
{
    return ScriptEdit->Text.Trim();
}

void __fastcall TAddPluginForm::CancelButtonClick(TObject* Sender)
{
    ModalResult = mrCancel;
}

String TAddPluginForm::GetPluginPath() const
{
    return ScriptEdit->Text.Trim();
}

String TAddPluginForm::GetPluginName() const
{
    return NameEdit->Text.Trim();
}

void __fastcall TAddPluginForm::OkButtonClick(TObject* Sender)
{
    if (NameEdit->Text.Trim().IsEmpty()) {
        ErrorMessage(L"Name field cannot be empty");
        NameEdit->SetFocus();
        return;
    }

    if (ScriptEdit->Text.Trim().IsEmpty()) {
        ErrorMessage(L"Script field cannot be empty");
        ScriptEdit->SetFocus();
        return;
    }

    bool scriptExists = false;
    String scriptPath = ScriptEdit->Text.Trim();

    for (int i = 0; i < FPluginsList->Count; i++) {
        String fullPath = FPluginsList->Strings[i];
        String relPath = GetRelativePath(fullPath);

        if (relPath.CompareIC(scriptPath) == 0) {
            scriptExists = true;
            break;
        }
    }

    ModalResult = mrOk;
}

void __fastcall TAddPluginForm::NameEditExit(TObject* Sender)
{
    OkButton->Enabled =
        (!ScriptEdit->Text.IsEmpty() && !NameEdit->Text.IsEmpty());
}
//---------------------------------------------------------------------------

void __fastcall TAddPluginForm::ScriptEditExit(TObject* Sender)
{
    OkButton->Enabled =
        (!ScriptEdit->Text.IsEmpty() && !NameEdit->Text.IsEmpty());
}
//---------------------------------------------------------------------------

void __fastcall TAddPluginForm::NameEditChangeTracking(TObject* Sender)
{
    OkButton->Enabled =
        (!ScriptEdit->Text.IsEmpty() && !NameEdit->Text.IsEmpty());
}
//---------------------------------------------------------------------------

void __fastcall TAddPluginForm::NameEditKeyDown(
    TObject* Sender, WORD &Key, System::WideChar &KeyChar, TShiftState Shift)
{
    if (Key == VK_RETURN)
        ScriptEdit->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TAddPluginForm::FormShow(TObject* Sender)
{
    NameEdit->SetFocus();
}
//---------------------------------------------------------------------------

