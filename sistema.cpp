#include "sistema.h"

//Cerrojos
omp_lock_t cerrojoAparcado, cerrojoEspera;

bool parar;

sistema::sistema(int numCoches) {
    //Inicializar plazas
    for (int i = 0; i < NUM_PLAZAS; i++) {
        p[i].num = i;
        p[i].idCoche = -1;
        p[i].ocupado = false;
    }
    this->lastId = 1;
    this->coches = new vector<coche *>();
    //Inicializamos coches
    for (int i = 0; i < numCoches; i++) {
        coche *newCoche = new coche;
        newCoche->id = this->lastId;
        newCoche->estado = OCIOSO;
        this->coches->push_back(newCoche);
        this->lastId++;

    }
    this->numCoches = numCoches;
}

void sistema::ejecutar() {
    bool parar = false;
    omp_init_lock(&cerrojoAparcado);
    omp_init_lock(&cerrojoEspera);

#pragma omp parallel for num_threads(21)
    for (int i = 0; i < 21; i++) {
        if (omp_get_thread_num() == 0) {
            this->menu();
        } else {
            mover(this->coches->at(i - 1));
        }
    }
}

void sistema::menu() {
    while (true) {
        char opcion;
        cout << "\nOpciones:\n"
                "(P) -Parar y Continuar el sistema\n"
                "(I) -Imprimir el sistema\n"
                "(A) -Añadir coche\n"
                "(S) -Salir" << endl;

        cin >> opcion;

        switch (opcion) {
            case 'P':
            case 'p':
                parar = !parar;
                break;
            case 'I':
            case 'i':
                this->imprimirParking();
                this->imprimirCoches();
                break;
            case 'A':
            case 'a':
                this->addCoche();
                break;
            case 'S':
            case 's':
                exit(0);
            default:
                cout << "Error! No existe la opción";
                break;
        }
    }


}

void sistema::mover(coche *coche) {
    int randTime;
    while (!parar) {
        switch (coche->estado) {
            case APARCADO:
                randTime = rand() % (MIN - +MAX) + MIN;
                usleep(randTime);
                if (!parar) {
                    omp_set_lock(&cerrojoAparcado);
                    this->salir(coche);
                    omp_unset_lock(&cerrojoAparcado);
                }
                break;
            case ESPERA:
                omp_set_lock(&cerrojoEspera);
                aparcar(coche);
                omp_unset_lock(&cerrojoEspera);
                break;
            case OCIOSO:
                randTime = rand() % (MIN - +MAX) + MIN;
                if (!parar) {
                    usleep(randTime);
                    coche->estado = ESPERA;
                }
                break;
        }
    }

    while (parar) {
        asm("nop");   //Sistema parado
    }
}


void sistema::aparcar(coche *coche) {
    plaza *plazaLibre = bucarPlazaLibre();
    if (plazaLibre) {
        plazaLibre->ocupado = true;
        plazaLibre->idCoche = coche->id;
        coche->estado = APARCADO;
    } else {
        coche->estado = ESPERA;
    }
}

void sistema::salir(coche *coche) {
    plaza *plazaCoche = buscarPlazaCoche(coche);
    if (plazaCoche) {
        plazaCoche->idCoche = -1;
        plazaCoche->ocupado = false;
        coche->estado = OCIOSO;
    }
}

plaza *sistema::bucarPlazaLibre() {
    for (int i = 0; i < NUM_PLAZAS; i++) {
        if (!p[i].ocupado) {
            return &p[i];
        }
    }
    return NULL;
}

plaza *sistema::buscarPlazaCoche(coche *coche) {
    for (int i = 0; i < NUM_PLAZAS; i++) {
        if (p[i].idCoche == coche->id) {
            return &p[i];
        }
    }
    return NULL;
}

void sistema::addCoche() {
    coche *newCoche = new coche;
    newCoche->id = this->lastId;
    newCoche->estado = OCIOSO;
    this->coches->push_back(newCoche);
    this->lastId++;
    this->numCoches++;
#pragma omp parallel for num_threads(2)
    for (int i = 0; i < 2; i++) {
        if (omp_get_thread_num() == 0) {
            mover(newCoche);
        } else {
            this->menu();
        }
    }

}

int sistema::getNumCoche() {
    return this->numCoches;
}

void sistema::imprimirParking() {
    for (int i = 0; i < NUM_PLAZAS; i++) {
        cout << "Plaza " << p[i].num << ": ";
        if (p[i].ocupado) {
            cout << "Coche " << p[i].idCoche << endl;
        } else {
            cout << "Libre" << endl;
        }
    }
}

void sistema::imprimirCoches() {
    for (auto c : *(this->coches)) {
        if (c->estado == OCIOSO || c->estado == ESPERA) {
            cout << "Coche " << c->id << " " << (c->estado == OCIOSO ? "Ocioso" : "Espera") << endl;
        }
    }
}