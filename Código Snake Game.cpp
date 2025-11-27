#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h> // Para Sleep() e SetConsoleCursorPosition
#include <time.h>

// ==========================================
// 🏗️ STRUCTS (Os Blocos de Construção)
// ==========================================

// 1. O Ponto: Coordenada (X, Y)
typedef struct {
    int x;
    int y;
} Ponto;

// 2. A Cobra: Corpo, Tamanho, Direção
typedef struct {
    Ponto corpo[1000];
    int tamanho;
    int dirX;
    int dirY;
} Cobra;

// 3. O Jogo: Variáveis gerais do ambiente, incluindo o NOVO TIMER!
typedef struct {
    int largura;
    int altura;
    int gameOver;
    int score;
    Ponto fruta;
    // ⭐ NOVIDADE: Variáveis de Velocidade
    int timerMovimento;     // Contador que aumenta a cada loop
    const int VELOCIDADE;   // Valor que o timer deve atingir para a cobra se mover (menor é mais rápido)
} Jogo;

// Variáveis Globais
Cobra snake;
Jogo game = { // Inicializando a struct Game com um valor padrão para a velocidade
    .VELOCIDADE = 5 // O jogo se move a cada 10 ciclos do loop principal
};

// ==========================================
// ⚙️ FUNÇÕES ESSENCIAIS (Mantidas as anteriores)
// ==========================================

void OcultarCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void Setup() {
    game.largura = 40;
    game.altura = 25;
    game.gameOver = 0;
    game.score = 0;
    game.timerMovimento = 0; // Inicia o contador em zero

    // Configura a cobra no centro
    snake.tamanho = 1;
    snake.corpo[0].x = game.largura / 2;
    snake.corpo[0].y = game.altura / 2;
    snake.dirX = 0;
    snake.dirY = 0;

    // Coloca a primeira fruta
    srand(time(NULL));
    game.fruta.x = rand() % game.largura;
    game.fruta.y = rand() % game.altura;
    
    OcultarCursor();
}

void Draw() {
    // Posiciona o cursor no início
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    // ... (Código de desenho da tela e pontuação - É o mesmo) ...

    for (int i = 0; i < game.largura + 2; i++) printf("#");
    printf("\n");

    for (int i = 0; i < game.altura; i++) {
        for (int j = 0; j < game.largura; j++) {
            if (j == 0) printf("#");

            if (i == snake.corpo[0].y && j == snake.corpo[0].x) {
                printf("O");
            } else if (i == game.fruta.y && j == game.fruta.x) {
                printf("@");
            } else {
                int printou = 0;
                for (int k = 1; k < snake.tamanho; k++) {
                    if (snake.corpo[k].x == j && snake.corpo[k].y == i) {
                        printf("o");
                        printou = 1;
                        break;
                    }
                }
                if (!printou) printf(" ");
            }

            if (j == game.largura - 1) printf("#");
        }
        printf("\n");
    }

    for (int i = 0; i < game.largura + 2; i++) printf("#");
    printf("\n");
    
    printf("Score: %d | Velocidade: %d\n", game.score, game.VELOCIDADE);
}

void Input() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'a': if(snake.dirX == 1) break;
                      snake.dirX = -1; snake.dirY = 0; break;
            case 'd': if(snake.dirX == -1) break;
                      snake.dirX = 1;  snake.dirY = 0; break;
            case 'w': if(snake.dirY == 1) break;
                      snake.dirX = 0;  snake.dirY = -1; break;
            case 's': if(snake.dirY == -1) break;
                      snake.dirX = 0;  snake.dirY = 1; break;
            case 'x': game.gameOver = 1; break;
        }
    }
}

void Logic() {
    // 1. Lógica da Cauda
    for (int i = snake.tamanho - 1; i > 0; i--) {
        snake.corpo[i] = snake.corpo[i - 1]; 
    }

    // 2. Move a Cabeça
    snake.corpo[0].x += snake.dirX;
    snake.corpo[0].y += snake.dirY;

    // 3. Colisões (Paredes e Corpo)
    if (snake.corpo[0].x >= game.largura || snake.corpo[0].x < 0 || 
        snake.corpo[0].y >= game.altura || snake.corpo[0].y < 0) {
        game.gameOver = 1;
    }
    for (int i = 1; i < snake.tamanho; i++) {
        if (snake.corpo[0].x == snake.corpo[i].x && 
            snake.corpo[0].y == snake.corpo[i].y) {
            game.gameOver = 1;
        }
    }

    // 4. Comer a Fruta
    if (snake.corpo[0].x == game.fruta.x && snake.corpo[0].y == game.fruta.y) {
        game.score += 10;
        if (snake.tamanho < 100) snake.tamanho++; // Evita Overflow
        game.fruta.x = rand() % game.largura;
        game.fruta.y = rand() % game.altura;
    }
}

// ==========================================
// 🏃 MOTOR PRINCIPAL
// ==========================================

int main() {
    Setup();

    while (!game.gameOver) {
        
        Input(); // ➡️ Sempre lemos o teclado imediatamente!

        // ⏱️ O NOVO SISTEMA DE TIMER
        game.timerMovimento++; // Aumenta o contador a cada ciclo

        if (game.timerMovimento >= game.VELOCIDADE) {
            // ⭐ SÓ MOVE A COBRA SE O TIMER ATINGIR O VALOR DE VELOCIDADE
            Logic();
            game.timerMovimento = 0; // Zera o contador para começar de novo
        }
        
        Draw(); // 🖼️ Sempre desenhamos (para que o desenho não fique piscando)
        
        Sleep(10); // Pausa muito pequena de 10ms para não consumir CPU
    }
    
    printf("\nGAME OVER! Pontos: %d\n", game.score);
    system("pause");
    return 0;
}