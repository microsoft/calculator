// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// SupplementaryResults.xaml.h
// Declaration of the SupplementaryResults class
//

using System;
using System.Diagnostics;

namespace CalculatorApp
{
    //public class DelighterUnitToStyleConverter : Windows.UI.Xaml.Data.IValueConverter
    //{
    //    DelighterUnitToStyleConverter()
    //    {
    //        m_delighters = new Windows.UI.Xaml.ResourceDictionary();
    //        m_delighters.Source = new Uri("ms-appx:///Views/DelighterUnitStyles.xaml");
    //    }

    //    Windows.UI.Xaml.ResourceDictionary m_delighters;

    //    public object Convert(object value, Type targetType, object parameter, String language)
    //    {
    //        Unit unit = (Unit)(value);

    //        Debug.Assert(unit.GetModelUnit().isWhimsical);
    //        if (!unit.GetModelUnit().isWhimsical)
    //        {
    //            return null;
    //        }

    //        std.string key = "Unit_";
    //        key.append(std.to_wstring(unit.GetModelUnit().id));
    //        return IStyle > (m_delighters.Lookup(new String(key.c_str())));
    //    }

    //    public object ConvertBack(object value, Type targetType, object parameter, String language)
    //    {
    //        // We never use convert back, only one way binding supported
    //        Debug.Assert(false);
    //        return null;
    //    }

    //};
}