#include <vector>

#include "../Tools/fStringStream.h"
#include "fFile.h"

// Object-Oriented Config parser.
class OOConfig
{
public:

    struct Object
    {
        struct Value{
            std::string name;
            std::string rawvalue;
        };

        std::string name;
        std::vector<Value> values;
        std::vector<std::string> flags;

        bool findValue(const std::string& vname, Value& writeInto);
        bool hasFlag(const std::string& flagName);

        std::string getStringVal(const std::string& vname, std::string elseReturn = "");
        int getIntVal(const std::string& vname, int elseReturn = 0);
        float getFloatVal(const std::string& vname, float elseReturn = 0.0f);
    };

    fresult load(const std::string& fname);
    void parse();
    bool findObject(const std::string& objname, Object& writeInto);

    std::vector<Object> outputArray;

private:

    // runtime variables
    std::string rtToken;
    std::string rtTokenValue;
    Object* rtObject = 0;


    std::vector<Object> runtimeObjects;

    fStringStream readStream;


    std::ostream& syntaxisErr();


    void processToken();

    void processObjVarToken();
    void processObjFlagToken();
    void processNewObjToken();
    void processSysToken();

};