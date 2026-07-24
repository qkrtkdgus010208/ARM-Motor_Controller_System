#include "device_driver.h"
#include <stdio.h>

void Motor_Controller(void)
{
    switch (cur_motor_state)
    {
    case STOP:
        MOTOR_Stop();
        break;

    case CW:
        MOTOR_CW();
        break;

    case CCW:
        MOTOR_CCW();
        break;

    default:
        MOTOR_Stop();
        break;
    }
}