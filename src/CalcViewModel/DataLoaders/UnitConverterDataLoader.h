// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcManager/UnitConverter.h"
#include "Common/NavCategory.h"

namespace CalculatorApp
{
    namespace ViewModel::Common
    {
        struct OrderedUnit : UnitConversionManager::Unit
        {
            OrderedUnit()
            {
            }

            OrderedUnit(
                int id,
                std::wstring name,
                std::wstring abbreviation,
                int order,
                bool isConversionSource = false,
                bool isConversionTarget = false,
                bool isWhimsical = false)
                : UnitConversionManager::Unit(id, name, abbreviation, isConversionSource, isConversionTarget, isWhimsical)
                , order(order)
            {
            }

            int order;
        };

        struct UnitData
        {
            CalculatorApp::ViewModel::Common::ViewMode categoryId;
            int unitId;
            double factor;
        };

        struct ExplicitUnitConversionData : UnitConversionManager::ConversionData
        {
            ExplicitUnitConversionData()
            {
            }
            ExplicitUnitConversionData(
                CalculatorApp::ViewModel::Common::ViewMode categoryId,
                int parentUnitId,
                int unitId,
                double ratio,
                double offset,
                bool offsetFirst = false)
                : categoryId(categoryId)
                , parentUnitId(parentUnitId)
                , unitId(unitId)
                , UnitConversionManager::ConversionData(ratio, offset, offsetFirst)
            {
            }

            CalculatorApp::ViewModel::Common::ViewMode categoryId;
            int parentUnitId;
            int unitId;
        };

        class UnitConverterDataLoader : public UnitConversionManager::IConverterDataLoader, public std::enable_shared_from_this<UnitConverterDataLoader>
        {
        public:
            UnitConverterDataLoader(Windows::Globalization::GeographicRegion ^ region);

        private:
            // IConverterDataLoader
            void LoadData() override;
            std::vector<UnitConversionManager::Category> GetOrderedCategories() override;
            std::vector<UnitConversionManager::Unit> GetOrderedUnits(const UnitConversionManager::Category& c) override;
            std::unordered_map<UnitConversionManager::Unit, UnitConversionManager::ConversionData, UnitConversionManager::UnitHash>
            LoadOrderedRatios(const UnitConversionManager::Unit& unit) override;
            bool SupportsCategory(const UnitConversionManager::Category& target) override;
            // IConverterDataLoader

            void GetCategories(_In_ std::shared_ptr<std::vector<UnitConversionManager::Category>> categoriesList);
            void GetUnits(_In_ std::unordered_map<CalculatorApp::ViewModel::Common::ViewMode, std::vector<CalculatorApp::ViewModel::Common::OrderedUnit>>& unitMap);
            void GetConversionData(_In_ std::unordered_map<CalculatorApp::ViewModel::Common::ViewMode, std::unordered_map<int, double>>& categoryToUnitConversionMap);
            void GetExplicitConversionData(_In_ std::unordered_map<int, std::unordered_map<int, UnitConversionManager::ConversionData>>& unitToUnitConversionList);

            std::wstring GetLocalizedStringName(_In_ Platform::String ^ stringId);

            std::shared_ptr<std::vector<UnitConversionManager::Category>> m_categoryList;
            std::shared_ptr<UnitConversionManager::CategoryToUnitVectorMap> m_categoryIDToUnitsMap;
            std::shared_ptr<UnitConversionManager::UnitToUnitToConversionDataMap> m_ratioMap;
            Platform::String ^ m_currentRegionCode;
        };
    }
}
