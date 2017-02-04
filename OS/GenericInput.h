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
        mTokenStrMap[ALTTAB]        = "ALTTAB";
        mTokenStrMap[CTRL]          = "CTRL";
    }

    std::string tokenToString(TokenType_E inToken) const
    {
        auto it = mTokenStrMap.find(inToken);
        if (it != mTokenStrMap.end())
            return mTokenStrMap.at(inToken);
        else
            return mTokenStrMap.at(UNASSIGNED);
    }

    int tokenToInt(const std::string& inToken)
    {
        int mToken = TokenType_E::UNASSIGNED;
        for(std::map<int, std::string>::iterator it = mTokenStrMap.begin(); it != mTokenStrMap.end(); ++it) {
              if (inToken.compare(it->second) == 0)
              {
                  mToken = static_cast<TokenType_E>(it->first);
                  break;
              }
        }
        if (mToken == TokenType_E::UNASSIGNED)
            throw std::invalid_argument("Unrecognized Enum");

        return static_cast<int>(mToken);
    }

    virtual bool SwitchToPreviousWindow() = 0;
    virtual void SendKeys() = 0;
    virtual void AddStringToQueue(const std::string& inString) = 0;
    virtual void AddTokenToQueue(int inToken) = 0;

protected:
    std::map<int, std::string> mTokenStrMap;
};

}

#endif // GENERIC_INPUT_H
