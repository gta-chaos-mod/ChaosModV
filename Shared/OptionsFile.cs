using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace Shared
{
    public class OptionsFile(string filePath, params string[] compatFilePaths)
    {
        public string FoundFilePath { get; private set; } = string.Empty;
        public string[] CompatFilePaths { get; private set; } = compatFilePaths;

        private readonly string m_FilePath = filePath;
        private bool m_IsJson = false;
        private Dictionary<string, JToken?> m_Options = new();

        public bool HasKey(params string[] keys)
        {
            foreach (var key in keys)
            {
                if (m_Options.ContainsKey(key))
                    return true;
            }

            return false;
        }

        public IEnumerable<string> GetKeys()
        {
            foreach (var option in m_Options)
                yield return option.Key;
        }

        public T? ReadValue<T>(string key, T? defaultValue = default, params string[] compatKeys)
        {
            var keys = compatKeys.Prepend(key);
            foreach (var _key in keys)
            {
                if (!m_Options.ContainsKey(_key) || m_Options[_key] is null)
                    continue;

                if (typeof(T) == typeof(bool) && !m_IsJson)
                {
                    string? str = ReadValue<string>(_key, $"{((bool?)Convert.ChangeType(defaultValue, typeof(bool)) == true ? "1" : "0")}");
                    if (str is null)
                        return defaultValue;

                    if (int.TryParse(str, out int result))
                        return (T?)Convert.ChangeType(result != 0, typeof(T));
                }

                try
                {
                    return m_Options[_key]!.ToObject<T>();
                }
                catch
                {
                    return defaultValue;
                }
            }

            return defaultValue;
        }

        public void WriteValue<T>(string key, T? value)
        {
            if (value is null || (value is string && string.IsNullOrEmpty(value as string)))
                m_Options[key] = null;
            else if (value is JObject o)
                m_Options[key] = o;
            else if (value is JArray a)
                m_Options[key] = a;
            else
                m_Options[key] = new JValue(value);
        }

        public void WriteValueAsInt(string key, string? value)
        {
            m_Options[key] = value is null || string.IsNullOrEmpty(value) ? null : new JValue(int.Parse(value));
        }

        public void ReadFile()
        {
            static string? readData(string fileName)
            {
                if (!File.Exists(fileName))
                    return null;

                string _data = File.ReadAllText(fileName);
                if (_data.Length == 0)
                    return null;

                return _data;
            }

            string? data;
            if ((data = readData(m_FilePath)) != null)
                FoundFilePath = m_FilePath;
            else
            {
                bool dataRead = false;
                foreach (var compatFileName in CompatFilePaths)
                {
                    if ((data = readData(compatFileName)) != null)
                    {
                        dataRead = true;
                        FoundFilePath = compatFileName;
                        break;
                    }
                }

                if (!dataRead)
                    return;
            }

            m_Options.Clear();

            if (data is not null && FoundFilePath is not null)
            {
                if (FoundFilePath.EndsWith(".json"))
                {
                    m_IsJson = true;

                    var json = JObject.Parse(data);
                    foreach (var (key, value) in json)
                        m_Options[key] = value;
                }
                else if (FoundFilePath.EndsWith(".ini"))
                {
                    m_IsJson = false;

                    foreach (string line in data.Split('\n'))
                    {
                        if (!line.Contains('='))
                            continue;

                        var keyValuePair = line.Split('=', 2, System.StringSplitOptions.RemoveEmptyEntries
                            | System.StringSplitOptions.TrimEntries);

                        m_Options[keyValuePair[0]] = keyValuePair.Length == 2 ? new JValue(keyValuePair[1]) : null;
                    }
                }
            }
        }

        public void WriteFile()
        {
            if (m_FilePath is null)
                return;

            FoundFilePath = m_FilePath;

            var json = new JObject();
            foreach (var (key, value) in m_Options)
                json[key] = value;

            var directory = Path.GetDirectoryName(m_FilePath);
            if (directory is not null)
                Directory.CreateDirectory(directory);

            File.WriteAllText(m_FilePath, JsonConvert.SerializeObject(json));
        }

        public void ResetFile()
        {
            if (m_FilePath is null)
                return;

            var directory = Path.GetDirectoryName(m_FilePath);
            if (directory is not null)
                Directory.CreateDirectory(directory);

            File.WriteAllText(m_FilePath, null);

            // Reset all options too
            m_Options = new();
        }

        public bool HasCompatFile()
        {
            foreach (var compatFileName in CompatFilePaths)
            {
                if (File.Exists(compatFileName))
                    return true;
            }

            return false;
        }
    }
}
