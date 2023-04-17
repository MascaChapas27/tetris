#include <ctime>
#include <thread>
#include <chrono>
#include <Windows.h>
#include "Tablero.hpp"
using namespace std;

// Coordenada de cada parte del bloque
struct Coordenada{
    int x;
    int y;
};

struct Bloque{
    char forma;
    int giro;
    Coordenada bloques[4];
};

// Constantes de importancia
const int pixelesBloque = 32;
const int altura = 15;
const int anchura = 9;
const int PUNTOS_CAIDA = 1;
const int PUNTOS_LINEA = 30;

// Flag de fin de partida (se activa si al intentar generar un bloque no hay espacio)
bool gameOver = false;

// Función que devuelve un bloque aleatorio
Bloque generaBloque(char matriz[altura][anchura]){
    Bloque b;
    char formas[7] = {'L','O','J','I','T','S','Z'};
    b.forma = formas[rand()%7];
    b.giro = 0;
    switch(b.forma){
    case 'L':
        b.bloques[0].x = anchura/2-1;
        b.bloques[0].y = 1;
        b.bloques[1].x = anchura/2-1;
        b.bloques[1].y = 0;
        b.bloques[2].x = anchura/2;
        b.bloques[2].y = 0;
        b.bloques[3].x = anchura/2+1;
        b.bloques[3].y = 0;
        break;
    case 'O':
        b.bloques[0].x = anchura/2-1;
        b.bloques[0].y = 0;
        b.bloques[1].x = anchura/2;
        b.bloques[1].y = 0;
        b.bloques[2].x = anchura/2-1;
        b.bloques[2].y = 1;
        b.bloques[3].x = anchura/2;
        b.bloques[3].y = 1;
        break;
    case 'J':
        b.bloques[0].x = anchura/2-1;
        b.bloques[0].y = 0;
        b.bloques[1].x = anchura/2;
        b.bloques[1].y = 0;
        b.bloques[2].x = anchura/2+1;
        b.bloques[2].y = 0;
        b.bloques[3].x = anchura/2+1;
        b.bloques[3].y = 1;
        break;
    case 'I':
        b.bloques[0].x = anchura/2-2;
        b.bloques[0].y = 0;
        b.bloques[1].x = anchura/2-1;
        b.bloques[1].y = 0;
        b.bloques[2].x = anchura/2;
        b.bloques[2].y = 0;
        b.bloques[3].x = anchura/2+1;
        b.bloques[3].y = 0;
        break;
    case 'T':
        b.bloques[0].x = anchura/2-1;
        b.bloques[0].y = 0;
        b.bloques[1].x = anchura/2;
        b.bloques[1].y = 0;
        b.bloques[2].x = anchura/2+1;
        b.bloques[2].y = 0;
        b.bloques[3].x = anchura/2;
        b.bloques[3].y = 1;
        break;
    case 'S':
        b.bloques[0].x = anchura/2-1;
        b.bloques[0].y = 1;
        b.bloques[1].x = anchura/2;
        b.bloques[1].y = 1;
        b.bloques[2].x = anchura/2;
        b.bloques[2].y = 0;
        b.bloques[3].x = anchura/2+1;
        b.bloques[3].y = 0;
        break;
    case 'Z':
        b.bloques[0].x = anchura/2-1;
        b.bloques[0].y = 0;
        b.bloques[1].x = anchura/2;
        b.bloques[1].y = 0;
        b.bloques[2].x = anchura/2;
        b.bloques[2].y = 1;
        b.bloques[3].x = anchura/2+1;
        b.bloques[3].y = 1;
        break;
    }

    // Se termina la partida si hemos intentado generar un bloque encima de otro
    gameOver = matriz[b.bloques[0].y][b.bloques[0].x]!=' ' ||
                matriz[b.bloques[1].y][b.bloques[1].x]!=' ' ||
                matriz[b.bloques[2].y][b.bloques[2].x]!=' ' ||
                matriz[b.bloques[3].y][b.bloques[3].x]!=' ';

    return b;
};

// Función que coloca un nuevo bloque en la matriz
void colocaBloque(char matriz[altura][anchura], Bloque bloque){
    matriz[bloque.bloques[0].y][bloque.bloques[0].x] = bloque.forma;
    matriz[bloque.bloques[1].y][bloque.bloques[1].x] = bloque.forma;
    matriz[bloque.bloques[2].y][bloque.bloques[2].x] = bloque.forma;
    matriz[bloque.bloques[3].y][bloque.bloques[3].x] = bloque.forma;
}

