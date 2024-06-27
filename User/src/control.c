#include "control.h"
#include "pstwo.h"
#include "bc7210.h"


ControlMode currentControlMode = PS2_CONTROL;  // 初始状态为手柄控制
// ControlMode currentControlMode = IR_CONTROL;  // 初始状态为红外遥控控制

void processControl(void) {
    // 根据当前控制模式执行对应的控制函数
    if (currentControlMode == PS2_CONTROL) {
        PstwoControl();
    } else {
        InfraredControl();
    }
}

void ToggleControlMode(void) {
    if (currentControlMode == PS2_CONTROL) {
        currentControlMode = IR_CONTROL;
    } else {
        currentControlMode = PS2_CONTROL;
    }
}

