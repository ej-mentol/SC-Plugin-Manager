//---------------------------------------------------------------------------
#ifndef pluginmanagerH
#define pluginmanagerH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.SysUtils.hpp>
#include <System.IOUtils.hpp>
#include <memory>

#include "vdf_parser.hpp"
#include "plugin.hpp"
#include "pluginlist.hpp"

// Use a different name to avoid conflicts with the one in pluginlist.hpp
enum class PluginManagerResult
{
    Success,
    ParseError,
    FileError,
    InvalidIndex
};

class TPluginManager
{
private:
    TPluginList* FPlugins;

public:
    __fastcall TPluginManager();
    __fastcall ~TPluginManager();

    // VDF-specific functionality
    PluginManagerResult __fastcall ImportFromVdf(const String& FileName);
    PluginManagerResult __fastcall ExportToVdf(const String& FileName);

    // Delegate to TPluginList for JSON operations
    PluginManagerResult __fastcall SaveToJson(const String& FileName);
    PluginManagerResult __fastcall LoadFromJson(const String& FileName);

    // Plugin management operations (delegated to TPluginList)
    PluginManagerResult __fastcall AddPlugin(const String& Name, const String& Script,
                                              TAdminLevel AdminLevel = TAdminLevel::ADMIN_NONE,
                                              const String& Concommands = "",
                                              bool Enabled = true);
    PluginManagerResult __fastcall RemovePluginByName(const String& Name);
    PluginManagerResult __fastcall RemovePluginByIndex(int Index);
    PluginManagerResult __fastcall UpdatePlugin(const String& Name, const String& NewScript,
                                                 TAdminLevel NewAdminLevel,
                                                 const String& NewConcommands,
                                                 bool NewEnabled);
    PluginManagerResult __fastcall SetPluginEnabled(const String& Name, bool Enabled);

    // Access methods
    int __fastcall GetPluginCount() const;
	TPlugin* __fastcall GetPluginByIndex(int Index) const;
	TPlugin* __fastcall GetPluginByName(const String& Name) const;
    TPlugin* __fastcall GetPluginByScriptName(const String& Name) const;
    void __fastcall ClearPlugins();

    // Event forwarding
    void __fastcall SetOnPluginAdded(TPluginEvent Value);
    void __fastcall SetOnPluginRemoved(TPluginEvent Value);
    void __fastcall SetOnPluginModified(TPluginEvent Value);

    // Properties
    __property int PluginCount = { read = GetPluginCount };
    __property TPluginEvent OnPluginAdded = { write = SetOnPluginAdded };
    __property TPluginEvent OnPluginRemoved = { write = SetOnPluginRemoved };
    __property TPluginEvent OnPluginModified = { write = SetOnPluginModified };
};

#endif
