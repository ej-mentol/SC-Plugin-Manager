// CodeGear C++Builder
// Copyright (c) 1995, 2025 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'plugin.pas' rev: 36.00 (Windows)

#ifndef pluginHPP
#define pluginHPP

#pragma delphiheader begin
#pragma option push
#if defined(__BORLANDC__) && !defined(__clang__)
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#endif
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>

//-- user supplied -----------------------------------------------------------

namespace Plugin
{
//-- forward type declarations -----------------------------------------------
class DELPHICLASS TPlugin;
//-- type declarations -------------------------------------------------------
enum DECLSPEC_DENUM TAdminLevel : unsigned char { ADMIN_NONE, ADMIN_YES, ADMIN_NO, ADMIN_OWNER };

class PASCALIMPLEMENTATION TPlugin : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	System::UnicodeString FName;
	System::UnicodeString FScript;
	TAdminLevel FAdminLevel;
	System::UnicodeString FConcommands;
	bool FEnabled;
	void __fastcall SetName(const System::UnicodeString Value);
	void __fastcall SetScript(const System::UnicodeString Value);
	void __fastcall SetAdminLevel(TAdminLevel Value);
	void __fastcall SetConcommands(const System::UnicodeString Value);
	
public:
	__fastcall virtual TPlugin()/* overload */;
	__fastcall virtual TPlugin(const System::UnicodeString Name, const System::UnicodeString Script, TAdminLevel AdminLevel, const System::UnicodeString Concommands, bool Enabled)/* overload */;
	HIDESBASE bool __fastcall Equals(TPlugin* const Other);
	TPlugin* __fastcall Clone();
	__property System::UnicodeString Name = {read=FName, write=SetName};
	__property System::UnicodeString Script = {read=FScript, write=SetScript};
	__property TAdminLevel AdminLevel = {read=FAdminLevel, write=SetAdminLevel, nodefault};
	__property System::UnicodeString Concommands = {read=FConcommands, write=SetConcommands};
	__property bool Enabled = {read=FEnabled, write=FEnabled, default=1};
public:
	/* TObject.Destroy */ inline __fastcall virtual ~TPlugin() { }
	
};


//-- var, const, procedure ---------------------------------------------------
}	/* namespace Plugin */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_PLUGIN)
using namespace Plugin;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// pluginHPP
