#ifndef KEYSEQUENCER_HPP
#define KEYSEQUENCER_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>

//#ifdef Q_WS_WIN
#define WINVER 0x0500
#include "Windows.h"
#include "WinUser.h"
//#endif


class KeyCommandInterface
{
public:
    virtual void SendKeys() = 0;
};


class StringKeys : public KeyCommandInterface
{
public:
    StringKeys(const std::wstring& inString)
    : KeyCommandInterface(),
      mStr()
    {
        mStr.assign(inString.begin(), inString.end());
    }

    StringKeys(const std::string& inString)
    : KeyCommandInterface(),
      mStr()
    {
        mStr.assign(inString.begin(), inString.end());
    }

    virtual ~StringKeys() {}

    void SendKeys()
    {
        SendStringToActiveWindow(mStr);
    }


    int SendStringToActiveWindow( const std::wstring& text ) const
    {
        INPUT *keystroke;
        UINT i, character_count, keystrokes_to_send, keystrokes_sent;

        //Fill in the array of keystrokes to send.
        character_count = static_cast<UINT>(text.size());
        keystrokes_to_send = character_count * 2;
        keystroke = new INPUT[ keystrokes_to_send ];
        for( i = 0; i < character_count; ++i )
        {
            keystroke[ i * 2 ].type = INPUT_KEYBOARD;
            keystroke[ i * 2 ].ki.wVk = 0;
            keystroke[ i * 2 ].ki.wScan = text[ i ];
            keystroke[ i * 2 ].ki.dwFlags = KEYEVENTF_UNICODE;
            keystroke[ i * 2 ].ki.time = 0;
            keystroke[ i * 2 ].ki.dwExtraInfo = GetMessageExtraInfo();

            keystroke[ i * 2 + 1 ].type = INPUT_KEYBOARD;
            keystroke[ i * 2 + 1 ].ki.wVk = 0;
            keystroke[ i * 2 + 1 ].ki.wScan = text[ i ];
            keystroke[ i * 2 + 1 ].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
            keystroke[ i * 2 + 1 ].ki.time = 0;
            keystroke[ i * 2 + 1 ].ki.dwExtraInfo = GetMessageExtraInfo();
        }

        //Send the keystrokes.
        std::wcout << "SendInput(" << text << ")" << std::endl;

        keystrokes_sent = SendInput( ( UINT )keystrokes_to_send, keystroke, sizeof( *keystroke ) );
        delete [] keystroke;

        return keystrokes_sent == keystrokes_to_send;
    }

protected:
    std::wstring mStr;

};


class TokenKeys : public KeyCommandInterface
{
public:
    enum TokenType_E {
        UNASSIGNED = -1,
        BACK = VK_BACK,
        TAB = VK_TAB,
        SPACE = VK_SPACE,
        LEFT = VK_LEFT,
        UP = VK_UP,
        RIGHT = VK_RIGHT,
        DOWN = VK_DOWN,
        RETURN = VK_RETURN,
        ALTTAB = 0x100,
    };


    TokenKeys(const TokenType_E& inToken)
    : KeyCommandInterface(),
      mToken(inToken)
    {
        initialize();
    }

    TokenKeys(const std::string& inToken)
    : KeyCommandInterface(),
      mToken()
    {
        initialize();

        mToken = TokenType_E::UNASSIGNED;
        for(std::map<int, std::string>::iterator it = mStrMap.begin(); it != mStrMap.end(); ++it) {
              if (inToken.compare(it->second) == 0)
              {
                  mToken = static_cast<TokenType_E>(it->first);
                  break;
              }              
        }
        if (mToken == TokenType_E::UNASSIGNED)
            throw std::invalid_argument("Unrecognized Enum");
    }

    void initialize()
    {
        mStrMap[BACK]   = "BACK";
        mStrMap[TAB]    = "TAB";
        mStrMap[SPACE]  = "SPACE";
        mStrMap[LEFT]   = "LEFT";
        mStrMap[UP]     = "UP";
        mStrMap[RIGHT]  = "RIGHT";
        mStrMap[DOWN]   = "DOWN";
        mStrMap[RETURN] = "RETURN";
        mStrMap[ALTTAB] = "ALTTAB";
    }

    virtual ~TokenKeys() {}

    void SendKeys()
    {
        switch (mToken)
        {
        case TokenType_E::BACK:
        case TokenType_E::TAB:
        case TokenType_E::SPACE:
        case TokenType_E::LEFT:
        case TokenType_E::UP:
        case TokenType_E::RIGHT:
        case TokenType_E::DOWN:
        case TokenType_E::RETURN:
            SimulateKeyPress(static_cast<char>(mToken));
            break;
        case TokenType_E::ALTTAB:
            SimulateKeyPress(static_cast<char>(mToken), true);
            break;
        default:
            break;
        }

        try
        {
            std::cout << "Token " << mStrMap.at(static_cast<int>(mToken)) << std::endl;
        }
        catch (std::invalid_argument)
        {
            std::cout << "INVALID Token: " << mToken << std::endl;
        }
    }

    void SimulateKeyPress(const char& key, bool bAlt = false, bool bShift = false,
                          bool bCtrl = false, bool bWin = false, bool bRealKeyCode = false)
    {
        int intReturn = -1;

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
            intReturn = SendInput(1, &input, sizeof(INPUT));

            if (intReturn != 1)
                throw ("Special key down failed");
        }

        input.ki.dwFlags = 0;

        // Key down the actual key-code
        if (!bRealKeyCode)
            input.ki.wVk = VkKeyScan(key);
        else
            input.ki.wVk = key;

        intReturn = SendInput(1, &input, sizeof(INPUT));

        if (intReturn != 1)
            throw std::exception("Key down failed");

        // Key up the actual key-code
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        intReturn = SendInput(1, &input, sizeof(INPUT));

        if (intReturn != 1)
            throw std::exception("Key up failed");

        // Key up shift, ctrl, and/or alt
        if (bShift)
        {
            // This makes no sense, but KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP
            // releases the right key, and KEYEVENTF_KEYUP releases the
            // left key.
            input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP;
            input.ki.wVk = VK_SHIFT;
            SendInput(1, &input, sizeof(INPUT));
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
            intReturn = SendInput(1, &input, sizeof(INPUT));
            if (intReturn != 1)
                throw std::exception("Special key up failed");
        }

        input.ki.dwFlags = 0;
    }


    std::string toString() const
    {
        return mStrMap.at(mToken);
    }

    int toInt() const
    {
        return static_cast<int>(mToken);
    }

    std::map<int, std::string> mStrMap;
    TokenType_E mToken;
};

#endif // KEYSEQUENCER_HPP
