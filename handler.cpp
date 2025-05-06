#pragma hdrstop

#include "handler.h"
#include "settings.h"
#include "main.h"
#include "input.h"
#include "addplugin.h"
#include "filetree.h"
#include "message.h"
#import "plugin.hpp" // optional import

//---------------------------------------------------------------------------
#pragma package(smart_init)

TMainForm *FForm;

__fastcall TGUIHandler::TGUIHandler()
{
	FForm = MainForm;
	FPluginManager = new TPluginManager();

    // Misc
    FScriptsTreeView = FForm->ScriptsTreeView;
	FProfileComboBox = FForm->ProfileComboBox;

	PluginsTable = FForm->PluginsTable;
	FProfileComboBox->OnChange = this->OnProfileChanged;
	PluginsTable->OnCellClick = this->OnCellClick;
	FAniLoadingIndicator = FForm->AniLoadingIndicator;
	PluginsTable->OnEditingDone = this->OnPluginsTableEditingDone;
	ScriptsTreeView->OnDblClick = this->OnTreeViewDblClick;
	ScriptsTreeView->OnMouseDown = this->OnTreeViewMouseDown;
    FForm->OnKeyDown = this->FormKeyDown;

	// Buttons
	FAddProfileButton = FForm->AddProfileButton;
	FDeleteProfileButton = FForm->DeleteProfileButton;
	FSettingsButton = FForm->SettingsButton;
	FSavePluginsListButton = FForm->SavePluginsListButton;
	FExportToVdfButton = FForm->ExportToVdfButton;
    FCloneProfileButton = FForm->CloneProfileButton;

	FAddProfileButton->OnClick = this->OnAddProfileButton;
	FDeleteProfileButton->OnClick = this->OnDeleteProfile;
	FSettingsButton->OnClick = this->OnShowSettingsForm;
	FSavePluginsListButton->OnClick = this->OnSavePluginsListButton;
	FExportToVdfButton->OnClick = this->OnExportToVdfButton;
    FCloneProfileButton->OnClick = this->OnCloneProfileButton;
	// Popup Menu
	FPopupMenu = FForm->PopupMenu1;
    FPopupMenu->OnPopup = this->OnPopupMenu;
	FAddPluginButton = FForm->AddPluginButton;
	FDeletePluginButton = FForm->DeletePluginButton;
	// FEditPluginButton = FForm->EditPluginButton;
	FEnableDisablePluginsButton = FForm->EnableDisablePluginsButton;
	FClearPluginsButton = FForm->ClearPluginsButton;

	FAddPluginButton->OnClick = this->OnAddPluginButton;
	FDeletePluginButton->OnClick = this->OnDeletePluginButton;
	// FEditPluginButton->OnClick = this->OnEditPluginButton;
	FEnableDisablePluginsButton->OnClick = this->OnEnableDisablePluginsButton;

	LoadSettings();
	LoadProfileList();
}

void __fastcall TGUIHandler::OnPluginsTableEditingDone(TObject *Sender, const int ACol, const int ARow)
{
 if (!FPluginManager) return;

	UnicodeString value = Sysutils::Trim(PluginsTable->Cells[ACol][ARow]);
	TPlugin* plugin = FPluginManager->GetPluginByIndex(ARow);
	if (!plugin) return;

	if ((ACol == 1 || ACol == 2) && value.IsEmpty())
	{
		if (ACol == 1) PluginsTable->Cells[ACol][ARow] = plugin->Name;
		if (ACol == 2) PluginsTable->Cells[ACol][ARow] = plugin->Script;
		return;
    }

	switch (ACol)
	{
		case 1: // Name
		{
			for (int i = 0; i < FPluginManager->PluginCount; ++i)
			{
				if (i == ARow) continue;
				TPlugin* other = FPluginManager->GetPluginByIndex(i);
				if (other && Sysutils::SameText(other->Name, value))
				{
					ErrorMessage( L"A plugin named '%s' already exists.", other->Name );
					PluginsTable->Cells[ACol][ARow] = plugin->Name;
					return;
				}
			}
			plugin->Name = value;
			break;
		}
		case 2: // Script
		{
			UnicodeString checkedPath = StringReplace(value, L"\\", L"/", TReplaceFlags() << TReplaceFlag::rfReplaceAll);
			if (checkedPath != value)
			{
				PluginsTable->Cells[ACol][ARow] = checkedPath;
				value = checkedPath;
			}

			plugin->Script = value;
			break;
		}
		case 3: // Concommands
			plugin->Concommands = value;
			break;
		case 4: // AdminLevel
		{
			TPopupColumn* comboColumn = dynamic_cast<TPopupColumn*>(PluginsTable->Columns[4]);
			if (comboColumn)
			{
				int index = comboColumn->Items->IndexOf(value);
				if (index >= 0)
					plugin->AdminLevel = static_cast<TAdminLevel>(index);
			}
			break;
		}
	}
}

