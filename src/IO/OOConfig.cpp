#include "OOConfig.h"

#include <iostream>

bool OOConfig::Object::findValue(const std::string &vname, Value &writeInto)
{
    for(Value& val : values){
        if(val.name == vname){
            writeInto = val;
            return 1;
        }
    }

    return 0;
}

bool OOConfig::Object::hasFlag(const std::string &flagName)
{
    for(const std::string& flag : flags){
        if(flag == flagName) return 1;
    }
    return 0;
}

std::string OOConfig::Object::getStringVal(const std::string &vname, std::string elseReturn)
{
    std::string result = elseReturn;
    Value value;
    if(findValue(vname, value)) result = value.rawvalue;
    return result;
}

int OOConfig::Object::getIntVal(const std::string &vname, int elseReturn)
{
    int result = elseReturn;
    Value value;
    if(findValue(vname, value)) result = atoi(value.rawvalue.data());
    return result;
}

float OOConfig::Object::getFloatVal(const std::string &vname, float elseReturn)
{
    float result = elseReturn;
    Value value;
    if(findValue(vname, value)) result = atof(value.rawvalue.data());
    return result;
}

fresult OOConfig::load(const std::string &fname)
{
    fFile inputFile(fFile::AS_INPUT);

    fresult result = inputFile.Open(fname);

    if(result != fFile::CODE_SUCCESS){
        return result;
    }

    readStream.myData = inputFile.fileData;

    return result;
}

void OOConfig::parse()
{

    for(;;)
    {
        if(!readStream.tryReadWord(rtToken)) break;

        // value after one-char prefix
        if(rtToken.size())
            rtTokenValue = rtToken.substr(1, rtToken.size()-1);

        processToken();
    }
}

bool OOConfig::findObject(const std::string &objname, Object &writeInto)
{
    for(Object& obj : runtimeObjects){
        if(obj.name == objname){
            writeInto = obj;
            return 1;
        }
    }
    return 0;
}

std::ostream &OOConfig::syntaxisErr()
{
    // TODO: syntaxis error should give information about position like "X;Y" / "Line;Char" 
    return std::cout<<"["<<readStream.currentPos<<"]OOConfig Syntaxis error: ";
}

void OOConfig::processToken()
{
    switch(rtToken[0])
    {
    default:
        processObjVarToken();
        break;

    case '.':
        processObjFlagToken();
        break;

    case '@':
        processNewObjToken();
        break;

    case '*':
        processSysToken();
        break;
    }
}

void OOConfig::processObjVarToken()
{
    if(!rtObject){
        syntaxisErr() << "Attempt to create variable out of object field\n";
        return;
    }

    rtObject->values.emplace_back();
    
    // erase ':' at the end
    rtObject->values.back().name = rtToken.substr(0, rtToken.size()-1);
    if(!readStream.tryReadAny(rtObject->values.back().rawvalue)){
        syntaxisErr() << "Created variable has no any value\n";
        return;
    }
}

void OOConfig::processObjFlagToken()
{
    if(!rtObject){
        syntaxisErr() << "Attempt to create flag out of object field\n";
        return;
    }

    rtObject->flags.push_back(rtTokenValue);
}

void OOConfig::processNewObjToken()
{
    if(rtTokenValue[0] == '$'){
        syntaxisErr() << "Couldnt create object: Prefix '$' is used by system.\n";
        return;
    }

    rtObject = &runtimeObjects.emplace_back();
    rtObject->name = rtTokenValue;
}

void OOConfig::processSysToken()
{
    if(rtTokenValue == "outputArray:"){
        for(;;){
            if(!readStream.tryReadAny(rtToken)) return;

            if(rtToken == "$none") return;

            if(!findObject(rtToken, outputArray.emplace_back())){
                syntaxisErr() << "Couldnt add object into output array: No such object with name '"<<rtToken<<"'\n";
                outputArray.pop_back();
            }

            if(!readStream.tryReadAny(rtToken)) return;

            if(rtToken != ","){
                readStream.currentPos -= rtToken.size();
                return;
            }
        }
        return;
    }

    syntaxisErr() << "Unknown system value '"<<rtTokenValue<<"'\n";
}
