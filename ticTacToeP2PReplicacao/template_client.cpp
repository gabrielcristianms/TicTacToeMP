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

    int connSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Socket number: " << connSocketFD << std::endl;
    
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(connSocketFD, (sockaddr*) &server_addr, sizeof(sockaddr_in))) {
        //int err = WSAGetLastError();
        //std::cout << "Connect error: " << err << std::endl;
        close(connSocketFD);
        return -2;
    }
    else{
        std::cout << "Cliente conectado com sucesso!!! Que comecem os jogos..." << std::endl;
    }

    game_state s;
    char buff[BUFLEN];
    bzero(buff, BUFLEN);
    int cell = 0;
    while (true) {

        //Limpa o tabuleiro se houve vitória/empate
        s.clear_on_win();
   
        //Espera resposta do servidor
        read(connSocketFD, buff, sizeof(buff));
        unwrap(buff, cell);

        //Fecha o jogo se o servidor pediu
        if(cell == 9) {
            std::cout << "O servidor saiu! RAGEQUIT!!!" << std::endl;
            break;
        }

        //Replica o estado do jogo
        s.play_x(cell);

        //Limpa o tabuleiro se houve derrota/empate
        s.check_end();

        //Imprime estado e lê comando do terminal
        s.print_board();
        cell = s.read_cell();

        //Envia o comando para o servidor
        wrap(buff, cell);
        write(connSocketFD, buff, 1);

        //Fecha o jogo se o usuário pediu
        if(cell == 9) {
            std::cout << "O usuário saiu." << std::endl;
            break;
        }

        //Insere O na célula e reimprime
        //Atualiza pontuação se houve vitória/empate
        s.play_o(cell);
        s.print_board();

    }

    // Close connection
    close(connSocketFD);
    return EXIT_SUCCESS;
}

