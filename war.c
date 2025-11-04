// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
//
// ============================================================================

// Inclusão das bibliotecas padrão necessárias para entrada/saída, alocação de memória, manipulação de strings e tempo.

// --- Constantes Globais ---
// Definem valores fixos para o número de territórios, missões e tamanho máximo de strings, facilitando a manutenção.

// --- Estrutura de Dados ---
// Define a estrutura para um território, contendo seu nome, a cor do exército que o domina e o número de tropas.

// --- Protótipos das Funções ---
// Declarações antecipadas de todas as funções que serão usadas no programa, organizadas por categoria.
// Funções de setup e gerenciamento de memória:
// Funções de interface com o usuário:
// Funções de lógica principal do jogo:
// Função utilitária:

// --- Função Principal (main) ---
// Função principal que orquestra o fluxo do jogo, chamando as outras funções em ordem.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define MAX_TERRITORIES 42
#define MAX_NAME_LEN 64
#define MAX_MISSIONS 3

typedef struct {
    char name[MAX_NAME_LEN];
    char owner_color[16];
    int troops;
} Territory;

/* Protótipos */
Territory *allocateMap(size_t n);
void initializeTerritories(Territory *map, size_t n);
void freeMap(Territory *map);
void displayMenu(void);
void displayMap(const Territory *map, size_t n);
void displayMission(int missionId);
void attackPhase(Territory *map, size_t n);
void simulateAttack(Territory *a, Territory *b);
int drawMission(void);
int checkVictory(const Territory *map, size_t n, int missionId, const char *playerColor);
void clearInputBuffer(void);

int main(void) {
    setlocale(LC_ALL, "");
    srand((unsigned)time(NULL));

    Territory *map = allocateMap(MAX_TERRITORIES);
    if (!map) {
        fprintf(stderr, "Falha ao alocar mapa\n");
        return EXIT_FAILURE;
    }

    initializeTerritories(map, MAX_TERRITORIES);

    const char playerColor[] = "VERMELHO"; /* exemplo */
    int mission = drawMission();
    int choice;

    do {
        displayMap(map, MAX_TERRITORIES);
        displayMission(mission);
        displayMenu();

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            choice = -1;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                attackPhase(map, MAX_TERRITORIES);
                break;
            case 2:
                if (checkVictory(map, MAX_TERRITORIES, mission, playerColor)) {
                    printf("Missão cumprida! Você venceu!\n");
                    choice = 0; /* encerra o loop */
                } else {
                    printf("Missão ainda não cumprida.\n");
                }
                break;
            case 0:
                printf("Saindo do jogo...\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
        printf("\nPressione Enter para continuar...");
        getchar();
    } while (choice != 0);

    freeMap(map);
    return 0;
}

/* Implementações mínimas */

Territory *allocateMap(size_t n) {
    Territory *m = calloc(n, sizeof(Territory));
    return m;
}

void initializeTerritories(Territory *map, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        snprintf(map[i].name, MAX_NAME_LEN, "Território %zu", i + 1);
        strncpy(map[i].owner_color, "NEUTRO", sizeof(map[i].owner_color)-1);
        map[i].owner_color[sizeof(map[i].owner_color)-1] = '\0';
        map[i].troops = 1 + rand() % 10;
    }
}

void freeMap(Territory *map) {
    free(map);
}

void displayMenu(void) {
    puts("Menu:");
    puts("1 - Fase de ataque");
    puts("2 - Verificar missão");
    puts("0 - Sair");
    printf("Escolha: ");
}

void displayMap(const Territory *map, size_t n) {
    puts("Mapa de territórios:");
    puts("Idx  Nome                 Dono       Tropas");
    for (size_t i = 0; i < n; ++i) {
        printf("%2zu   %-20s %-9s %3d\n", i, map[i].name, map[i].owner_color, map[i].troops);
    }
}

void displayMission(int missionId) {
    printf("Missão (id=%d): ", missionId);
    switch (missionId) {
        case 0: puts("Destruir o exército AZUL"); break;
        case 1: puts("Conquistar 10 territórios"); break;
        case 2: puts("Destruir o exército VERDE"); break;
        default: puts("Missão desconhecida"); break;
    }
}

void attackPhase(Territory *map, size_t n) {
    printf("Fase de ataque: (modo simplificado)\n");
    size_t src, dst;
    printf("Índice do território de origem: ");
    if (scanf("%zu", &src) != 1) { clearInputBuffer(); return; }
    printf("Índice do território de destino: ");
    if (scanf("%zu", &dst) != 1) { clearInputBuffer(); return; }
    clearInputBuffer();

    if (src >= n || dst >= n) {
        puts("Índices inválidos.");
        return;
    }
    if (src == dst) {
        puts("Origem e destino devem ser diferentes.");
        return;
    }
    if (map[src].troops < 2) {
        puts("Território de origem precisa ter pelo menos 2 tropas para atacar.");
        return;
    }
    simulateAttack(&map[src], &map[dst]);
}

void simulateAttack(Territory *a, Territory *b) {
    int rollA = 1 + rand() % 6;
    int rollB = 1 + rand() % 6;
    printf("Ataque: %s (%d tropas) vs %s (%d tropas)\n", a->name, a->troops, b->name, b->troops);
    printf("Rolagem A=%d  B=%d\n", rollA, rollB);
    if (rollA > rollB) {
        b->troops -= 1;
        printf("%s perde 1 tropa\n", b->name);
        if (b->troops <= 0) {
            /* conquista */
            strncpy(b->owner_color, a->owner_color, sizeof(b->owner_color)-1);
            b->owner_color[sizeof(b->owner_color)-1] = '\0';
            b->troops = 1;
            a->troops -= 1;
            printf("%s conquistado por %s\n", b->name, b->owner_color);
        }
    } else {
        a->troops -= 1;
        printf("%s perde 1 tropa\n", a->name);
        if (a->troops < 1) a->troops = 1;
    }
}

int drawMission(void) {
    return rand() % MAX_MISSIONS;
}

int checkVictory(const Territory *map, size_t n, int missionId, const char *playerColor) {
    if (missionId == 1) { /* conquistar 10 territórios */
        int count = 0;
        for (size_t i = 0; i < n; ++i) {
            if (strcmp(map[i].owner_color, playerColor) == 0) ++count;
        }
        return count >= 10;
    }
    if (missionId == 0) { /* destruir AZUL */
        for (size_t i = 0; i < n; ++i) {
            if (strcmp(map[i].owner_color, "AZUL") == 0) return 0;
        }
        return 1;
    }
    if (missionId == 2) { /* destruir VERDE */
        for (size_t i = 0; i < n; ++i) {
            if (strcmp(map[i].owner_color, "VERDE") == 0) return 0;
        }
        return 1;
    }
    return 0;
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}
