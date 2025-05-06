unit pluginlist;

interface

uses
  System.Generics.Collections, System.Classes, System.SysUtils,
  plugin;

type
  TPluginOperationResult = (Success, ParseError, FileError, InvalidIndex);

  TPluginEvent = procedure(Sender: TObject; APlugin: TPlugin) of object;
  TPluginCompareFunc = reference to function(const Left, Right: TPlugin): Integer;

  TPluginList = class(TObjectList<TPlugin>)
  private
    FOnPluginAdded: TPluginEvent;
    FOnPluginRemoved: TPluginEvent;
    FOnPluginModified: TPluginEvent;
  public
    function FindByName(const AName: string): TPlugin;
    function FindByScript(const AScript: string): TPlugin;

    function AddPlugin(const Name, Script: string; AdminLevel: TAdminLevel = ADMIN_NONE;
                      const Concommands: string = ''; Enabled: Boolean = True): Integer;
    function RemovePluginByName(const Name: string): TPluginOperationResult;
    function UpdatePlugin(const Name, NewScript: string; NewAdminLevel: TAdminLevel;
                         const NewConcommands: string; NewEnabled: Boolean): TPluginOperationResult;
    function SetPluginEnabled(const Name: string; Enabled: Boolean): TPluginOperationResult;

    procedure SortByName(Ascending: Boolean = True);
    procedure SortByAdminLevel(Ascending: Boolean = True);
    procedure CustomSort(const CompareFunc: TPluginCompareFunc);

    function FilterByEnabled(Enabled: Boolean): TPluginList;
    function FilterByAdminLevel(AdminLevel: TAdminLevel): TPluginList;

    function ExportToJSON(const FileName: string): Boolean;
    function ImportFromJSON(const FileName: string): Boolean;

    property OnPluginAdded: TPluginEvent read FOnPluginAdded write FOnPluginAdded;
    property OnPluginRemoved: TPluginEvent read FOnPluginRemoved write FOnPluginRemoved;
    property OnPluginModified: TPluginEvent read FOnPluginModified write FOnPluginModified;
  end;

implementation

uses
  System.JSON, System.IOUtils;

function TPluginList.FindByName(const AName: string): TPlugin;
var
  Plugin: TPlugin;
begin
  Result := nil;
  for Plugin in Self do
    if SameText(Plugin.Name, AName) then
      Exit(Plugin);
end;


function TPluginList.FindByScript(const AScript: string): TPlugin;
var
  Plugin: TPlugin;
begin
  Result := nil;
  for Plugin in Self do
    if SameText(Plugin.Script, AScript) then
      Exit(Plugin);
end;

function TPluginList.AddPlugin(const Name, Script: string; AdminLevel: TAdminLevel;
                              const Concommands: string; Enabled: Boolean): Integer;
var
  Plugin: TPlugin;
begin
  if FindByName(Name) <> nil then
    Exit(-1);

  Plugin := TPlugin.Create(Name, Script, AdminLevel, Concommands, Enabled);
  Result := Add(Plugin);

  if Assigned(FOnPluginAdded) then
    FOnPluginAdded(Self, Plugin);
end;

function TPluginList.RemovePluginByName(const Name: string): TPluginOperationResult;
var
  Plugin: TPlugin;
  Index: Integer;
begin
  Plugin := FindByName(Name);
  if Plugin = nil then
    Exit(TPluginOperationResult.InvalidIndex);

  Index := IndexOf(Plugin);
  if Index >= 0 then
  begin
    if Assigned(FOnPluginRemoved) then
      FOnPluginRemoved(Self, Plugin);

    Delete(Index);
    Result := TPluginOperationResult.Success;
  end
  else
    Result := TPluginOperationResult.InvalidIndex;
end;

function TPluginList.UpdatePlugin(const Name, NewScript: string; NewAdminLevel: TAdminLevel;
                                 const NewConcommands: string; NewEnabled: Boolean): TPluginOperationResult;
var
  Plugin: TPlugin;
begin
  Plugin := FindByName(Name);
  if Plugin = nil then
    Exit(TPluginOperationResult.InvalidIndex);

  Plugin.Script := NewScript;
  Plugin.AdminLevel := NewAdminLevel;
  Plugin.Concommands := NewConcommands;
  Plugin.Enabled := NewEnabled;

  if Assigned(FOnPluginModified) then
    FOnPluginModified(Self, Plugin);

  Result := TPluginOperationResult.Success;
end;

function TPluginList.SetPluginEnabled(const Name: string; Enabled: Boolean): TPluginOperationResult;
var
  Plugin: TPlugin;
begin
  Plugin := FindByName(Name);
  if Plugin = nil then
    Exit(TPluginOperationResult.InvalidIndex);

  Plugin.Enabled := Enabled;

  if Assigned(FOnPluginModified) then
    FOnPluginModified(Self, Plugin);

  Result := TPluginOperationResult.Success;
end;

procedure TPluginList.SortByName(Ascending: Boolean = True);
var
  i, j: Integer;
  Temp: TPlugin;
