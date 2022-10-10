#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "traits.h"

char text[num_lines][line_size];
int counter;
pthread_mutex_t write_lock, count_lock;

void* keep_connection(void* p_client_sockfd){
  int client_sockfd = *((int*) p_client_sockfd);
  free(p_client_sockfd);
  struct message msg;
  
  while(1){
    printf("Socket esperando\n");
    read(client_sockfd, &msg, sizeof(msg));
    
    if(msg.type == GET){
      pthread_mutex_lock(&write_lock);
      write(client_sockfd, text[msg.idx], line_size);
      pthread_mutex_unlock(&write_lock);
      printf("Texto lido: \"%s\" na linha: %d\n", text[msg.idx], msg.idx);
    }
    else if(msg.type == ADD){
      int result = 0;
      pthread_mutex_lock(&write_lock);
      strcpy(text[msg.idx], msg.text);
      write(client_sockfd, &result, sizeof(int));
      pthread_mutex_unlock(&write_lock);

      printf("Texto escrito: \"%s\" na linha: %d\n", text[msg.idx], msg.idx);
    }
    else
      break;
  }

  close(client_sockfd);

  pthread_mutex_lock(&count_lock);
  counter--;
  printf("Clientes conectados: %d\n", counter);
  pthread_mutex_unlock(&count_lock);

  return NULL;
}

int main(){
  counter = 0;
  
  int server_sockfd, client_sockfd, server_len, client_len;
  struct sockaddr_in server_address, client_address;

  server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(9734);
  server_len = sizeof(server_address);

  if (pthread_mutex_init(&write_lock, NULL) || pthread_mutex_init(&count_lock, NULL)){
    printf("Não foi possível inicializar o mutex\n");
    exit(1);
  } 

  bind(server_sockfd, (struct sockaddr *) &server_address, server_len);
	listen(server_sockfd, 5);
	
  while(1) {
    printf("Servidor esperando\n");
    client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);

    if(counter >= max_clients) {
      printf("Número de clientes superior ao limite\n");
      int result = -1;
      write(client_sockfd, &result, sizeof(int));
      close(client_sockfd);
      continue;
    } else {
      printf("Conexão estabelecida com sucesso\n");
      int result = 0;
      write(client_sockfd, &result, sizeof(int));
    }

    pthread_t thread;
    int *p_client_sockfd = malloc(sizeof(int));
    *p_client_sockfd = client_sockfd;
    pthread_create(&thread, NULL, keep_connection, p_client_sockfd);

    pthread_mutex_lock(&count_lock);
    counter++;
    printf("Clientes conectados: %d\n", counter);
    pthread_mutex_unlock(&count_lock);
  }
  
  exit(0);
}