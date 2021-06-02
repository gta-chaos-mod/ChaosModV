using System.Collections.Generic;
using System.IO;

namespace Shared
{
    public class OptionsFile
    {
        private string m_fileName;
        private Dictionary<string, string> m_options = new Dictionary<string, string>();

        public OptionsFile(string fileName)
        {
            m_fileName = fileName;
        }

        public bool HasKey(string key)
        {
            return m_options.ContainsKey(key);
        }

        public IEnumerable<string> GetKeys()
        {
            foreach (var option in m_options)
            {
                yield return option.Key;
            }
        }

        public string ReadValue(string key, string defaultValue = null)
        {
            return HasKey(key) ? m_options[key] : defaultValue;
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
                m_options[key] = value;
            }
            else
            {
                m_options.Remove(key);
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
            if (!File.Exists(m_fileName))
            {
                return;
            }

            string data = File.ReadAllText(m_fileName);
            if (data.Length == 0)
            {
                return;
            }

            m_options.Clear();

            foreach (string line in data.Split('\n'))
            {
                string[] keyValuePair = line.Split('=');
                if (keyValuePair.Length != 2)
                {
                    continue;
                }

                m_options[keyValuePair[0]] = keyValuePair[1];
            }
        }

        public void WriteFile()
        {
            string data = string.Empty;

            foreach (var option in m_options)
            {
                data += $"{option.Key}={option.Value}\n";
            }

            File.WriteAllText(m_fileName, data);
        }

        public void ResetFile()
        {
            File.WriteAllText(m_fileName, null);

            // Reset all options too
            m_options = new Dictionary<string, string>();
        }
    }
}
