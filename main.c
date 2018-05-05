/*
    Davide Segullo

    formato data: gg/mm/aaaa
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Farmaco{
    char nome[20];
    struct tm data_scadenza;
    int disponibilita;
    float prezzo;
} Farmaco;

typedef struct Nodo {
    Farmaco farm;
    struct Nodo *next;
}Nodo;

void creaLista(Nodo **lista, Farmaco f){
    *lista = (Nodo*)malloc(sizeof(Nodo));
    (*lista)->farm = f;
    (*lista)->next = NULL;
}

void inserimentoOrdinato(Nodo **lista, Farmaco f){

    if (lista == NULL) {
        creaLista(lista,f);
        return;
    }

    Nodo *current = NULL;
    Nodo *successive = NULL;

    successive = *lista;

    while (successive != NULL && strcmp(successive->farm.nome,f.nome) < 0){

        if (strcmp(successive->farm.nome,f.nome) == 0){
            successive->farm.disponibilita += f.disponibilita;
            return;
        }

        current = successive;
        successive = successive->next;
    }

    Nodo *p = (Nodo*)malloc(sizeof(Nodo));
    p->farm = f;
    p->next = NULL;

    if (current == NULL){
        p->next = *lista;
        *lista = p;
        return;
    }

    if (successive == NULL){
        current->next = p;
        return;
    }

    p->next = successive;
    current->next = p;
}

void initList(Nodo **lista){

    FILE *file;
    Farmaco f;

    file = fopen("C:/Users/theda/Desktop/Progetti C/Esercizio_Farmacia_N1/bin/Debug/farmaci.txt", "r");

    if (file == NULL) {
        printf("FILE NOT FOUND");
        return;
    }

    while (!feof(file)) {
        fscanf(file, "%s\t%d/%d/%d\t%d\t%f\n", f.nome, &f.data_scadenza.tm_mday, &f.data_scadenza.tm_mon, &f.data_scadenza.tm_year, &f.disponibilita, &f.prezzo);
        f.data_scadenza.tm_hour = f.data_scadenza.tm_min = f.data_scadenza.tm_sec = 0;
        inserimentoOrdinato(lista, f);
    }

    fclose(file);
}

void saveFile(Nodo *lista){
    FILE *file;

    file = fopen("C:/Users/theda/Desktop/Progetti C/Esercizio_Farmacia_N1/bin/Debug/farmaci.txt", "w");

    if (file == NULL) {
        printf("FILE NOT FOUND");
        return;
    }

    Nodo *p = NULL;

    for(p = lista; p!=NULL; p=p->next)
        fprintf(file, "%s\t%d/%d/%d\t%d\t%f\n", p->farm.nome, p->farm.data_scadenza.tm_mday, p->farm.data_scadenza.tm_mon, p->farm.data_scadenza.tm_year, p->farm.disponibilita, p->farm.prezzo);

    fclose(file);
}

void stampaLista(Nodo *lista){
    if (lista == NULL)
        return;

    Nodo *p = NULL;

    for(p = lista; p!=NULL; p=p->next)
        printf("Nome: %s\tDisponibilita': %d\tScadenza: %d/%d/%d\n", p->farm.nome,p->farm.disponibilita,p->farm.data_scadenza.tm_mday,p->farm.data_scadenza.tm_mon,p->farm.data_scadenza.tm_year);
}

void stampaListaDisponibilita(Nodo *lista){
    if (lista == NULL)
        return;

    Nodo *p = NULL;

    for(p = lista; p!=NULL; p=p->next)
        if(p->farm.disponibilita>0)
            printf("Nome: %s\tDisponibilita': %d\tScadenza: %d/%d/%d\n", p->farm.nome,p->farm.disponibilita,p->farm.data_scadenza.tm_mday,p->farm.data_scadenza.tm_mon,p->farm.data_scadenza.tm_year);
}

void acquistaFarmaco(Nodo **lista){

    if (lista == NULL){
        printf("Non ci sono farmaci disponibili");
        return;
    }

    int n_farmaci;
    char nome_farmaco[20];

    printf("Quale farmaco vuoi acquistare?\n:");
    scanf("%s",nome_farmaco);
    printf("Quante confezioni vuoi comprare?\n:");
    scanf("%d",&n_farmaci);

    time_t t = time(NULL);
    struct tm loc_time = *localtime(&t);
    loc_time.tm_hour = loc_time.tm_min = loc_time.tm_sec = 0;

    Nodo *farmaco = NULL;

    for(farmaco=*lista;farmaco!=NULL && strcmp(farmaco->farm.nome,nome_farmaco)!=0;farmaco=farmaco->next);

    if(farmaco == NULL){
        printf("Il prodotto cercato non e' presente nel catalogo\n");
        return;
    }

    struct tm tm = farmaco->farm.data_scadenza;
    tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
    tm.tm_mon -= 1;
    tm.tm_year -= 1900;

    if(difftime(mktime(&loc_time),mktime(&tm))<=0){
        if(farmaco->farm.disponibilita>0 && farmaco->farm.disponibilita>=n_farmaci){
                farmaco->farm.disponibilita -= n_farmaci;
                printf("Ha acquistato %d scatole di %s\nPrezzo totale:%.2f\n",n_farmaci,nome_farmaco,(farmaco->farm.prezzo*n_farmaci));
                saveFile(*lista);
        } else
            printf("Non disponibile\n");
    } else
        printf("Farmaco scaduto\n");
}

void inserimentoFarmaco(Nodo **lista){

    Farmaco f;
    printf("Inserire i dati del farmaco\nNome: ");
    scanf("%s",f.nome);
    printf("Data di scadenza (GG/MM/AAAA): ");
    scanf("%d/%d/%d",&f.data_scadenza.tm_mday,&f.data_scadenza.tm_mon,&f.data_scadenza.tm_year);
    printf("Disponibilita': ");
    scanf("%d",&f.disponibilita);
    printf("Prezzo: ");
    scanf("%f",&f.prezzo);

    inserimentoOrdinato(lista,f);
    saveFile(*lista);

}

void cancellaLista(Nodo **lista, char nome[]){

    if (lista == NULL){
        printf("Non ci sono farmaci");
        return;
    }

    Nodo *current = NULL;
    Nodo *successive = NULL;

    for(successive=*lista;successive!=NULL && strcmp(successive->farm.nome,nome)!=0;successive=successive->next)
        current = successive;

    if(successive==NULL){
        printf("Farmaco non trovato");
        return;
    }

    if(current==NULL){
        *lista = successive->next;
    } else
        current->next = (successive->next==NULL) ? NULL : successive->next;

    free(successive);
}

void cancellaFarmaci(Nodo **lista){

    if (lista == NULL){
        printf("Non ci sono farmaci");
        return;
    }

    Nodo *successive = NULL;

    time_t t = time(NULL);
    struct tm loc_time = *localtime(&t);
    loc_time.tm_hour = loc_time.tm_min = loc_time.tm_sec = 0;

    successive = *lista;

    do {
        struct tm tm = successive->farm.data_scadenza;
        tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
        tm.tm_mon -= 1;
        tm.tm_year -= 1900;

        if (difftime(mktime(&loc_time),mktime(&tm))>=0 || successive->farm.disponibilita == 0){
                cancellaLista(lista,successive->farm.nome);
        }

        successive = successive->next;
    } while (successive != NULL);

    saveFile(*lista);
}

int main() {

    Nodo *lista = NULL;
    int scelta;

    initList(&lista);

    do{
        printf("Che operazione vuoi eseguire?\n1.Stampa Farmaci\n2.Stampa Farmaci Disponibilita'\n3.Acquista farmaci\n4.Inserisci farmaco\n5.Elimina farmaco\n6.Esci\n:");
        scanf("%d",&scelta);

        switch(scelta){
        case 1:
            stampaLista(lista);
            break;
        case 2:
            stampaListaDisponibilita(lista);
            break;
        case 3:
            acquistaFarmaco(&lista);
            break;
        case 4:
            inserimentoFarmaco(&lista);
            break;
        case 5:
            cancellaFarmaci(&lista);
            break;
        case 6:
            printf("Arrivederci...");
            break;
        default:
            printf("Operazione non valida");
            break;
        }
    } while(scelta!=6);

    return 0;
}