__fastcall TGUIHandler::~TGUIHandler()
{
    if (TSettingsManager::GetInstance() != nullptr)
    {
        try
        {
			if (!SaveSettings()) ErrorMessage( L"Error while saving settings." );
		}
        __finally
        {
			TSettingsManager::FreeInstance();
			if( FPluginManager ) delete FPluginManager;
        }
	}
}

void __fastcall TGUIHandler::ShowPluginsList( bool loadFromFile )
{
	ClearPluginsTable();
	if (!FPluginManager || !ProfileComboBox->Selected ) return;

	if (loadFromFile)
		{
			PluginManagerResult result = LoadPluginsList();
			if (result != PluginManagerResult::Success)
				{
					if (result == PluginManagerResult::FileError) ErrorMessage( L"Profile file is not found." );
					else if (result == PluginManagerResult::ParseError) ErrorMessage( L"Error parsing profile file." );
					else ErrorMessage( L"Unknown error loading profile." );
					return;
				}
		}

	PluginsTable->RowCount = FPluginManager->PluginCount;
	PluginsTable->BeginUpdate();

	for (int i = 0; i < FPluginManager->PluginCount; i++)
	{
		TPlugin* plugin = FPluginManager->GetPluginByIndex(i);
		if( !plugin ) continue;
		PluginsTable->Cells[0][i] = BoolToStr(plugin->Enabled, true);
		PluginsTable->Cells[1][i] = plugin->Name;
		PluginsTable->Cells[2][i] = Sysutils::Trim(plugin->Script);
		PluginsTable->Cells[3][i] = plugin->Concommands;

		TPopupColumn* comboColumn = dynamic_cast<TPopupColumn*>(FForm->PluginsTable->Columns[4]);
		if (comboColumn)
		{
			int adminLevelIndex = static_cast<int>(plugin->AdminLevel);
			if (adminLevelIndex >= 0 && adminLevelIndex < comboColumn->Items->Count)
				PluginsTable->Cells[4][i] = comboColumn->Items->Strings[adminLevelIndex];
			else
				PluginsTable->Cells[4][i] = L"None";
		}
		else
		{
			PluginsTable->Cells[4][i] = L"N/A";
		}
	}

	PluginsTable->EndUpdate();
}

void __fastcall TGUIHandler::ToggleDefaultProfile( bool AShowDefault )
	{
        //
	}

void __fastcall TGUIHandler::LoadProfileList()
{
	/*
		At this point, it's easier to clear the profile list than to
			insert or extract the default profile at various code locations.
	*/

	ProfileComboBox->Clear();
	if (FSettings->GenerateDefaltProfile && FSettings->ValidatePath(FSettings->PluginsFilePath, false))
		LoadDefaultProfile();

	const UnicodeString rootFolder = FSettings->ProfilesFolderPath;
	TStringDynArray profileFiles = TDirectory::GetFiles(rootFolder, L"*.json");

	for (const auto& file : profileFiles)
	{
		UnicodeString profileName = Ioutils::TPath::GetFileNameWithoutExtension(file);
        ProfileComboBox->Items->Add(profileName);
	}

	if (ProfileComboBox->Items->Count > 0) ProfileComboBox->ItemIndex = 0;
		else ClearPluginsTable();
}

