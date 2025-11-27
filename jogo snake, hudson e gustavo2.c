#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

// Definições de constantes (aumentadas)
#define LARGURA 60
#define ALTURA 30
#define VELOCIDADE_BASE 200

// Códigos de cores para Windows
#define COR_VERDE 10
#define COR_ROXA 13
#define COR_BRANCA 15
#define COR_AMARELA 14
#define COR_CIANO 11
#define COR_VERMELHA 12

// Direções
typedef enum {
    CIMA,
    BAIXO,
    ESQUERDA,
    DIREITA
} Direcao;

// Modos de dificuldade
typedef enum {
    FACIL,
    MEDIO,
    DIFICIL
} Dificuldade;

// Estrutura para uma posição no mapa
typedef struct {
    int x;
    int y;
} Posicao;

// Estrutura para a cobra (lista encadeada)
typedef struct NoSnake {
    Posicao pos;
    struct NoSnake* proximo;
} NoSnake;

// Estrutura principal do jogo
typedef struct {
    NoSnake* cabeca;
    NoSnake* cauda;
    Posicao comida;
    Direcao direcao;
    int pontuacao;
    int jogo_ativo;
    int crescer_proximo_movimento;
    int velocidade;
    int comidas_coletadas;
    Dificuldade dificuldade;
    char mapa[ALTURA][LARGURA];
} JogoSnake;

// Protótipos de funções
void inicializar_jogo(JogoSnake* jogo, Dificuldade dificuldade);
void desenhar_mapa(JogoSnake* jogo);
void processar_input(JogoSnake* jogo);
void atualizar_jogo(JogoSnake* jogo);
void gerar_comida(JogoSnake* jogo);
int verificar_colisao(JogoSnake* jogo, Posicao nova_pos);
void adicionar_segmento(JogoSnake* jogo, Posicao pos);
void mover_cobra(JogoSnake* jogo);
void limpar_tela();
void configurar_terminal();
void liberar_memoria(JogoSnake* jogo);
void setColor(int color);
void gotoxy(int x, int y);
void mostrar_menu();
void desenhar_banner();
char* obter_nome_dificuldade(Dificuldade dificuldade);

int main() {
    JogoSnake jogo;
    int opcao;
    
    configurar_terminal();
    srand(time(NULL));
    
    do {
        limpar_tela();
        desenhar_banner();
        mostrar_menu();
        
        printf("\nEscolha a dificuldade (1-3): ");
        scanf("%d", &opcao);
        
        if (opcao >= 1 && opcao <= 3) {
            Dificuldade dificuldade = (Dificuldade)(opcao - 1);
            inicializar_jogo(&jogo, dificuldade);
            
            limpar_tela();
            
            while (jogo.jogo_ativo) {
                gotoxy(0, 0);
                desenhar_mapa(&jogo);
                processar_input(&jogo);
                atualizar_jogo(&jogo);
                Sleep(jogo.velocidade);
            }
            
            liberar_memoria(&jogo);
            
            gotoxy(0, ALTURA + 3);
            setColor(COR_BRANCA);
            printf("\nFim de jogo! Pontuacao final: %d\n", jogo.pontuacao);
            printf("Comidas coletadas: %d\n", jogo.comidas_coletadas);
            printf("Dificuldade: %s\n", obter_nome_dificuldade(jogo.dificuldade));
            printf("Pressione qualquer tecla para voltar ao menu...");
            getch();
            getch(); // Limpar buffer
        }
        
    } while (opcao != 0);
    
    return 0;
}

char* obter_nome_dificuldade(Dificuldade dificuldade) {
    switch (dificuldade) {
        case FACIL: return "FACIL";
        case MEDIO: return "MEDIO";
        case DIFICIL: return "DIFICIL";
        default: return "DESCONHECIDO";
    }
}

