#include "Input.h"

//NES keys:
//[Up][Left][Right][Down][A][B][Start][Select]

//Mapped to standard keyboard keys:
//[Up][Left][Right][Down][Z][X][Enter][RShift]

//Mapped to standard Xbox controller buttons:
//[Up][Left][Right][Down][A][X][Start][Select]
//                          or
//                          [B]

Input::Input(QObject *parent)
    : QObject(parent)
{
    QObject::connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonPressEvent,
                        this, &Input::gamepadButtonPressed);
    QObject::connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent,
                        this, &Input::gamepadButtonReleased);
}

void Input::gamepadButtonPressed(int id, QGamepadManager::GamepadButton button, double value) {
        switch(button) {
            case QGamepadManager::ButtonA:
                joy1KeyA = true;
                break;
            case QGamepadManager::ButtonB:
            case QGamepadManager::ButtonX:
                joy1KeyB = true;
                break;
            case QGamepadManager::ButtonStart:
                joy1KeyStart = true;
                break;
            case QGamepadManager::ButtonSelect:
                joy1KeySelect = false;
                break;
            case QGamepadManager::ButtonLeft:
                joy1KeyLeft = true;
                break;
            case QGamepadManager::ButtonRight:
                joy1KeyRight = true;
                break;
            case QGamepadManager::ButtonUp:
                joy1KeyUp = true;
                break;
            case QGamepadManager::ButtonDown:
                joy1KeyDown = true;
                break;
        }
}

void Input::gamepadButtonReleased(int id, QGamepadManager::GamepadButton button) {
    switch(button) {
        case QGamepadManager::ButtonA:
            joy1KeyA = false;
            break;
        case QGamepadManager::ButtonB:
        case QGamepadManager::ButtonX:
            joy1KeyB = false;
            break;
        case QGamepadManager::ButtonStart:
            joy1KeyStart = false;
            break;
        case QGamepadManager::ButtonSelect:
            joy1KeySelect = false;
            break;
        case QGamepadManager::ButtonLeft:
            joy1KeyLeft = false;
            break;
        case QGamepadManager::ButtonRight:
            joy1KeyRight = false;
            break;
        case QGamepadManager::ButtonUp:
            joy1KeyUp = false;
            break;
        case QGamepadManager::ButtonDown:
            joy1KeyDown = false;
            break;
    }
}

void Input::joyPadRegisterWrite(uchar newValue, bool isJoyPad2) {
    bool newStrobeValue = (newValue != 0);

    if(!isJoyPad2)
    {
        joy1PreviousStrobeValue = joy1Strobing;
        joy1Strobing = newStrobeValue;

        if(joy1PreviousStrobeValue && !joy1Strobing)
        {
            //Reset joy input reading
            joy1NextInputRead = 0;
        }
    }
    else
    {
        joy2PreviousStrobeValue = joy2Strobing;
        joy2Strobing = newStrobeValue;

        if (joy2PreviousStrobeValue && !joy2Strobing)
        {
            //Reset joy input reading
            joy2NextInputRead = 0;
        }
    }
}

