#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <WINSOCK2.H>

#define portnum 12345
#define FILE_SIZE 500
#define BUFFER_SIZE 1024

//server��
//server�����������󣬲������ļ���S->C��
int main()
{
    //�󶨶˿�
    struct sockaddr_in {
        unsigned short sin_family;
        unsigned short sin_port;
        struct in_addr sin_addr;
        char sin_zero[8];
    }server_addr;

	server_addr.sin_family=AF_INET;   //Internet��ַ��=AF_INET(IPv4Э��)
	server_addr.sin_port=htons(portnum);  //�������ֽ���ת��Ϊ�����ֽ��� ,portnum�Ƕ˿ں�
	(server_addr.sin_addr).s_addr=htonl(INADDR_ANY);//IP��ַ

    //����WSAStartup��ʼ���׽���
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2,0), &WSAData) != 0)
    {
        return FALSE;
    }
    //��ʼ���׽���
	int server_fd=socket(AF_INET,SOCK_STREAM,0);
	if(server_fd==-1)
	{
		printf("����socketʧ��\n");
		exit(1);
	}

    // �ѿͻ��˵�socket�Ϳͻ��˵�socket��ַ�ṹ��
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))==-1)
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }

    // server_socket���ڼ���
    printf("��ʼ����\n");
    if (listen(server_fd, 10))
    {
        printf("Server Listen Failed!\n");
        exit(1);
    }

    while(1){
        //����ͻ��˵�socket�ͻ��˵ĵ�ַ�ṹ
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
            printf("Server Recieve Data Failed!\n");//û�л������
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

                // ����buffer�е��ַ�����new_server_socket,ʵ���Ͼ��Ƿ��͸��ͻ���
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