void __fastcall TGUIHandler::LoadDefaultProfile()
{
	PluginManagerResult PMResult = FPluginManager->ImportFromVdf(FSettings->PluginsFilePath);
	switch (PMResult) {
		case PluginManagerResult::FileError:
			ErrorMessage( L"Can't open '%s'. Check that the path is correct.", FSettings->PluginsFilePath );
			break;
		case PluginManagerResult::ParseError:
			ErrorMessage( L"Parsing error in '%s'. Check the syntax of the file. There may be comments in the file that are not currently supported", FSettings->PluginsFilePath );
			// Application->Terminate();
			break;
		case PluginManagerResult::Success:
			{
				PluginsTable->RowCount = FPluginManager->PluginCount;
				TListBoxItem* item = new TListBoxItem(FForm->ProfileComboBox);
				item->Text = L"Default Plugins";
				item->TagString = L"default";
				FForm->ProfileComboBox->InsertObject(0, item);
				item->StyledSettings = item->StyledSettings >> TStyledSetting::Style;
				item->Font->Style = item->Font->Style << TFontStyle::fsBold;
				item->ImageIndex = 2;
				break;
			}
		case PluginManagerResult::InvalidIndex:
			ErrorMessage(L"Invalid index error." );
			break;
	}
}

void __fastcall TGUIHandler::OnShowSettingsForm( TObject *Sender )
{

	std::unique_ptr<TSettingsForm> SForm(new TSettingsForm(FForm));

	SForm->DefaultProfileEdit->Text = FSettings->PluginsFilePath;
	SForm->PluginsPathEdit->Text = FSettings->PluginsFolderPath;
	SForm->ProfilesPathEdit->Text = FSettings->ProfilesFolderPath;
	SForm->IsDefailtProfileEnabledCheckBox->IsChecked = FSettings->GenerateDefaltProfile;

	if (SForm->ShowModal() == mrOk)
	{
		if( ConfirmMessage( L"Unsaved data will be lost, do you want to continue?" ) == mrNo ) return;
		FSettings->PluginsFilePath = SForm->DefaultProfileEdit->Text;
		FSettings->PluginsFolderPath = SForm->PluginsPathEdit->Text;
		FSettings->ProfilesFolderPath = SForm->ProfilesPathEdit->Text;
		FSettings->GenerateDefaltProfile = SForm->IsDefailtProfileEnabledCheckBox->IsChecked;
		SaveSettings();
		LoadProfileList();
		GetFilesTree();
		ScriptsTreeView->Repaint();
	}
}

void __fastcall TGUIHandler::OnProfileChanged( TObject *Sender )
	{
		FSavePluginsListButton->Enabled = ( ProfileComboBox->Selected && ProfileComboBox->Selected->TagString != L"default" );
		FCloneProfileButton->Enabled = (ProfileComboBox->Selected != nullptr);
		ShowPluginsList( true );
	}

void __fastcall TGUIHandler::ClearPluginsTable() { PluginsTable->RowCount = 0; FPluginManager->ClearPlugins(); }

TProfileInfo __fastcall TGUIHandler::GetCurrentProfileInfo()
{
	TProfileInfo info;
	TListBoxItem* item = ProfileComboBox->Selected;
	if (item)
	{
		info.Name = item->Text;
		info.IsDefault = (item->TagString == L"default");

		if (info.IsDefault) info.ProfileFilePath = FSettings->PluginsFilePath;
			else info.ProfileFilePath = Ioutils::TPath::Combine(FSettings->ProfilesFolderPath, info.Name + L".json");
	}
	else
	{
		info.Name = L"";
		info.IsDefault = false;
		info.ProfileFilePath = L"";
	}
	return info;
}

