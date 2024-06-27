#ifndef CONTROL_H
#define CONTROL_H

#include <stdint.h>

typedef enum {
    PS2_CONTROL = 0,
    IR_CONTROL
} ControlMode;

void processControl(void);
void ToggleControlMode(void);  // ��� ToggleControlMode ��������
#endif /* CONTROL_H */
