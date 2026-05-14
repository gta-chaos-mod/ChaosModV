using static ConfigApp.Effects;

namespace ConfigApp
{
    public sealed class EffectData
    {
        public bool? Enabled;
        public EffectTimedType? TimedType;
        public int? CustomTime;
        public int? WeightMult;
        public bool? ExcludedFromVoting;
        public string? CustomName;
        public int? ShortcutKeycode;
    }
}
