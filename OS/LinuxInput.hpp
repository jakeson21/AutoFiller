#ifndef LINUX_INPUT_H
#define LINUX_INPUT_H

#include <QProcess>
#include <OS/GenericInput.h>
#include <iostream>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
#include <vector>
#include <map>

namespace OS {

class LinuxInput : public GenericInput
{
public:
    enum InputType_E
    {
        SPECIALKEY = 0,
        TEXTSTRING,
    };

    typedef std::pair<InputType_E, std::string> KeyStrokeEntry;

    LinuxInput() {}
    virtual ~LinuxInput() {}

    virtual bool SwitchToPreviousWindow()
    {
        std::cout << "Set nextWindow to foreground" << std::endl;
        int ret = system("xdotool getactivewindow windowminimize && sleep 0.2");
        return (ret != -1);
    }

    virtual void SendKeys()
    {
        for (size_t n = 0; n < mKeyStrokes.size(); n++)
        {
            if (mKeyStrokes[n].first == SPECIALKEY)
            {
                std::cout << "Sending: " <<mKeyStrokes[n].second << std::endl;
                std::stringstream ss;
                ss << "xdotool getactivewindow key " << mKeyStrokes[n].second;
                system(ss.str().c_str());
            }
            else
            {
                std::cout << "Sending: " <<mKeyStrokes[n].second << std::endl;
                std::stringstream ss;
                ss << "xdotool getactivewindow type " << mKeyStrokes[n].second;
                system(ss.str().c_str());
            }
        }
        mKeyStrokes.clear();
    }

    virtual void AddStringToQueue(const std::string& inString)
    {
        std::stringstream ss;
        ss << "\"" << inString << "\"" ;
        mKeyStrokes.emplace_back(KeyStrokeEntry(TEXTSTRING, ss.str()));
    }

    virtual void AddSpecialKeyToQueue(int inSpecialKey)
    {
        std::string key;
        switch (inSpecialKey)
        {
        case SpecialKeyType_E::BACK:
            key = "BackSpace";
            break;
        case SpecialKeyType_E::TAB:
            key = "Tab";
            break;
        case SpecialKeyType_E::SPACE:
            key = "space";
            break;
        case SpecialKeyType_E::LEFT:
            key = "Left";
            break;
        case SpecialKeyType_E::UP:
            key = "Up";
            break;
        case SpecialKeyType_E::RIGHT:
            key = "Right";
            break;
        case SpecialKeyType_E::DOWN:
            key = "Down";
            break;
        case SpecialKeyType_E::RETURN:
            key = "Return";
            break;
        case SpecialKeyType_E::ALT:
            key = "Alt_L";
            return;
            break;
        case SpecialKeyType_E::CTRL:
            key = "Control_L";
            break;
        default:
            return;
            break;
        }

        mKeyStrokes.emplace_back(KeyStrokeEntry(SPECIALKEY, key));
    }

protected:

    std::vector<std::pair<InputType_E, std::string> > mKeyStrokes;
};

}

#endif // LINUX_INPUT_H
