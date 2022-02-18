using Newtonsoft.Json;
using System.Collections.Generic;
using System.IO;

namespace Shared
{
    public class EffectOption
    {
        public string Name;
        public bool Enabled;
        public int timeMode;
        public int customTime;
        public int weightMultiplier;
        public bool permanent;
        public bool excludeFromVoting;
        public string customName;
        public int shortcutKey;
    }

    public class EffectOptionsFile
    {
        private string m_fileName;
        private Dictionary<string, EffectOption> m_options = new Dictionary<string, EffectOption>();
        private bool parsedFile;

        public EffectOptionsFile(string fileName)
        {
            m_fileName = fileName;
        }

        public List<EffectOption> GetOptions()
        {
            List<EffectOption> options = new List<EffectOption>();
            foreach (var option in m_options)
            {
                options.Add(option.Value);
            }
            return options;
        }

        public bool HasParsedFile()
        {
            return parsedFile;
        }


        public void SetValue(string key, EffectOption value)
        {
            m_options[key] = value;
        }

        public void ReadFile()
        {
            parsedFile = false;
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

            List<EffectOption> allOptions = JsonConvert.DeserializeObject<List<EffectOption>>(data);

            foreach (EffectOption option in allOptions)
            {
                m_options[option.Name] = option;
            }
            parsedFile = true;
        }

        public void WriteFile()
        {
            List<EffectOption> allOptions = new List<EffectOption>();
            foreach (var option in m_options)
            {
                allOptions.Add(option.Value);
            }
            string data = JsonConvert.SerializeObject(allOptions, Formatting.Indented);
            File.WriteAllText(m_fileName, data);
        }

        public void ResetFile()
        {
            File.WriteAllText(m_fileName, null);

            // Reset all options too
            m_options = new Dictionary<string, EffectOption>();
        }
    }
}
