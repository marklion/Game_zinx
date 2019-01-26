#include "zinx.h"
#include "Player/player_channel.h"

class StdinChannel:public Achannel{
public:
    StdinChannel():Achannel(NULL)
    {
        m_fd = 0;
    }
    virtual bool init() override
    {
        return true;
    }
    virtual void fini() override{}
    virtual bool readFd(uint32_t _event, RawData * pstData) override
    {
        long Temp;
        
        read(m_fd, &Temp, sizeof(Temp));
    
        Server::GetServer()->ShutDownMainLoop();

        return true;
    }
    virtual bool writeFd(const RawData * pstData) override
    {
        return false;            
    }
};

int main()
{
    Server *pxServer = Server::GetServer();
    pxServer->init();

    pxServer->install_channel(new PlayerLstChannel());
	pxServer->install_channel(new StdinChannel());
    
    pxServer->run();
    delete pxServer;
}
