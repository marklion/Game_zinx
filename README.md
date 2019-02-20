# Game_zinx
这个软件包含一个服务器框架和一个在线游戏的demo

## 目录布局
zinx目录是框架本身

customer目录用来放置业务实现，当前放置了一个游戏服务器的demo

## 依赖
1. libprotoc 3.6.1
2. cmake version 3.13.2

## 编译
```shell
mkdir build
cd build
cmake ../
make
make install
```

## 框架简介
zinx框架是用来处理通用IO，协议和事件的。
框架包含以下类：
+ server类
> 框架包含一个server类的单例对象，所有IO和数据处理的业务都应该依附于该对象。
+ Amessage类
> 这是一个抽象类，负责封装业务相关的数据。***开发者应该继承该类实现自定义的各种消息类型。***
+ Achannel类
> 这是一个抽象类，负责处理文件层面的数据IO。其中大部分方法都是基于linux的系统调用实现。类实例应该按需安装(install)或卸载(uninstall)。***开发者应该继承该类并重写部分方法，实现原始数据收发功能***
+ Aprotocol类
> 这是一个抽象类，负责将Achannel对象收发的原始数据和Amessage对象之间进行转换。每个Achannel对象可以绑定最多一(0~1)个Aprotocol对象。***开发者应该继承该类并重写部分方法，实现通信协议功能***
+ Arole类
> 这是一个抽象类，可以绑定最多一个Achannel对象作为输出通道。该类负责处理收到的各类Amessage对象，或产生并发送Amessage对象。***开发者应该核心的业务处理在该类的派生类中实现***
+ Request结构和Response结构
> 这两个结构体的布局完全相同只是成员变量名称不同。用来将Amessage对象和处理它的Arole或发送它的Arole封装起来。***开发者只需按照C语言的风格使用这个结构体，不要去无谓的继承该结构体***

框架还提供以下实用类帮助开发者快速开发
+ TcpListenChannel类
> 该类继承自Achannel类，用于处理TCP监听。构造对象时需要携带参数usPort作为监听端口号，将该类的实例添加到server中后，若有tcp连接到来则成员函数TcpAfterConnection会被调用。***开发者应该重写TcpAfterConnection方法以实现所需功能***
+ TcpDataChannel类
> 该类继承自Achannel类，用于处理TCP数据报文。构造对象时必须携带参数_iDataFd作为TCP数据socket。该类实例应该在客户端连接到来后构造，然后添加到server中，若socket收到数据，则成员函数TcpProcDataIn会被调用；若socket发生异常（如对端关闭）则成员函数TcpProcHup会被调用，然后该实例会自动从server中摘除并析构掉。***不建议开发人员重写该类的虚函数，一般建议将数据层面的处理逻辑放到该类绑定的Aprotocol对象中处理。***
+ IdMessage类
> 该类继承自Amessage类，用于封装有固定序号的消息。构造时必须通过参数_id指定消息序号。***若开发者需要使用数值作为消息类型或ID时，可以继承该类并添加业务相关的变量***
+ IIdMsgProc类
> 该类是纯虚类，包含一个纯虚函数ProcMsg用于处理IdMessage。***若开发者需要使用IdMessage类或其派生类作为消息封装，则建议继承该类并在ProcMsg中实现具体功能***
+ IdMsgRole类
> 该类继承自Arole类，用于处理IdMessage类型的消息。将其实例化后需调用register_id_func函数将消息类型和对应的消息处理对象（IIdMsgProc对象）进行注册。该类构造的对象添加到server中后，若有IdMessage被指定由该对象处理，则之前注册的相应的IIdMsgProc会被调用处理该消息。***开发者的纯业务处理应该由该类的派生类实现，并合理地注册多个IIdMsgProc对业务请求分类处理***

## 常用API
开发者要想正常使用框架，那么以下列出的API是几乎是最常用到的。开发者需要重写或调用这些函数。

### 1. 需要被调用的API

`Server *Server::GetServer()`
+ 描述：该函数用于获取server实例。因为server类被设计为单例模式，所以能且只能通过该函数获取唯一server实例。
+ 返回值：server对象；若失败则返回空。

`bool Server::init()`
+ 描述：该函数用于初始化server实例，应该在第一次获取server实例后调用。
+ 返回值：true->成功，false->失败

`bool Server::install_channel(Achannel * pxChannel);`
+ 描述:该函数用于将Achannel及其派生类对象安装到server实例中。
+ 参数：pxChannel是待添加的通道对象，必须是堆对象。
+ 返回值：true->成功，false->失败

`void Server::uninstall_channel(Achannel * pxChannel);`
+ 描述：该函数用于摘除server中的Arole对象。**摘除后pxChannel对象并没有被释放。**
+ 参数：pxChannel是待摘除对象。

`bool Server::add_role(std::string szCharacter, Arole * pxRole);`
+ 描述：该函数用于将pxRole对象添加到server实例中。
+ 参数：
  - szCharacter用于指定pxRole对象的特征或分类。通常可以设置为类名，被指定的特征相同的pxRole会在server内被组成一张线性表。
  - pxRole表示待添加的角色对象，必须是堆对象。
+ 返回值：true->成功，false->失败

`void Server::del_role(std::string szCharacter, Arole * pxRole);`
+ 描述：该函数用于摘除server对象中的Arole对象。**摘除后pxRole对象并没有被释放。**
+ 参数：
  - szCharacter表示pxRole对象的特征或分类。
  - pxRole表示待摘除对象。
  
`list<Arole*>* Server::GetRoleListByCharacter(std::string szCharacter)`
+ 描述：该函数用于获取特征为szCharacter的所有Arole对象。Arole对象被组织到list中，**这个list不应该被修改内容和或释放。**
+ 参数：szCharacter表示pxRole对象的特征或分类。
+ 返回值：包含所有符合条件的Arole对象；没找到则为空。

