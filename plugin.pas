unit plugin;

interface

type
  TAdminLevel = (ADMIN_NONE, ADMIN_YES, ADMIN_NO, ADMIN_OWNER);

  TPlugin = class
  private
    FName: string;
    FScript: string;
    FAdminLevel: TAdminLevel;
    FConcommands: string;
    FEnabled: Boolean;

    procedure SetName(const Value: string);
    procedure SetScript(const Value: string);
    procedure SetAdminLevel(Value: TAdminLevel);
    procedure SetConcommands(const Value: string);
  public
    constructor Create; overload; virtual;
    constructor Create(const Name, Script: string; AdminLevel: TAdminLevel = ADMIN_NONE;
      const Concommands: string = ''; Enabled: Boolean = True); overload; virtual;

    function Equals(const Other: TPlugin): Boolean; reintroduce;
    function Clone: TPlugin;

    property Name: string read FName write SetName;
    property Script: string read FScript write SetScript;
    property AdminLevel: TAdminLevel read FAdminLevel write SetAdminLevel;
    property Concommands: string read FConcommands write SetConcommands;
    property Enabled: Boolean read FEnabled write FEnabled default True;
  end;

implementation

uses
  System.SysUtils;

{ TPlugin }

constructor TPlugin.Create;
begin
  inherited Create;
  FName := '';
  FScript := '';
  FAdminLevel := ADMIN_NONE;
  FConcommands := '';
  FEnabled := True;
end;

constructor TPlugin.Create(const Name, Script: string; AdminLevel: TAdminLevel;
  const Concommands: string; Enabled: Boolean);
begin
  inherited Create;
  FName := Name;
  FScript := Script;
  FAdminLevel := AdminLevel;
  FConcommands := Concommands;
  FEnabled := Enabled;
end;

procedure TPlugin.SetName(const Value: string);
begin
  if Trim(Value) <> '' then
    FName := Value;
end;

procedure TPlugin.SetScript(const Value: string);
begin
  FScript := Value;
end;

procedure TPlugin.SetAdminLevel(Value: TAdminLevel);
begin
  FAdminLevel := Value;
end;

procedure TPlugin.SetConcommands(const Value: string);
begin
  FConcommands := Value;
end;

function TPlugin.Equals(const Other: TPlugin): Boolean;
begin
  Result := (Other <> nil) and
            (FName = Other.FName) and
            (FScript = Other.FScript) and
            (FAdminLevel = Other.FAdminLevel) and
            (FConcommands = Other.FConcommands) and
            (FEnabled = Other.FEnabled);
end;

function TPlugin.Clone: TPlugin;
begin
  Result := TPlugin.Create(FName, FScript, FAdminLevel, FConcommands, FEnabled);
end;

end.
