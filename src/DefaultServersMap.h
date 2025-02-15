#include "IO/OOConfig.h"

#include "VProtocol/VPacket.h"
#include "VProtocol/ServerSocket.h"

enum DefaultServerFlag {
    DSF_Deprecated = 1 << 0,
};

struct DefaultServerInfo
{
    std::string name;
    port_t port;
    std::string engine;
    std::string host;
    std::string description;
    char flags = 0;
};

class DefaultServersMap
{
public:
    void load();
    void print();
    
    std::vector<DefaultServerInfo> servers;

private:
};