// Función que mueve un bloque dado
bool mueveBloque(char matriz[altura][anchura], Bloque* bloque, char dir){
    // Eliminamos el bloque actual momentáneamente
    matriz[bloque->bloques[0].y][bloque->bloques[0].x] = ' ';
    matriz[bloque->bloques[1].y][bloque->bloques[1].x] = ' ';
    matriz[bloque->bloques[2].y][bloque->bloques[2].x] = ' ';
    matriz[bloque->bloques[3].y][bloque->bloques[3].x] = ' ';

    // Comprobamos si se puede mover hacia la izquierda y luego hacia la derecha
    if(dir=='L' &&
        bloque->bloques[0].x-1 >= 0 &&
        bloque->bloques[1].x-1 >= 0 &&
        bloque->bloques[2].x-1 >= 0 &&
        bloque->bloques[3].x-1 >= 0 &&
        matriz[bloque->bloques[0].y][bloque->bloques[0].x-1]==' ' &&
        matriz[bloque->bloques[1].y][bloque->bloques[1].x-1]==' ' &&
        matriz[bloque->bloques[2].y][bloque->bloques[2].x-1]==' ' &&
        matriz[bloque->bloques[3].y][bloque->bloques[3].x-1]==' '){

            bloque->bloques[0].x-=1;
            bloque->bloques[1].x-=1;
            bloque->bloques[2].x-=1;
            bloque->bloques[3].x-=1;
            return true;

    } else if(dir=='R' &&
        bloque->bloques[0].x+1 < anchura &&
        bloque->bloques[1].x+1 < anchura &&
        bloque->bloques[2].x+1 < anchura &&
        bloque->bloques[3].x+1 < anchura &&
        matriz[bloque->bloques[0].y][bloque->bloques[0].x+1]==' ' &&
        matriz[bloque->bloques[1].y][bloque->bloques[1].x+1]==' ' &&
        matriz[bloque->bloques[2].y][bloque->bloques[2].x+1]==' ' &&
        matriz[bloque->bloques[3].y][bloque->bloques[3].x+1]==' '){

            bloque->bloques[0].x+=1;
            bloque->bloques[1].x+=1;
            bloque->bloques[2].x+=1;
            bloque->bloques[3].x+=1;
            return true;
    }
    return false;
}

// Función que baja el bloque actual en uno y devuelve si se ha bajado con éxito o ha colisionado
bool bajaBloque(char matriz[altura][anchura], Bloque* bloque, int bajar=1){
    matriz[bloque->bloques[0].y][bloque->bloques[0].x] = ' ';
    matriz[bloque->bloques[1].y][bloque->bloques[1].x] = ' ';
    matriz[bloque->bloques[2].y][bloque->bloques[2].x] = ' ';
    matriz[bloque->bloques[3].y][bloque->bloques[3].x] = ' ';

    if(bloque->bloques[0].y < altura &&
       bloque->bloques[1].y < altura &&
       bloque->bloques[2].y < altura &&
       bloque->bloques[3].y < altura &&
       matriz[bloque->bloques[0].y+bajar][bloque->bloques[0].x] == ' ' &&
       matriz[bloque->bloques[1].y+bajar][bloque->bloques[1].x] == ' ' &&
       matriz[bloque->bloques[2].y+bajar][bloque->bloques[2].x] == ' ' &&
       matriz[bloque->bloques[3].y+bajar][bloque->bloques[3].x] == ' '){

        bloque->bloques[0].y+=bajar;
        bloque->bloques[1].y+=bajar;
        bloque->bloques[2].y+=bajar;
        bloque->bloques[3].y+=bajar;
        return true;
    } else {
        colocaBloque(matriz,*bloque);
        return false;
    }
}

