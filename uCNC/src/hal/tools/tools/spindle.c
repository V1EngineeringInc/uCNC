/*
    Name: spindle.c
    Description: Defines a spindle tool using PWM0-speed and DOUT0-dir for µCNC.
                 Defines a coolant output using DOUT1 and DOUT2.

    Copyright: Copyright (c) João Martins
    Author: João Martins
    Date: 16/12/2021

    µCNC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version. Please see <http://www.gnu.org/licenses/>

    µCNC is distributed WITHOUT ANY WARRANTY;
    Also without the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the	GNU General Public License for more details.
*/

#include "../../../cnc.h"

/**
 * This configures a simple spindle control with a pwm assigned to PWM0 and dir invert assigned to DOUT0
 * This spindle also has a coolant pin assigned to DOUT1
 *
 * */

// give names to the pins (easier to identify)
#define SPINDLE_PWM PWM0
#define SPINDLE_DIR DOUT0
#define COOLANT_FLOOD DOUT1
#define COOLANT_MIST DOUT2
#define SPINDLE_FEEDBACK ANALOG0

static uint8_t spindle1_speed;

void spindle1_set_speed(uint8_t value, bool invert)
{
    // easy macro to execute the same code as below
    // SET_SPINDLE(SPINDLE_PWM, SPINDLE_DIR, value, invert);
    spindle1_speed = value;
// speed optimized version (in AVR it's 24 instruction cycles)
#if SPINDLE_DIR >= 0
    if (SPINDLE_DIR > 0)
    {
        if (!invert)
        {
            mcu_clear_output(SPINDLE_DIR);
        }
        else
        {
            mcu_set_output(SPINDLE_DIR);
        }
    }
#endif

#if SPINDLE_PWM >= 0
    if (SPINDLE_PWM > 0)
    {
        mcu_set_pwm(SPINDLE_PWM, value);
    }
#endif
}

void spindle1_set_coolant(uint8_t value)
{
    // easy macro
    SET_COOLANT(COOLANT_FLOOD, COOLANT_MIST, value);
}

uint8_t spindle1_get_speed(void)
{
#if SPINDLE_PWM >= 0
    return spindle1_speed;
#else
    return 0;
#endif
}

#if PID_CONTROLLERS > 0
void spindle1_pid_update(int16_t value)
{
#if SPINDLE_PWM >= 0
    if (spindle1_speed != 0)
    {
        uint8_t newval = CLAMP(0, mcu_get_pwm(SPINDLE_PWM) + value, 255);
        mcu_set_pwm(SPINDLE_PWM, newval);
    }
#endif
}

int16_t spindle1_pid_error(void)
{
#if SPINDLE_FEEDBACK >= 0 && SPINDLE_PWM >= 0
    uint8_t reader = mcu_get_analog(ANALOG0);
    return (spindle1_speed - reader);
#else
    return 0;
#endif
}
#endif

const tool_t __rom__ spindle1 = {
    .startup_code = NULL,
    .shutdown_code = NULL,
    .set_speed = &spindle1_set_speed,
    .set_coolant = &spindle1_set_coolant,
#if PID_CONTROLLERS > 0
    .pid_update = &spindle1_pid_update,
    .pid_error = &spindle1_pid_error,
#endif
    .get_speed = &spindle1_get_speed};
