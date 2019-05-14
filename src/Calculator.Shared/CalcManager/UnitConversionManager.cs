

namespace UnitConversionManager
{
    public struct Unit
    {
        public Unit(int id, string name, string abbreviation, bool isConversionSource, bool isConversionTarget, bool isWhimsical)
        {
            this.id = id;
            this.name = name;
            this.accessibleName = name;
            this.abbreviation = abbreviation;
            this.isConversionSource = isConversionSource;
            this.isConversionTarget = isConversionTarget;
            this.isWhimsical = isWhimsical;
        }

        public Unit(
            int id,
            string currencyName,
            string countryName,
            string abbreviation,
            bool isRtlLanguage,
            bool isConversionSource,
            bool isConversionTarget)
        {
            this.id = id;
            this.abbreviation = abbreviation;
            this.isConversionSource = isConversionSource;
            this.isConversionTarget = isConversionTarget;
            this.isWhimsical = false;

            string nameValue1 = isRtlLanguage ? currencyName : countryName;
            string nameValue2 = isRtlLanguage ? countryName : currencyName;

            name = nameValue1 + " - " + nameValue2;
            accessibleName = nameValue1 + " " + nameValue2;
        }

        public readonly int id;
        public readonly string name;
        public readonly string accessibleName;
        public readonly string abbreviation;
        public readonly bool isConversionSource;
        public readonly bool isConversionTarget;
        public readonly bool isWhimsical;

        //    bool operator !=(Unit that) const
        //     {
        //        return that.id != id;
        //    }

        //bool operator ==(Unit that) const
        //    {
        //        return that.id == id;
        //    }
    }
}