`bool Server::run();`
+ 描述：该函数是框架的运行入口，调用该函数后程序会进入循环阻塞等待之前添加的Achannel对象产生相应的IO并作出处理。该函数不会返回除非server实例未初始化或在运行时成员函数ShutDownMainLoop被调用。
+ 返回值：true->调用ShutDownMainLoop退出，false->server实例未初始化成功。

`bool Server::send_resp(Response * pstResp)`
+ 描述：当运行时需要向外发送请求时，应该调用该函数。不建议直接调用Achannel对象的发送方法。
+ 参数：pstResp中必须包含待发送的Amessage对象和发送该消息的Arole对象。若该对象Arole没有绑定出口Achannel，则该函数什么都不会做。
+ 返回值：true->成功，false->失败。

`void Server::ShutDownMainLoop()`
+ 描述： 该函数用于停止框架主循环。可以在server实例run起来之后任意时间调用。

`bool IdMsgRole::register_id_func(int _id, IIdMsgProc *Iproc)`
+ 描述：注册消息类型和其对应的处理接口。该函数应该在IdMsgRole对象创建后调用。
+ 参数: 
  - \_id是消息的类型编号
  - Iproc是处理消息的派生对象。
+ 返回值：true->成功，false->失败。

### 2. 需要被重写的API

`bool Aprotocol::raw2request(const RawData *pstData, std::list<Request *> &_ReqList);`
+ 描述：该函数会在绑定的Achannel对象收到数据后调用，用来把原始数据pstData转换成若干Request对象组成的list。开发者需要在派生类重写该函数将报文解析，封包，校验等功能实现。
+ 参数：
  - pstData是待处理的原始数据。其结构包含unsigned char \*pucData和int iLength两个核心变量分别存储数据缓冲区指针和数据长度。
  - \_ReqList是传出参数，用来存放转换后的若干Request对象。开发者需要将堆对象添加到表中，当数据处理完成后框架会自动释放堆内存。
+ 返回值：若正常产生了Request对象则返回true；若由于数据不全等因素导致转换失败则返回false。

`bool Aprotocol::response2raw(const Response * pstResp, RawData * pstData); `
+ 描述：该函数会在Achannel对象的发送函数被调用之前调用。在该函数内需要开发者实现请求消息的序列化，从而方便后续数据发送。
+ 参数：
  - pstResp包含待发送消息Amessage和发送者Arole。
  - pstData是传出参数，用来存放序列化完成的数据。
+ 返回值：若正常序列化则返回true，否则返回false。

`bool TcpListenChannel::TcpAfterConnection(int _iDataFd, struct sockaddr_in *pstClientAddr)`
+ 描述：该函数会在新的TCP连接建立后被调用。一般情况下，需要开发者重写该函数，在函数内创建TcpDataChannel对象并添加到server实例中。
+ 参数：
  - \_iDataFd是新建立的数据socket。
  - pstClientAddr是客户端的地址结构封装。
+ 返回值：一般返回true。若返回false，则该TCP监听端口会被从server实例中摘除并关闭。

`bool TcpDataChannel::TcpProcDataIn(RawData * pstData)`
+ 描述：该函数用于读取tcp客户端发来的数据。未重写之前的行为是将原始数据存到pstData中
+ 参数：pstData用于存储读取到的数据
+ 返回值：若正常读取到数据则返回true，否则返回false。返回false后TcpDataChannel对象将会被摘除并析构。

`bool IIdMsgProc::ProcMsg(IdMsgRole *_pxRole, IdMessage *_pxMsg);`
+ 描述：该函数会在IdMsgRole对象处理消息的过程中被调用。开发者需要重写该函数，按照实际的业务需求处理各种消息。
+ 参数：
  - \_pxRole是当前正在处理消息的对象。
  - \_pxMsg是消息本身
+ 返回值：业务流程正常执行则返回true。遇到意料之外的系统错误则返回false。

## 举例
### 1. 以下代码实现了监听TCP并将客户端发来的信息回传给该客户端
```c++
#include "zinx/zinx.h" //框架核心头文件

//定义SampleTcpData类用于处理tcp客户端发来的数据
class SampleTcpData:public TcpDataChannel{
public:
    //构造函数需要制定数据socket，调用父类构造函数，指定绑定的Aprotocol对象为NULL
    SampleTcpData(int _iDataFd):TcpDataChannel(_iDataFd, NULL)
    {
    }
    
    //重写TcpProcDataIn函数，添加回传功能
    virtual bool TcpProcDataIn(RawData * pstData)
    {
        //先调用父类方法读取到数据
        if (true == TcpDataChannel::TcpProcDataIn(pstData))
        {
            //将读取到的数据写回去
            return writeFd(pstData);
        }
    
        return false;
    }
};

//定义SampleTcpLst处理tcp连接
class SampleTcpLst:public TcpListenChannel{
public:
    //调用父类构造函数指定监听端口为6789
    SampleTcpLst():TcpListenChannel(6789)
    {
    }
    
    //重写TcpAfterConnection
    virtual bool TcpAfterConnection(int _iDataFd, struct sockaddr_in * pstClientAddr)
    {
        //创建SampleTcpData对象并添加到server实例中
        return Server::GetServer()->install_channel(new SampleTcpData(_iDataFd));
    }
};

int main()
{
    //获取server单例并将其初始化
    Server *pxServer = Server::GetServer();
    pxServer->init();

    //安装SampleTcpLst对象用于监听tcp，并运行server实例
    pxServer->install_channel(new SampleTcpLst());
    pxServer->run();

    return 0;
}

```
