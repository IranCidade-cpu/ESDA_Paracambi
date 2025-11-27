#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ------------------------
      VARIÁVEIS
-------------------------*/
char tabuleiro[3][3];

/* ------------------------
      FUNÇÕES DO TABULEIRO
-------------------------*/
void iniciar_tabuleiro() {
    char n = '1';
    int i, j;
    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            tabuleiro[i][j] = n++;
        }
    }
}

void desenhar_tabuleiro() {
    int i, j;
    printf("\n");
    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            printf(" %c ", tabuleiro[i][j]);
            if(j < 2) printf("|");
        }
        printf("\n");
        if(i < 2) printf("---+---+---\n");
    }
    printf("\n");
}

int posicao_livre(int i, int j) {
    return tabuleiro[i][j] != 'X' && tabuleiro[i][j] != 'O';
}

int ha_jogadas_restantes() {
    int i, j;
    for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
            if(posicao_livre(i, j)) return 1;
    return 0;
}

int checar_vitoria_estado(char s[3][3], char p) {
    int i;
    for(i = 0; i < 3; i++) {
        if(s[i][0] == p && s[i][1] == p && s[i][2] == p) return 1;
        if(s[0][i] == p && s[1][i] == p && s[2][i] == p) return 1;
    }
    if(s[0][0] == p && s[1][1] == p && s[2][2] == p) return 1;
    if(s[0][2] == p && s[1][1] == p && s[2][0] == p) return 1;
    return 0;
}

int checar_vitoria(char p) {
    return checar_vitoria_estado(tabuleiro, p);
}

/* ------------------------
         IA FÁCIL
   (JOGA ALEATÓRIO)
-------------------------*/
void ia_facil() {
    int livres[9][2], qtd = 0;
    int i, j;

    for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
            if(posicao_livre(i, j)) {
                livres[qtd][0] = i;
                livres[qtd][1] = j;
                qtd++;
            }

    if(qtd == 0) return;

    int r = rand() % qtd;
    tabuleiro[livres[r][0]][livres[r][1]] = 'O';
}

/* ------------------------
         IA MÉDIA
    (HEURÍSTICA SIMPLES)
-------------------------*/
void ia_media() {
    int i, j;

    // 1) Jogar para ganhar
    for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
            if(posicao_livre(i, j)) {
                tabuleiro[i][j] = 'O';
                if(checar_vitoria('O')) return;
                tabuleiro[i][j] = '1' + i * 3 + j; // Restaura valor original
            }

    // 2) Bloquear jogador
    for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
            if(posicao_livre(i, j)) {
                tabuleiro[i][j] = 'X';
                if(checar_vitoria('X')) {
                    tabuleiro[i][j] = 'O';
                    return;
                }
                tabuleiro[i][j] = '1' + i * 3 + j; // Restaura valor original
            }

    // 3) Centro
    if(posicao_livre(1, 1)) {
        tabuleiro[1][1] = 'O';
        return;
    }

    // 4) Cantos
    int cantos[4][2] = {{0,0}, {0,2}, {2,0}, {2,2}};
    int k;
    for(k = 0; k < 4; k++) {
        i = cantos[k][0];
        j = cantos[k][1];
        if(posicao_livre(i, j)) {
            tabuleiro[i][j] = 'O';
            return;
        }
    }

    // 5) Aleatório
    ia_facil();
}

/* ------------------------
         MINIMAX
      (MODO IMPOSSÍVEL)
-------------------------*/
int minimax(int profundidade, int maximizando) {
    // Avalia estado atual
    if(checar_vitoria('O')) return 10 - profundidade;
    if(checar_vitoria('X')) return -10 + profundidade;
    if(!ha_jogadas_restantes()) return 0;

    int melhor;
    int i, j;

    if(maximizando) {
        melhor = -1000;
        for(i = 0; i < 3; i++) {
            for(j = 0; j < 3; j++) {
                if(posicao_livre(i, j)) {
                    char original = tabuleiro[i][j];
                    tabuleiro[i][j] = 'O';
                    
                    int valor = minimax(profundidade + 1, 0);
                    
                    tabuleiro[i][j] = original;
                    
                    if(valor > melhor) melhor = valor;
                }
            }
        }
        return melhor;
    } else {
        melhor = 1000;
        for(i = 0; i < 3; i++) {
            for(j = 0; j < 3; j++) {
                if(posicao_livre(i, j)) {
                    char original = tabuleiro[i][j];
                    tabuleiro[i][j] = 'X';
                    
                    int valor = minimax(profundidade + 1, 1);
                    
                    tabuleiro[i][j] = original;
                    
                    if(valor < melhor) melhor = valor;
                }
            }
        }
        return melhor;
    }
}

void ia_impossivel() {
    int melhorValor = -1000;
    int melhorI = -1, melhorJ = -1;
    int i, j;

    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            if(posicao_livre(i, j)) {
                char original = tabuleiro[i][j];
                tabuleiro[i][j] = 'O';
                
                int valorMovimento = minimax(0, 0);
                
                tabuleiro[i][j] = original;
                
                if(valorMovimento > melhorValor) {
                    melhorValor = valorMovimento;
                    melhorI = i;
                    melhorJ = j;
                }
            }
        }
    }

    if(melhorI != -1 && melhorJ != -1) {
        tabuleiro[melhorI][melhorJ] = 'O';
    }
}

/* ------------------------
     ESCOLHA DA IA
-------------------------*/
void ia_jogar(int nivel) {
    if(nivel == 1) ia_facil();
    else if(nivel == 2) ia_media();
    else ia_impossivel();
}

/* ------------------------
           MAIN
-------------------------*/
int main() {
    srand(time(NULL));
    iniciar_tabuleiro();

    int nivel;
    printf("Escolha dificuldade:\n1 - Facil\n2 - Medio\n3 - Impossivel\n-> ");
    scanf("%d", &nivel);
    if(nivel < 1 || nivel > 3) nivel = 1;

    while(1) {
        desenhar_tabuleiro();

        int jogada;
        printf("Sua jogada (1-9): ");
        scanf("%d", &jogada);

        if(jogada < 1 || jogada > 9) {
            printf("Jogada invalida! Escolha entre 1 e 9.\n");
            continue;
        }

        int i = (jogada - 1) / 3;
        int j = (jogada - 1) % 3;

        if(!posicao_livre(i, j)) {
            printf("Posicao ja ocupada! Escolha outra.\n");
            continue;
        }

        tabuleiro[i][j] = 'X';

        if(checar_vitoria('X')) {
            desenhar_tabuleiro();
            printf("Parabens! Você venceu!\n");
            break;
        }

        if(!ha_jogadas_restantes()) {
            desenhar_tabuleiro();
            printf("Empate!\n");
            break;
        }

        ia_jogar(nivel);

        if(checar_vitoria('O')) {
            desenhar_tabuleiro();
            printf("A IA venceu!\n");
            break;
        }

        if(!ha_jogadas_restantes()) {
            desenhar_tabuleiro();
            printf("Empate!\n");
            break;
        }
    }

    return 0;
}
