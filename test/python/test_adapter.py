import sys
from socket import *
S_HOST = '192.168.1.168'
D_HOST = '192.168.1.100'
S_PORT = 62556
D_PORT = 5999  # 端口号
BUFFSIZE = 1024*8  # 接收消息的缓冲大小

udpCliSock = 0

class resultFromDest(object):
    test_id = 0
    ret_code = -1
    data = ''


def ts_call_single(*args):
    global udpCliSock
    if(not udpCliSock):
        print("api_call,not udpCliSock")
        udpCliSock = socket(AF_INET, SOCK_DGRAM)  # 创建客户端套接字
        udpCliSock.settimeout(10)
        udpCliSock.setsockopt(SOL_SOCKET,SO_REUSEADDR,1)
        udpCliSock.setsockopt(SOL_SOCKET,SO_SNDBUF,BUFFSIZE)
        # udpCliSock.bind((S_HOST,S_PORT))
    else:
        print("api_call now,udpCliSock")
        udpCliSock.settimeout(10)
    cmd_str = ''
    for arg in args:
        cmd_str += str(arg)+'|'
    print("API CALL:", cmd_str)
    udpCliSock.sendto(bytes(cmd_str, 'utf-8'),  (D_HOST, D_PORT))  # 客户端发送消息
    try:
        result, host_addr = udpCliSock.recvfrom(BUFFSIZE)  # 接收回应消息
        print(result)
        str_utf8 = result.decode('utf-8')
        print(str_utf8)
        list = str_utf8.split(',|')
        retResult = resultFromDest()
        retResult.test_id = int(list[0])
        retResult.ret_code = int(list[1])
        if(retResult.ret_code == 1):
            retResult.data = list[2]

        print(retResult)
        return retResult
    except:
        print("some errors")
        pass

def resource_release():
     global udpCliSock
     if not udpCliSock:
         return
     else:
         udpCliSock.close()
