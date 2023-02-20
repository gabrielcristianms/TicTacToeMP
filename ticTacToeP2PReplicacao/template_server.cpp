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

    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(sockaddr_in);
    std::cout << "Waiting for client connection..." << std::endl;
    int connSocketFD = accept(listenSocketFD, (sockaddr*) &client_addr, &addr_len);
    if (connSocketFD <= 0) {
        close(listenSocketFD);
        return -4;
    }
    else{
        std::cout << "Cliente conectado com sucesso!!! Que comecem os jogos..." << std::endl;
    }

    game_state s;
    char buff[BUFLEN];
    bzero(buff, BUFLEN);
    int cell = 0;
      
    while (true) {

        //Limpa o tabuleiro se houve derrota/empate
        s.check_end();

        //Imprime estado e lê comando do terminal
        cell = s.read_cell();

        //Envia o comando para o cliente
        wrap(buff, cell);
        write(connSocketFD, buff, 1);

        //Fecha o jogo se o usuário pediu
        if(cell == 9) {
            std::cout << "O usuário saiu." << std::endl;
            break;
        }

        //Insere X na célula e reimprime
        //Atualiza pontuação se houve vitória/empate
        s.play_x(cell);
        s.print_board();

        //Limpa o tabuleiro se houve vitória/empate
        s.clear_on_win();

        //Espera resposta do cliente
        read(connSocketFD, buff, sizeof(buff));
        unwrap(buff, cell);

        //Fecha o jogo se o cliente pediu
        if(cell == 9) {
            std::cout << "O cliente saiu! RAGE QUIT!!!" << std::endl;
            break;
        }

        //Replica o estado do jogo
        s.play_o(cell);
        s.print_board();

    }

    // Close connection
    close(connSocketFD);
    return EXIT_SUCCESS;
}

