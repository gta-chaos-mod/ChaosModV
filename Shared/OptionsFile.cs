using System.IO;

namespace Shared
{
    public class OptionsFile
    {
        private readonly string m_FileName;
        private readonly string[] m_CompatFileNames;
        private Dictionary<string, string?> m_Options = new();

        public OptionsFile(string fileName, params string[] compatFileNames)
        {
            m_FileName = fileName;
            m_CompatFileNames = compatFileNames;
        }

        public bool HasKey(params string[] keys)
        {
            foreach (var key in keys)
            {
                if (m_Options.ContainsKey(key))
                {
                    return true;
                }
            }

            return false;
        }

        public IEnumerable<string> GetKeys()
        {
            foreach (var option in m_Options)
            {
                yield return option.Key;
            }
        }

        public string? ReadValue(string key, string? defaultValue = null, params string[] compatKeys)
        {
            var keys = compatKeys.Prepend(key);
            foreach (var _key in keys)
            {
                if (!m_Options.ContainsKey(_key) || m_Options[_key] is null)
                {
                    continue;
                }

                return m_Options[_key];
            }

            return defaultValue;
        }

        public int ReadValueInt(string key, int defaultValue, params string[] compatKeys)
        {
            if (!int.TryParse(ReadValue(key, null, compatKeys), out int result))
            {
                result = defaultValue;
            }

            return result;
        }

        public long ReadValueLong(string key, long defaultValue, params string[] compatKeys)
        {
            if (!long.TryParse(ReadValue(key, null, compatKeys), out long result))
            {
                result = defaultValue;
            }

            return result;
        }

        public bool ReadValueBool(string key, bool defaultValue, params string[] compatKeys)
        {
            var keys = compatKeys.Prepend(key);
            foreach (var _key in keys)
            {
                if (!m_Options.ContainsKey(_key) || m_Options[_key] == null)
                {
                    continue;
                }

                if (int.TryParse(ReadValue(_key), out int result))
                {
                    return result != 0;
                }
            }

            return defaultValue;
        }

        public void WriteValue(string key, string? value)
        {
            m_Options[key] = value is null || string.IsNullOrEmpty(value) ? null : value;
        }

        public void WriteValue(string key, int? value)
        {
            WriteValue(key, $"{value}");
        }

        public void WriteValue(string key, bool? value)
        {
            WriteValue(key, value is true ? 1 : 0);
        }

        public void ReadFile()
        {
            static string? readData(string fileName)
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

            string? data;
            if ((data = readData(m_FileName)) == null)
            {
                bool dataRead = false;
                foreach (var compatFileName in m_CompatFileNames)
                {
                    if ((data = readData(compatFileName)) != null)
                    {
                        dataRead = true;
                        break;
                    }
                }

                if (!dataRead)
                {
                    return;
                }
            }

            m_Options.Clear();

            if (data is not null)
            {
                foreach (string line in data.Split('\n'))
                {
                    if (!line.Contains('='))
                    {
                        continue;
                    }

                    var keyValuePair = line.Split('=', 2, System.StringSplitOptions.RemoveEmptyEntries
                        | System.StringSplitOptions.TrimEntries);

                    m_Options[keyValuePair[0]] = keyValuePair.Length == 2 ? keyValuePair[1] : null;
                }
            }
        }

        public void WriteFile()
        {
            if (m_FileName is null)
            {
                return;
            }

            string data = string.Empty;

            foreach (var option in m_Options)
            {
                data += $"{option.Key}={option.Value}\n";
            }

            var directory = Path.GetDirectoryName(m_FileName);
            if (directory is not null)
            {
                Directory.CreateDirectory(directory);
            }
            File.WriteAllText(m_FileName, data);
        }

        public void ResetFile()
        {
            if (m_FileName is null)
            {
                return;
            }

            var directory = Path.GetDirectoryName(m_FileName);
            if (directory is not null)
            {
                Directory.CreateDirectory(directory);
            }
            File.WriteAllText(m_FileName, null);

            // Reset all options too
            m_Options = new();
        }

        public bool HasCompatFile()
        {
            foreach (var compatFileName in m_CompatFileNames)
            {
                if (File.Exists(compatFileName))
                {
                    return true;
                }
            }

            return false;
        }

        public bool HasCompatFile(string fileName)
        {
            return m_CompatFileNames.Contains(fileName) && File.Exists(fileName);
        }

        public string[] GetCompatFiles()
        {
            return m_CompatFileNames;
        }
    }
}
