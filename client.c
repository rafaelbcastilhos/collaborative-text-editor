#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "traits.h"

#define STRC(x) #x
#define STR(X) STRC(X)

void get_line(int sockfd, int idx, char* text){
  struct message msg;
  msg.idx = idx;
  msg.type = GET;
	
  write(sockfd, &msg, sizeof(msg));
  read(sockfd, text, num_lines);
}

int add_line(int sockfd, int idx, char* text){
  struct message msg;
  strcpy(msg.text, text);
  msg.idx = idx;
  msg.type  = ADD;

  int result;
  write(sockfd, &msg, sizeof(msg));
  read(sockfd, &result, sizeof(int));

  return result;
}

void close_connection(int sockfd){
  struct message msg;
  msg.type = EXIT;
  write(sockfd, &msg, sizeof(msg));
}

int main(){
  struct sockaddr_in address;
  int result, sockfd, option = -1;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(9734);

  result = connect(sockfd, (struct sockaddr *) &address, sizeof(address));
  if(result == -1){
		printf("Erro de conexão com o servidor\n");
		exit(1);
  }

  read(sockfd, &result, sizeof(int));
  if(result == -1){
		printf("Não há espaço para mais clientes\n");
    	exit(1);
  }

  while(option){
    printf("\n0 - Encerrar conexão");
    printf("\n1 - Ler linha");
    printf("\n2 - Escrever linha");
    printf("\nOpção: ");
    scanf("%d", &option);

    char text[num_lines];
    int idx;
    switch(option){
      case 1:
          printf("Linha: ");
          scanf("%d", &idx);
          if(idx < 0 || idx >= num_lines){
            printf("Linha inválida\n");
            break;
          }
          get_line(sockfd, idx, text);
          if(text[0] == '\0')
            printf("Linha vazia\n");
          else
            printf("Linha %d: %s\n", idx, text);
          break;
  
      case 2:
          printf("Linha: ");
          scanf("%d", &idx);
          if(idx < 0 || idx >= num_lines){
            printf("Linha inválida\n");
            break;
          }
          printf("Texto: ");
          scanf(" %" STR(num_lines) "[^\n]", text);
          if(strlen(text) > line_size){
            printf("Texto muito grande\n");
            break;
          }
          if(add_line(sockfd, idx, text) == 1)
            printf("Erro ao inserir linha %d\n", idx);
          break;

      case 0: 
          close_connection(sockfd);
          break;
    }
  }

  close(sockfd);
  exit(0);
}