// Función que rota el bloque actual si es posible
bool rotaBloque(char matriz[altura][anchura], Bloque* bloque, bool debug = true){

    // Borramos el bloque momentáneamente
    matriz[bloque->bloques[0].y][bloque->bloques[0].x] = ' ';
    matriz[bloque->bloques[1].y][bloque->bloques[1].x] = ' ';
    matriz[bloque->bloques[2].y][bloque->bloques[2].x] = ' ';
    matriz[bloque->bloques[3].y][bloque->bloques[3].x] = ' ';

    // Según la forma vamos viendo cómo girarlo
    switch(bloque->forma){
    case 'L':
        switch(bloque->giro){
        case 0:
            if(bloque->bloques[0].y-2 >= 0 &&
               matriz[bloque->bloques[0].y-2][bloque->bloques[0].x] == ' ' &&
               matriz[bloque->bloques[1].y-1][bloque->bloques[1].x+1] == ' ' &&
               matriz[bloque->bloques[2].y][bloque->bloques[2].x] == ' ' &&
               matriz[bloque->bloques[3].y+1][bloque->bloques[3].x-1] == ' '){

               bloque->bloques[0].y-=2;
               bloque->bloques[1].y-=1;
               bloque->bloques[1].x+=1;
               bloque->bloques[3].y+=1;
               bloque->bloques[3].x-=1;

               bloque->giro = 1;
               return true;
            }
            break;
        case 1:
            if(bloque->bloques[0].x+2 < anchura &&
               matriz[bloque->bloques[0].y][bloque->bloques[0].x+2] == ' ' &&
               matriz[bloque->bloques[1].y+1][bloque->bloques[1].x+1] == ' ' &&
               matriz[bloque->bloques[2].y][bloque->bloques[2].x] == ' ' &&
               matriz[bloque->bloques[3].y-1][bloque->bloques[3].x-1] == ' '){

               bloque->bloques[0].x+=2;
               bloque->bloques[1].y+=1;
               bloque->bloques[1].x+=1;
               bloque->bloques[3].y-=1;
               bloque->bloques[3].x-=1;

               bloque->giro = 2;
               return true;
            }
            break;
        case 2:
            if(bloque->bloques[1].y+1 < altura &&
               matriz[bloque->bloques[0].y+2][bloque->bloques[0].x] == ' ' &&
               matriz[bloque->bloques[1].y+1][bloque->bloques[1].x-1] == ' ' &&
               matriz[bloque->bloques[2].y][bloque->bloques[2].x] == ' ' &&
               matriz[bloque->bloques[3].y-1][bloque->bloques[3].x+1] == ' '){

               bloque->bloques[0].y+=2;
               bloque->bloques[1].y+=1;
               bloque->bloques[1].x-=1;
               bloque->bloques[3].y-=1;
               bloque->bloques[3].x+=1;

               bloque->giro = 3;
               return true;
            }
            break;
        case 3:
            if(bloque->bloques[1].x+-1 >= 0 &&
               matriz[bloque->bloques[0].y][bloque->bloques[0].x-2] == ' ' &&
               matriz[bloque->bloques[1].y-1][bloque->bloques[1].x-1] == ' ' &&
               matriz[bloque->bloques[2].y][bloque->bloques[2].x] == ' ' &&
               matriz[bloque->bloques[3].y+1][bloque->bloques[3].x+1] == ' '){

               bloque->bloques[0].x-=2;
               bloque->bloques[1].y-=1;
               bloque->bloques[1].x-=1;
               bloque->bloques[3].y+=1;
               bloque->bloques[3].x+=1;

               bloque->giro = 0;
               return true;
            }
            break;
        }
        break;
    case 'J':
        switch(bloque->giro){
        case 0:
            if(bloque->bloques[0].y-1 >= 0 &&
               matriz[bloque->bloques[0].y-1][bloque->bloques[0].x+1] == ' ' &&
               matriz[bloque->bloques[1].y][bloque->bloques[1].x] == ' ' &&
               matriz[bloque->bloques[2].y+1][bloque->bloques[2].x-1] == ' ' &&
               matriz[bloque->bloques[3].y][bloque->bloques[3].x-2] == ' '){

               bloque->bloques[0].y-=1;
               bloque->bloques[0].x+=1;
               bloque->bloques[2].y+=1;
               bloque->bloques[2].x-=1;
               bloque->bloques[3].x-=2;

               bloque->giro = 1;
               return true;
            }
            break;
        case 1:
            if(bloque->bloques[0].x+1 < anchura &&
               matriz[bloque->bloques[0].y+1][bloque->bloques[0].x+1] == ' ' &&
               matriz[bloque->bloques[1].y][bloque->bloques[1].x] == ' ' &&
               matriz[bloque->bloques[2].y-1][bloque->bloques[2].x-1] == ' ' &&
               matriz[bloque->bloques[3].y-2][bloque->bloques[3].x] == ' '){

               bloque->bloques[0].y+=1;
               bloque->bloques[0].x+=1;
               bloque->bloques[2].y-=1;
               bloque->bloques[2].x-=1;
               bloque->bloques[3].y-=2;

               bloque->giro = 2;
               return true;
            }
            break;
        case 2:
            if(bloque->bloques[0].y+1 < altura &&
               matriz[bloque->bloques[0].y+1][bloque->bloques[0].x-1] == ' ' &&
               matriz[bloque->bloques[1].y][bloque->bloques[1].x] == ' ' &&
               matriz[bloque->bloques[2].y-1][bloque->bloques[2].x+1] == ' ' &&
               matriz[bloque->bloques[3].y][bloque->bloques[3].x+2] == ' '){

               bloque->bloques[0].y+=1;
               bloque->bloques[0].x-=1;
               bloque->bloques[2].y-=1;
               bloque->bloques[2].x+=1;
               bloque->bloques[3].x+=2;

               bloque->giro = 3;
               return true;
            }
            break;
        case 3:
            if(bloque->bloques[0].x+-1 >= 0 &&
               matriz[bloque->bloques[0].y-1][bloque->bloques[0].x-1] == ' ' &&
               matriz[bloque->bloques[1].y][bloque->bloques[1].x] == ' ' &&
               matriz[bloque->bloques[2].y+1][bloque->bloques[2].x+1] == ' ' &&
               matriz[bloque->bloques[3].y+2][bloque->bloques[3].x] == ' '){

               bloque->bloques[0].y-=1;
               bloque->bloques[0].x-=1;
               bloque->bloques[2].y+=1;
               bloque->bloques[2].x+=1;
               bloque->bloques[3].y+=2;

               bloque->giro = 0;
               return true;
            }
            break;
        }
        break;
    case 'I':
        switch(bloque->giro){
        case 0:
            if(bloque->bloques[0].y-2 >= 0 &&
               bloque->bloques[3].y+1 < altura &&
               matriz[bloque->bloques[0].y-2][bloque->bloques[0].x+2] == ' ' &&
               matriz[bloque->bloques[1].y-1][bloque->bloques[1].x+1] == ' ' &&
               matriz[bloque->bloques[2].y][bloque->bloques[2].x] == ' ' &&
               matriz[bloque->bloques[3].y+1][bloque->bloques[3].x-1] == ' '){

               bloque->bloques[0].y-=2;
               bloque->bloques[0].x+=2;
               bloque->bloques[1].y-=1;
               bloque->bloques[1].x+=1;
               bloque->bloques[3].y+=1;
               bloque->bloques[3].x-=1;

               bloque->giro = 1;
               return true;
            }
            break;
        case 1:
            if(bloque->bloques[0].x-2 >= 0 &&
               bloque->bloques[3].x+1 < anchura &&
               matriz[bloque->bloques[0].y+2][bloque->bloques[0].x-2] == ' ' &&
               matriz[bloque->bloques[1].y+1][bloque->bloques[1].x-1] == ' ' &&
               matriz[bloque->bloques[2].y][bloque->bloques[2].x] == ' ' &&
               matriz[bloque->bloques[3].y-1][bloque->bloques[3].x+1] == ' '){

               bloque->bloques[0].y+=2;
               bloque->bloques[0].x-=2;
               bloque->bloques[1].y+=1;
               bloque->bloques[1].x-=1;
               bloque->bloques[3].y-=1;
               bloque->bloques[3].x+=1;

               bloque->giro = 0;
               return true;
            }
            break;
        }
        break;
    case 'T':
        switch(bloque->giro){
        case 0:
            if(bloque->bloques[0].y-1 >= 0 &&
               matriz[bloque->bloques[0].y-1][bloque->bloques[0].x+1] == ' ' &&
               matriz[bloque->bloques[1].y][bloque->bloques[1].x] == ' ' &&
               matriz[bloque->bloques[2].y+1][bloque->bloques[2].x-1] == ' ' &&
               matriz[bloque->bloques[3].y-1][bloque->bloques[3].x-1] == ' '){

               bloque->bloques[0].y-=1;
               bloque->bloques[0].x+=1;
               bloque->bloques[2].y+=1;
               bloque->bloques[2].x-=1;
               bloque->bloques[3].y-=1;
               bloque->bloques[3].x-=1;

               bloque->giro = 1;
               return true;
            }
            break;
        case 1:
            if(bloque->bloques[0].x+1 < anchura &&
               matriz[bloque->bloques[0].y+1][bloque->bloques[0].x+1] == ' ' &&
               matriz[bloque->bloques[1].y][bloque->bloques[1].x] == ' ' &&
               matriz[bloque->bloques[2].y-1][bloque->bloques[2].x-1] == ' ' &&
               matriz[bloque->bloques[3].y-1][bloque->bloques[3].x+1] == ' '){

               bloque->bloques[0].y+=1;
               bloque->bloques[0].x+=1;
               bloque->bloques[2].y-=1;
               bloque->bloques[2].x-=1;
               bloque->bloques[3].y-=1;
               bloque->bloques[3].x+=1;

               bloque->giro = 2;
               return true;
            }
            break;
        case 2:
            if(bloque->bloques[0].y+1 < altura &&
               matriz[bloque->bloques[0].y+1][bloque->bloques[0].x-1] == ' ' &&
               matriz[bloque->bloques[1].y][bloque->bloques[1].x] == ' ' &&
               matriz[bloque->bloques[2].y-1][bloque->bloques[2].x+1] == ' ' &&
               matriz[bloque->bloques[3].y+1][bloque->bloques[3].x+1] == ' '){

               bloque->bloques[0].y+=1;
               bloque->bloques[0].x-=1;
               bloque->bloques[2].y-=1;
               bloque->bloques[2].x+=1;
               bloque->bloques[3].y+=1;
               bloque->bloques[3].x+=1;

               bloque->giro = 3;
               return true;
            }
            break;
        case 3:
            if(bloque->bloques[0].x+-1 >= 0 &&
               matriz[bloque->bloques[0].y-1][bloque->bloques[0].x-1] == ' ' &&
               matriz[bloque->bloques[1].y][bloque->bloques[1].x] == ' ' &&
               matriz[bloque->bloques[2].y+1][bloque->bloques[2].x+1] == ' ' &&
               matriz[bloque->bloques[3].y+1][bloque->bloques[3].x-1] == ' '){

               bloque->bloques[0].y-=1;
               bloque->bloques[0].x-=1;
               bloque->bloques[2].y+=1;
               bloque->bloques[2].x+=1;
               bloque->bloques[3].y+=1;
               bloque->bloques[3].x-=1;

               bloque->giro = 0;
               return true;
            }
            break;
        }
        break;
    case 'S':
        switch(bloque->giro){
        case 0:
            if(bloque->bloques[3].y+2 < altura &&
               matriz[bloque->bloques[0].y-1][bloque->bloques[0].x+1] == ' ' &&
               matriz[bloque->bloques[1].y][bloque->bloques[1].x] == ' ' &&
               matriz[bloque->bloques[2].y+1][bloque->bloques[2].x+1] == ' ' &&
               matriz[bloque->bloques[3].y+2][bloque->bloques[3].x] == ' '){

               bloque->bloques[0].y-=1;
               bloque->bloques[0].x+=1;
               bloque->bloques[2].y+=1;
               bloque->bloques[2].x+=1;
               bloque->bloques[3].y+=2;

               bloque->giro = 1;
               return true;
            }
            break;
        case 1:
            if(bloque->bloques[0].x-1 >= 0 &&
               matriz[bloque->bloques[0].y+1][bloque->bloques[0].x-1] == ' ' &&
               matriz[bloque->bloques[1].y+1][bloque->bloques[1].x-1] == ' ' &&
               matriz[bloque->bloques[2].y-1][bloque->bloques[2].x-1] == ' ' &&
               matriz[bloque->bloques[3].y-2][bloque->bloques[3].x] == ' '){

               bloque->bloques[0].y+=1;
               bloque->bloques[0].x-=1;
               bloque->bloques[2].y-=1;
               bloque->bloques[2].x-=1;
               bloque->bloques[3].y-=2;

               bloque->giro = 0;
               return true;
            }
            break;
        }
        break;
    case 'Z':
        switch(bloque->giro){
        case 0:
            if(bloque->bloques[3].y+1 < altura &&
               matriz[bloque->bloques[0].y][bloque->bloques[0].x+2] == ' ' &&
               matriz[bloque->bloques[1].y+1][bloque->bloques[1].x+1] == ' ' &&
               matriz[bloque->bloques[2].y][bloque->bloques[2].x] == ' ' &&
               matriz[bloque->bloques[3].y+1][bloque->bloques[3].x-1] == ' '){

               bloque->bloques[0].x+=2;
               bloque->bloques[1].y+=1;
               bloque->bloques[1].x+=1;
               bloque->bloques[3].y+=1;
               bloque->bloques[3].x-=1;

               bloque->giro = 1;
               return true;
            }
            break;
        case 1:
            if(bloque->bloques[0].x-2 >= 0 &&
               matriz[bloque->bloques[0].y][bloque->bloques[0].x-2] == ' ' &&
               matriz[bloque->bloques[1].y-1][bloque->bloques[1].x-1] == ' ' &&
               matriz[bloque->bloques[2].y][bloque->bloques[2].x] == ' ' &&
               matriz[bloque->bloques[3].y-1][bloque->bloques[3].x+1] == ' '){

               bloque->bloques[0].x-=2;
               bloque->bloques[1].y-=1;
               bloque->bloques[1].x-=1;
               bloque->bloques[3].y-=1;
               bloque->bloques[3].x+=1;

               bloque->giro = 0;
               return true;
            }
            break;
        }
        break;
    case 'O':
        return true;
        break;
    }
    // No se ha podido girar, lo movemos uno a la izquierda
    if(debug){
        // Comprobamos si se puede girar moviéndolo a la izquierda
        if(mueveBloque(matriz,bloque,'L')){
            if(rotaBloque(matriz,bloque,false)) return true;
            // No se ha podido, lo colocamos en su sitio
            mueveBloque(matriz,bloque,'R');
        }
        if(mueveBloque(matriz,bloque,'R')){
            // Ahora probamos a la derecha
            if(rotaBloque(matriz,bloque,false)) return true;
            // Si es un bloque I puede que moviéndolo dos veces a la derecha quepa (porque es más largo y necesita más espacio)
            if(bloque->forma=='I' && mueveBloque(matriz,bloque,'R')){
                if(rotaBloque(matriz,bloque,false)) return true;
                else mueveBloque(matriz,bloque,'L');
            }
            // No se ha podido, lo colocamos en su sitio
            mueveBloque(matriz,bloque,'L');
        }
        if(bajaBloque(matriz,bloque)){
            // Probamos a bajar el bloque una posición
            if(rotaBloque(matriz,bloque,false)) return true;
            // Si es un bloque I puede que moviéndolo dos veces abajo quepa (porque es más largo y necesita más espacio)
            if(bloque->forma=='I' && bajaBloque(matriz,bloque)){
                if(rotaBloque(matriz,bloque,false)) return true;
                else bajaBloque(matriz,bloque,-1);
            }
            // No se ha podido, lo colocamos en su sitio (bajaBloque tiene un argumento opcional que permite cambiar el movimiento)
            bajaBloque(matriz,bloque,-1);
        }
    }
    // No se ha podido hacer nada, se devuelve false
    return false;
}

