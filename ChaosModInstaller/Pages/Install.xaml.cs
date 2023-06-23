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
        private Installer _folderInstaller;
        private Installer _scriptInstaller;
        LoadingControl _busyspinner;
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

        private void CreateBusySpinner()
        {
            _busyspinner = new LoadingControl();
            _busyspinner.Diameter = 80;
            _busyspinner.Color1 = (Color)ColorConverter.ConvertFromString("Green");
            _busyspinner.Color2 = (Color)ColorConverter.ConvertFromString("#FF232844");
            _busyspinner.VerticalAlignment = VerticalAlignment.Bottom;
            _busyspinner.HorizontalAlignment = HorizontalAlignment.Center;

            spinnerpanel.Children.Add(_busyspinner);
            InitializeComponent();
            _busyspinner.InitializeComponent();
            
        }

        private void RemoveBusySpinner()
        {
            spinnerpanel.Children.Remove(_busyspinner);
        }

        private void SetOutputText(string msg)
        {
            install_output.Content = msg;
        }

        public Install(string gtaPath, string fiveMPath = "")
        {
            _gtaPath = gtaPath;
            _fiveMPath = fiveMPath;

            _chaosModFiles = Path.Combine(Directory.GetCurrentDirectory() + "\\bin\\chaosmod");
            _chaosModScript = Path.Combine(Directory.GetCurrentDirectory() + "\\bin\\ChaosMod.asi");

            InitializeComponent();

            next_btn.Content = "Start";
            next_btn.Click += OnClick;

            SetOutputText("Ready to install.");
            SetState(InstallationState.READY);
        }

        private void SetState(InstallationState state)
        {
            _state = state;
            switch (state)
            {
                case InstallationState.EXECUTING:
                    
                    next_btn.Visibility = Visibility.Hidden;
                    break;
                case InstallationState.FINISHED:
                    
                    next_btn.Content = "Finish";
                    next_btn.Visibility = Visibility.Visible;
                    break;
                case InstallationState.READY:
                    
                    next_btn.Content = "Install";
                    next_btn.Visibility = Visibility.Visible;
                    break;
            }
        }

        private void SimulateWork()
        {
            Thread.Sleep(1500);
        }

        private void StartInstall() // Run on a seperate thred, so we call Dispathcer.Invoke, for any functions that modify the UI. (or it will crash)
        {
            this.Dispatcher.Invoke(SetState, InstallationState.EXECUTING);
            this.Dispatcher.Invoke(CreateBusySpinner);
            SimulateWork();
            Dispatcher.Invoke(SetOutputText, "Copying ChaosMod data to directory...");
            Installer.CopyDirectoryToLocation(_chaosModFiles, _gtaPath, true);
            SimulateWork(); // Just to make the user think heavy work is happening ;)
            Dispatcher.Invoke(SetOutputText, "Copying ChaosMod Script to directory...");
            Installer.CopyFileToLocation(_chaosModScript, _fiveMPath);
            SimulateWork();
            Dispatcher.Invoke(SetOutputText, "Finishing up...");
            SimulateWork();
            this.Dispatcher.Invoke(SetState, InstallationState.FINISHED);
            Dispatcher.Invoke(SetOutputText, "ChaosMod Succesfully Installed!");
            this.Dispatcher.Invoke(RemoveBusySpinner);
        }

        private void OnClick(object sender, EventArgs e)
        {
            switch (_state)
            {
                case InstallationState.READY:
                    Thread t = new Thread(new ThreadStart(StartInstall));
                    t.Start();
                    break;
                case InstallationState.FINISHED:
                default:
                    MainWindow.Exit();
                    break;
            }
        }
    }
}