PluginManagerResult __fastcall TGUIHandler::LoadPluginsList()
{
	TProfileInfo profileInfo = GetCurrentProfileInfo();
	if (profileInfo.IsDefault && !profileInfo.ProfileFilePath.IsEmpty() )
		return FPluginManager->ImportFromVdf(profileInfo.ProfileFilePath);
	else
		return FPluginManager->LoadFromJson(profileInfo.ProfileFilePath);
}

PluginManagerResult __fastcall TGUIHandler::SavePluginsList()
{
	TProfileInfo profileInfo = GetCurrentProfileInfo();

	if (profileInfo.ProfileFilePath.IsEmpty() && !profileInfo.IsDefault)
	{
		ErrorMessage( L"Profile is empty." );
		return PluginManagerResult::FileError;
	}

	if (profileInfo.IsDefault)
		return FPluginManager->ExportToVdf(profileInfo.ProfileFilePath);
	 else
		return FPluginManager->SaveToJson(profileInfo.ProfileFilePath);
}

void __fastcall TGUIHandler::OnDeleteProfile(TObject *Sender)
	{
		if (ProfileComboBox->ItemIndex < 0) return;

		TListBoxItem* item = ProfileComboBox->Selected;
		if (!item) return;
		if (item->TagString == L"default")
		{
			ErrorMessage( L"Unable to delete the default profile." );
			return;
		}

		UnicodeString profileName = item->Text;
		UnicodeString profileFilePath = Ioutils::TPath::Combine( FSettings->ProfilesFolderPath,	profileName + L".json" );

		if (!TFile::Exists(profileFilePath))
		{
			ErrorMessage( L"Profile file not found." );
			return;
		}

		if ( ConfirmMessage( L"Are you sure you want to delete profile '%s'?", profileName ) != mrYes) return;
		try
		{
			TFile::Delete(profileFilePath);
			ProfileComboBox->Items->Delete(ProfileComboBox->ItemIndex);

			if (ProfileComboBox->Count > 0) ProfileComboBox->ItemIndex = 0; // ShowPluginsList(true);
			else
			{
				ClearPluginsTable();
				FPluginManager->ClearPlugins();
			}
		}
		catch (const Exception &e)
		{
			ErrorMessage( L"Error deleting profile. '%s'", e.Message );
		}
}

bool TGUIHandler::LoadSettings()
{
    bool Result = false;
	FSettings = TSettingsManager::GetInstance();
    FSettings->LoadSettings();
    if (FSettings->IsSettingsLoaded() || FSettings->IsInitialized())
	{
		GetFilesTree();
		FForm->Height = FSettings->FormHeight;
		FForm->Width = FSettings->FormWidth;
        FForm->LeftLayout->Width = FSettings->GetSeparatorSize();
        Result = true;
    }
    return Result;
}

bool TGUIHandler::SaveSettings()
{
	FSettings->FormWidth = FForm->Width;
	FSettings->FormHeight = FForm->Height;
	FSettings->SeparatorSize = FForm->LeftLayout->Width;
	FSettings->SaveSettings();

	return true;
}

void __fastcall TGUIHandler::OnAddProfileButton(TObject *Sender)
{
    std::unique_ptr<TInputForm> IForm(new TInputForm(FForm));
    if (IForm->ShowModal() == mrOk)
	{
		UnicodeString profileName = Sysutils::Trim( IForm->NameEdit->Text );
		if (!Ioutils::TPath::HasValidFileNameChars(profileName, false))
		{
			ErrorMessage( L"Profile name contains invalid characters." );
			return;
		}

		UnicodeString profileFilePath = Ioutils::TPath::Combine(FSettings->ProfilesFolderPath, profileName + L".json");

		if (ProfileComboBox->Items->IndexOf(profileName) >= 0 || TFile::Exists(profileFilePath))
		{
			ErrorMessage( L"A profile named '%s' already exists.", profileName );
			return;
		}

		try
		{
			TFile::WriteAllText(profileFilePath, L"{\"plugins\":[]}", TEncoding::UTF8);
		}
		catch (const Exception &e)
		{
			ErrorMessage( L"Error creating profile: '%s'", e.Message );
            return;
        }

		ProfileComboBox->Items->Add(profileName);
        ProfileComboBox->ItemIndex = ProfileComboBox->Items->Count - 1;
    }
}