void desenhar_banner() {
    setColor(COR_CIANO);
    printf("\n");
    printf("   ██████   ██    ██ ███████ ███    ██  █████  ██   ██ ███████ ███████  ██████  ███    ██ \n");
    printf("  ██    ██  ██    ██ ██      ████   ██ ██   ██ ██  ██  ██      ██      ██    ██ ████   ██ \n");
    printf("  ██    ██  ██    ██ ███████ ██ ██  ██ ███████ █████   █████   ███████ ██    ██ ██ ██  ██ \n");
    printf("  ██    ██   ██  ██       ██ ██  ██ ██ ██   ██ ██  ██  ██           ██ ██    ██ ██  ██ ██ \n");
    printf("   ██████     ████   ███████ ██   ████ ██   ██ ██   ██ ███████ ███████  ██████  ██   ████ \n");
    printf("\n");
    setColor(COR_BRANCA);
}

void mostrar_menu() {
    printf("\n");
    setColor(COR_VERDE);
    printf("╔══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                MENU PRINCIPAL                                ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════╣\n");
    setColor(COR_AMARELA);
    printf("║  1. FACIL    - Velocidade lenta, ideal para iniciantes                      ║\n");
    printf("║  2. MEDIO    - Velocidade moderada, bom desafio                             ║\n");
    printf("║  3. DIFICIL  - Velocidade rapida, para experts                              ║\n");
    printf("║  0. SAIR                                                                     ║\n");
    setColor(COR_VERDE);
    printf("╚══════════════════════════════════════════════════════════════════════════════╝\n");
    setColor(COR_BRANCA);
}

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void configurar_terminal() {
    // Configurações para terminal maior
    char command[100];
    sprintf(command, "mode con: cols=%d lines=%d", LARGURA + 10, ALTURA + 10);
    system(command);
    
    // Esconder o cursor
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void limpar_tela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void inicializar_jogo(JogoSnake* jogo, Dificuldade dificuldade) {
    jogo->cabeca = NULL;
    jogo->cauda = NULL;
    
    // Posição inicial da cobra no centro do mapa maior
    Posicao pos_inicial = {LARGURA / 2, ALTURA / 2};
    adicionar_segmento(jogo, pos_inicial);
    
    pos_inicial.x--;
    adicionar_segmento(jogo, pos_inicial);
    
    pos_inicial.x--;
    adicionar_segmento(jogo, pos_inicial);
    
    // Configurações iniciais baseadas na dificuldade
    jogo->direcao = DIREITA;
    jogo->pontuacao = 0;
    jogo->jogo_ativo = 1;
    jogo->crescer_proximo_movimento = 0;
    jogo->comidas_coletadas = 0;
    jogo->dificuldade = dificuldade;
    
    // Velocidade baseada na dificuldade
    switch (dificuldade) {
        case FACIL:
            jogo->velocidade = VELOCIDADE_BASE;
            break;
        case MEDIO:
            jogo->velocidade = VELOCIDADE_BASE - 50;
            break;
        case DIFICIL:
            jogo->velocidade = VELOCIDADE_BASE - 100;
            break;
    }
    
    gerar_comida(jogo);
}

void adicionar_segmento(JogoSnake* jogo, Posicao pos) {
    NoSnake* novo_no = (NoSnake*)malloc(sizeof(NoSnake));
    novo_no->pos = pos;
    novo_no->proximo = NULL;
    
    if (jogo->cabeca == NULL) {
        jogo->cabeca = novo_no;
        jogo->cauda = novo_no;
    } else {
        jogo->cauda->proximo = novo_no;
        jogo->cauda = novo_no;
    }
}

void desenhar_mapa(JogoSnake* jogo) {
    // Inicializar mapa vazio
    for (int y = 0; y < ALTURA; y++) {
        for (int x = 0; x < LARGURA; x++) {
            jogo->mapa[y][x] = ' ';
        }
    }
    
    // Desenhar bordas
    for (int x = 0; x < LARGURA; x++) {
        jogo->mapa[0][x] = '#';
        jogo->mapa[ALTURA-1][x] = '#';
    }
    for (int y = 0; y < ALTURA; y++) {
        jogo->mapa[y][0] = '#';
        jogo->mapa[y][LARGURA-1] = '#';
    }
    
    // Desenhar comida
    jogo->mapa[jogo->comida.y][jogo->comida.x] = '*';
    
    // Desenhar cobra
    NoSnake* atual = jogo->cabeca;
    int primeiro = 1;
    while (atual != NULL) {
        if (primeiro) {
            jogo->mapa[atual->pos.y][atual->pos.x] = 'O';
            primeiro = 0;
        } else {
            jogo->mapa[atual->pos.y][atual->pos.x] = 'o';
        }
        atual = atual->proximo;
    }
    
    // Cabeçalho com informações do jogo
    setColor(COR_CIANO);
    printf("GUSNAKESON");
    
    // Painel de score no canto superior direito
    int scoreX = LARGURA - 25;
    gotoxy(scoreX, 0);
    setColor(COR_AMARELA);
    printf("SCORE: %05d", jogo->pontuacao);
    
    gotoxy(scoreX, 1);
    setColor(COR_VERDE);
    printf("DIFICULDADE: %s", obter_nome_dificuldade(jogo->dificuldade));
    
    gotoxy(scoreX, 2);
    setColor(COR_ROXA);
    printf("COMIDAS: %03d", jogo->comidas_coletadas);
    
    gotoxy(scoreX, 3);
    setColor(COR_CIANO);
    printf("VELOCIDADE: %d", jogo->velocidade);
    
    // Voltar para desenhar o mapa
    gotoxy(0, 4);
    
    // Imprimir mapa com cores
    for (int y = 0; y < ALTURA; y++) {
        for (int x = 0; x < LARGURA; x++) {
            char c = jogo->mapa[y][x];
            
            if (c == '#') {
                setColor(COR_VERDE);
                printf("%c", c);
            }
            else if (c == 'O' || c == 'o') {
                setColor(COR_ROXA);
                printf("%c", c);
            }
            else if (c == '*') {
                setColor(COR_AMARELA);
                printf("%c", c);
            }
            else {
                setColor(COR_BRANCA);
                printf("%c", c);
            }
        }
        printf("\n");
    }
    
    // Rodapé com controles
    setColor(COR_BRANCA);
    printf("Controles: W-A-S-D | X: Sair | Comida: +5 pontos");
    printf("          \n");
}

void processar_input(JogoSnake* jogo) {
    if (_kbhit()) {
        char tecla = _getch();
        
        switch (tecla) {
            case 'w':
            case 'W':
                if (jogo->direcao != BAIXO)
                    jogo->direcao = CIMA;
                break;
            case 's':
            case 'S':
                if (jogo->direcao != CIMA)
                    jogo->direcao = BAIXO;
                break;
            case 'a':
            case 'A':
                if (jogo->direcao != DIREITA)
                    jogo->direcao = ESQUERDA;
                break;
            case 'd':
            case 'D':
                if (jogo->direcao != ESQUERDA)
                    jogo->direcao = DIREITA;
                break;
            case 'x':
            case 'X':
                jogo->jogo_ativo = 0;
                break;
        }
    }
}

void atualizar_jogo(JogoSnake* jogo) {
    Posicao nova_pos = jogo->cabeca->pos;
    
    switch (jogo->direcao) {
        case CIMA: nova_pos.y--; break;
        case BAIXO: nova_pos.y++; break;
        case ESQUERDA: nova_pos.x--; break;
        case DIREITA: nova_pos.x++; break;
    }
    
    if (verificar_colisao(jogo, nova_pos)) {
        jogo->jogo_ativo = 0;
        return;
    }
    
    if (nova_pos.x == jogo->comida.x && nova_pos.y == jogo->comida.y) {
        jogo->pontuacao += 5; // +5 pontos por comida
        jogo->comidas_coletadas++;
        jogo->crescer_proximo_movimento = 1;
        
        // Aumentar velocidade conforme coleta mais comidas
        switch (jogo->dificuldade) {
            case FACIL:
                if (jogo->velocidade > 100) jogo->velocidade -= 2;
                break;
            case MEDIO:
                if (jogo->velocidade > 80) jogo->velocidade -= 3;
                break;
            case DIFICIL:
                if (jogo->velocidade > 60) jogo->velocidade -= 5;
                break;
        }
        
        gerar_comida(jogo);
    }
    
    mover_cobra(jogo);
    jogo->cabeca->pos = nova_pos;
}

void mover_cobra(JogoSnake* jogo) {
    if (jogo->cabeca == NULL) return;
    
    if (jogo->crescer_proximo_movimento) {
        Posicao nova_cauda_pos;
        
        if (jogo->cauda == jogo->cabeca) {
            nova_cauda_pos = jogo->cauda->pos;
            switch (jogo->direcao) {
                case CIMA: nova_cauda_pos.y++; break;
                case BAIXO: nova_cauda_pos.y--; break;
                case ESQUERDA: nova_cauda_pos.x++; break;
                case DIREITA: nova_cauda_pos.x--; break;
            }
        } else {
            NoSnake* atual = jogo->cabeca;
            NoSnake* penultimo = NULL;
            
            while (atual->proximo != NULL) {
                penultimo = atual;
                atual = atual->proximo;
            }
            
            nova_cauda_pos = jogo->cauda->pos;
            
            if (penultimo != NULL) {
                if (penultimo->pos.x == jogo->cauda->pos.x) {
                    if (penultimo->pos.y < jogo->cauda->pos.y) {
                        nova_cauda_pos.y++;
                    } else {
                        nova_cauda_pos.y--;
                    }
                } else {
                    if (penultimo->pos.x < jogo->cauda->pos.x) {
                        nova_cauda_pos.x++;
                    } else {
                        nova_cauda_pos.x--;
                    }
                }
            }
        }
        
        adicionar_segmento(jogo, nova_cauda_pos);
        jogo->crescer_proximo_movimento = 0;
        
    } else {
        if (jogo->cabeca->proximo == NULL) return;
        
        NoSnake* atual = jogo->cauda;
        NoSnake* anterior = NULL;
        
        NoSnake* temp = jogo->cabeca;
        while (temp->proximo != atual) {
            temp = temp->proximo;
        }
        anterior = temp;
        
        while (anterior != NULL) {
            atual->pos = anterior->pos;
            atual = anterior;
            
            if (anterior == jogo->cabeca) {
                anterior = NULL;
            } else {
                temp = jogo->cabeca;
                while (temp->proximo != anterior) {
                    temp = temp->proximo;
                }
                anterior = temp;
            }
        }
    }
}

int verificar_colisao(JogoSnake* jogo, Posicao nova_pos) {
    if (nova_pos.x <= 0 || nova_pos.x >= LARGURA-1 || 
        nova_pos.y <= 0 || nova_pos.y >= ALTURA-1) {
        return 1;
    }
    
    NoSnake* atual = jogo->cabeca->proximo;
    while (atual != NULL) {
        if (atual->pos.x == nova_pos.x && atual->pos.y == nova_pos.y) {
            return 1;
        }
        atual = atual->proximo;
    }
    
    return 0;
}

void gerar_comida(JogoSnake* jogo) {
    int posicao_valida = 0;
    int tentativas = 0;
    
    while (!posicao_valida && tentativas < 100) {
        jogo->comida.x = (rand() % (LARGURA - 2)) + 1;
        jogo->comida.y = (rand() % (ALTURA - 2)) + 1;
        
        posicao_valida = 1;
        NoSnake* atual = jogo->cabeca;
        while (atual != NULL) {
            if (atual->pos.x == jogo->comida.x && atual->pos.y == jogo->comida.y) {
                posicao_valida = 0;
                break;
            }
            atual = atual->proximo;
        }
        tentativas++;
    }
    
    if (!posicao_valida) {
        jogo->comida.x = LARGURA / 4;
        jogo->comida.y = ALTURA / 4;
    }
}

void liberar_memoria(JogoSnake* jogo) {
    NoSnake* atual = jogo->cabeca;
    while (atual != NULL) {
        NoSnake* proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    jogo->cabeca = NULL;
    jogo->cauda = NULL;
}