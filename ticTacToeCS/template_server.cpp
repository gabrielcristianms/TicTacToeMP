#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "tictactoe.hpp"

#define SERVER_IP "127.0.0.1"
#define PORT 51234
#define BUFLEN 100

void wrap(char* buf, int cell){
    buf[0] = static_cast<char>(cell);
}

void unwrap(char* buf, int &cell){
    cell = static_cast<int>(buf[0]);
}

int main () {

    int listenSocketFD = socket(AF_INET, SOCK_STREAM, 0);    
    std::cout << "Socket number: " << listenSocketFD << std::endl;
    char buff[BUFLEN];
    bzero(buff, BUFLEN);
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    if (bind(listenSocketFD, (sockaddr*) &addr, sizeof(sockaddr_in))) {
        std::cout << "Binding error" << std::endl;
        close(listenSocketFD);
        return -2;
    }


    if (listen(listenSocketFD, SOMAXCONN)) {
        std::cout << "Listen error" << std::endl;
        close(listenSocketFD);
        return -3;
    }

    sockaddr_in client1_addr;
    socklen_t addr1_len = sizeof(sockaddr_in);
    std::cout << "Waiting for client connection..." << std::endl;
    int conn1SocketFD = accept(listenSocketFD, (sockaddr*) &client1_addr, &addr1_len);
    if (conn1SocketFD <= 0) {
        close(listenSocketFD);
        return -4;
    }
    else{
        std::cout << "Cliente 1 conectado com sucesso!!!" << std::endl;
        buff[0] = 'x';
        write(conn1SocketFD, buff, 1);
    }

    if (listen(listenSocketFD, SOMAXCONN)) {
        std::cout << "Listen error" << std::endl;
        close(listenSocketFD);
        return -3;
    }

    sockaddr_in client2_addr;
    socklen_t addr2_len = sizeof(sockaddr_in);
    std::cout << "Waiting for client connection..." << std::endl;
    int conn2SocketFD = accept(listenSocketFD, (sockaddr*) &client2_addr, &addr2_len);
    if (conn2SocketFD <= 0) {
        close(listenSocketFD);
        return -4;
    }
    else{
        std::cout << "Cliente 2 conectado com sucesso!!! Que comecem os jogos..." << std::endl;
        buff[0] = 'o';
        write(conn2SocketFD, buff, 1);
    }
    
    game_state s;
    bzero(buff, BUFLEN);
    int cell = 0;
    while (true) {
        read(conn1SocketFD, buff, sizeof(buff));
        //Envia o comando para o cliente
        write(conn2SocketFD, buff, 1);

        unwrap(buff, cell);
        if (cell == 9 ){
            std::cout << "O jogador 1 saiu da sessão. O jogo será encerrado..." << std::endl;
            break;
        }
        
        //Espera resposta do cliente
        read(conn2SocketFD, buff, sizeof(buff));
        write(conn1SocketFD, buff, 1);
        unwrap(buff, cell);
        if (cell == 9 ){
            std::cout << "O jogador 2 saiu da sessão. O jogo será encerrado..." << std::endl;
            break;
        }
    }
    // Close connection
    close(conn1SocketFD);
    close(conn2SocketFD);
    return EXIT_SUCCESS;
}