void __fastcall TGUIHandler::OnCellClick(TColumn * const Column, const int Row)
	{
		if (dynamic_cast<TCheckColumn*>(Column) != nullptr)
			{
				UnicodeString cellText = PluginsTable->Cells[Column->Index][Row];
				bool currentValue = (cellText == "True") || (cellText == "true") || (cellText == "1");
				bool newValue = !currentValue;
				PluginsTable->Cells[Column->Index][Row] = newValue ? "True" : "False";
				FPluginManager->GetPluginByIndex(Row)->Enabled = newValue;// ? true : false;
			}
	}


void __fastcall TGUIHandler::GetFilesTree()
{
    if (!FScriptsTreeView) return;

    const UnicodeString rootFolder = FSettings->PluginsFolderPath;
	FAniLoadingIndicator->Visible = true;
	FileTreeUtils::BuildFileTree(
		FScriptsTreeView,
		rootFolder,
		L"*.as",
		[this]() {
			FAniLoadingIndicator->Visible = false;
			if (FScriptsTreeView->Count > 0) FScriptsTreeView->ItemByIndex(0)->IsSelected = true;
		}
	);
}

void __fastcall TGUIHandler::OnAddPluginButton(TObject *Sender)
{
	std::unique_ptr<TAddPluginForm> AddPluginForm(new TAddPluginForm(FForm));
	TStringDynArray plugins;

    try
    {
		plugins = TDirectory::GetFiles(FSettings->PluginsFolderPath, L"*.as", TSearchOption::soAllDirectories);
    }
    catch (const Exception& e)
    {
		ErrorMessage( L"Failed to get plugin scripts: '%s'", e.Message );
		return;
    }

    AddPluginForm->SetFileList(plugins, FSettings->PluginsFolderPath);

	if (AddPluginForm->ShowModal() != mrOk) return; // странна€ ошибка

    const String formPluginName = Sysutils::Trim(AddPluginForm->GetPluginName());
    const String formPluginPath = Sysutils::Trim(AddPluginForm->GetPluginPath());

    if (formPluginName.IsEmpty())
    {
		AlertMessage( L"Plugin name cannot be empty." );
		return;
	}

	if (formPluginPath.IsEmpty())
	{
		AlertMessage( L"Plugin path cannot be empty." );
		return;
	}

	if (IsPluginExists(formPluginName, formPluginPath))
		return;

	if (!AddPlugin(formPluginName, formPluginPath))
	{
		ErrorMessage( L"Failed to add plugin." );
		return;
	}

	ShowPluginsList(false);
}

bool TGUIHandler::IsPluginExists(const String& name, const String& path)
{
    if (FPluginManager->GetPluginByName(name) != nullptr)
    {
		AlertMessage( L"A plugin named '%s' already exists.", name );
		return true;
	}

	if (FPluginManager->GetPluginByScriptName(path) != nullptr)
	{
		AlertMessage( L"A plugin script '%s' already exists.", path );
		return true;
	}

    return false;
}

bool TGUIHandler::AddPlugin(const String& name, const String& script )
{
	try
	{
		FPluginManager->AddPlugin( name, script );
		return true;
	}
	catch (...)
	{
		return false;
	}
}

