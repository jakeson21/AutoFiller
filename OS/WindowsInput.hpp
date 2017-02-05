#ifndef WINDOWS_INPUT_H
#define WINDOWS_INPUT_H

#include "Windows.h"
#include "WinUser.h"
#include <OS/GenericInput.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace OS {

class WindowsInput : public GenericInput
{
public:
    WindowsInput()
    : mKeyStrokes()
    {}

    virtual ~WindowsInput() {}

    virtual bool SwitchToPreviousWindow()
    {
        HWND handle = GetForegroundWindow(); // Handle of this window
        std::cout << "Foreground window = " << handle << std::endl;

        // Get handle of last active window: the window we want to send keystrokes to
        HWND nextWindow = GetNextWindow(handle, GW_HWNDNEXT);

        // Move to its true parent handle
        while (true)
        {
            HWND temp = GetParent(nextWindow);
            if (temp == NULL) break;
            nextWindow = temp;
        }
        std::cout << "nextWindow = " << nextWindow << std::endl;

        if( !SetForegroundWindow(nextWindow) )
        {
            std::cout << "Could not nextWindow to foreground" << std::endl;
            return false;
        }
        std::cout << "Set nextWindow to foreground" << std::endl;
        return true;
    }

    virtual void SendKeys()
    {        
        for (size_t n = 0; n < mKeyStrokes.size(); n++)
        {
            int intReturn = -1;
            intReturn = SendInput(1, &mKeyStrokes[n], sizeof(INPUT));
            if (intReturn != 1)
                throw ("Key failed to send");
        }
        mKeyStrokes.clear();
    }

    virtual void AddStringToQueue(const std::string& inString)
    {
        INPUT keystroke;
        UINT i, character_count;

        std::wstring text;
        text.assign(inString.begin(), inString.end());

        //Fill in the array of keystrokes to send.
        character_count = static_cast<UINT>(text.size());

        std::wcout << "Sending: " << text << std::endl;

        for( i = 0; i < character_count; ++i )
        {
            keystroke.type = INPUT_KEYBOARD;
            keystroke.ki.wVk = 0;
            keystroke.ki.wScan = text[ i ];
            keystroke.ki.dwFlags = KEYEVENTF_UNICODE;
            keystroke.ki.time = 0;
            keystroke.ki.dwExtraInfo = GetMessageExtraInfo();

            mKeyStrokes.emplace_back(keystroke);

            keystroke.type = INPUT_KEYBOARD;
            keystroke.ki.wVk = 0;
            keystroke.ki.wScan = text[ i ];
            keystroke.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
            keystroke.ki.time = 0;
            keystroke.ki.dwExtraInfo = GetMessageExtraInfo();

            mKeyStrokes.emplace_back(keystroke);
        }
    }

    virtual void AddSpecialKeyToQueue(int inSpecialKey)
    {
        int key = 0;
        switch (inSpecialKey)
        {
        case SpecialKeyType_E::BACK:
            key = VK_BACK;
            break;
        case SpecialKeyType_E::TAB:
            key = VK_TAB;
            break;
        case SpecialKeyType_E::SPACE:
            key = VK_SPACE;
            break;
        case SpecialKeyType_E::LEFT:
            key = VK_LEFT;
            break;
        case SpecialKeyType_E::UP:
            key = VK_UP;
            break;
        case SpecialKeyType_E::RIGHT:
            key = VK_RIGHT;
            break;
        case SpecialKeyType_E::DOWN:
            key = VK_DOWN;
            break;
        case SpecialKeyType_E::RETURN:
            key = VK_RETURN;
            break;
        case SpecialKeyType_E::ALT:
            key = VK_MENU;
            SimulateKeyPress(static_cast<char>(key), true);
            return;
            break;
        case SpecialKeyType_E::ALTTAB:
            key = VK_TAB;
            SimulateKeyPress(static_cast<char>(key), true);
            return;
            break;
        case SpecialKeyType_E::CTRL:
            key = VK_CONTROL;
            SimulateKeyPress(static_cast<char>(key), false, false, true);
            return;
            break;
        default:
            return;
            break;
        }

        SimulateKeyPress(static_cast<char>(key));
    }

    //  Original implementation borrowed from:
    //  https://sites.google.com/site/jozsefbekes/Home/windows-programming/keyboard-and-mouse-simulations
    //
    void SimulateKeyPress(const char& key, bool bAlt = false, bool bShift = false,
                          bool bCtrl = false, bool bWin = false, bool bRealKeyCode = false)
    {
        INPUT input;
        input.type = INPUT_KEYBOARD;

        // Key down shift, ctrl, and/or alt
        input.ki.wScan = KEYEVENTF_SCANCODE;
        input.ki.time = 0;
        input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
        input.ki.wVk = 0;
        input.ki.dwExtraInfo = GetMessageExtraInfo();

        if (bShift)
        {
            input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
            input.ki.wVk = VK_SHIFT;
        }
        if (bCtrl)
            input.ki.wVk = VK_CONTROL;
        if (bAlt)
            input.ki.wVk = VK_MENU;
        if (bWin)
            input.ki.wVk = VK_LWIN;

        if (input.ki.wVk)
        {
            mKeyStrokes.emplace_back(input);
        }

        input.ki.dwFlags = 0;

        // Key down the actual key-code
        if (!bRealKeyCode)
            input.ki.wVk = VkKeyScan(key);
        else
            input.ki.wVk = key;

        mKeyStrokes.emplace_back(input);

        // Key up the actual key-code
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        mKeyStrokes.emplace_back(input);

        // Key up shift, ctrl, and/or alt
        if (bShift)
        {
            // This makes no sense, but KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP
            // releases the right key, and KEYEVENTF_KEYUP releases the
            // left key.
            input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP;
            input.ki.wVk = VK_SHIFT;
            mKeyStrokes.emplace_back(input);
            input.ki.dwFlags = KEYEVENTF_KEYUP;
        }
        if (bCtrl)
            input.ki.wVk = VK_CONTROL;
        if (bAlt)
            input.ki.wVk = VK_MENU;
        if (bWin)
            input.ki.wVk = VK_LWIN;

        if (input.ki.wVk)
        {
            mKeyStrokes.emplace_back(input);
        }
    }

protected:
    std::vector<INPUT> mKeyStrokes;
};

}

#endif // WINDOWS_INPUT_H
