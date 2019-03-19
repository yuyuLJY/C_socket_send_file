#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WINSOCK2.H>

#define portnum 12345
#define BUFFER_SIZE 1024
#define FILE_SIZE 500
#define IP "192.168.126.132"
int sendfile(int sockfd);
//Client端
//接收S端的文件，并写入指定的文件
int main(){
    char buf[BUFFER_SIZE];
    struct hostent *h;
    struct sockaddr_in  client_addr;
    h=gethostbyname(IP);
    //写出linux端的IP和端口
    //Linux端的IP号：192.168.126.130
    //调用WSAStartup初始化套接字

    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2,0), &WSAData) != 0)
    {
        return FALSE;
    }

    int client_fd=socket(AF_INET,SOCK_STREAM,0);
	if(client_fd==-1)
	{
		printf("创建socket失败\n");
		exit(0);
	}
	printf("创建socket成功\n");
	printf("IP:%s\n",IP);
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(portnum);
    client_addr.sin_addr.s_addr = inet_addr(IP);

    /*
	if( inet_pton(AF_INET, IP, &client_addr.sin_addr) <= 0){
        printf("端口和IP错误\n");
        exit(0);
	}*/
	if( connect(client_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0){
        printf("连接错误\n");
        exit(0);
	}

    recievfile(client_fd); //向server发送文件
    return 0;

}

int recievfile(int client_fd){
    // 输入文件名 并放到缓冲区buffer中等待发送
    char  *fname="C:\\Users\\yuyu\\Desktop\\新建文件夹\\网络安全\\write.txt";  //写进这个里边
    char buffer[BUFFER_SIZE];
    memset( buffer,0, sizeof(buffer) );
    strncpy(buffer, fname, strlen(fname)>sizeof(buffer)?sizeof(buffer):strlen(fname));

     FILE *fp = fopen(fname,"w");
     if( fp==NULL )
     {
         printf("File:\t%s Can Not Open To Write\n", fname);
         exit(1);
     }

     int length = 0;
     memset( buffer,0, sizeof(buffer) );
    while(length = recv(client_fd, buffer, sizeof(buffer),0))
    {
        if( length<0)
        {
            printf("Recieve Failed\n");
            break;
        }
        int write_length = fwrite(buffer, sizeof(char), length, fp);
		if(write_length<length)
		{
		    printf("File:\t%s Write Failed\n", fname);
			break;
		}
		printf("%d\n",length);
        memset( buffer,0, sizeof(buffer) );
    }

	// 接收成功后，关闭文件，关闭socket
     printf("Receive File:\t%s From Server IP Successful!\n", fname);
     close(fp);
     close(client_fd);
     return 0;
}