void __fastcall TGUIHandler::OnDeletePluginButton(TObject *Sender)
{
    int selectedIndex = PluginsTable->Row;

	if (selectedIndex < 0 || selectedIndex >= FPluginManager->PluginCount)
    {
		AlertMessage( L"Please select a plugin to delete." );
		return;
	}

	TPlugin *plugin = FPluginManager->GetPluginByIndex(selectedIndex);
	String pluginName = plugin ? plugin->Name : L"<unknown plugin>";

	if ( ConfirmMessage( L"Are you sure you want to delete '%s' from the current profile?", pluginName ) == mrNo) return;

	PluginManagerResult result = FPluginManager->RemovePluginByIndex(selectedIndex);

	if (result == PluginManagerResult::Success) ShowPluginsList(false);
	else if (result == PluginManagerResult::InvalidIndex) ErrorMessage( L"Invalid index for deleting plugin '%s'.", pluginName );
	else ErrorMessage( L"Error occurred while deleting plugin '%s'.", pluginName );
}

//void __fastcall TGUIHandler::OnEditPluginButton(TObject *Sender)
//{
//}

void __fastcall TGUIHandler::OnEnableDisablePluginsButton(TObject *Sender)
{
	if (FPluginManager == nullptr || FPluginManager->PluginCount == 0)
	{
		AlertMessage( L"No plugins available." );
        return;
    }

	bool anyEnabled = false;
	for (int i = 0; i < FPluginManager->PluginCount; ++i)
	{
		TPlugin* plugin = FPluginManager->GetPluginByIndex(i);
        if (plugin && plugin->Enabled)
        {
            anyEnabled = true;
            break;
        }
	}

	bool newEnabledState = !anyEnabled;

    for (int i = 0; i < FPluginManager->PluginCount; ++i)
    {
        TPlugin* plugin = FPluginManager->GetPluginByIndex(i);
		if (plugin) plugin->Enabled = newEnabledState;
	}

	ShowPluginsList(false);
}

void __fastcall TGUIHandler::OnSavePluginsListButton(TObject *Sender)
{
	if ( SavePluginsList() != PluginManagerResult::Success )
		ErrorMessage( L"Failed to save plugins." );
	 else Beep();
}

void __fastcall TGUIHandler::OnExportToVdfButton(TObject *Sender)
{
	UnicodeString pluginsFilePath = FSettings->PluginsFilePath;
	if ( ProfileComboBox->Selected == nullptr || pluginsFilePath.IsEmpty() )
	{
		ErrorMessage( L"An error occured while exporting to the VDF file. Check the Settings." );
		return;
	}

	if ( SavePluginsList() != PluginManagerResult::Success ) ErrorMessage( L"To avoid data loss during export, the current profile is saved automatically. However, an error occurred during the saving process Ч the file might be corrupted or deleted." );
	if ( FPluginManager->ExportToVdf( FSettings->PluginsFilePath ) == PluginManagerResult::Success)
	 {
		InfoMessage( L"Profile exported to VDF successfully." );
		int currentIndex = ProfileComboBox->ItemIndex;
		ProfileComboBox->BeginUpdate();
		LoadProfileList();
		ShowPluginsList();
		if( currentIndex > 0 ) ProfileComboBox->ItemIndex = currentIndex;
		ProfileComboBox->EndUpdate();
	 }
	else ErrorMessage( L"Failed to export profile to VDF." );
}

void __fastcall TGUIHandler::OnTreeViewDblClick( TObject *Sender )
	{
		TTreeViewItem *item = dynamic_cast<TTreeViewItem*>( ScriptsTreeView->Selected );

		if ( !item ) return;
		if( item->ImageIndex == 0 ){ item->IsExpanded = !item->IsExpanded; return; }
		UnicodeString newText = Sysutils::Trim( item->Text );
		UnicodeString newScript = Sysutils::Trim( item->TagString );

		for (int row = 0; row < PluginsTable->RowCount; ++row)
		{
			UnicodeString existingText = Sysutils::Trim( PluginsTable->Cells[1][row] );
			UnicodeString existingScript = Sysutils::Trim( PluginsTable->Cells[2][row] );
			if (Sysutils::SameText(existingText, newText) )
				{
					AlertMessage( L"Plugin named '%s' already exists.", existingText );
					return;
				}
			if( Sysutils::SameText(existingScript, newScript) )
				{
					AlertMessage( L"Path named '%s' already exists.", existingScript );
					return;
				}           		}

		if (!AddPlugin(newText, item->TagString))
			{
				ErrorMessage( L"Failed to add plugin." );
				return;
			}

		ShowPluginsList(false);
	}

