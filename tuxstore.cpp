/* =============================================================
   Curso      : Estructura de Datos (24UC00428)
   Evaluacion : C2 - Medicion de competencias - Semana 12
   Proyecto   : Tux Store v2 (kit base)
   Institucion: Universidad Continental
   Semestre   : 2026-20
   Compilador : Dev-C++ (MinGW / GCC, C++11 o superior)
   NRC        : 42964
   Grupo      : B
   Integrantes: FERNANDEZ RAMIREZ PATRICK MICHAEL
                Ticona Pacco Alex Royer
                MACHACA APAZA EDSON JAVIER
                Flores Castillo Basiler Emanuel
                QUISPE MOLLENIDO JUAN DIEGO
   ============================================================= */

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

const int DISPONIBLE = 0;
const int EN_FILA    = 1;
const int INSTALADA  = 2;
const string ARCHIVO = "tuxstore.txt";

struct NodoApp {
    string nombre;
    string version;
    int    mb;
    int    prioridad;
    int    estado;
    int    inicio, fin, espera;
    NodoApp *sig;
};

struct NodoFila {
    string nombre;
    int    llegada;
    int    prioridad;
    NodoFila *sig;
};

struct NodoBloque {
    string nombre;
    int    mb;
    NodoBloque *sig;
};

// ===== FICHA ===== (copiar tal cual en tuxstore.cpp)
const string SERVIDOR = "pinguino-emperador";
const int VELOCIDAD = 6;
const int DISCO = 310;
const int N_APPS = 8;
string fNombre[N_APPS] = {"firefox", "neovim", "vlc", "krita", "neofetch",
"fail2ban", "python3", "openssh"};
string fVersion[N_APPS] = {"128", "0.9", "3.0", "5.2", "7.1", "1.0", "3.12",
"9.6"};
int fMb[N_APPS] = {210, 18, 45, 180, 1, 2, 38, 8};
int fPrio[N_APPS] = {2, 2, 3, 3, 3, 1, 2, 1};
// =================

string estadoTexto(int e) {
    if (e == DISPONIBLE) return "disponible";
    if (e == EN_FILA)    return "en fila";
    return "instalada";
}

NodoApp* crearApp(string nombre, string version, int mb, int prioridad) {
    NodoApp *n = new NodoApp;
    n->nombre = nombre; n->version = version; n->mb = mb; n->prioridad = prioridad;
    n->estado = DISPONIBLE;
    n->inicio = n->fin = n->espera = -1;
    n->sig = nullptr;
    return n;
}

void mostrarApp(NodoApp *a) {
    cout << "  " << left << setw(12) << a->nombre << setw(9) << a->version
         << right << setw(5) << a->mb << " MB   prioridad " << a->prioridad
         << "   " << estadoTexto(a->estado) << endl;
}

bool aEntero(string s, int &v) {
    if (s == "") return false;
    int i = 0, signo = 1; v = 0;
    if (s[0] == '-') { signo = -1; i = 1; if (s.size() == 1) return false; }
    for (; i < (int)s.size(); i++) {
        if (s[i] < '0' || s[i] > '9') return false;
        v = v * 10 + (s[i] - '0');
    }
    v *= signo;
    return true;
}

// TODO R1a
NodoApp* buscarApp(NodoApp *catalogo, string nombre) {
    NodoApp *p = catalogo;
    while (p != nullptr) {
        if (p->nombre == nombre) return p;
        p = p->sig;
    }
    return nullptr;
}

// TODO R1b
bool agregarAlFinal(NodoApp *&catalogo, string nombre, string version, int mb, int prioridad) {
    if (nombre == "" || version == "" || mb <= 0 || prioridad < 1 || prioridad > 3) return false;
    if (buscarApp(catalogo, nombre) != nullptr) return false;
    
    NodoApp *nuevo = crearApp(nombre, version, mb, prioridad);
    if (catalogo == nullptr) {
        catalogo = nuevo;
    } else {
        NodoApp *p = catalogo;
        while (p->sig != nullptr) p = p->sig;
        p->sig = nuevo;
    }
    return true;
}

