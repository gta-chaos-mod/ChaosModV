using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace ChaosModInstaller
{
    public class Installer
    { 
        public static void CopyDirectoryToLocation(string sourceDir, string destinationDir, bool recursive)
        {
            var dir = new DirectoryInfo(sourceDir);

            if (!dir.Exists)
                return;

            DirectoryInfo[] dirs = dir.GetDirectories();

            Directory.CreateDirectory(destinationDir);

            foreach (FileInfo file in dir.GetFiles())
            {
                string targetFilePath = Path.Combine(destinationDir, file.Name);
                if (!File.Exists(targetFilePath))
                    file.CopyTo(targetFilePath);
            }

            if (recursive)
            {
                foreach (DirectoryInfo subDir in dirs)
                {
                    string newDestinationDir = Path.Combine(destinationDir, subDir.Name);
                    CopyDirectoryToLocation(subDir.FullName, newDestinationDir, true);
                }
            }
        }

        public static void CopyFileToLocation(string sourceFile, string destinationDir)
        {
            var fil = new FileInfo(sourceFile);

            if (!fil.Exists)
                return;

            var dir = new DirectoryInfo(destinationDir);

            if (!dir.Exists)
                Directory.CreateDirectory(destinationDir);

            if (!File.Exists(Path.Combine(destinationDir, fil.Name)))
                fil.CopyTo(Path.Combine(destinationDir, fil.Name));
        }
    }
}