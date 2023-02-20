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
    if((bufsz == 0) && (s.score_x == s.score_o == 0)){
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
        if( (buf[i] == 'x') || (buf[i] == 'o') ){
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

    game_state s;
    char buff[BUFLEN];
    bzero(buff, BUFLEN);  
    while (true) {
        //Limpa o tabuleiro se houver derrota/empate
        s.check_end();
            
        //Imprime estado e lê comando do terminal
        s.print_board();
        int cell = s.read_cell();
        
        //Insere X na célula e reimprime
        //Atualiza pontuação se houve vitória/empate
        s.play_x(cell);

        //Envia o estado para o cliente(wrap) 
        int szServer = wrap(buff, s);
        write(connSocketFD, buff, szServer);
        bzero(buff, BUFLEN);
        //Checa se venceu para limpar o board
        if(s.get_winner() == 'x'){
            s.clear();
        }

        //Espera reposta do cliente      
        int szClient = read(connSocketFD, buff, sizeof(buff));       

        //Reescreve estado do jogo(unwrap)     
        unwrap(buff, szClient, s);
        bzero(buff, BUFLEN);        
    }

    // Close connection
    close(connSocketFD);
    return EXIT_SUCCESS;
}