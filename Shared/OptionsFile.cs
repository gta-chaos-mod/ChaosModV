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
            return keys.Any(m_Options.ContainsKey);
        }

        public IEnumerable<string> GetKeys()
        {
            foreach (var option in m_Options)
                yield return option.Key;
        }

        public T? ReadValue<T>(string key, T? defaultValue = default, params string[] compatKeys)
        {
            foreach (var candidateKey in compatKeys.Prepend(key))
            {
                if (!m_Options.TryGetValue(candidateKey, out var token) || token is null)
                    continue;

                if (TryReadLegacyBool<T>(candidateKey, defaultValue, out var boolValue))
                    return boolValue;

                try
                {
                    return token.ToObject<T>();
                }
                catch
                {
                    return defaultValue;
                }
            }

            return defaultValue;
        }

        private bool TryReadLegacyBool<T>(string key, T? defaultValue, out T? value)
        {
            value = defaultValue;

            if (typeof(T) != typeof(bool) || m_IsJson)
                return false;

            var legacyValue = ReadValue<string>(key, defaultValue is bool boolDefault && boolDefault ? "1" : "0");
            if (legacyValue is null)
                return true;

            if (!int.TryParse(legacyValue, out var parsedValue))
                return true;

            value = (T?)(object)(parsedValue != 0);
            return true;
        }

        public void WriteValue<T>(string key, T? value)
        {
            m_Options[key] = value switch
            {
                null => null,
                string text when string.IsNullOrEmpty(text) => null,
                JObject objectValue => objectValue,
                JArray arrayValue => arrayValue,
                _ => new JValue(value)
            };
        }

        public void WriteValueAsInt(string key, string? value)
        {
            m_Options[key] = int.TryParse(value, out var parsedValue) ? new JValue(parsedValue) : null;
        }

        public void ReadFile()
        {
            FoundFilePath = string.Empty;
            m_Options.Clear();

            var data = ReadFirstAvailableFile();
            if (data is null)
            {
                m_IsJson = false;
                return;
            }

            if (FoundFilePath.EndsWith(".json"))
            {
                m_IsJson = true;
                LoadJson(data);
            }
            else if (FoundFilePath.EndsWith(".ini"))
            {
                m_IsJson = false;
                LoadIni(data);
            }
        }

        private string? ReadFirstAvailableFile()
        {
            foreach (var fileName in new[] { m_FilePath }.Concat(CompatFilePaths))
            {
                var data = ReadData(fileName);
                if (data is null)
                    continue;

                FoundFilePath = fileName;
                return data;
            }

            return null;
        }

        private static string? ReadData(string fileName)
        {
            if (!File.Exists(fileName))
                return null;

            var data = File.ReadAllText(fileName);
            return data.Length == 0 ? null : data;
        }

        private void LoadJson(string data)
        {
            foreach (var (key, value) in JObject.Parse(data))
                m_Options[key] = value;
        }

        private void LoadIni(string data)
        {
            foreach (var line in data.Split('\n'))
            {
                if (!line.Contains('='))
                    continue;

                var keyValuePair = line.Split('=', 2, StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
                m_Options[keyValuePair[0]] = keyValuePair.Length == 2 ? new JValue(keyValuePair[1]) : null;
            }
        }

        public void WriteFile()
        {
            FoundFilePath = m_FilePath;

            var directory = Path.GetDirectoryName(m_FilePath);
            if (directory is not null)
                Directory.CreateDirectory(directory);

            var json = new JObject();
            foreach (var (key, value) in m_Options)
                json[key] = value;

            File.WriteAllText(m_FilePath, JsonConvert.SerializeObject(json));
        }

        public void ResetFile()
        {
            var directory = Path.GetDirectoryName(m_FilePath);
            if (directory is not null)
                Directory.CreateDirectory(directory);

            File.WriteAllText(m_FilePath, string.Empty);
            m_Options = new();
        }

        public bool HasCompatFile()
        {
            return CompatFilePaths.Any(File.Exists);
        }
    }
}
