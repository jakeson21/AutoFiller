#ifndef GENERIC_INPUT_H
#define GENERIC_INPUT_H

#include <string>
#include <map>
#include <algorithm>    // std::find_if

namespace OS {

enum TokenType_E {
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
        mTokenStrMap[UNASSIGNED]    = "UNASSIGNED";
        mTokenStrMap[BACK]          = "BACK";
        mTokenStrMap[TAB]           = "TAB";
        mTokenStrMap[SPACE]         = "SPACE";
        mTokenStrMap[LEFT]          = "LEFT";
        mTokenStrMap[UP]            = "UP";
        mTokenStrMap[RIGHT]         = "RIGHT";
        mTokenStrMap[DOWN]          = "DOWN";
        mTokenStrMap[RETURN]        = "RETURN";
        mTokenStrMap[ALT]           = "TAB";
        mTokenStrMap[CTRL]          = "CTRL";
    }

    std::string tokenToString(TokenType_E inToken) const
    {
        auto it = mTokenStrMap.find(inToken);
        if (it != my_map.end())
            return mTokenStrMap.at(inToken);
        else
            return mTokenStrMap[UNASSIGNED];
    }

    int tokenToInt(const std::string& inToken) const
    {
        return static_cast<int>(mToken);
    }

    virtual bool SwitchToPreviousWindow() = 0;
    virtual void SendKeys() = 0;
    virtual void AddStringToQueue(const std::string& inString) = 0;
    virtual void AddTokenToQueue(TokenType_E inToken) = 0;
    virtual void ClearKeys() = 0;

protected:
    std::map<TokenType_E, std::string> mTokenStrMap;
};

}

#endif // GENERIC_INPUT_H
