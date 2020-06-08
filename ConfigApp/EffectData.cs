using static ConfigApp.Effects;

namespace ConfigApp
{
    public class EffectData
    {
        public EffectTimedType EffectTimedType;
        public int EffectCustomTime;
        public int EffectWeight;
        public bool EffectPermanent;
        public bool EffectExcludedFromVoting;

        public EffectData(EffectTimedType effectTimedType, int effectCustomTime, int effectWeight, bool effectPermanent, bool effectExcludedFromVoting)
        {
            EffectTimedType = effectTimedType;
            EffectCustomTime = effectCustomTime;
            EffectWeight = effectWeight;
            EffectPermanent = effectPermanent;
            EffectExcludedFromVoting = effectExcludedFromVoting;
        }
    }
}