uchar Input::joyPadRegisterRead(bool isJoyPad2) {
    uchar status = 0;

    if (!isJoyPad2)
    {
        if(joy1Strobing)
        {
            //Reset input cycle
            joy1NextInputRead = 0;
        }

        if (joy1Connected)
        {
            switch(joy1NextInputRead)
            {
                case 0:
                {
                    status = joy1KeyA ? 1 : 0;
                    break;
                }
                case 1:
                {
                    status = joy1KeyB ? 1 : 0;
                    break;
                }
                case 2:
                {
                    status = joy1KeySelect ? 1 : 0;
                    break;
                }
                case 3:
                {
                    status = joy1KeyStart ? 1 : 0;
                    break;
                }
                case 4:
                {
                    status = joy1KeyUp ? 1 : 0;
                    break;
                }
                case 5:
                {
                    status = joy1KeyDown ? 1 : 0;
                    break;
                }
                case 6:
                {
                    status = joy1KeyLeft ? 1 : 0;
                    break;
                }
                case 7:
                {
                    status = joy1KeyRight ? 1 : 0;
                    break;
                }
            }

            joy1NextInputRead = (joy1NextInputRead + 1) % 8;

            return (uchar)(status | 0x40);
        }
    }
    else
    {
        if (joy2Strobing)
        {
            //Reset input cycle
            joy2NextInputRead = 0;
        }

        if (joy2Connected)
        {
            switch (joy2NextInputRead)
            {
                case 0:
                {
                    status = joy2KeyA ? 1 : 0;
                    break;
                }
                case 1:
                {
                    status = joy2KeyB ? 1 : 0;
                    break;
                }
                case 2:
                {
                    status = joy2KeySelect ? 1 : 0;
                    break;
                }
                case 3:
                {
                    status = joy2KeyStart ? 1 : 0;
                    break;
                }
                case 4:
                {
                    status = joy2KeyUp ? 1 : 0;
                    break;
                }
                case 5:
                {
                    status = joy2KeyDown ? 1 : 0;
                    break;
                }
                case 6:
                {
                    status = joy2KeyLeft ? 1 : 0;
                    break;
                }
                case 7:
                {
                    status = joy2KeyRight ? 1 : 0;
                    break;
                }
            }

            joy2NextInputRead = (joy2NextInputRead + 1) % 8;

            return (uchar)(status | 0x40);
        }
    }

    return 0x01;
}

void Input::setJoy1KeyInput(int keyCode, bool enabled) {
    switch (keyCode)
    {
        case 0:
        {
            joy1KeyUp = enabled;
            break;
        }
        case 1:
        {
            joy1KeyDown = enabled;
            break;
        }
        case 2:
        {
            joy1KeyLeft = enabled;
            break;
        }
        case 3:
        {
            joy1KeyRight = enabled;
            break;
        }
        case 4:
        {
            joy1KeyStart = enabled;
            break;
        }
        case 5:
        {
            joy1KeySelect = enabled;
            break;
        }
        case 6:
        {
            joy1KeyA = enabled;
            break;
        }
        case 7:
        {
            joy1KeyB = enabled;
            break;
        }
    }
}


Input::~Input() {
    QObject::disconnect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonPressEvent,
                        this, &Input::gamepadButtonPressed);
    QObject::disconnect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent,
                        this, &Input::gamepadButtonReleased);
}

void Input::setKeyInput(int keyCode, bool enabled)
{
    switch (keyCode)
    {
        case 0:
        {
            joy1KeyUp = enabled;
            break;
        }
        case 1:
        {
            joy1KeyDown = enabled;
            break;
        }
        case 2:
        {
            joy1KeyLeft = enabled;
            break;
        }
        case 3:
        {
            joy1KeyRight = enabled;
            break;
        }
        case 4:
        {
            joy1KeyStart = enabled;
            break;
        }
        case 5:
        {
            joy1KeySelect = enabled;
            break;
        }
        case 6:
        {
            joy1KeyA = enabled;
            break;
        }
        case 7:
        {
            joy1KeyB = enabled;
            break;
        }
    }
}

bool Input::eventFilter(QObject *obj, QEvent *event) {
    bool keyPressed = event->type() == QEvent::KeyPress;
    bool keyReleased = event->type() == QEvent::KeyRelease;

    if (keyPressed || keyReleased) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        int key = keyEvent->key();

        if (key == Qt::Key_Z) {
            setKeyInput(6, keyPressed);
        }
        if (key == Qt::Key_X) {
            setKeyInput(7, keyPressed);
        }
        if (key == Qt::Key_Return) {
            setKeyInput(4, keyPressed);
        }
        if (key == Qt::Key_Shift) {
            setKeyInput(5, keyPressed);
        }
        if (key == Qt::Key_Right) {
            setKeyInput(3, keyPressed);
        }
        if (key == Qt::Key_Left) {
            setKeyInput(2, keyPressed);
        }
        if (key == Qt::Key_Up) {
            setKeyInput(0, keyPressed);
        }
        if (key == Qt::Key_Down) {
            setKeyInput(1, keyPressed);
        }
    }

    return QObject::eventFilter(obj, event);
}

