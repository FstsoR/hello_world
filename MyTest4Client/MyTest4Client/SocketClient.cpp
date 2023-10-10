﻿#include <winSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define DEFAULT_IP "127.0.0.1"		// 服务器为本机
#define DEFAULT_PORT "27015"		// 默认端口
#define DEFAULT_BUFLEN	512			// 字符缓冲区长度

// 1. 初始化socket客户端
int SC_Init()
{
	int ret;	//结果
	//WSADATA结构包含有关Windows Sockets实现的信息。
	WSADATA wsaData;
	//Winsock进行初始化
	//调用 WSAStartup 函数以启动使用 WS2 _32.dll
	//WSAStartup的 MAKEWORD (2，2) 参数发出对系统上 Winsock 版本2.2 的请求，并将传递的版本设置为调用方可以使用的最新版本的 Windows 套接字支持
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (ret != 0) {
		printf("WSAStartup 失败: %d\n", ret);
		return 1;
	}
	return 0;
}

// 2. 为客户端创建套接字并连接
int SC_Connect(SOCKET *ConnectSocket)
{
	int ret = 0;
	//初始化之后实例套接字对象供客户端使用
	//（1）创建套接字
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	// ZeroMemory 函数，将内存块的内容初始化为零
	ZeroMemory(&hints, sizeof(hints));
	//addrinfo在getaddrinfo()调用中使用的结构
	hints.ai_family = AF_INET; //AF _INET 用于指定 IPv4 地址族
	hints.ai_socktype = SOCK_STREAM;// SOCK _STREAM 用于指定流套接字
	hints.ai_protocol = IPPROTO_TCP;// IPPROTO _TCP 用于指定 tcp 协议
	hints.ai_flags = AI_PASSIVE;
	//从本机中获取ip地址等信息为了sockcet 使用
	//解析服务器地址和端口
	//getaddrinfo函数提供从ANSI主机名到地址的独立于协议的转换。
	//参数1：该字符串包含一个主机(节点)名称或一个数字主机地址字符串。
	//参数2：服务名或端口号。
	// 参数3：指向addrinfo结构的指针，该结构提供有关调用方支持的套接字类型的提示。
	//参数4：指向一个或多个包含主机响应信息的addrinfo结构链表的指针。
	ret = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
	if (ret != 0) {
		printf("getaddrinfo 失败: %d\n", ret);
		WSACleanup();
		return 1;
	}
	//*ConnectSocket = INVALID_SOCKET;	//创建套接字对象
	//尝试连接到返回的第一个地址。
	*ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//检查是否存在错误，以确保套接字为有效套接字。
	if (*ConnectSocket == INVALID_SOCKET) {
		//WSAGetLastError返回与上次发生的错误相关联的错误号。
		printf("套接字错误: %ld\n", WSAGetLastError());
		//调用 freeaddrinfo 函数以释放由 getaddrinfo 函数为此地址信息分配的内存
		freeaddrinfo(result);
		WSACleanup();//用于终止 WS2 _ 32 DLL 的使用。
		return 1;
	}
	//（2）连接套接字
	for (struct addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		//调用getaddrinfo
		//尝试连接到一个地址，直到一个成功	
		*ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		//检查是否存在错误，以确保套接字为有效套接字。
		if (*ConnectSocket == INVALID_SOCKET) {
			//WSAGetLastError返回与上次发生的错误相关联的错误号。
			printf("socket failed with error: %ld\n", WSAGetLastError());
			//调用 freeaddrinfo 函数以释放由 getaddrinfo 函数为此地址信息分配的内存
			freeaddrinfo(result);
			WSACleanup();//用于终止 WS2 _ 32 DLL 的使用。
			return 1;
		}
		//调用 connect 函数，将创建的套接字和 sockaddr 结构作为参数传递。
		//connect函数建立到指定套接字的连接。
		//参数1：标识未连接套接字的描述符。
		//参数2：一个指向要建立连接的sockaddr结构的指针。
		//参数3：参数所指向的sockaddr结构的长度，以字节为单位
		ret = connect(*ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (ret == SOCKET_ERROR) {
			closesocket(*ConnectSocket);//关闭一个已存在的套接字。
			*ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	//应该尝试getaddrinfo返回的下一个地址,如果连接调用失败。但对于这个简单的例子，我们只是释放资源。由getaddrinfo返回并打印一个错误消息
	freeaddrinfo(result);//释放由 getaddrinfo 函数为此地址信息分配的内存
	if (*ConnectSocket == INVALID_SOCKET) {
		printf("无法连接到服务器！！\n");
		WSACleanup();
		return 1;
	}
	return 0;
}

// 3. 在客户端上发送和接收数据
int SC_Commu(SOCKET* ConnectSocket)
{
	int ret = 0;
	//下面的代码演示建立连接后客户端使用的发送和接收功能。
	int recvbuflen = DEFAULT_BUFLEN;	//缓冲区
	char sendbuf[DEFAULT_BUFLEN] = "Hello World";
	char input[DEFAULT_BUFLEN] = { 0 };
	printf("请输入要发送的指令：\n");
	//scanf_s("%s", input, sizeof(input));
	gets_s(input, sizeof(input));
	strcpy_s(sendbuf, sizeof(input), input);
	char recvbuf[DEFAULT_BUFLEN] = { 0 };
	//发送一个初始缓冲区
	//send函数参数1：标识已连接套接字的描述符。
	//参数2：指向包含要传送的数据的缓冲区的指针。
	//参数3：参数buf所指向的缓冲区中数据的长度(以字节为单位)。strlen获取字符串长度
	//参数4：指定调用方式的一组标志。
	ret = send(*ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (ret == SOCKET_ERROR) {
		printf("发送失败: %d\n", WSAGetLastError());
		closesocket(*ConnectSocket);	//关闭套接字
		WSACleanup();
		return 1;
	}
	printf("字节发送: %ld\n", ret);
	printf("发送的信息：");
	puts(sendbuf);
	//关闭正在发送的连接，因为不再发送数据
	//客户端仍然可以使用ConnectSocket来接收数据
	//shutdown禁止套接字上的发送或接收功能。
	//参数1：套接字描述符
	//参数2：关闭类型描述符。1代表关闭发送操作
	ret = shutdown(*ConnectSocket, SD_SEND);
	if (ret == SOCKET_ERROR) {
		printf("关闭失败: %d\n", WSAGetLastError());
		closesocket(*ConnectSocket);	//关闭套接字
		WSACleanup();
		return 1;
	}
	//接收数据，直到服务器关闭连接
	do {
		//recv函数从已连接的套接字或已绑定的⽆连接套接字接收数据。
		//参数1：套接字描述符
		//参数2：⼀个指向缓冲区的指针，⽤来接收传⼊的数据。
		//参数3：参数buf所指向的缓冲区的长度，以字节为单位。
		//参数4：⼀组影响此函数⾏为的标志
		ret = recv(*ConnectSocket, recvbuf, recvbuflen, 0);
		if (ret > 0)
		{
			printf("接收的字节数: %d\n", ret);
			printf("接收的信息：");
			puts(recvbuf);
		}
		else if (ret == 0)
			printf("连接关闭\n");
		else
			printf("连接失败！！: %d\n", WSAGetLastError());
	} while (ret > 0);
	return 0;
}

// 4. 断开客户端连接
void SC_Close(SOCKET* ConnectSocket)
{
	//两种方法断开客户端连接
	// 这里和服务器断开连接写在最后不同, 客户端断开连接写在 发送后 和 接收前
	// shutdown(ConnectSocket, SD_SEND) SD_SEND表示socket的发送数据端虽然关闭(为了服务器释放客户端连接资源), 但是仍然能接收服务端的数据
	//shutdown禁止套接字上的发送或接收功能。
	//参数1：套接字描述符
	//参数2：关闭类型描述符。1代表关闭发送操作
	//注意：这时客户端应用程序仍可以在套接字上接收数据。
	//iResult = shutdown(ClientSocket, SD_SEND);
	//if (iResult == SOCKET_ERROR) {
	//  printf("shutdown failed: %d\n", WSAGetLastError());
	//  closesocket(ClientSocket);
	//  WSACleanup();
	//  return 1;
	//}
	closesocket(*ConnectSocket);
	WSACleanup();
}

// socket客户端
int SocketClient()
{
	printf("启动客户端\n");

	SOCKET ConnectSocket;

	if (SC_Init())
	{
		return 1;
	}
	if (SC_Connect(&ConnectSocket))
	{
		return 1;
	}
	if (SC_Commu(&ConnectSocket))
	{
		return 1;
	}
	SC_Close(&ConnectSocket);

	return 0;
}