void __fastcall TGUIHandler::OnTreeViewMouseDown( TObject *Sender, TMouseButton Button, TShiftState Shift, float X, float Y )
	{
		TTreeViewItem* item = ScriptsTreeView->ItemByPoint(X, Y);
		if (!item) ScriptsTreeView->Selected = nullptr;
	}

void __fastcall TGUIHandler::OnPopupMenu(TObject *Sender)
	{
		TPointF screenPos;
		IFMXMouseService* MouseService = nullptr;
		if (TPlatformServices::Current->SupportsPlatformService(__uuidof(IFMXMouseService), reinterpret_cast<void**>(&MouseService))) screenPos = MouseService->GetMousePos();
		TPointF localPos = PluginsTable->ScreenToLocal(screenPos);

		int cellCol = -1, cellRow = -1, rowCount = PluginsTable->RowCount;
		bool cellFound = PluginsTable->CellByPoint(localPos.X, localPos.Y, cellCol, cellRow);

		FDeletePluginButton->Enabled = cellFound;
		FEnableDisablePluginsButton->Enabled = (rowCount > 0);
		FClearPluginsButton->Enabled = (bool)rowCount;

		if (!cellFound)
			PluginsTable->SelectCell(-1, -1);
		else
			PluginsTable->SelectCell(cellCol, cellRow);
	}

void __fastcall TGUIHandler::FormKeyDown(TObject *Sender, WORD &Key, System::WideChar &KeyChar, TShiftState Shift)
{
	if (Key == 0x53 && Shift.Contains(ssCtrl))
    {
        if (ProfileComboBox->Selected != nullptr)
        {
            if (ProfileComboBox->Selected->TagString == L"default")
            {
                if (ConfirmMessage(L"Do you want to export the default profile?\nSelect 'No' to cancel this action.") == mrNo)
					return;
            }
			OnSavePluginsListButton(FForm);
        }
	}
	else if (Key == 0x45 && Shift.Contains(ssCtrl))
    {
        OnExportToVdfButton(FForm);
	}
}

void __fastcall TGUIHandler::OnCloneProfileButton(TObject *Sender)
{
    std::unique_ptr<TInputForm> IForm(new TInputForm(FForm));

    UnicodeString defaultName = ProfileComboBox->Selected->Text + L" Copy";
    int counter = 1;
    UnicodeString profileName = defaultName;
    while (ProfileComboBox->Items->IndexOf(profileName) >= 0 ||
		   TFile::Exists(Ioutils::TPath::Combine(FSettings->ProfilesFolderPath, profileName + L".json")))
    {
		profileName = defaultName + L" (" + IntToStr(counter++) + L")";
    }

	IForm->NameEdit->Text = profileName;

    if (IForm->ShowModal() == mrOk)
    {
		profileName = Sysutils::Trim(IForm->NameEdit->Text);
        if (!Ioutils::TPath::HasValidFileNameChars(profileName, false))
        {
            ErrorMessage(L"Profile name contains invalid characters.");
            return;
        }

        UnicodeString profileFilePath = Ioutils::TPath::Combine(FSettings->ProfilesFolderPath, profileName + L".json");

        if (ProfileComboBox->Items->IndexOf(profileName) >= 0 || TFile::Exists(profileFilePath))
        {
            ErrorMessage(L"A profile named '%s' already exists.", profileName);
			return;
        }

		if ( FPluginManager->SaveToJson(profileFilePath) != PluginManagerResult::Success)
			{
				ErrorMessage(L"Failed to save the cloned profile.");
				return;
			}

		ProfileComboBox->Items->Add(profileName);
		ProfileComboBox->ItemIndex = ProfileComboBox->Items->Count - 1;
		ShowPluginsList();
	}
}
