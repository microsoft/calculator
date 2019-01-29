// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace Calculator.UIAutomationLibrary.Components
{
    public interface ICanFocusWithClicks
    {
        /// <summary>
        /// Sets focus on an object by clicking on it, without causing further action.
        /// Supports AccSpot scans by raising click events.
        /// </summary>
        void FocusWithClicks();
    }
}

