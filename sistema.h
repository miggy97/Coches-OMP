#ifndef PARKING_SISTEMA_H
#define PARKING_SISTEMA_H
#include <iostream>
#include <string>
#include <vector>
#include <omp.h>
#include <time.h>
#include <unistd.h>

#define NUM_PLAZAS 10

#define APARCADO 0
#define ESPERA 1
#define OCIOSO 2

#define MIN 1000000
#define MAX 3000000


using namespace std;

typedef struct coche{
    int id;
    int estado; //Aparcado, Ocioso, Espera
};

typedef struct plaza{
    int num;
    bool ocupado;
    int idCoche;
};

class sistema {
private:
    plaza p[NUM_PLAZAS];
    vector <coche*> *coches;
    int numCoches;
    int lastId;

public:
    sistema(int numCoches);
    void ejecutar();
    void mover(coche *coche);
    void menu ();
    void aparcar(coche *coche);
    void salir(coche *coche);
    plaza *bucarPlazaLibre();
    plaza *buscarPlazaCoche(coche *coche);
    void addCoche();
    int getNumCoche();
    void imprimirParking();
    void imprimirCoches();
};


#endif //PARKING_SISTEMA_H