// TODO R2a
void encolarConPrioridad(NodoFila *&frente, NodoFila *&final, string nombre, int llegada, int prioridad) {
    NodoFila *nuevo = new NodoFila;
    nuevo->nombre = nombre; nuevo->llegada = llegada; nuevo->prioridad = prioridad; nuevo->sig = nullptr;

    if (frente == nullptr) {
        frente = final = nuevo;
    } else if (prioridad < frente->prioridad) {
        nuevo->sig = frente;
        frente = nuevo;
    } else {
        NodoFila *p = frente;
        while (p->sig != nullptr && p->sig->prioridad <= prioridad) {
            p = p->sig;
        }
        nuevo->sig = p->sig;
        p->sig = nuevo;
        if (nuevo->sig == nullptr) final = nuevo;
    }
}

void leerOrden(string linea, string &orden, string &a1, string &a2, string &a3, string &a4) {
    string t[8]; int n = 0;
    stringstream ss(linea);
    while (n < 8 && ss >> t[n]) n++;
    orden = a1 = a2 = a3 = a4 = "";
    if (n == 0) return;
    int k = 0;
    if (t[0] == "sudo") k = 1;
    string c = (k < n) ? t[k] : "";
    if (c == "apt" && k + 1 < n) {
        string s = t[k + 1];
        if (s == "list") orden = "catalogo";
        else if (s == "search") orden = "buscar";
        else if (s == "show") orden = "info";
        else if (s == "install") orden = "instalar";
        else if (s == "purge") orden = "retirar";
        else orden = "apt " + s;
        k += 2;
    } else if (c == "dnf" && k + 2 < n && t[k + 1] == "history" && t[k + 2] == "undo") {
        orden = "deshacer"; k = n;
    } else if (c == "dnf" && k + 1 < n && t[k + 1] == "history") {
        orden = "historial"; k = n;
    } else if (c == "dpkg" && k + 1 < n && t[k + 1] == "--get-selections") { orden = "guardar"; k = n; }
    else if (c == "dpkg" && k + 1 < n && t[k + 1] == "--set-selections") { orden = "cargar"; k = n; }
    else if (c == "df")   { orden = "espacio"; k = n; }
    else if (c == "man" || c == "help" || c == "--help") { orden = "ayuda"; k = n; }
    else if (c == "exit") { orden = "salir"; k = n; }
    else { orden = c; k++; }
    if (k < n) a1 = t[k];
    if (k + 1 < n) a2 = t[k + 1];
    if (k + 2 < n) a3 = t[k + 2];
    if (k + 3 < n) a4 = t[k + 3];
}

void ayuda() {
    cout << "  ORDEN (espanol)            EN LINUX                     QUE HACE" << endl;
    cout << "  catalogo                   apt list                     ver todas las apps" << endl;
    cout << "  buscar <texto>             apt search <texto>           buscar por nombre" << endl;
    cout << "  info <app>                 apt show <app>               ficha de una app" << endl;
    cout << "  agregar <app> <v> <MB> <p> -                            nueva app (p = 1, 2 o 3)" << endl;
    cout << "  prioridad <app> <p>        -                            cambiar la prioridad" << endl;
    cout << "  retirar <app>              sudo apt purge <app>         borrar del catalogo" << endl;
    cout << "  instalar <app> <seg>       sudo apt install <app> <seg> pedir una app" << endl;
    cout << "  fila                       -                            ver la fila de descargas" << endl;
    cout << "  procesar                   -                            descargar la del frente" << endl;
    cout << "  historial                  dnf history                  bloques de disco (pila)" << endl;
    cout << "  deshacer                   dnf history undo last        liberar el ultimo bloque" << endl;
    cout << "  espacio                    df                           estado del disco" << endl;
    cout << "  guardar                    dpkg --get-selections        guardar en tuxstore.txt" << endl;
    cout << "  cargar                     dpkg --set-selections        cargar tuxstore.txt" << endl;
    cout << "  reporte                    -                            resumen del turno" << endl;
    cout << "  salir                      exit                         terminar" << endl;
}

