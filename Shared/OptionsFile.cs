using System.Collections.Generic;
using System.IO;

namespace Shared
{
    public class OptionsFile
    {
        private string m_FileName;
        private string m_CompatFileName;
        private Dictionary<string, string> m_Options = new Dictionary<string, string>();

        public OptionsFile(string fileName, string compatFileName = null)
        {
            m_FileName = fileName;
            m_CompatFileName = compatFileName;
        }

        public bool HasKey(string key)
        {
            return m_Options.ContainsKey(key);
        }

        public IEnumerable<string> GetKeys()
        {
            foreach (var option in m_Options)
            {
                yield return option.Key;
            }
        }

        public string ReadValue(string key, string defaultValue = null)
        {
            return HasKey(key) ? m_Options[key] : defaultValue;
        }

        public int ReadValueInt(string key, int defaultValue)
        {
            int result;
            if (!int.TryParse(ReadValue(key), out result))
            {
                result = defaultValue;
            }

            return result;
        }

        public bool ReadValueBool(string key, bool defaultValue)
        {
            bool result = defaultValue;
            if (HasKey(key))
            {
                if (int.TryParse(ReadValue(key), out int tmp))
                {
                    result = tmp != 0;
                }
            }

            return result;
        }

        public void WriteValue(string key, string value)
        {
            if (value != null && value.Trim().Length > 0)
            {
                m_Options[key] = value;
            }
            else
            {
                m_Options.Remove(key);
            }
        }

        public void WriteValue(string key, int value)
        {
            WriteValue(key, $"{value}");
        }

        public void WriteValue(string key, bool value)
        {
            WriteValue(key, value ? 1 : 0);
        }

        public void ReadFile()
        {
            string readData(string fileName)
            {
                if (!File.Exists(fileName))
                {
                    return null;
                }

                string _data = File.ReadAllText(fileName);
                if (_data.Length == 0)
                {
                    return null;
                }

                return _data;
            }

            string data;
            if ((data = readData(m_FileName)) == null)
            {
                if ((data = readData(m_CompatFileName)) == null)
                {
                    return;
                }
            }

            m_Options.Clear();

            foreach (string line in data.Split('\n'))
            {
                string[] keyValuePair = line.Split('=');
                if (keyValuePair.Length != 2)
                {
                    continue;
                }

                m_Options[keyValuePair[0]] = keyValuePair[1];
            }
        }

        public void WriteFile()
        {
            string data = string.Empty;

            foreach (var option in m_Options)
            {
                data += $"{option.Key}={option.Value}\n";
            }

            Directory.CreateDirectory(Path.GetDirectoryName(m_FileName));
            File.WriteAllText(m_FileName, data);
        }

        public void ResetFile()
        {
            Directory.CreateDirectory(Path.GetDirectoryName(m_FileName));
            File.WriteAllText(m_FileName, null);

            // Reset all options too
            m_Options = new Dictionary<string, string>();
        }

        public bool HasCompatFile()
        {
            return m_CompatFileName != null && File.Exists(m_CompatFileName);
        }

        public string GetCompatFile()
        {
            return m_CompatFileName;
        }
    }
}
