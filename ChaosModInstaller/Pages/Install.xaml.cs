using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Threading;

using System.IO;

namespace ChaosModInstaller
{
    public partial class Install : Page
    {
        private InstallationType _type;
        private Installer _folderInstaller;
        private Installer _scriptInstaller;
        CancellationTokenSource cts = new CancellationTokenSource();

        private string _chaosModFiles;
        private string _chaosModScript;

        private string _gtaPath;
        private string _fiveMPath;

        private enum InstallationState
        { 
            READY, EXECUTING, FINISHED
        }

        private InstallationState _state = InstallationState.READY;

        public Install(InstallationType type, string gtaPath, string fiveMPath = "")
        {
            _gtaPath = gtaPath;
            _fiveMPath = fiveMPath;
            _type = type;

            _chaosModFiles = Path.Combine(Directory.GetCurrentDirectory() + "\\bin\\chaosmod");
            _chaosModScript = Path.Combine(Directory.GetCurrentDirectory() + "\\bin\\ChaosMod.asi");

            InitializeComponent();

            next_btn.Content = "Start";
            next_btn.Click += OnClick;

            SetState(InstallationState.READY);
        }

        private void SetState(InstallationState state)
        {
            _state = state;
            switch (state)
            {
                case InstallationState.EXECUTING:
                    busyspinner.Visibility = Visibility.Visible;
                    next_btn.Visibility = Visibility.Hidden;
                    break;
                case InstallationState.FINISHED:
                    busyspinner.Visibility = Visibility.Hidden;
                    next_btn.Content = "Finish";
                    next_btn.Visibility = Visibility.Visible;
                    break;
                case InstallationState.READY:
                    busyspinner.Visibility = Visibility.Hidden;
                    next_btn.Content = "Install";
                    next_btn.Visibility = Visibility.Visible;
                    break;
            }
        }

        private void SimulateWork()
        {
            Thread.SpinWait(500000);
        }

        private void StartInstall()
        {
            SetState(InstallationState.EXECUTING);

            SimulateWork();
            Installer.CopyDirectoryToLocation(_chaosModFiles, _gtaPath, true);
            SimulateWork();
            if (_type == InstallationType.GTA5)
            {
                Installer.CopyFileToLocation(_chaosModScript, _gtaPath);
            }
            else
            {
                Installer.CopyFileToLocation(_chaosModScript, _fiveMPath);
            }
            SimulateWork();

            SetState(InstallationState.FINISHED);
        }

        private void OnClick(object sender, EventArgs e)
        {
            switch (_state)
            {
                case InstallationState.READY:
                    StartInstall();
                    break;
                case InstallationState.FINISHED:
                case InstallationState.ERRORED:
                default:
                    MainWindow.Exit();
                    break;
            }
        }
    }
}
