### AT框架说明

为了让at框架和硬件做解耦，特优化或者说是更新以前的版本，让AT框架的本质回归到AT。

#### 安装配置

在使用at框架之前，需要系统的配置文件配置LOSCFG_ENABLE_AT。

```
如在target_config.h中配置
#define LOSCFG_ENABLE_AT     1
```

然后可以安装AT框架了。

```
bool_t los_at_init(const char *devname);           //install the at module
```

为了保证AT框架和具体的硬件驱动解耦，此处使用的注册的设备驱动，开发者配置驱动设备的名字额时候，应该保证该驱动已经注册成功。同时应该注意到，at设备接口的特点：开发者需要在意的是读取的一定是一帧AT返回数据，如果使用串口，可以用串口的IDLE中断来区别不同的帧；其他的设备接口也会有自己的相应说明该帧的区分形式（一般是几个字节的时间没有数据到达），当调用该函数之后，我们就可以使用at的接口发送AT命令了，同时我们提供了shell端口的atcmd命令用作调试（如果shell已经被配置的话）；

####  AT命令使用

```
s32_t  at_command(u8_t *cmd, s32_t cmdlen,const char *index,u8_t *respbuf,s32_t respbuflen,u32_t timeout); //send at command and receive response
cmd：需要发送的命令，其中cmdlen表示的cmd的长度，cmd可以允许不是string；不允许cmd为NULL
cmdlen:cmd的长度
index:期待返回的数据中包含的字符串，比方期待返回“ABC”，如果返回“12ABCXX”，则会命中，允许为空，如果为空则发送之后直接返回
respbuf:如果返回的数据命中，则将其存储到该buf中，允许为空
respbuflen:提供的respbuf的长度
timeout:等待返回时间；当命令发送出去之后，如果有期望返回（index不为空），则会阻塞timeout时间直道index命中
```

####  AT上报数据监控

对于AT设备而言，除了我们发送命令其会返回数据外，其有可能会主动上报数据。为了处理该情况，提供该接口给用户使用监听。

```
typedef s32_t (*fnoob)(u8_t *data,s32_t datalen);
bool_t at_oobregister(fnoob func,const char *index);  //register a out of band data dealer
```

用户注册该接口之后，当收到的数据命中index时，则会回调注册的func。该index和at命令不同的是，该index一定是从头开始进行匹配的，用户可以注册多个监听接口。