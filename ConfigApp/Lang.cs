using System.Collections.Generic;

namespace ConfigApp
{
    class Lang
    {
        public static List<string> langs = new List<string> { "English", "Español"};
        public static string app_tags(int index, int language)
        {
            if (language == 1)
            {
                return esES.EffectsCategoryLabels[index];
            }
            else
            {
                return enEN.EffectsCategoryLabels[index];
            }
        }
        public static string app_labels(string key, int language)
        {
            if (language == 1)
            {
                return esES.App[key];
            }
            else
            {
                return enEN.App[key];
            }
        }
        public static string app_effects(string key, int language)
        {
            if (language == 1)
            {
                return esES.EffectsMap[key];
            }
            else
            {
                return enEN.EffectsMap[key];
            }
        }
        public static string get_error(string key, string language)
        {
            if (language == "es-ES")
            {
                return esES.PopupMSGs[key];
            }
            else if(language == "1")
            {
                return esES.PopupMSGs[key];
            }
            else if (language == "0")
            {
                return enEN.PopupMSGs[key];
            }
            else
            {
                return enEN.PopupMSGs[key];
            }
        }
        public static string effect_config(string key, int language)
        {
            if (language == 1)
            {
                return esES.EffectConfig[key];
            }
            else
            {
                return enEN.EffectConfig[key];
            }
        }
        public static int check_for_lang(string locale)
        {
            if (locale == "es-ES")
            {
                return 1;
            }
            return 0;
        }
    }
}
