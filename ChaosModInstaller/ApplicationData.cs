using System;
using System.IO;
using System.Windows;
using System.Collections.Generic;
using System.Text;
using System.Configuration;
using System.Configuration.Assemblies;

namespace ChaosModInstaller
{
    public class ApplicationData
    {
        public static void SetValue(string key, object value)
        {
            Properties.Settings.Default[key] = value;
            Properties.Settings.Default.Save();
        }

        public static object GetValue(string key)
        {
            return Properties.Settings.Default[key];
        }

        public static void Reset()
        {
            Properties.Settings.Default.Reset();
        }
    }
}
