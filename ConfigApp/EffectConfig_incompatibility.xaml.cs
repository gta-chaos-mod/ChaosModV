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
using System.Windows.Shapes;
using static ConfigApp.Effects;

namespace ConfigApp
{


    public partial class EffectConfig_incompatibility : Window
    {
        private bool m_IsSaved = false;
        public bool IsSaved
        {
            get
            {
                return m_IsSaved;
            }
        }
        public EffectType[] effectsCompatibility = new EffectType[] {};
        private List<EffectType> effectsCompatibilityList = new List<EffectType>();
        private Dictionary<EffectType, ListData> effects = new Dictionary<EffectType, ListData>();
        struct ListData
        {
            public string name;
            public bool inList;
        }

        private void InitList(EffectData effectData, EffectInfo effectInfo)
        {
            effects.Clear();
            foreach (EffectType item in (EffectType[])Enum.GetValues(typeof(EffectType)))
            {
                if (item == EffectType._EFFECT_ENUM_MAX || EffectsMap[item].Id == effectInfo.Id)
                {
                    continue;
                }
                ListData listData = new ListData();
                listData.name = EffectsMap[item].Name;
                if (effectData.IncompatibleWith != null)
                {
                    if (effectData.IncompatibleWith.Contains(item))
                    {
                        listData.inList = true;
                    }
                }
                effects.Add(item, listData);
            }
            foreach (KeyValuePair<EffectType, ListData> item in effects)
            {
                string name = item.Value.name;
                bool inList = item.Value.inList;
                if (inList)
                {
                    effectconf_custom_incompatibility_remove_list.Items.Add(name);
                } 
                else
                {
                    effectconf_custom_incompatibility_add_list.Items.Add(name);
                }
            }
        }

        public EffectConfig_incompatibility(EffectData effectData, EffectInfo effectInfo)
        {
            InitializeComponent();

            if (String.IsNullOrEmpty(effectData.CustomName))
            {
                Title = effectInfo.Name + " - Incompatibility Config";

            }
            else
            {
                Title = effectData.CustomName + " - Incompatibility Config";
            }

            InitList(effectData, effectInfo);

            effectconf_custom_incompatibility_remove_button.Click += (object sender, RoutedEventArgs e) =>
            {
                string selectedName = (string)effectconf_custom_incompatibility_remove_list.SelectedItem;
                if (String.IsNullOrEmpty(selectedName)) { return; }
                EffectType selectedType = EffectType._EFFECT_ENUM_MAX;
                foreach (KeyValuePair<EffectType, ListData> item in effects)
                {
                    if (item.Value.name == selectedName)
                    {
                        selectedType = item.Key;
                        break;
                    }
                }
                if (selectedType == EffectType._EFFECT_ENUM_MAX) { return; }
                ListData selectedData = effects[selectedType];
                selectedData.inList = false;
                effects[selectedType] = selectedData;
                effectconf_custom_incompatibility_remove_list.Items.Remove(selectedName);
                effectconf_custom_incompatibility_add_list.Items.Add(selectedName);
            };

            effectconf_custom_incompatibility_add_button.Click += (object sender, RoutedEventArgs e) =>
            {
                string selectedName = (string)effectconf_custom_incompatibility_add_list.SelectedItem;
                if (String.IsNullOrEmpty(selectedName)) { return; }
                EffectType selectedType = EffectType._EFFECT_ENUM_MAX;
                foreach (KeyValuePair<EffectType, ListData> item in effects)
                {
                    if (item.Value.name == selectedName)
                    {
                        selectedType = item.Key;
                        break;
                    }
                }
                if (selectedType == EffectType._EFFECT_ENUM_MAX) { return; }
                ListData selectedData = effects[selectedType];
                selectedData.inList = true;
                effects[selectedType] = selectedData;
                effectconf_custom_incompatibility_add_list.Items.Remove(selectedName);
                effectconf_custom_incompatibility_remove_list.Items.Add(selectedName);
            };

            reset_button.Click += (object sender, RoutedEventArgs e) =>
            {
                effectconf_custom_incompatibility_remove_list.Items.Clear();
                effectconf_custom_incompatibility_add_list.Items.Clear();
                try
                {
                    Array.Clear(effectsCompatibility, 0, effectsCompatibility.Length);
                }
                catch (Exception)
                {
                }
                InitList(effectData, effectInfo);
            };

            save_button.Click += (object sender, RoutedEventArgs e) =>
            {
                foreach (KeyValuePair<EffectType, ListData> item in effects)
                {
                    if (item.Value.inList == true)
                    {
                        effectsCompatibilityList.Add(item.Key);
                    }
                }
                effectsCompatibility = effectsCompatibilityList.ToArray();
                m_IsSaved = true;
                Close();
            };
        }

    }
}
