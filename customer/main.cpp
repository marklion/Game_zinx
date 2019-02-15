#include "zinx.h"
#include "Player/player_channel.h"
#include "Player/player_role.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>
extern RandomName g_xRandModule;

void daemon_init(void) {
 	// 1. 重新设置 umask
	umask(0); 
	
	// 2. 调用 fork 并脱离父进程
	pid_t pid = fork()  ; 
	
	if(pid < 0)
		exit(1); 
	else if(pid > 0)
		exit(0);

	// 3. 重启 session 会话
	setsid();

	// 4. 不接受标准输入，输入，错误
	int fd = open("/dev/null", O_RDWR);
	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);
	close(fd);

    while (true)
    {
        int iPid = fork();
        int status = 1;
        if (iPid > 0)
        {
            wait(&status);
        }
        else if (iPid < 0)
        {
            exit(1);
        }
        else
        {
            break;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        if (0 == strcmp(argv[1], "daemon"))
        {
            daemon_init();
            if ((argc == 3) && (0 == strcmp(argv[2], "log")))
            {
                LOG_SetStdOut("game_std_out.txt");
                LOG_SetStdErr("game_std_err.txt");
            }
        }
    }
    else
    {
        std::cout<<"Usage:"<<argv[0]<<" <daemon [log] | debug>"<<std::endl;
        return 0;
    }

    Server *pxServer = Server::GetServer();
    pxServer->init();
    g_xRandModule.LoadFile();

    pxServer->install_channel(new PlayerLstChannel());
    
    pxServer->run();
    delete pxServer;
}
