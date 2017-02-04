#ifndef WINDOWS_INPUT_H
#define WINDOWS_INPUT_H

#include "Windows.h"
#include "WinUser.h"

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
        HWND nextWindow = GetNextWindow( handle, GW_HWNDNEXT);

        // Move to its true parent handle
        while (true)
        {
            HWND temp = GetParent(nextWindow);
            if (temp == NULL) break;
            nextWindow = temp;
        }
        std::cout << "nextWindow = " << nextWindow << std::endl;

        if( !SetForegroundWindow( nextWindow ) )
        {
            std::cout << "Could not nextWindow to foreground" << std::endl;
            return false;
        }
        std::cout << "Set nextWindow to foreground" << std::endl;
        return true;
    }

    virtual void SendKeys()
    {
        UINT keystrokes_sent;
        for (size_t n = 0; n < mKeyStrokes.size(); n++)
        {
            keystrokes_sent = SendInput((UINT)1, mKeyStrokes[n] , sizeof(INPUT));
        }
    }

    virtual void AddStringToQueue(const std::string& inString)
    {
        UINT i, character_count;

        std::wstring text;
        text.assign(inString.begin(), inString.end());

        //Fill in the array of keystrokes to send.
        character_count = static_cast<UINT>(text.size());

        for( i = 0; i < character_count; ++i )
        {
            INPUT keystrokeDn;
            INPUT keystrokeUp;

            keystrokeDn.type = INPUT_KEYBOARD;
            keystrokeDn.ki.wVk = 0;
            keystrokeDn.ki.wScan = text[ i ];
            keystrokeDn.ki.time = 0;
            keystrokeDn.ki.dwExtraInfo = GetMessageExtraInfo();

            keystrokeUp.type = INPUT_KEYBOARD;
            keystrokeUp.ki.wVk = 0;
            keystrokeUp.ki.wScan = text[ i ];
            keystrokeUpki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
            keystrokeUp.ki.time = 0;
            keystrokeUp.ki.dwExtraInfo = GetMessageExtraInfo();

            mKeyStrokes.push_back(keystrokeDn);
            mKeyStrokes.push_back(keystrokeUp);
        }
    }

    virtual void AddTokenToQueue(TokenType_E inToken)
    {

    }

    virtual void ClearKeys()
    {
        mKeyStrokes.clear();
    }

protected:
    std::vector<INPUT> mKeyStrokes;
};

}

#endif // WINDOWS_INPUT_H
