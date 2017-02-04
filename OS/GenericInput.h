#ifndef GENERIC_INPUT_H
#define GENERIC_INPUT_H

#include <string>
#include <map>
#include <algorithm>    // std::find_if

namespace OS {

enum SpecialKeyType_E {
    UNASSIGNED  = -1,
    BACK        = 0,
    TAB,
    SPACE,
    LEFT,
    UP,
    RIGHT,
    DOWN,
    RETURN,
    ALT,
    ALTTAB,
    CTRL,
};

// Abstract class
class GenericInput
{
public:
    GenericInput()
    {
        initialize();
    }

    virtual ~GenericInput() {}

    virtual void initialize()
    {
        mSpecialKeyStrMap[UNASSIGNED]    = "UNASSIGNED";
        mSpecialKeyStrMap[BACK]          = "BACK";
        mSpecialKeyStrMap[TAB]           = "TAB";
        mSpecialKeyStrMap[SPACE]         = "SPACE";
        mSpecialKeyStrMap[LEFT]          = "LEFT";
        mSpecialKeyStrMap[UP]            = "UP";
        mSpecialKeyStrMap[RIGHT]         = "RIGHT";
        mSpecialKeyStrMap[DOWN]          = "DOWN";
        mSpecialKeyStrMap[RETURN]        = "RETURN";
        mSpecialKeyStrMap[ALT]           = "TAB";
        mSpecialKeyStrMap[ALTTAB]        = "ALTTAB";
        mSpecialKeyStrMap[CTRL]          = "CTRL";
    }

    std::string specialKeyToString(SpecialKeyType_E inSpecialKey) const
    {
        auto it = mSpecialKeyStrMap.find(inSpecialKey);
        if (it != mSpecialKeyStrMap.end())
            return mSpecialKeyStrMap.at(inSpecialKey);
        else
            return mSpecialKeyStrMap.at(UNASSIGNED);
    }

    int specialKeyToInt(const std::string& inSpecialKey)
    {
        int mSpecialKey = SpecialKeyType_E::UNASSIGNED;
        for(std::map<int, std::string>::iterator it = mSpecialKeyStrMap.begin(); it != mSpecialKeyStrMap.end(); ++it) {
              if (inSpecialKey.compare(it->second) == 0)
              {
                  mSpecialKey = static_cast<SpecialKeyType_E>(it->first);
                  break;
              }
        }
        if (mSpecialKey == SpecialKeyType_E::UNASSIGNED)
            throw std::invalid_argument("Unrecognized Enum");

        return static_cast<int>(mSpecialKey);
    }

    virtual bool SwitchToPreviousWindow() = 0;
    virtual void SendKeys() = 0;
    virtual void AddStringToQueue(const std::string& inString) = 0;
    virtual void AddSpecialKeyToQueue(int inSpecialKey) = 0;

protected:
    std::map<int, std::string> mSpecialKeyStrMap;
};

}

#endif // GENERIC_INPUT_H
