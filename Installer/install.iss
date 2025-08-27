#include "../vendor/CodeDependencies.iss"

#define VerFile FileOpen("..\version.txt")
#define AppVer FileRead(VerFile)
#expr FileClose(VerFile)
#undef VerFile

[Setup]
AppName=Chaos Mod
AppVersion={#AppVer}
DefaultDirName=""
DisableDirPage=yes
CreateAppDir=no
Uninstallable=no
DisableWelcomePage=no
CreateUninstallRegKey=no
OutputDir="dst"
OutputBaseFilename="ChaosModInstaller"
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

[Dirs]
Name: "{app}"; 
Name: "{app}/chaosmod"; Permissions: everyone-full

[Files]
Source: "src/ChaosMod.asi"; DestDir: "{app}"; Flags: ignoreversion; Permissions: everyone-full
Source: "src/chaosmod/*"; DestDir: "{app}/chaosmod"; Flags: ignoreversion recursesubdirs createallsubdirs; Permissions: everyone-full
Source: "src/README.txt"; DestDir: "{app}/chaosmod"; Flags: ignoreversion isreadme; Permissions: everyone-full

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

var
  DownloadPage: TDownloadWizardPage;
  GExtractionPage: TOutputProgressWizardPage;
  GScriptHookPath: String;

function OnDownloadProgress(const Url, FileName: String; const Progress, ProgressMax: Int64): Boolean;
begin
  if Progress = ProgressMax then
    Log(Format('Successfully downloaded file to {tmp}: %s', [FileName]));
  Result := True;
end;

function ShowDownloadPage: Boolean;
var
  StringArray: TArrayOfString;
  i: Integer;
  StartPos: Integer;
  EndPos: Integer;
  DownloadLink: String;
begin
  DownloadPage.Clear;
  DownloadPage.Show;
  try
    try
      DownloadTemporaryFile('http://www.dev-c.com/gtav/scripthookv/', 'page.html', '', @OnDownloadProgress);

      LoadStringsFromFile(ExpandConstant('{tmp}\page.html'), StringArray);
      for i := 0 to Length(StringArray) - 1 do
        if Pos('Download</a>', StringArray[i]) <> 0 then
        begin
          StartPos := Pos('href="', StringArray[i]) + 6;
          DownloadLink := Copy(StringArray[i], StartPos, Length(StringArray[i]));
          EndPos := Pos('"', DownloadLink) - 1;
          DownloadLink := Copy(DownloadLink, 1, EndPos);
          DownloadPage.Add(DownloadLink, 'ScriptHookV.zip', '');
          DownloadPage.Download;
          GScriptHookPath := ExpandConstant('{tmp}\ScriptHookV.zip');
          break;
        end;
      Result := True;
    except
      if DownloadPage.AbortedByUser then
        Log('Aborted by user.')
      else
        SuppressibleMsgBox(AddPeriod(GetExceptionMessage), mbCriticalError, MB_OK, IDOK);
      Result := False;
    end;
  finally
    DownloadPage.Hide;
  end;
end;

const
  NO_PROGRESS_BOX = 4;
  RESPOND_YES_TO_ALL = 16;

procedure UnZip(ZipPath, FileName, TargetPath: string); 
var
  Shell: Variant;
  ZipFile: Variant;
  Item: Variant;
  TargetFolder: Variant;
begin
  Shell := CreateOleObject('Shell.Application');

  ZipFile := Shell.NameSpace(ZipPath);
  if VarIsClear(ZipFile) then
    RaiseException(Format('Cannot open ZIP file "%s" or does not exist', [ZipPath]));

  Item := ZipFile.ParseName(FileName);
  if VarIsClear(Item) then
    RaiseException(Format('Cannot find "%s" in "%s" ZIP file', [FileName, ZipPath]));

  TargetFolder := Shell.NameSpace(TargetPath);
  if VarIsClear(TargetFolder) then
    RaiseException(Format('Target path "%s" does not exist', [TargetPath]));

  TargetFolder.CopyHere(Item, NO_PROGRESS_BOX or RESPOND_YES_TO_ALL);
end;

procedure UnZipAndShowProgress(ZipPath, FileName, TargetPath: string);
begin
  GExtractionPage.SetText('Extracting', FileName);
  UnZip(ZipPath, FileName, TargetPath);
end;

procedure ShowExtractionPage(Folder: String);
begin
  GExtractionPage.Show;
  try
    try
      UnZipAndShowProgress(GScriptHookPath, 'bin\ScriptHookV.dll', Folder);
      if FileExists(Folder + '\GTA5.exe') then
      begin
        UnZipAndShowProgress(GScriptHookPath, 'bin\dinput8.dll', Folder);
      end
      else if FileExists(Folder + '\GTA5_Enhanced.exe') then
      begin
        UnZipAndShowProgress(GScriptHookPath, 'bin\xinput1_4.dll', Folder);
      end
      else
      begin
        UnZipAndShowProgress(GScriptHookPath, 'bin\dinput8.dll', Folder);
        UnZipAndShowProgress(GScriptHookPath, 'bin\xinput1_4.dll', Folder);
      end;

      GExtractionPage.SetText('Extracted!', '');
      SuppressibleMsgBox('ScriptHookV installed successfully', mbInformation, MB_OK, IDOK);
    except
      SuppressibleMsgBox('Couldn''t extract ScriptHookV: ' + AddPeriod(GetExceptionMessage), mbCriticalError, MB_OK, IDOK);
    end;
  finally
    GExtractionPage.Hide;
  end;
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

  if not (FileExists(Folder + '\ScriptHookV.dll') and 
          ((FileExists(Folder + '\dinput8.dll') and FileExists(Folder + '\GTA5.exe')) or
          (FileExists(Folder + '\xinput1_4.dll') and FileExists(Folder + '\GTA5_Enhanced.exe')))) then
    if MsgBox('Warning: ScriptHook does not seem to be installed! ScriptHook is required to run chaos mod. Do you want to install it automatically?', mbError, MB_YESNO) = IDYES then
    begin
      ShowDownloadPage;
      if GScriptHookPath <> '' then
        ShowExtractionPage(Folder);
    end;

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

  DownloadPage := CreateDownloadPage(SetupMessage(msgWizardPreparing), SetupMessage(msgPreparingDesc), @OnDownloadProgress);
  DownloadPage.ShowBaseNameInsteadOfUrl := True;
  GExtractionPage := CreateOutputProgressPage(SetupMessage(msgWizardPreparing), SetupMessage(msgPreparingDesc));
end;

function InitializeSetup: Boolean;
begin
  Dependency_AddDotNet90Desktop;
  Dependency_AddVC2015To2022;

  Result := True;
end;