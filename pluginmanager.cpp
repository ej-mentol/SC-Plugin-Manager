//---------------------------------------------------------------------------
#pragma hdrstop
#include "pluginmanager.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

__fastcall TPluginManager::TPluginManager()
{
    FPlugins = new TPluginList();
}

__fastcall TPluginManager::~TPluginManager()
{
    delete FPlugins;
}

PluginManagerResult __fastcall TPluginManager::ImportFromVdf(const String& FileName)
{
    try {
        if (!TFile::Exists(FileName)) return PluginManagerResult::FileError;

        std::ifstream file(AnsiString(FileName).c_str());
        if (!file.is_open()) return PluginManagerResult::FileError;

        auto root = tyti::vdf::read<tyti::vdf::multikey_object>(file);
		ClearPlugins();
		auto pluginRange = root.childs.equal_range("plugin");
        if (pluginRange.first == pluginRange.second) {
            auto pluginsRange = root.childs.equal_range("plugins");
            if (pluginsRange.first == pluginsRange.second) return PluginManagerResult::ParseError;

            auto& pluginsNode = *(pluginsRange.first->second);
            pluginRange = pluginsNode.childs.equal_range("plugin");

            if (pluginRange.first == pluginRange.second) return PluginManagerResult::ParseError;
        }
		for (auto it = pluginRange.first; it != pluginRange.second; ++it) {
            auto& plugin = *(it->second);

			std::string name, script, concommands;
            TAdminLevel adminLevel = TAdminLevel::ADMIN_NONE;

            auto nameIt = plugin.attribs.find("name");
            if (nameIt == plugin.attribs.end()) continue;
            name = nameIt->second;

            auto scriptIt = plugin.attribs.find("script");
            if (scriptIt == plugin.attribs.end()) continue;
            script = scriptIt->second;

            auto concommandsIt = plugin.attribs.find("concommands");
            if (concommandsIt != plugin.attribs.end()) {
                concommands = concommandsIt->second;
            }

            auto adminlevelIt = plugin.attribs.find("adminlevel");
            if (adminlevelIt != plugin.attribs.end()) {
                std::string adminLevelStr = adminlevelIt->second;
                if (adminLevelStr == "ADMIN_YES") {
                    adminLevel = TAdminLevel::ADMIN_YES;
                } else if (adminLevelStr == "ADMIN_NO") {
                    adminLevel = TAdminLevel::ADMIN_NO;
                } else if (adminLevelStr == "ADMIN_OWNER") {
                    adminLevel = TAdminLevel::ADMIN_OWNER;
                }
            }

			PluginManagerResult result = AddPlugin(
                name.c_str(),
                script.c_str(),
                adminLevel,
                concommands.c_str(),
                true);

            if (result != PluginManagerResult::Success) {
                return result;
			}
		}

		return PluginManagerResult::Success;
	}
	catch (const std::exception&) {
		return PluginManagerResult::ParseError;
	}
}

PluginManagerResult __fastcall TPluginManager::ExportToVdf(const String& FileName)
{
    try {
        std::unique_ptr<TStringList> vdfContent(new TStringList());

        vdfContent->Add("\"plugins\"");
        vdfContent->Add("{");

        for (int i = 0; i < FPlugins->Count; i++) {
            TPlugin* plugin = FPlugins->Items[i];

			if( plugin->Enabled == false ) continue;

            vdfContent->Add("\t\"plugin\"");
            vdfContent->Add("\t{");
            vdfContent->Add(Format("\t\t\"name\" \"%s\"", ARRAYOFCONST((plugin->Name))));
            vdfContent->Add(Format("\t\t\"script\" \"%s\"", ARRAYOFCONST((plugin->Script))));

            if (plugin->AdminLevel != TAdminLevel::ADMIN_NONE) {
                String adminLevelStr;
                switch (plugin->AdminLevel) {
                    case TAdminLevel::ADMIN_YES:
                        adminLevelStr = "ADMIN_YES";
                        break;
                    case TAdminLevel::ADMIN_NO:
                        adminLevelStr = "ADMIN_NO";
                        break;
                    case TAdminLevel::ADMIN_OWNER:
                        adminLevelStr = "ADMIN_OWNER";
                        break;
                    default:
                        // Skip this field for ADMIN_NONE
                        break;
                }

                if (!adminLevelStr.IsEmpty()) {
                    vdfContent->Add(Format("\t\t\"adminlevel\" \"%s\"", ARRAYOFCONST((adminLevelStr))));
                }
            }

            if (!plugin->Concommands.IsEmpty()) {
                vdfContent->Add(Format("\t\t\"concommandns\" \"%s\"", ARRAYOFCONST((plugin->Concommands))));
            }

            vdfContent->Add("\t}");

            if (i < FPlugins->Count - 1) {
                vdfContent->Add("");
            }
        }

        vdfContent->Add("}");
        vdfContent->SaveToFile(FileName);
        return PluginManagerResult::Success;
    }
    catch (const Exception&) {
        return PluginManagerResult::FileError;
    }
}

