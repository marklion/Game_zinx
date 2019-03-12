#ifndef _AMESSAGE_H_
#define _AMESSAGE_H_
#include "include.h"

class Arole;

/*
这是一个抽象类，负责封装业务相关的数据。
开发者应该继承该类实现自定义的各种消息类型。
*/
class Amessage{
public:
    Amessage();
    virtual ~Amessage();
};

/*
这两个结构体的布局完全相同只是成员变量名称不同。
用来将Amessage对象和处理它的Arole或发送它的Arole封装起来。
开发者只需按照C语言的风格使用这个结构体，不要去无谓的继承该结构体
*/
struct Request{
    Amessage *pxMsg = NULL;
    Arole *pxProcessor = NULL;
    ~Request();
};
struct Response{
    Amessage *pxMsg = NULL;
    Arole *pxSender = NULL;
    ~Response();
};

/*
该类继承自Amessage类，用于封装有固定序号的消息。
构造时必须通过参数_id指定消息序号。
若开发者需要使用数值作为消息类型或ID时，可以继承该类并添加业务相关的变量
*/
class IdMessage:public Amessage{
public:
    explicit IdMessage(int _id);  
    int Id;
    virtual ~IdMessage()
    {
    }
};


#endif
