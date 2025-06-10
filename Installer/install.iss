#include "../vendor/CodeDependencies.iss"

[Setup]
AppName=Chaos Mod
AppVersion=2.2-beta
DefaultDirName=""
DisableDirPage=yes
CreateAppDir=no
Uninstallable=no
CreateUninstallRegKey=no
OutputDir="dst"
OutputBaseFilename="ChaosModInstaller"

[Files]
Source: "ChaosMod.asi"; DestDir: "{app}"; Flags: ignoreversion
Source: "chaosmod/*"; DestDir: "{app}/chaosmod"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "README.txt"; DestDir: "{app}/chaosmod"; Flags: ignoreversion isreadme

[Code]
function HasGtaInFolder(Folder: String): Boolean;
begin
  Result := FileExists(Folder + '\GTA5.exe') or FileExists(Folder + '\GTA5_Enhanced.exe');
end;

function GetPaths: TStrings;
var
  Path: String;
  List: TStringList;
  KeyNames: TArrayOfString;
  NodeNames: TArrayOfString;

  i: integer;
  j: integer;
begin
  NodeNames := ['InstallFolder', 'InstallFolderSteam', 'InstallFolderEpic'];
  KeyNames  := [
    'Software\WOW6432Node\Rockstar Games\Grand Theft Auto V',
    'Software\WOW6432Node\Rockstar Games\GTA V Enhanced',
    'Software\WOW6432Node\Rockstar Games\GTAV Enhanced'
  ];

  List := TStringList.Create;
    
  for i := 0 to Length(KeyNames) - 1 do
    for j := 0 to Length(NodeNames) - 1 do
      if RegQueryStringValue(HKLM, KeyNames[i], NodeNames[j], Path) and HasGtaInFolder(Path) then
        List.Append(Path);

  Result := List;
end;

var GHasCustomPath: boolean;

function ShowNoGtaWarning: Integer;
begin
  Result := MsgBox('Warning: GTA5 executable does not seem to be in the folder! Do you still want to install chaos mod here?', mbError, MB_YESNO);
end;

// 0 - exit; 1 - reselect; 2 - confirm
function BrowseForGTAFolder(Out SelectedFolder: String): integer;
var 
  Success: boolean;
  MsgBoxResult: Integer;
begin
  Success := BrowseForFolder('Select GTA V installation folder', SelectedFolder, true);
  if not Success then
  begin
    Result := 0;
    Exit;
  end;

  if not HasGtaInFolder(SelectedFolder) then
  begin
    MsgBoxResult := ShowNoGtaWarning;
    if MsgBoxResult = IDYES then
      Result := 2
    else
      Result := 1;
  end
  else
  begin
    Result := 2;
  end;
end;

procedure PathsClick(Sender: TObject);
var
  ComboBox: TComboBox;
  SelectedIndex: integer;
  SelectedFolder: string;
  BrowseStatus: integer;
begin
  ComboBox := TComboBox(Sender);
  SelectedIndex := ComboBox.ItemIndex;
  if SelectedIndex = ComboBox.Items.Count - 1 then
  begin
    BrowseStatus := BrowseForGTAFolder(SelectedFolder);
    if BrowseStatus = 2 then
    begin
      if not GHasCustomPath then
      begin
        ComboBox.Items.Insert(0, SelectedFolder);
        GHasCustomPath := true;
      end
      else
      begin
        ComboBox.Items.Strings[0] := SelectedFolder;
      end;
      ComboBox.ItemIndex := 0;
    end
    else if BrowseStatus = 1 then
    begin
      PathsClick(Sender);
    end
    else
    begin
      ComboBox.ItemIndex := -1;
    end;
  end;
end;

var GDestDir: String;

function GetDestDir(Param: String): String;
begin
  MsgBox('GetDestDir: ' + Param + ' ' + GDestDir, mbInformation, MB_OK);
  Result := GDestDir;
end;

function PathSelectorPageNextClick(Sender: TWizardPage): Boolean;
var
  PathSelectionPage: TInputQueryWizardPage;
  PathsComboBox: TNewComboBox;
  Folder: String;
begin
  PathSelectionPage := TInputQueryWizardPage(Sender);
  PathsComboBox := TNewComboBox(PathSelectionPage.FindComponent('PathsComboBox'));
  Folder := PathsComboBox.Text;
  if not DirExists(Folder) then
  begin
    MsgBox('Select a valid folder!', mbCriticalError, MB_OK);
    Result := false;
    Exit;
  end;

  WizardForm.DirEdit.Text := Folder;

  Result := true;
end;

procedure InitializeWizard;
var
  PathSelectionPage: TInputQueryWizardPage;
  PathsComboBox: TNewComboBox;
  Paths: TStrings;

  i: integer;
begin
  GHasCustomPath := false;

  PathSelectionPage :=
    CreateInputQueryPage(
      wpSelectDir, 'Preparing to install', 'Select your GTA installation path', '');
  PathSelectionPage.Add('Installation Path', False);
  PathSelectionPage.Values[0] := '';

  // Create TNewComboBox on the same parent control and
  // the same location as edit box
  PathsComboBox := TNewComboBox.Create(PathSelectionPage);
  PathsComboBox.Parent := PathSelectionPage.Edits[0].Parent;
  PathsComboBox.Left := PathSelectionPage.Edits[0].Left;
  PathsComboBox.Top := PathSelectionPage.Edits[0].Top;
  PathsComboBox.Width := PathSelectionPage.Edits[0].Width;
  PathsComboBox.Height := PathSelectionPage.Edits[0].Height;
  PathsComboBox.TabOrder := PathSelectionPage.Edits[0].TabOrder;

  PathsComboBox.Name := 'PathsComboBox';
  PathsComboBox.Text := '';

  Paths := GetPaths;
  Paths.Append('Other...');

  for i := 0 to Paths.Count - 1 do
    PathsComboBox.Items.Add(Paths.Strings[i]);

  PathsComboBox.OnClick := @PathsClick;

  // Hide the original edit box
  PathSelectionPage.Edits[0].Visible := False;
  
  // Link the label to the combo box
  // (has a practical effect only if there were
  // a keyboard accelerator on the label)
  PathSelectionPage.PromptLabels[0].FocusControl := PathsComboBox;

  PathSelectionPage.OnNextButtonClick := @PathSelectorPageNextClick;
end;

function InitializeSetup: Boolean;
begin
  Dependency_AddDotNet90;
  Dependency_AddVC2015To2022;

  Result := True;
end;