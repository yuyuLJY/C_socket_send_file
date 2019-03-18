#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <WINSOCK2.H>

#define portnum 12345
#define FILE_SIZE 500
#define BUFFER_SIZE 1024

//server端
//server接收连接请求，并发送文件（S->C）
int main()
{
    //绑定端口
    struct sockaddr_in {
        unsigned short sin_family;
        unsigned short sin_port;
        struct in_addr sin_addr;
        char sin_zero[8];
    }server_addr;

	server_addr.sin_family=AF_INET;   //Internet地址族=AF_INET(IPv4协议)
	server_addr.sin_port=htons(portnum);  //将主机字节序转化为网络字节序 ,portnum是端口号
	(server_addr.sin_addr).s_addr=htonl(INADDR_ANY);//IP地址

    //调用WSAStartup初始化套接字
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2,0), &WSAData) != 0)
    {
        return FALSE;
    }
    //初始化套接字
	int server_fd=socket(AF_INET,SOCK_STREAM,0);
	if(server_fd==-1)
	{
		printf("创建socket失败\n");
		exit(1);
	}

    // 把客户端的socket和客户端的socket地址结构绑定
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))==-1)
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }

    // server_socket用于监听
    printf("开始监听\n");
    if (listen(server_fd, 10))
    {
        printf("Server Listen Failed!\n");
        exit(1);
    }

    while(1){
        //定义客户端的socket客户端的地址结构
        struct sockaddr_in client_addr;
        int length = sizeof(client_addr);
        int new_server_socket = accept(server_fd, (struct sockaddr*)&client_addr, &length);
        if (new_server_socket < 0)
        {
            printf("Server Accept Failed!\n");
            break;
        }

        char buffer[BUFFER_SIZE];
        memset(buffer, 0,sizeof(buffer));
        length = recv(new_server_socket, buffer, BUFFER_SIZE, 0);
        if (length < 0)
        {
            printf("Server Recieve Data Failed!\n");//没有获得数据
            break;
        }

        char file_name[FILE_SIZE + 1];
        memset(file_name, 0,sizeof(file_name));
        strncpy(file_name, buffer,
                strlen(buffer) > FILE_SIZE ? FILE_SIZE : strlen(buffer));

        FILE *fp = fopen(file_name, "r");
        if (fp == NULL)
        {
            printf("File:\t%s Not Found!\n", file_name);
        }else{
            memset(buffer, 0,BUFFER_SIZE);
            int file_block_length = 0;
            while( (file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
            {
                printf("file_block_length = %d\n", file_block_length);

                // 发送buffer中的字符串到new_server_socket,实际上就是发送给客户端
                if (send(new_server_socket, buffer, file_block_length, 0) < 0)
                {
                    printf("Send File:\t%s Failed!\n", file_name);
                    break;
                }

                memset(buffer,0, sizeof(buffer));
            }
            fclose(fp);
            printf("File:\t%s Transfer Finished!\n", file_name);
        }
        close(new_server_socket);
    }
    close(server_fd);
    printf("Hello world!\n");
    return 0;
}
