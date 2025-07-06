// CodeGear C++Builder
// Copyright (c) 1995, 2025 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'pluginlist.pas' rev: 36.00 (Windows)

#ifndef pluginlistHPP
#define pluginlistHPP

#pragma delphiheader begin
#pragma option push
#if defined(__BORLANDC__) && !defined(__clang__)
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#endif
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>
#include <System.Generics.Collections.hpp>
#include <System.Classes.hpp>
#include <System.SysUtils.hpp>
#include <plugin.hpp>
#include <System.Generics.Defaults.hpp>
#include <System.Types.hpp>

//-- user supplied -----------------------------------------------------------

namespace Pluginlist
{
//-- forward type declarations -----------------------------------------------
__interface DELPHIINTERFACE TPluginCompareFunc;
typedef System::DelphiInterface<TPluginCompareFunc> _di_TPluginCompareFunc;
class DELPHICLASS TPluginList;
//-- type declarations -------------------------------------------------------
enum DECLSPEC_DENUM TPluginOperationResult : unsigned char { Success, ParseError, FileError, InvalidIndex };

typedef void __fastcall (__closure *TPluginEvent)(System::TObject* Sender, Plugin::TPlugin* APlugin);

__interface TPluginCompareFunc  : public System::IInterface 
{
	virtual int __fastcall Invoke(Plugin::TPlugin* const Left, Plugin::TPlugin* const Right) = 0 ;
};

class PASCALIMPLEMENTATION TPluginList : public System::Generics::Collections::TObjectList__1<Plugin::TPlugin*>
{
	typedef System::Generics::Collections::TObjectList__1<Plugin::TPlugin*> inherited;
	
private:
	TPluginEvent FOnPluginAdded;
	TPluginEvent FOnPluginRemoved;
	TPluginEvent FOnPluginModified;
	
public:
	Plugin::TPlugin* __fastcall FindByName(const System::UnicodeString AName);
	Plugin::TPlugin* __fastcall FindByScript(const System::UnicodeString AScript);
	int __fastcall AddPlugin(const System::UnicodeString Name, const System::UnicodeString Script, Plugin::TAdminLevel AdminLevel = (Plugin::TAdminLevel)(0x0), const System::UnicodeString Concommands = System::UnicodeString(), bool Enabled = true);
	TPluginOperationResult __fastcall RemovePluginByName(const System::UnicodeString Name);
	TPluginOperationResult __fastcall UpdatePlugin(const System::UnicodeString Name, const System::UnicodeString NewScript, Plugin::TAdminLevel NewAdminLevel, const System::UnicodeString NewConcommands, bool NewEnabled);
	TPluginOperationResult __fastcall SetPluginEnabled(const System::UnicodeString Name, bool Enabled);
	void __fastcall SortByName(bool Ascending = true);
	void __fastcall SortByAdminLevel(bool Ascending = true);
	void __fastcall CustomSort(const _di_TPluginCompareFunc CompareFunc);
	TPluginList* __fastcall FilterByEnabled(bool Enabled);
	TPluginList* __fastcall FilterByAdminLevel(Plugin::TAdminLevel AdminLevel);
	bool __fastcall ExportToJSON(const System::UnicodeString FileName);
	bool __fastcall ImportFromJSON(const System::UnicodeString FileName);
	__property TPluginEvent OnPluginAdded = {read=FOnPluginAdded, write=FOnPluginAdded};
	__property TPluginEvent OnPluginRemoved = {read=FOnPluginRemoved, write=FOnPluginRemoved};
	__property TPluginEvent OnPluginModified = {read=FOnPluginModified, write=FOnPluginModified};
public:
	/* {System_Generics_Collections}TObjectList<plugin_TPlugin>.Create */ inline __fastcall TPluginList()/* overload */ : System::Generics::Collections::TObjectList__1<Plugin::TPlugin*>() { }
	/* {System_Generics_Collections}TObjectList<plugin_TPlugin>.Create */ inline __fastcall TPluginList(bool AOwnsObjects)/* overload */ : System::Generics::Collections::TObjectList__1<Plugin::TPlugin*>(AOwnsObjects) { }
	/* {System_Generics_Collections}TObjectList<plugin_TPlugin>.Create */ inline __fastcall TPluginList(const System::DelphiInterface<System::Generics::Defaults::IComparer__1<Plugin::TPlugin*> > AComparer, bool AOwnsObjects)/* overload */ : System::Generics::Collections::TObjectList__1<Plugin::TPlugin*>(AComparer, AOwnsObjects) { }
	/* {System_Generics_Collections}TObjectList<plugin_TPlugin>.Create */ inline __fastcall TPluginList(System::Generics::Collections::TEnumerable__1<Plugin::TPlugin*>* const Collection, bool AOwnsObjects)/* overload */ : System::Generics::Collections::TObjectList__1<Plugin::TPlugin*>(Collection, AOwnsObjects) { }
	/* {System_Generics_Collections}TObjectList<plugin_TPlugin>.Destroy */ inline __fastcall virtual ~TPluginList() { }
	
public:
	/* {System_Generics_Collections}TList<plugin_TPlugin>.Create */ inline __fastcall TPluginList(const System::DelphiInterface<System::Generics::Defaults::IComparer__1<Plugin::TPlugin*> > AComparer)/* overload */ : System::Generics::Collections::TObjectList__1<Plugin::TPlugin*>(AComparer) { }
	/* {System_Generics_Collections}TList<plugin_TPlugin>.Create */ inline __fastcall TPluginList(System::Generics::Collections::TEnumerable__1<Plugin::TPlugin*>* const Collection)/* overload */ : System::Generics::Collections::TObjectList__1<Plugin::TPlugin*>(Collection) { }
	/* {System_Generics_Collections}TList<plugin_TPlugin>.Create */ inline __fastcall TPluginList(const System::DelphiInterface<System::IEnumerable__1<Plugin::TPlugin*> > Collection)/* overload */ : System::Generics::Collections::TObjectList__1<Plugin::TPlugin*>(Collection) { }
	/* {System_Generics_Collections}TList<plugin_TPlugin>.Create */ inline __fastcall TPluginList(Plugin::TPlugin* const *Values, const System::NativeInt Values_High)/* overload */ : System::Generics::Collections::TObjectList__1<Plugin::TPlugin*>(Values, Values_High) { }
	
};


//-- var, const, procedure ---------------------------------------------------
}	/* namespace Pluginlist */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_PLUGINLIST)
using namespace Pluginlist;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// pluginlistHPP
