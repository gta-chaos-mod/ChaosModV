using static ConfigApp.Effects;

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

        public EffectData(EffectTimedType timedType, int customTime, int weightMult, bool permanent, bool excludedFromVoting, string customName, int shortcut)
        {
            TimedType = timedType;
            CustomTime = customTime;
            WeightMult = weightMult;
            Permanent = permanent;
            ExcludedFromVoting = excludedFromVoting;
            CustomName = customName;
            Shortcut = shortcut;
        }
    }
}
