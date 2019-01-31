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

	// 4. 改变工作目录
	//chdir("/"); 

	// 5. 得到并关闭文件描述符
	struct rlimit rl;
	getrlimit(RLIMIT_NOFILE, &rl);
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for(unsigned int i = 0; i < rl.rlim_max; i++)	
		close(i); 
	
	// 6. 不接受标准输入，输入，错误
	int fd = open("/dev/null", O_RDWR);
	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);
	close(fd);
}

int main()
{
    daemon_init();

    Server *pxServer = Server::GetServer();
    pxServer->init();
    g_xRandModule.LoadFile();

    pxServer->install_channel(new PlayerLstChannel());
    
    pxServer->run();
    delete pxServer;
}
