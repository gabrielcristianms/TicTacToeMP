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

int wrap(char* buf, game_state &s){
    //criar contador para o tamanho de bytes do buffer
    int bufsz = 0;
    //comprimir o board (espaços vazios não são contados)
    int i = 0;
    for (auto &&square : s.board){
        if (square != ' '){
            buf[bufsz] = square;
            buf[bufsz+1] = static_cast<char>(i); //representa até 255
            bufsz += 2; //1 byte pra peça e outro pra posição
        }
        ++i;
    }

    //colocar o valor dos int na ordem
    if( (bufsz == 0) and (s.score_x == s.score_o == 0)){
        return 0;
    }
    else{
        buf[bufsz] = static_cast<char>(s.score_x); //representa até 255
        buf[bufsz+1] = static_cast<char>(s.score_o); //representa até 255
        bufsz += 2; //1 byte para cada score
    }

    //ordem de serialização: boardComprimido -> score_x -> score_o
    //std::cout << buf[0] << static_cast<int>(buf[1]) << buf[2] << static_cast<int>(buf[3]) << static_cast<int>(buf[4]) << static_cast<int>(buf[5]) << std::endl;
    return bufsz;
}

void unwrap(char* buf, int n, game_state &s){
    //estado de jogo padrão
    if(n == 0) return;
    //ler buffer
    for (int i = 0; i < n-2; i+=2){
        if( (buf[i] == 'x') or (buf[i] == 'o') ){
            //atualizar board
            int pos = static_cast<int>(buf[i+1]);
            s.board[pos] = buf[i];
        }
    }
    //atualizar scores
    s.score_x = static_cast<int>(buf[n-2]);
    s.score_o = static_cast<int>(buf[n-1]);
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
    while (true) {
        //Espera resposta do servidor
        int szServer = read(connSocketFD, buff, sizeof(buff));

        //Reescreve estado do jogo(unwrap)
        unwrap(buff, szServer, s);
        bzero(buff, szServer);
        
        //Limpa tabuleiro se houve derrota/empate
        s.check_end();

        //Imprime estado e lê comando do terminal
        s.print_board();
        int cell = s.read_cell();

        //Insere O na célula e reimprime
        //Atualiza pontuação se houve vitória/empate
        s.play_o(cell);

        //Envia o estado para o servidor
        int szClient = wrap(buff, s);
        write(connSocketFD, buff, szClient);
        bzero(buff, szClient);
        //Checa se venceu para limpar o board
        if(s.get_winner() == 'o'){
            s.clear();
        }          
    }

    // Close connection
    close(connSocketFD);
    return EXIT_SUCCESS;
}