int main() {
    NodoApp    *catalogo = nullptr;
    NodoFila   *frente = nullptr, *final = nullptr;
    NodoBloque *tope = nullptr;

    int reloj = 0, libre = DISCO, ultimaLlegada = 0;
    int instalaciones = 0, sumaEspera = 0, mbDescargados = 0;

    for (int i = 0; i < N_APPS; i++) agregarAlFinal(catalogo, fNombre[i], fVersion[i], fMb[i], fPrio[i]);

    cout << "Tux Store - servidor " << SERVIDOR << " (" << VELOCIDAD << " MB/s, "
         << DISCO << " MB). Escribe ayuda." << endl;

    string linea;
    while (true) {
        cout << "tux@" << SERVIDOR << ":~$ ";
        if (!getline(cin, linea)) break;
        string orden, a1, a2, a3, a4;
        leerOrden(linea, orden, a1, a2, a3, a4);
        if (orden == "") continue;
        cout << "> " << linea << endl;

        if (orden == "catalogo") {
            // TODO R1c
            if (catalogo == nullptr) cout << "(catalogo vacio)\n";
            else {
                NodoApp *p = catalogo;
                while (p != nullptr) { mostrarApp(p); p = p->sig; }
            }
        }
        else if (orden == "buscar") {
            // TODO R1d
            bool hallado = false;
            NodoApp *p = catalogo;
            while (p != nullptr) {
                if (p->nombre.find(a1) != string::npos) { mostrarApp(p); hallado = true; }
                p = p->sig;
            }
            if (!hallado) cout << "Sin resultados para " << a1 << "\n";
        }
        else if (orden == "info") {
            // TODO R1e
            NodoApp *app = buscarApp(catalogo, a1);
            if (app == nullptr) cout << "E: No existe la app " << a1 << "\n";
            else {
                mostrarApp(app);
                if (app->estado == INSTALADA) cout << "  inicio " << app->inicio << " | fin " << app->fin << " | espera " << app->espera << "\n";
            }
        }
        else if (orden == "agregar") {
            // TODO R1f
            int mb, prio;
            if (a1 == "" || a2 == "" || !aEntero(a3, mb) || !aEntero(a4, prio)) cout << "Uso: agregar <app> <version> <MB> <prioridad>\n";
            else {
                if (agregarAlFinal(catalogo, a1, a2, mb, prio)) cout << "Agregada: " << a1 << "\n";
                else cout << "E: No se pudo agregar " << a1 << "\n";
            }
        }
        else if (orden == "instalar") {
            // TODO R2b
            int seg;
            if (a1 == "" || !aEntero(a2, seg)) cout << "Uso: instalar <app> <seg>\n";
            else {
                NodoApp *app = buscarApp(catalogo, a1);
                if (app == nullptr) cout << "E: No existe la app " << a1 << "\n";
                else if (app->estado != DISPONIBLE) cout << "E: La app " << a1 << " ya esta " << estadoTexto(app->estado) << "\n";
                else if (seg < ultimaLlegada || seg < 0) cout << "E: El segundo debe ser >= " << ultimaLlegada << "\n";
                else {
                    encolarConPrioridad(frente, final, a1, seg, app->prioridad);
                    app->estado = EN_FILA;
                    ultimaLlegada = seg;
                }
            }
        }
        else if (orden == "fila") {
            // TODO R2c
            if (frente == nullptr) cout << "(fila vacia)\n";
            else {
                cout << "  frente -> ";
                NodoFila *p = frente;
                while (p != nullptr) {
                    cout << p->nombre << " (" << p->prioridad << ") -> ";
                    p = p->sig;
                }
                cout << "final\n";
            }
        }
        else if (orden == "procesar") {
            // TODO R3
            if (frente == nullptr) cout << "La fila esta vacia\n";
            else {
                NodoFila *nodo = frente;
                frente = frente->sig;
                if (frente == nullptr) final = nullptr;

                NodoApp *app = buscarApp(catalogo, nodo->nombre);
                if (app->mb > libre) {
                    cout << "E: No hay espacio suficiente para " << app->nombre << ": necesita " << app->mb << " MB, libres " << libre << " MB\n";
                    app->estado = DISPONIBLE;
                } else {
                    int descarga = (app->mb + VELOCIDAD - 1) / VELOCIDAD;
                    int inicio = max(reloj, nodo->llegada);
                    app->inicio = inicio;
                    app->fin = inicio + descarga;
                    app->espera = inicio - nodo->llegada;
                    
                    reloj = app->fin;
                    libre -= app->mb;
                    app->estado = INSTALADA;
                    instalaciones++;
                    sumaEspera += app->espera;
                    mbDescargados += app->mb;
                    
                    cout << app->nombre << ": descarga " << descarga << " s | inicio " << app->inicio << " | fin " << app->fin << "\n";
                    
                    NodoBloque *nb = new NodoBloque;
                    nb->nombre = app->nombre; nb->mb = app->mb; nb->sig = tope;
                    tope = nb;
                }
                delete nodo;
            }
        }
        else if (orden == "historial") {
            // TODO R4a
           if (tope == nullptr) {
                cout << "(historial vacio)" << endl;
            } else {
                NodoBloque *aux = tope;
                while (aux != nullptr) {
                    cout << "  " << left << setw(12) << aux->nombre << right << setw(5) << aux->mb << " MB" << endl;
                    aux = aux->sig;
                }
           }
        }
        else if (orden == "deshacer") {
            // TODO R4b
           if (tope == nullptr) {
              cout << "Nada que deshacer" << endl;
        }  else {
             NodoBloque *borrar = tope;
              tope = tope->sig;
              NodoApp *app = buscarApp(catalogo, borrar->nombre);
              if (app != nullptr) {
                 app->estado = DISPONIBLE;
                 app->inicio = app->fin = app->espera = -1;
              }
              libre = libre + borrar->mb;
              cout << "Deshecho: " << borrar->nombre << " (" <<borrar->mb << " MB liberados, libres " << libre << " MB)" << endl;
              delete borrar;
           }
        }
           
        else if (orden == "espacio") {
            // TODO R4c
           int cantBloques = 0;
            NodoBloque *aux = tope;
            while (aux != nullptr) {
                cantBloques++;
                aux = aux->sig;
            }
            int mbOcupados = DISCO - libre;
            cout << "Disco: total " << DISCO << " MB | ocupado " << mbOcupados
                 << " MB | libres " << libre << " MB | bloques " << cantBloques << endl;
        }
        else if (orden == "prioridad") {
            // TODO R5a
            int p;
            if (a1 == "" || !aEntero(a2, p) || p < 1 || p > 3) cout << "Uso: prioridad <app> <p>\n";
            else {
                NodoApp *app = buscarApp(catalogo, a1);
                if (app == nullptr) cout << "E: No existe la app\n";
                else if (app->estado == EN_FILA) cout << "E: No se puede cambiar la prioridad si esta en la fila\n";
                else { app->prioridad = p; cout << "Prioridad actualizada\n"; }
            }
        }
        else if (orden == "retirar") {
            // TODO R5b
            if (a1 == "") cout << "Uso: retirar <app>\n";
            else {
                NodoApp *ant = nullptr;
                NodoApp *act = catalogo;
                while (act != nullptr && act->nombre != a1) {
                    ant = act;
                    act = act->sig;
                }
                if (act == nullptr) cout << "E: No existe la app\n";
                else if (act->estado != DISPONIBLE) cout << "E: Solo se retira una app disponible\n";
                else {
                    if (ant == nullptr) catalogo = act->sig;
                    else ant->sig = act->sig;
                    delete act;
                    cout << "Retirada: " << a1 << "\n";
                }
            }
        }
        else if (orden == "guardar") {
            if (frente != nullptr) cout << "E: Procesen la fila antes de guardar" << endl;
            else {
                ofstream f(ARCHIVO);
                int n = 0, m = 0;
                // TODO R6a
                NodoApp *p = catalogo;
                while (p != nullptr) {
                    f << "A " << p->nombre << " " << p->version << " " << p->mb << " "
                      << p->prioridad << " " << p->estado << endl;
                    n++;
                    p = p->sig;
                }
                NodoBloque *b = tope;
                while (b != nullptr) {
                    f << "B " << b->nombre << " " << b->mb << endl;
                    m++;
                    b = b->sig;
                }               
                f.close();
                cout << "Guardado: " << n << " apps y " << m << " bloques en " << ARCHIVO << endl;
            }
        }
        else if (orden == "cargar") {
            ifstream f(ARCHIVO);
            if (!f) cout << "E: No existe " << ARCHIVO << endl;
            else if (frente != nullptr) cout << "E: Procesen la fila antes de cargar" << endl;
            else {
                int n = 0, m = 0;
                // TODO R6b
                while (catalogo != nullptr) {
                    NodoApp *aux = catalogo;
                    catalogo = catalogo->sig;
                    delete aux;
                }
                while (tope != nullptr) {
                    NodoBloque *aux = tope;
                    tope = tope->sig;
                    delete aux;
                }

                libre = DISCO;
                NodoBloque *finPila = nullptr;
                char letra;
                while (f >> letra) {
                    if (letra == 'A') {
                        string nom, ver;
                        int mb, prio, est;
                        f >> nom >> ver >> mb >> prio >> est;
                        if (agregarAlFinal(catalogo, nom, ver, mb, prio)) {
                            NodoApp *app = buscarApp(catalogo, nom);
                            if (app != nullptr) {
                                app->estado = est;
                            }
                            n++;
                        }
                    } else if (letra == 'B') {
                        string nom;
                        int mb;
                        f >> nom >> mb;
                        NodoBloque *nuevo = new NodoBloque;
                        nuevo->nombre = nom;
                        nuevo->mb = mb;
                        nuevo->sig = nullptr;

                        if (tope == nullptr) {
                            tope = nuevo;
                            finPila = nuevo;
                        } else {
                            finPila->sig = nuevo;
                            finPila = nuevo;
                        }
                        libre = libre - mb;
                        m++;
                    }
                }
                reloj = 0;
                ultimaLlegada = 0;
                instalaciones = 0;
                sumaEspera = 0;
                mbDescargados = 0;
                cout << "Cargado: " << n << " apps y " << m << " bloques | libres " << libre
                     << " MB | turno nuevo" << endl;
            }
        }
        else if (orden == "reporte") {
            // TODO R7a
            cout << "Descargas: " << instalaciones << " | MB descargados: " << mbDescargados << " | reloj: " << reloj << " s\n";
            if (instalaciones == 0) cout << "Espera promedio: no disponible\n";
            else cout << "Espera promedio: " << fixed << setprecision(2) << (double)sumaEspera/instalaciones << " s\n";
        }
        else if (orden == "ayuda") ayuda();
        else if (orden == "salir") break;
        else cout << orden << ": orden no encontrada (escribe ayuda)" << endl;
    }

    int liberados = 0;
    // TODO R7b
    while(catalogo != nullptr){ NodoApp* t=catalogo; catalogo=catalogo->sig; delete t; liberados++; }
    while(frente != nullptr){ NodoFila* t=frente; frente=frente->sig; delete t; liberados++; }
    while(tope != nullptr){ NodoBloque* t=tope; tope=tope->sig; delete t; liberados++; }
    
    cout << "Nodos liberados: " << liberados << ". Hasta pronto." << endl;
    return 0;
}
