#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netdb.h>
#define EOL "\r\n"
# define CHUNK_SIZE 512
#define HTTP "HTTP/1.1"

int recv_response(int s){
  
	int size_recv , total_size= 0 , flag = 0;
	char chunk[CHUNK_SIZE] , *ptr , *cl_finder  ;
    printf("\n\t\t[### RECIEVING RESPONSE ###]\t\n");
while(1)
	{
		memset(chunk ,0 , CHUNK_SIZE);	//clear the variable
		if((size_recv =  read(s , chunk , CHUNK_SIZE-2 ) ) < 0)
		{
   
            printf("something went wrong");
		}

		else
		{   chunk[size_recv+1] = 0 ;
            if (flag == 0 && (cl_finder = strstr(chunk , "Content-Length: "))!=NULL)
            {   //printf("\n\t\t[###CONTENT-TYPE HEADER###]\n");
                int len = strlen("Content-Length: ");
               // printf("\t\t\n%d find\n",len);
                cl_finder = cl_finder + len ;
                //printf("%s",cl_finder);
                char *d;
                d = strstr(cl_finder,"\r\n");
                int n = abs(cl_finder-d);
                //printf("\t\t\n%d\n\n",n);
                *(cl_finder+n) = 0;
                n = atoi(cl_finder);
                //printf("\n\n\n\n\n%d",n);
                total_size += size_recv;
                if(n != total_size ){
                    printf("%s",chunk);
                    printf("%d\n\n",total_size);
                }
                else
                {
                    printf("%s",chunk);
                    exit(0);
                }
            }
            else
                {   
               // printf("\n\t\t[###CHUNKED ENCODING HEADER###]\n");
                total_size += size_recv;
                ptr = &chunk[size_recv-5];
                if(strcmp(ptr,"0\r\n\r\n")==0){
                    printf("%s",chunk);
                    exit(0);
                }
			    printf("%s" , chunk);
            }
            
                
            }
            


           
		}
        return total_size ;
	}
	

	
char* createQuery(char* host, char* page)
{
    char* msg = "GET %s %s\r\nConnection: close\r\n\r\n";
    char* query = (char *)malloc( strlen(page) + strlen(msg) + strlen(HTTP) - 6 + 1);//-6: %s %s %s
    sprintf(query, msg, page, HTTP);
    return query;
}

char *dns_lookup(char *ip){
    struct hostent *host_info;
    struct in_addr *address;
    host_info = gethostbyname(ip);

    if(host_info == NULL) {
    printf("Couldn't lookup %s\n",ip);
    } 
    
    else {
    address = (struct in_addr *) (host_info->h_addr);

    ip = inet_ntoa(*address);

    return ip ;

}
}

int main(int argc , char *argv[]){


    if(argc != 2) {
    printf("Usage: %s <hostname>\n", argv[0]);
    exit(1);
    }

    struct sockaddr_in server ;
    int socket_fd ;
    char *ip;
    ip = (char *)malloc(strlen(argv[1])*sizeof(char));
    strcpy(ip,argv[1]);

    ip = dns_lookup(ip);
    socket_fd = socket(AF_INET,SOCK_STREAM,0);

    server.sin_family = AF_INET ;
    server.sin_port = htons(80);
    server.sin_addr.s_addr = inet_addr(ip);
    if(connect(socket_fd,(struct sockaddr *)&server ,sizeof(server))<0)
    {
        fprintf(stderr,"connection failed");
        exit(-1);
    }

    else
    {   system("clear");
        printf("connected\n");
        int n = 0 ;
        char recv_msg[20000] ,*page = "/";
        char *http_headers = createQuery(argv[1],page);

        printf("\t\t[SENDING HEADERS]\n");
        puts(http_headers);
        if((n=send(socket_fd , http_headers , strlen(http_headers),0))<0)
        {    printf("sending failed\n");
             exit(-1);
        }
    recv_response(socket_fd);
 
    close(socket_fd);
 return 0 ;   
}
}