begin
  for i := 0 to Count - 2 do
    for j := 0 to Count - i - 2 do
    begin
      if Ascending then
      begin
        if CompareText(Items[j].Name, Items[j + 1].Name) > 0 then
        begin
          Temp := Items[j];
          Items[j] := Items[j + 1];
          Items[j + 1] := Temp;
        end;
      end
      else
      begin
        if CompareText(Items[j].Name, Items[j + 1].Name) < 0 then
        begin
          Temp := Items[j];
          Items[j] := Items[j + 1];
          Items[j + 1] := Temp;
        end;
      end;
    end;
end;

procedure TPluginList.SortByAdminLevel(Ascending: Boolean = True);
var
  i, j: Integer;
  Temp: TPlugin;
begin
  for i := 0 to Count - 2 do
    for j := 0 to Count - i - 2 do
    begin
      if Ascending then
      begin
        if Ord(Items[j].AdminLevel) > Ord(Items[j + 1].AdminLevel) then
        begin
          Temp := Items[j];
          Items[j] := Items[j + 1];
          Items[j + 1] := Temp;
        end;
      end
      else
      begin
        if Ord(Items[j].AdminLevel) < Ord(Items[j + 1].AdminLevel) then
        begin
          Temp := Items[j];
          Items[j] := Items[j + 1];
          Items[j + 1] := Temp;
        end;
      end;
    end;
end;

procedure TPluginList.CustomSort(const CompareFunc: TPluginCompareFunc);
var
  i, j: Integer;
  Temp: TPlugin;
begin
  for i := 0 to Count - 2 do
    for j := 0 to Count - i - 2 do
    begin
      if CompareFunc(Items[j], Items[j + 1]) > 0 then
      begin
        Temp := Items[j];
        Items[j] := Items[j + 1];
        Items[j + 1] := Temp;
      end;
    end;
end;

function TPluginList.FilterByEnabled(Enabled: Boolean): TPluginList;
var
  Plugin: TPlugin;
begin
  Result := TPluginList.Create(True);
  for Plugin in Self do
    if Plugin.Enabled = Enabled then
      Result.Add(Plugin.Clone);
end;

function TPluginList.FilterByAdminLevel(AdminLevel: TAdminLevel): TPluginList;
var
  Plugin: TPlugin;
begin
  Result := TPluginList.Create(True);
  for Plugin in Self do
    if Plugin.AdminLevel = AdminLevel then
      Result.Add(Plugin.Clone);
end;

function TPluginList.ExportToJSON(const FileName: string): Boolean;
var
  JsonArray: TJSONArray;
  RootObj: TJSONObject;
  JsonObj: TJSONObject;
  Plugin: TPlugin;
  JsonText: string;
begin
  Result := False;
  try
    JsonArray := TJSONArray.Create;
    try
      for Plugin in Self do
      begin
        JsonObj := TJSONObject.Create;
        JsonObj.AddPair('Name', Plugin.Name);
        JsonObj.AddPair('Script', Plugin.Script);
        JsonObj.AddPair('AdminLevel', TJSONNumber.Create(Ord(Plugin.AdminLevel)));
        JsonObj.AddPair('Concommands', Plugin.Concommands);
        JsonObj.AddPair('Enabled', TJSONBool.Create(Plugin.Enabled));
        JsonArray.AddElement(JsonObj);
      end;

      RootObj := TJSONObject.Create;
      try
        RootObj.AddPair('plugins', JsonArray);

        JsonText := RootObj.ToString;
        TFile.WriteAllText(FileName, JsonText);
      finally
        RootObj.Free;
      end;

      Result := True;
    except
      JsonArray.Free;
      raise;
    end;
  except
    Result := False;
  end;
end;

function TPluginList.ImportFromJSON(const FileName: string): Boolean;
var
  JsonValue, PluginsValue: TJSONValue;
  JsonArray: TJSONArray;
  JsonObj: TJSONObject;
  PluginValue: TJSONValue;
  PluginObj: TJSONObject;
  JsonStr: string;
  Name, Script, Concommands: string;
  AdminLevel: Integer;
  Enabled: Boolean;
begin
  Result := False;

  if not TFile.Exists(FileName) then
    Exit(False);

  try
    Clear;

    JsonStr := TFile.ReadAllText(FileName);
    JsonValue := TJSONObject.ParseJSONValue(JsonStr);

    if not Assigned(JsonValue) then
      Exit(False);

    try
      if not (JsonValue is TJSONObject) then
        Exit(False);

      JsonObj := JsonValue as TJSONObject;
      PluginsValue := JsonObj.GetValue('plugins');
      if not Assigned(PluginsValue) or not (PluginsValue is TJSONArray) then
        Exit(False);

      JsonArray := PluginsValue as TJSONArray;

      for PluginValue in JsonArray do
      begin
        if not (PluginValue is TJSONObject) then
          Continue;

        PluginObj := TJSONObject(PluginValue);

        Name := PluginObj.GetValue<string>('Name');
        Script := PluginObj.GetValue<string>('Script');
        AdminLevel := PluginObj.GetValue<Integer>('AdminLevel', 0);
        Concommands := PluginObj.GetValue<string>('Concommands', '');
        Enabled := PluginObj.GetValue<Boolean>('Enabled', True);

        Add(TPlugin.Create(
          Name,
          Script,
          TAdminLevel(AdminLevel),
          Concommands,
          Enabled
        ));
      end;

      Result := True;
    finally
      JsonValue.Free;
    end;
  except
    Result := False;
  end;
end;
end.