PluginManagerResult __fastcall TPluginManager::SaveToJson(const String& FileName)
{
    bool result = FPlugins->ExportToJSON(FileName);
    return result ? PluginManagerResult::Success : PluginManagerResult::FileError;
}

PluginManagerResult __fastcall TPluginManager::LoadFromJson(const String& FileName)
{
    bool result = FPlugins->ImportFromJSON(FileName);
    return result ? PluginManagerResult::Success : PluginManagerResult::FileError;
}

PluginManagerResult __fastcall TPluginManager::AddPlugin(const String& Name, const String& Script,
                                                         TAdminLevel AdminLevel,
                                                         const String& Concommands,
                                                         bool Enabled)
{
    int result = FPlugins->AddPlugin(Name, Script, AdminLevel, Concommands, Enabled);
    return result >= 0 ? PluginManagerResult::Success : PluginManagerResult::InvalidIndex;
}

PluginManagerResult __fastcall TPluginManager::RemovePluginByName(const String& Name)
{
    Pluginlist::TPluginOperationResult plResult = FPlugins->RemovePluginByName(Name);
	switch (plResult) {
        case Pluginlist::TPluginOperationResult::Success:
            return PluginManagerResult::Success;
        case Pluginlist::TPluginOperationResult::InvalidIndex:
            return PluginManagerResult::InvalidIndex;
        default:
            return PluginManagerResult::ParseError;
    }
}

PluginManagerResult __fastcall TPluginManager::RemovePluginByIndex(int Index)
{
    if (Index < 0 || Index >= FPlugins->Count) {
        return PluginManagerResult::InvalidIndex;
    }

    FPlugins->Delete(Index);
    return PluginManagerResult::Success;
}

PluginManagerResult __fastcall TPluginManager::UpdatePlugin(const String& Name,
                                                            const String& NewScript,
                                                            TAdminLevel NewAdminLevel,
                                                            const String& NewConcommands,
                                                            bool NewEnabled)
{
    Pluginlist::TPluginOperationResult plResult = FPlugins->UpdatePlugin(Name, NewScript, NewAdminLevel, NewConcommands, NewEnabled);
	switch (plResult) {
        case Pluginlist::TPluginOperationResult::Success:
            return PluginManagerResult::Success;
        case Pluginlist::TPluginOperationResult::InvalidIndex:
            return PluginManagerResult::InvalidIndex;
        default:
            return PluginManagerResult::ParseError;
    }
}

PluginManagerResult __fastcall TPluginManager::SetPluginEnabled(const String& Name, bool Enabled)
{
    Pluginlist::TPluginOperationResult plResult = FPlugins->SetPluginEnabled(Name, Enabled);
	switch (plResult) {
        case Pluginlist::TPluginOperationResult::Success:
            return PluginManagerResult::Success;
        case Pluginlist::TPluginOperationResult::InvalidIndex:
            return PluginManagerResult::InvalidIndex;
        default:
            return PluginManagerResult::ParseError;
    }
}

int __fastcall TPluginManager::GetPluginCount() const
{
    return FPlugins->Count;
}

TPlugin* __fastcall TPluginManager::GetPluginByIndex(int Index) const
{
    if (Index < 0 || Index >= FPlugins->Count) {
        return nullptr;
    }

    return FPlugins->Items[Index];
}

TPlugin* __fastcall TPluginManager::GetPluginByName(const String& Name) const
{
    return FPlugins->FindByName(Name);
}

TPlugin* __fastcall TPluginManager::GetPluginByScriptName( const String& Name ) const
{
	return FPlugins->FindByScript( Name );
}

void __fastcall TPluginManager::ClearPlugins()
{
    FPlugins->Clear();
}

void __fastcall TPluginManager::SetOnPluginAdded(TPluginEvent Value)
{
    FPlugins->OnPluginAdded = Value;
}

void __fastcall TPluginManager::SetOnPluginRemoved(TPluginEvent Value)
{
    FPlugins->OnPluginRemoved = Value;
}

void __fastcall TPluginManager::SetOnPluginModified(TPluginEvent Value)
{
    FPlugins->OnPluginModified = Value;
}
