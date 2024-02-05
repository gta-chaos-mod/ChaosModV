using Newtonsoft.Json;

using static ConfigApp.Effects;

namespace ConfigApp
{
    [JsonObject(ItemNullValueHandling = NullValueHandling.Ignore)]
    public class EffectData
    {
        private class Converter<T> : JsonConverter
        {
            public override void WriteJson(JsonWriter writer, object? value, JsonSerializer serializer)
            {
                if (value is null)
                {
                    return;
                }

                if (value is bool v)
                {
                    writer.WriteValue(v ? 1 : 0);
                }
                else
                {
                    writer.WriteValue((int)value);
                }
            }

            public override object? ReadJson(JsonReader reader, Type objectType, object? existingValue, JsonSerializer serializer)
            {
                if (typeof(T) == typeof(bool))
                {
                    return $"{reader.Value}" != "0";
                }
                else if (typeof(T) == typeof(EffectTimedType) && reader.Value is not null)
                {
                    return Enum.ToObject(typeof(EffectTimedType), reader.Value);
                }

                return null;
            }

            public override bool CanConvert(Type objectType)
            {
                return objectType == typeof(T);
            }
        }

        [JsonConverter(typeof(Converter<bool>))]
        public bool? Enabled;
        [JsonConverter(typeof(Converter<EffectTimedType>))]
        public EffectTimedType? TimedType;
        public int? CustomTime;
        public int? WeightMult;
        [JsonConverter(typeof(Converter<bool>))]
        public bool? ExcludedFromVoting;
        public string? CustomName;
        public int? ShortcutKeycode;
    }
}
