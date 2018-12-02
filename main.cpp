#include <iostream>
#include <omp.h>
#include <time.h>
#include <unistd.h>
#include "sistema.h"


using namespace std;

int main() {
    //Inicializar coches
    sistema *parking = new sistema(20);
    parking->ejecutar();
    return 0;
}