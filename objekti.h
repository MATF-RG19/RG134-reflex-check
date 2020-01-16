#ifndef OBJEKTI_H
#define OBJEKTI_H

#include <stdbool.h>

#define MAX_PREPREKA 30

typedef struct {
    float x;
    float z;
}Metak;

typedef struct {
    float x;
    float z;
    bool is_pogodjena;
}Prepreka;

void iscrtaj_ose(); 
void iscrtaj_pistolj();
void iscrtaj_metak();
Prepreka napravi_prepreke();
void inicijalizuj_prepreke();
void azuriraj_prepreke();
void nacrtaj_prepreke();
void iscrtaj_zid();
void ispisi_tekst(char * tekst, int x, int y, float r, float g, float b, int sirina_ekrana, int duzina_ekrana);

#endif
