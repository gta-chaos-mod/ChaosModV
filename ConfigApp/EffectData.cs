using Newtonsoft.Json;

using static ConfigApp.Effects;

namespace ConfigApp
{
    [JsonObject(ItemNullValueHandling = NullValueHandling.Ignore)]
    public class EffectData
    {
        private sealed class BoolAsIntJsonConverter : JsonConverter
        {
            public override void WriteJson(JsonWriter writer, object? value, JsonSerializer serializer)
            {
                if (value is null)
                    return;

                writer.WriteValue((bool)value ? 1 : 0);
            }

            public override object? ReadJson(JsonReader reader, Type objectType, object? existingValue, JsonSerializer serializer)
            {
                return reader.Value?.ToString() != "0";
            }

            public override bool CanConvert(Type objectType)
            {
                return objectType == typeof(bool) || objectType == typeof(bool?);
            }
        }

        private sealed class EffectTimedTypeJsonConverter : JsonConverter
        {
            public override void WriteJson(JsonWriter writer, object? value, JsonSerializer serializer)
            {
                if (value is null)
                    return;

                writer.WriteValue((int)value);
            }

            public override object? ReadJson(JsonReader reader, Type objectType, object? existingValue, JsonSerializer serializer)
            {
                if (reader.Value is null)
                    return null;

                return Enum.ToObject(typeof(EffectTimedType), reader.Value);
            }

            public override bool CanConvert(Type objectType)
            {
                return objectType == typeof(EffectTimedType) || objectType == typeof(EffectTimedType?);
            }
        }

        [JsonConverter(typeof(BoolAsIntJsonConverter))]
        public bool? Enabled;
        [JsonConverter(typeof(EffectTimedTypeJsonConverter))]
        public EffectTimedType? TimedType;
        public int? CustomTime;
        public int? WeightMult;
        [JsonConverter(typeof(BoolAsIntJsonConverter))]
        public bool? ExcludedFromVoting;
        public string? CustomName;
        public int? ShortcutKeycode;
    }
}
