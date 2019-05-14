using System;
using System.Collections.Generic;
using System.Text;

namespace CalculationManager
{
    public enum NUMOBJ_FMT
    {
        FMT_FLOAT,      // returns floating point, or exponential if number is too big
        FMT_SCIENTIFIC, // always returns scientific notation
        FMT_ENGINEERING // always returns engineering notation such that exponent is a multiple of 3
    }

    public enum ANGLE_TYPE
    {
        ANGLE_DEG, // Calculate trig using 360 degrees per revolution
        ANGLE_RAD, // Calculate trig using 2 pi  radians per revolution
        ANGLE_GRAD // Calculate trig using 400 gradients per revolution
    }
}