// Comprueba las líneas y elimina todas las que están llenas, desplazando las demás hacia abajo
int eliminaLineas(char matriz[altura][anchura]){
    int eliminadas = 0;
    for(int i=0;i<altura;i++){
        int espacios = 0;
        for(int j=0;j<anchura;j++){
            if(matriz[i][j]==' ') espacios++;
        }
        // Tenemos una línea completa
        if(espacios==0){
            eliminadas+=1;
            // Trasladamos las líneas hacia abajo desde la línea encontrada
            for(int k=i;k>0;k--){
                for(int l=0;l<anchura;l++){
                    matriz[k][l] = matriz[k-1][l];
                }
            }

            // Llenamos la primera línea de espacios
            for(int l=0;l<anchura;l++){
                matriz[0][l] = ' ';
            }
        }
    }
    return eliminadas;
}

int main()
{
    // Iniciamos la semilla del rand() con el tiempo actual
    srand(time(NULL));

    // Tablero que vamos a usar para jugar
    Tablero t;

    // Matriz de bloques
    char matriz[altura][anchura];

    // Se llena de espacios vacíos
    for(int i=0;i<altura;i++){
        for(int j=0;j<anchura;j++){
            matriz[i][j]=' ';
        }
    }

    // Bloque actual
    Bloque actual = generaBloque(matriz);

    // La ventana principal
    sf::RenderWindow ventana(sf::VideoMode(anchura*pixelesBloque,altura*pixelesBloque), "Tetris");

    // Para las animaciones guapisimas de cuando la ventana se mueve cuando pones piezas fua que guapo nene de verdad esto es el futuro
    // 0 significa que no hay animacion. 3 es que sí que hay (cada animación tiene 3 "frames")
    int frameAnimacion = 0;
    // L significa izquierda, D significa abajo, R significa derecha, B signfica romper (cuando completas una línea)
    char direccionAnimacion = 'B';

    // Textura gris de bloque para colorearlo después
    sf::Texture texturaGris;
    if(!texturaGris.loadFromFile("texturaGris.png")){
        cerr << "Error al cargar la textura del bloque gris" << endl;
        ventana.close();
        system("pause>0");
        exit(1);
    }

    // La letra que vamos a usar
    sf::Font fuente;
    if (!fuente.loadFromFile("upheaval.ttf"))
    {
        cerr << "Error al intentar cargar la fuente del archivo upheaval.ttf" << endl;
        ventana.close();
        exit(3);
    }

    //
    sf::Texture those;
    if(!those.loadFromFile("those.png")){
        ventana.close();
        exit(11);
    }

    //
    sf::Texture responsible;
    if(!responsible.loadFromFile("responsible.png")){
        ventana.close();
        exit(11);
    }

    //
    bool _ = false;

    // bystander
    sf::Sprite bystander;

    // responsible
    sf::Text in;
    in.setFont(fuente);
    in.setCharacterSize(20);
    in.setFillColor(sf::Color::White);
    in.setPosition(-20,200);
    in.setStyle(sf::Text::Regular);

    // listen
    sf::Music _1111;
    if(!_1111.openFromFile("1111.wav"))
        exit(11);

    sf::Music _sudden;
    if(!_sudden.openFromFile("sudden.wav"))
        exit(11);

    // Sonidos
    sf::SoundBuffer bufferCaer;
    if (!bufferCaer.loadFromFile("caer.wav")){
        cerr << "Error al cargar el sonido caer.wav" << endl;
        exit(2);
    }
    sf::Sound sonidoCaer;
    sonidoCaer.setBuffer(bufferCaer);

    sf::SoundBuffer bufferColocar;
    if (!bufferColocar.loadFromFile("colocar.wav")){
        cerr << "Error al cargar el sonido colocar.wav" << endl;
        exit(2);
    }
    sf::Sound sonidoColocar;
    sonidoColocar.setBuffer(bufferColocar);

    sf::SoundBuffer bufferGiro;
    if (!bufferGiro.loadFromFile("giro.wav")){
        cerr << "Error al cargar el sonido giro.wav" << endl;
        exit(2);
    }
    sf::Sound sonidoGiro;
    sonidoGiro.setBuffer(bufferGiro);

    sf::SoundBuffer bufferRomper;
    if (!bufferRomper.loadFromFile("romper.wav")){
        cerr << "Error al cargar el sonido romper.wav" << endl;
        exit(2);
    }
    sf::Sound sonidoRomper;
    sonidoRomper.setBuffer(bufferRomper);

    sf::SoundBuffer bufferPerder;
    if (!bufferPerder.loadFromFile("perder.wav")){
        cerr << "Error al cargar el sonido perder.wav" << endl;
        exit(2);
    }
    sf::Sound sonidoPerder;
    sonidoPerder.setBuffer(bufferPerder);

    // Sprite del bloque (gris y lo vamos coloreando)
    sf::Sprite bloqueGris;
    bloqueGris.setTexture(texturaGris);

    // Reloj de caída de bloque
    sf::Clock relojCaida;

    // Reloj de movimiento del jugador
    sf::Clock relojJugador;

    // Para los puntos
    int puntos = 0;
    sf::Text textoPuntos;
    textoPuntos.setFont(fuente);
    textoPuntos.setString("0");
    textoPuntos.setCharacterSize(20);
    textoPuntos.setFillColor(sf::Color::White);
    textoPuntos.setStyle(sf::Text::Regular);
    textoPuntos.setPosition(10,0);

    // El programa se ejecuta mientras que la ventana esté abierta
    while(!gameOver){
        // Dormimos el proceso durante 50 milisegundos para conseguir 20 frames por segundo (no necesitamos más)
        std::this_thread::sleep_for (std::chrono::milliseconds(50));

        if(frameAnimacion>0){
            if(frameAnimacion==3){
                switch(direccionAnimacion){
                case 'L':
                    ventana.setPosition(sf::Vector2i(ventana.getPosition().x-10,ventana.getPosition().y));
                    break;
                case 'D':
                    ventana.setPosition(sf::Vector2i(ventana.getPosition().x,ventana.getPosition().y+10));
                    break;
                case 'R':
                    ventana.setPosition(sf::Vector2i(ventana.getPosition().x+10,ventana.getPosition().y));
                    break;
                case 'B':
                    ventana.setPosition(sf::Vector2i(ventana.getPosition().x,ventana.getPosition().y+20));
                }
            } else {
                switch(direccionAnimacion){
                case 'L':
                    ventana.setPosition(sf::Vector2i(ventana.getPosition().x+5,ventana.getPosition().y));
                    break;
                case 'D':
                    ventana.setPosition(sf::Vector2i(ventana.getPosition().x,ventana.getPosition().y-5));
                    break;
                case 'R':
                    ventana.setPosition(sf::Vector2i(ventana.getPosition().x-5,ventana.getPosition().y));
                    break;
                case 'B':
                    ventana.setPosition(sf::Vector2i(ventana.getPosition().x,ventana.getPosition().y-10));
                }
            }
            frameAnimacion-=1;
        }

        // Cada segundo el bloque cae
        sf::Time tiempoCaida = relojCaida.getElapsedTime();
        if(tiempoCaida.asSeconds()>1.f){
            // Reiniciamos el reloj
            relojCaida.restart();
            // Movemos el bloque solo si no estamos moviéndolo ya
            if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
                if(!bajaBloque(matriz,&actual)){
                    actual = generaBloque(matriz);
                    int puntosPorEliminar = eliminaLineas(matriz)*PUNTOS_LINEA;
                    if(puntosPorEliminar) {
                        frameAnimacion=3;
                        direccionAnimacion = 'B';
                        puntos+=puntosPorEliminar;
                        sonidoRomper.play();
                    } else {
                        frameAnimacion=3;
                        direccionAnimacion = 'D';
                        sonidoColocar.play();
                    }
                } else sonidoCaer.play();
            }
        }

        // evento
        sf::Event evento;
        while(ventana.pollEvent(evento)){
            //
            if(evento.type == sf::Event::Closed){
                _=true;
                gameOver=true;
                puntos=-234987423;
            }
        }

        // Cada 0.10 segundos se puede mover el bloque
        sf::Time tiempoJugador = relojJugador.getElapsedTime();

        if(tiempoJugador.asSeconds()>0.10){
            // Podemos mover el bloque hacia izquierda, derecha o abajo
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
                if(!mueveBloque(matriz,&actual,'L')){
                    frameAnimacion=3;
                    direccionAnimacion='L';
                    sonidoColocar.play();
                } else sonidoGiro.play();
                relojJugador.restart();
            } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
                if(!mueveBloque(matriz,&actual,'R')){
                    frameAnimacion=3;
                    direccionAnimacion='R';
                    sonidoColocar.play();
                } else sonidoGiro.play();
                relojJugador.restart();
            } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
                puntos+=PUNTOS_CAIDA;
                relojCaida.restart();
                if(!bajaBloque(matriz,&actual)){
                    actual = generaBloque(matriz);
                    int puntosPorEliminar = eliminaLineas(matriz)*PUNTOS_LINEA;
                    if(puntosPorEliminar) {
                        frameAnimacion=3;
                        direccionAnimacion = 'B';
                        puntos+=puntosPorEliminar;
                        sonidoRomper.play();
                    } else {
                        frameAnimacion=3;
                        direccionAnimacion = 'D';
                        sonidoColocar.play();
                    }
                } else sonidoCaer.play();
                relojJugador.restart();
            } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
                sonidoGiro.play();
                rotaBloque(matriz,&actual);
                relojJugador.restart();
            }
        }

        // Colocamos el bloque actual después de todos los cambios
        colocaBloque(matriz,actual);

        // Borramos todo
        ventana.clear(sf::Color::Black);

        // Dibujamos cosicas
        for(int i=0;i<altura;i++){
            for(int j=0;j<anchura;j++){
                if(matriz[i][j]!=' '){
                    switch(matriz[i][j]){
                    case 'L':
                        bloqueGris.setColor(sf::Color(255,145,0));
                        break;
                    case 'O':
                        bloqueGris.setColor(sf::Color::Yellow);
                        break;
                    case 'J':
                        bloqueGris.setColor(sf::Color::Blue);
                        break;
                    case 'I':
                        bloqueGris.setColor(sf::Color::Cyan);
                        break;
                    case 'S':
                        bloqueGris.setColor(sf::Color::Green);
                        break;
                    case 'Z':
                        bloqueGris.setColor(sf::Color::Red);
                        break;
                    case 'T':
                        bloqueGris.setColor(sf::Color::Magenta);
                        break;
                    }
                    bloqueGris.setPosition(sf::Vector2((float)j*pixelesBloque,(float)i*pixelesBloque));
                    ventana.draw(bloqueGris);
                }
            }
        }

        // Actualizamos el texto con la puntuación
        textoPuntos.setString(std::to_string(puntos));
        ventana.draw(textoPuntos);

        // Termina el frame actual
        ventana.display();
    }

    if(tablero._){
        int schadenfreude=0;
        ventana.clear(sf::Color::Black);
        bystander.setTexture(those);
        while(schadenfreude<30){
            if(schadenfreude<20) ventana.setPosition(sf::Vector2i(ventana.getPosition().x+2,ventana.getPosition().y));
            else ventana.setPosition(sf::Vector2i(ventana.getPosition().x+2,ventana.getPosition().y));
            string rar = "";
            for(int i=0;i<30;i++){
                rar+=rand()%64+20;
            }
            in.setString(rar);
            in.setPosition(-1*schadenfreude*((rand()%3)),schadenfreude*12);
            ventana.draw(in);
            ventana.display();
            _sudden.play();
            std::this_thread::sleep_for(std::chrono::milliseconds(150-schadenfreude*5));
            schadenfreude+=1;
            _sudden.stop();
        }
        ventana.clear(sf::Color::Black);
        ventana.setPosition(sf::Vector2i(500,100));
        bystander.setPosition(90,170);
        ventana.draw(bystander);
        ventana.display();
        bystander.setPosition(0,0);
        _1111.play();
        sf::RenderWindow v[32];
        for(int i=0;i<32;i++){
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            v[i].create(sf::VideoMode(105,113), "null");
            v[i].setPosition(sf::Vector2i(rand()%(1366-105),rand()%(768-113)));
            v[i].draw(bystander);
            v[i].display();
        }
        ventana.clear(sf::Color::Black);
        bystander.setTexture(responsible);
        ventana.draw(bystander);
        ventana.display();
        _1111.setLoop(true);
        MessageBox(NULL,"", "", MB_ICONERROR | MB_ABORTRETRYIGNORE);
        MessageBox(NULL,"", "", MB_ICONERROR | MB_ABORTRETRYIGNORE);
        MessageBox(NULL,"", "", MB_ICONERROR | MB_ABORTRETRYIGNORE);
        MessageBox(NULL,"", "", MB_ICONERROR | MB_ABORTRETRYIGNORE);
        MessageBox(NULL,"", "", MB_ICONERROR | MB_ABORTRETRYIGNORE);
        MessageBox(NULL,"", "", MB_ICONERROR | MB_ABORTRETRYIGNORE);
        _1111.stop();
        ventana.close();
        for(int i=0;i<8;i++){
            v[i].close();
        }
    } else {
        sonidoPerder.play();
        int filaActual=altura-1;
        string stringPuntos=std::to_string(puntos);
        textoPuntos.setString(stringPuntos);
        while(ventana.isOpen()){
            std::this_thread::sleep_for (std::chrono::milliseconds(100));
            // Comprueba los eventos que han ocurrido desde la última iteración
            sf::Event evento;
            while(ventana.pollEvent(evento)){
                // Si queremos cerrar la ventana cerramos las dos
                if(evento.type == sf::Event::Closed)
                    ventana.close();
            }
            ventana.clear();
            // Dibujamos cosicas
            for(int i=0;i<altura;i++){
                for(int j=0;j<anchura;j++){
                    if(matriz[i][j]!=' '){
                        switch(matriz[i][j]){
                        case 'L':
                            bloqueGris.setColor(sf::Color(255,145,0));
                            break;
                        case 'O':
                            bloqueGris.setColor(sf::Color::Yellow);
                            break;
                        case 'J':
                            bloqueGris.setColor(sf::Color::Blue);
                            break;
                        case 'I':
                            bloqueGris.setColor(sf::Color::Cyan);
                            break;
                        case 'S':
                            bloqueGris.setColor(sf::Color::Green);
                            break;
                        case 'Z':
                            bloqueGris.setColor(sf::Color::Red);
                            break;
                        case 'T':
                            bloqueGris.setColor(sf::Color::Magenta);
                            break;
                        }
                        if(i>=filaActual)
                            bloqueGris.setColor(sf::Color(125,125,125));
                        bloqueGris.setPosition(sf::Vector2((float)j*pixelesBloque,(float)i*pixelesBloque));
                        ventana.draw(bloqueGris);
                    }
                }
            }

            ventana.draw(textoPuntos);

            if(filaActual>=0)
                filaActual-=1;

            // Termina el frame actual
            ventana.display();
        }
    }
}
