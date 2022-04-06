using static ConfigApp.Effects;
using System;
using System.Collections.Generic;

namespace ConfigApp
{
    public class EffectData
    {
        public EffectTimedType TimedType;
        public int CustomTime;
        public int WeightMult;
        public bool Permanent;
        public bool ExcludedFromVoting;
        public string CustomName;
        public int Shortcut;
        public EffectType[] IncompatibleWith = new EffectType[] {};

        public string GetIncompatibilityString()
        {
            List<string> list = new List<string>();
            foreach (EffectType item in IncompatibleWith)
            {
                list.Add(EffectsMap[item].Id);
            }
            string[] arr = list.ToArray();
            return String.Join(",", arr);
        }

        public EffectData(EffectTimedType timedType, int customTime, int weightMult, bool permanent, bool excludedFromVoting, string customName, int shortcut, EffectType[] incompatibleWith)
        {
            TimedType = timedType;
            CustomTime = customTime;
            WeightMult = weightMult;
            Permanent = permanent;
            ExcludedFromVoting = excludedFromVoting;
            CustomName = customName;
            Shortcut = shortcut;
            IncompatibleWith = incompatibleWith;
        }
    }
}
