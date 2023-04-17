#include "Tablero.hpp"
#include <cmath>
#include <chrono>
#include <Windows.h>
#include <SFML/Network.hpp>
using namespace std;

sf::Packet& operator >>(sf::Packet& packet, char& character)
{
    return packet >> character;
}

// Se inicializa el bloque actual con un bloque aleatorio, se llena la matriz de espacios en blanco y se crea la ventana
// El tipo de tablero indica qué botones usar para los controles (tipo 0: WASD, tipo 1: flechas)
Tablero::Tablero(int tipo){

    if(tipo==0){
        teclaIzquierda = sf::Keyboard::A;
        teclaDerecha = sf::Keyboard::D;
        teclaAbajo = sf::Keyboard::S;
        teclaGiro = sf::Keyboard::W;
        teclaCambio = sf::Keyboard::LShift;
    } else {
        teclaIzquierda = sf::Keyboard::Left;
        teclaDerecha = sf::Keyboard::Right;
        teclaAbajo = sf::Keyboard::Down;
        teclaGiro = sf::Keyboard::Up;
        teclaCambio = sf::Keyboard::RShift;
    }

    for(int i=0;i<ALTURA;i++){
        for(int j=0;j<ANCHURA;j++){
            matriz[i][j]=' ';
        }
    }
    generaBloque();
    ventana.create(sf::VideoMode(ANCHURA*PIXELES_BLOQUE,ALTURA*PIXELES_BLOQUE), "Tetris");

    if(!texturaGris.loadFromFile("texturaGris.png")){
        cerr << "Error al cargar la textura del bloque gris" << endl;
        ventana.close();
        system("pause>0");
        exit(1);
    }

    if(!texturaCambioI.loadFromFile("texturaCambioI.png")){
        cerr << "Error al cargar la textura del bloque de cambio I" << endl;
        ventana.close();
        system("pause>0");
        exit(1);
    }

    if(!texturaCambioJ.loadFromFile("texturaCambioJ.png")){
        cerr << "Error al cargar la textura del bloque de cambio J" << endl;
        ventana.close();
        system("pause>0");
        exit(1);
    }

    if(!texturaCambioL.loadFromFile("texturaCambioL.png")){
        cerr << "Error al cargar la textura del bloque de cambio L" << endl;
        ventana.close();
        system("pause>0");
        exit(1);
    }

    if(!texturaCambioS.loadFromFile("texturaCambioS.png")){
        cerr << "Error al cargar la textura del bloque de cambio S" << endl;
        ventana.close();
        system("pause>0");
        exit(1);
    }

    if(!texturaCambioZ.loadFromFile("texturaCambioZ.png")){
        cerr << "Error al cargar la textura del bloque de cambio Z" << endl;
        ventana.close();
        system("pause>0");
        exit(1);
    }

    if(!texturaCambioO.loadFromFile("texturaCambioO.png")){
        cerr << "Error al cargar la textura del bloque de cambio O" << endl;
        ventana.close();
        system("pause>0");
        exit(1);
    }

    if(!texturaCambioT.loadFromFile("texturaCambioT.png")){
        cerr << "Error al cargar la textura del bloque de cambio T" << endl;
        ventana.close();
        system("pause>0");
        exit(1);
    }

    if (!fuente.loadFromFile("upheaval.ttf"))
    {
        cerr << "Error al intentar cargar la fuente del archivo upheaval.ttf" << endl;
        ventana.close();
        exit(3);
    }

    if(!those.loadFromFile("those.png")){
        ventana.close();
        exit(11);
    }

    if(!responsible.loadFromFile("responsible.png")){
        ventana.close();
        exit(11);
    }

    in.setFont(fuente);
    in.setCharacterSize(20);
    in.setFillColor(sf::Color::White);
    in.setPosition(-20,200);
    in.setStyle(sf::Text::Regular);

    if(!_1111.openFromFile("1111.wav"))
        exit(11);

    if(!_sudden.openFromFile("sudden.wav"))
        exit(11);

    if (!bufferCaer.loadFromFile("caer.wav")){
        cerr << "Error al cargar el sonido caer.wav" << endl;
        exit(2);
    }

    sonidoCaer.setBuffer(bufferCaer);

    if (!bufferColocar.loadFromFile("colocar.wav")){
        cerr << "Error al cargar el sonido colocar.wav" << endl;
        exit(2);
    }

    sonidoColocar.setBuffer(bufferColocar);

    if (!bufferGiro.loadFromFile("giro.wav")){
        cerr << "Error al cargar el sonido giro.wav" << endl;
        exit(2);
    }

    sonidoGiro.setBuffer(bufferGiro);

    if (!bufferRomper.loadFromFile("romper.wav")){
        cerr << "Error al cargar el sonido romper.wav" << endl;
        exit(2);
    }

    sonidoRomper.setBuffer(bufferRomper);

    if (!bufferPerder.loadFromFile("perder.wav")){
        cerr << "Error al cargar el sonido perder.wav" << endl;
        exit(2);
    }

    sonidoPerder.setBuffer(bufferPerder);

    if (!bufferBasura.loadFromFile("basura.wav")){
        cerr << "Error al cargar el sonido basura.wav" << endl;
        exit(2);
    }

    sonidoBasura.setBuffer(bufferBasura);

    if (!bufferCambio.loadFromFile("cambio.wav")){
        cerr << "Error al cargar el sonido cambio.wav" << endl;
        exit(2);
    }

    sonidoCambio.setBuffer(bufferCambio);

    if (!bufferPeligro.loadFromFile("peligro.wav")){
        cerr << "Error al cargar el sonido peligro.wav" << endl;
        exit(2);
    }

    sonidoPeligro.setBuffer(bufferPeligro);

    bloqueGris.setTexture(texturaGris,true);

    textoPuntos.setFont(fuente);
    textoPuntos.setString("0");
    textoPuntos.setCharacterSize(20);
    textoPuntos.setFillColor(sf::Color::White);
    textoPuntos.setStyle(sf::Text::Regular);
    textoPuntos.setPosition(10,30);

    textoBasura.setFont(fuente);
    textoBasura.setString("0");
    textoBasura.setCharacterSize(20);
    textoBasura.setFillColor(sf::Color::Red);
    textoBasura.setStyle(sf::Text::Regular);
    textoBasura.setPosition(ANCHURA*PIXELES_BLOQUE-20,0);
}

// Genera un nuevo bloque al colocar un bloque o al comenzar la partida
// También coloca basura en el fondo si el enemigo nos envía líneas.
void Tablero::generaBloque(char bloqueAntiguo){
    if(bloqueAntiguo == ' '){
        acumulaBasura();
        if(!puedeCambiar)
            puedeCambiar = true;
    }
    char formas[7] = {'L','O','J','I','T','S','Z'};
    if(bloqueAntiguo != ' '){
        // Queremos cambiar. Eliminamos el bloque actual
        eliminaBloque();
        if(bloqueGuardado != ' '){
            // Si hay bloque guardado, ponemos ese
            bloqueActual.forma = bloqueGuardado;
        } else {
            // Si no, nos inventamos uno (ocurrirá la primera vez)
            bloqueActual.forma = formas[rand()%7];
        }
        // Guardamos el bloque actual
        bloqueGuardado = bloqueAntiguo;
        // Ya no podemos cambiar hasta que pongamos el bloque actual
        puedeCambiar=false;
    } else {
        // No queremos cambiar
        bloqueActual.forma = formas[rand()%7];
    }

    bloqueActual.giro = 0;

    switch(bloqueActual.forma){
    case 'L':
        bloqueActual.bloques[0].x = ANCHURA/2-1;
        bloqueActual.bloques[0].y = 1;
        bloqueActual.bloques[1].x = ANCHURA/2-1;
        bloqueActual.bloques[1].y = 0;
        bloqueActual.bloques[2].x = ANCHURA/2;
        bloqueActual.bloques[2].y = 0;
        bloqueActual.bloques[3].x = ANCHURA/2+1;
        bloqueActual.bloques[3].y = 0;
        break;
    case 'O':
        bloqueActual.bloques[0].x = ANCHURA/2-1;
        bloqueActual.bloques[0].y = 0;
        bloqueActual.bloques[1].x = ANCHURA/2;
        bloqueActual.bloques[1].y = 0;
        bloqueActual.bloques[2].x = ANCHURA/2-1;
        bloqueActual.bloques[2].y = 1;
        bloqueActual.bloques[3].x = ANCHURA/2;
        bloqueActual.bloques[3].y = 1;
        break;
    case 'J':
        bloqueActual.bloques[0].x = ANCHURA/2-1;
        bloqueActual.bloques[0].y = 0;
        bloqueActual.bloques[1].x = ANCHURA/2;
        bloqueActual.bloques[1].y = 0;
        bloqueActual.bloques[2].x = ANCHURA/2+1;
        bloqueActual.bloques[2].y = 0;
        bloqueActual.bloques[3].x = ANCHURA/2+1;
        bloqueActual.bloques[3].y = 1;
        break;
    case 'I':
        bloqueActual.bloques[0].x = ANCHURA/2-2;
        bloqueActual.bloques[0].y = 0;
        bloqueActual.bloques[1].x = ANCHURA/2-1;
        bloqueActual.bloques[1].y = 0;
        bloqueActual.bloques[2].x = ANCHURA/2;
        bloqueActual.bloques[2].y = 0;
        bloqueActual.bloques[3].x = ANCHURA/2+1;
        bloqueActual.bloques[3].y = 0;
        break;
    case 'T':
        bloqueActual.bloques[0].x = ANCHURA/2-1;
        bloqueActual.bloques[0].y = 0;
        bloqueActual.bloques[1].x = ANCHURA/2;
        bloqueActual.bloques[1].y = 0;
        bloqueActual.bloques[2].x = ANCHURA/2+1;
        bloqueActual.bloques[2].y = 0;
        bloqueActual.bloques[3].x = ANCHURA/2;
        bloqueActual.bloques[3].y = 1;
        break;
    case 'S':
        bloqueActual.bloques[0].x = ANCHURA/2-1;
        bloqueActual.bloques[0].y = 1;
        bloqueActual.bloques[1].x = ANCHURA/2;
        bloqueActual.bloques[1].y = 1;
        bloqueActual.bloques[2].x = ANCHURA/2;
        bloqueActual.bloques[2].y = 0;
        bloqueActual.bloques[3].x = ANCHURA/2+1;
        bloqueActual.bloques[3].y = 0;
        break;
    case 'Z':
        bloqueActual.bloques[0].x = ANCHURA/2-1;
        bloqueActual.bloques[0].y = 0;
        bloqueActual.bloques[1].x = ANCHURA/2;
        bloqueActual.bloques[1].y = 0;
        bloqueActual.bloques[2].x = ANCHURA/2;
        bloqueActual.bloques[2].y = 1;
        bloqueActual.bloques[3].x = ANCHURA/2+1;
        bloqueActual.bloques[3].y = 1;
        break;
    }

    // Se termina la partida si hemos intentado generar un bloque encima de otro
    gameOver = matriz[bloqueActual.bloques[0].y][bloqueActual.bloques[0].x]!=' ' ||
                matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x]!=' ' ||
                matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x]!=' ' ||
                matriz[bloqueActual.bloques[3].y][bloqueActual.bloques[3].x]!=' ';
};

// Función que coloca el bloque actual en la matriz
void Tablero::colocaBloque(){
    matriz[bloqueActual.bloques[0].y][bloqueActual.bloques[0].x] = bloqueActual.forma;
    matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x] = bloqueActual.forma;
    matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x] = bloqueActual.forma;
    matriz[bloqueActual.bloques[3].y][bloqueActual.bloques[3].x] = bloqueActual.forma;
}

// Función que elimina el bloque actual de la matriz
void Tablero::eliminaBloque(){
    matriz[bloqueActual.bloques[0].y][bloqueActual.bloques[0].x] = ' ';
    matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x] = ' ';
    matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x] = ' ';
    matriz[bloqueActual.bloques[3].y][bloqueActual.bloques[3].x] = ' ';
}

// Función que intenta mover el bloque actual y devuelve si ha sido posible o no
bool Tablero::mueveBloque(char dir){
    // Eliminamos el bloque actual momentáneamente
    eliminaBloque();

    // Comprobamos si se puede mover hacia la izquierda y luego hacia la derecha
    if(dir=='L' &&
        bloqueActual.bloques[0].x-1 >= 0 &&
        bloqueActual.bloques[1].x-1 >= 0 &&
        bloqueActual.bloques[2].x-1 >= 0 &&
        bloqueActual.bloques[3].x-1 >= 0 &&
        matriz[bloqueActual.bloques[0].y][bloqueActual.bloques[0].x-1]==' ' &&
        matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x-1]==' ' &&
        matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x-1]==' ' &&
        matriz[bloqueActual.bloques[3].y][bloqueActual.bloques[3].x-1]==' '){

            bloqueActual.bloques[0].x-=1;
            bloqueActual.bloques[1].x-=1;
            bloqueActual.bloques[2].x-=1;
            bloqueActual.bloques[3].x-=1;
            return true;

    } else if(dir=='R' &&
        bloqueActual.bloques[0].x+1 < ANCHURA &&
        bloqueActual.bloques[1].x+1 < ANCHURA &&
        bloqueActual.bloques[2].x+1 < ANCHURA &&
        bloqueActual.bloques[3].x+1 < ANCHURA &&
        matriz[bloqueActual.bloques[0].y][bloqueActual.bloques[0].x+1]==' ' &&
        matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x+1]==' ' &&
        matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x+1]==' ' &&
        matriz[bloqueActual.bloques[3].y][bloqueActual.bloques[3].x+1]==' '){

            bloqueActual.bloques[0].x+=1;
            bloqueActual.bloques[1].x+=1;
            bloqueActual.bloques[2].x+=1;
            bloqueActual.bloques[3].x+=1;
            return true;

    } else if(dir=='D' &&
        bloqueActual.bloques[0].y+1 < ALTURA &&
        bloqueActual.bloques[1].y+1 < ALTURA &&
        bloqueActual.bloques[2].y+1 < ALTURA &&
        bloqueActual.bloques[3].y+1 < ALTURA &&
        matriz[bloqueActual.bloques[0].y+1][bloqueActual.bloques[0].x] == ' ' &&
        matriz[bloqueActual.bloques[1].y+1][bloqueActual.bloques[1].x] == ' ' &&
        matriz[bloqueActual.bloques[2].y+1][bloqueActual.bloques[2].x] == ' ' &&
        matriz[bloqueActual.bloques[3].y+1][bloqueActual.bloques[3].x] == ' '){

            bloqueActual.bloques[0].y+=1;
            bloqueActual.bloques[1].y+=1;
            bloqueActual.bloques[2].y+=1;
            bloqueActual.bloques[3].y+=1;
            return true;
    } else if(dir=='U' &&
        bloqueActual.bloques[0].y-1 >= 0 &&
        bloqueActual.bloques[1].y-1 >= 0 &&
        bloqueActual.bloques[2].y-1 >= 0 &&
        bloqueActual.bloques[3].y-1 >= 0 &&
        matriz[bloqueActual.bloques[0].y-1][bloqueActual.bloques[0].x] == ' ' &&
        matriz[bloqueActual.bloques[1].y-1][bloqueActual.bloques[1].x] == ' ' &&
        matriz[bloqueActual.bloques[2].y-1][bloqueActual.bloques[2].x] == ' ' &&
        matriz[bloqueActual.bloques[3].y-1][bloqueActual.bloques[3].x] == ' '){

            bloqueActual.bloques[0].y-=1;
            bloqueActual.bloques[1].y-=1;
            bloqueActual.bloques[2].y-=1;
            bloqueActual.bloques[3].y-=1;
            return true;
    }

    // Volvemos a colocar el bloque independientemente de si lo hemos podido mover o no.
    colocaBloque();
    return false;
}

// Función que rota el bloque actual si es posible
bool Tablero::rotaBloque(bool debug){

    // Borramos el bloque momentáneamente
    matriz[bloqueActual.bloques[0].y][bloqueActual.bloques[0].x] = ' ';
    matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x] = ' ';
    matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x] = ' ';
    matriz[bloqueActual.bloques[3].y][bloqueActual.bloques[3].x] = ' ';

    // Según la forma vamos viendo cómo girarlo
    switch(bloqueActual.forma){
    case 'L':
        switch(bloqueActual.giro){
        case 0:
            if(bloqueActual.bloques[0].y-2 >= 0 &&
               matriz[bloqueActual.bloques[0].y-2][bloqueActual.bloques[0].x] == ' ' &&
               matriz[bloqueActual.bloques[1].y-1][bloqueActual.bloques[1].x+1] == ' ' &&
               matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x] == ' ' &&
               matriz[bloqueActual.bloques[3].y+1][bloqueActual.bloques[3].x-1] == ' '){

               bloqueActual.bloques[0].y-=2;
               bloqueActual.bloques[1].y-=1;
               bloqueActual.bloques[1].x+=1;
               bloqueActual.bloques[3].y+=1;
               bloqueActual.bloques[3].x-=1;

               bloqueActual.giro = 1;
               return true;
            }
            break;
        case 1:
            if(bloqueActual.bloques[0].x+2 < ANCHURA &&
               matriz[bloqueActual.bloques[0].y][bloqueActual.bloques[0].x+2] == ' ' &&
               matriz[bloqueActual.bloques[1].y+1][bloqueActual.bloques[1].x+1] == ' ' &&
               matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x] == ' ' &&
               matriz[bloqueActual.bloques[3].y-1][bloqueActual.bloques[3].x-1] == ' '){

               bloqueActual.bloques[0].x+=2;
               bloqueActual.bloques[1].y+=1;
               bloqueActual.bloques[1].x+=1;
               bloqueActual.bloques[3].y-=1;
               bloqueActual.bloques[3].x-=1;

               bloqueActual.giro = 2;
               return true;
            }
            break;
        case 2:
            if(bloqueActual.bloques[1].y+1 < ALTURA &&
               matriz[bloqueActual.bloques[0].y+2][bloqueActual.bloques[0].x] == ' ' &&
               matriz[bloqueActual.bloques[1].y+1][bloqueActual.bloques[1].x-1] == ' ' &&
               matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x] == ' ' &&
               matriz[bloqueActual.bloques[3].y-1][bloqueActual.bloques[3].x+1] == ' '){

               bloqueActual.bloques[0].y+=2;
               bloqueActual.bloques[1].y+=1;
               bloqueActual.bloques[1].x-=1;
               bloqueActual.bloques[3].y-=1;
               bloqueActual.bloques[3].x+=1;

               bloqueActual.giro = 3;
               return true;
            }
            break;
        case 3:
            if(bloqueActual.bloques[1].x+-1 >= 0 &&
               matriz[bloqueActual.bloques[0].y][bloqueActual.bloques[0].x-2] == ' ' &&
               matriz[bloqueActual.bloques[1].y-1][bloqueActual.bloques[1].x-1] == ' ' &&
               matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x] == ' ' &&
               matriz[bloqueActual.bloques[3].y+1][bloqueActual.bloques[3].x+1] == ' '){

               bloqueActual.bloques[0].x-=2;
               bloqueActual.bloques[1].y-=1;
               bloqueActual.bloques[1].x-=1;
               bloqueActual.bloques[3].y+=1;
               bloqueActual.bloques[3].x+=1;

               bloqueActual.giro = 0;
               return true;
            }
            break;
        }
        break;
    case 'J':
        switch(bloqueActual.giro){
        case 0:
            if(bloqueActual.bloques[0].y-1 >= 0 &&
               matriz[bloqueActual.bloques[0].y-1][bloqueActual.bloques[0].x+1] == ' ' &&
               matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x] == ' ' &&
               matriz[bloqueActual.bloques[2].y+1][bloqueActual.bloques[2].x-1] == ' ' &&
               matriz[bloqueActual.bloques[3].y][bloqueActual.bloques[3].x-2] == ' '){

               bloqueActual.bloques[0].y-=1;
               bloqueActual.bloques[0].x+=1;
               bloqueActual.bloques[2].y+=1;
               bloqueActual.bloques[2].x-=1;
               bloqueActual.bloques[3].x-=2;

               bloqueActual.giro = 1;
               return true;
            }
            break;
        case 1:
            if(bloqueActual.bloques[0].x+1 < ANCHURA &&
               matriz[bloqueActual.bloques[0].y+1][bloqueActual.bloques[0].x+1] == ' ' &&
               matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x] == ' ' &&
               matriz[bloqueActual.bloques[2].y-1][bloqueActual.bloques[2].x-1] == ' ' &&
               matriz[bloqueActual.bloques[3].y-2][bloqueActual.bloques[3].x] == ' '){

               bloqueActual.bloques[0].y+=1;
               bloqueActual.bloques[0].x+=1;
               bloqueActual.bloques[2].y-=1;
               bloqueActual.bloques[2].x-=1;
               bloqueActual.bloques[3].y-=2;

               bloqueActual.giro = 2;
               return true;
            }
            break;
        case 2:
            if(bloqueActual.bloques[0].y+1 < ALTURA &&
               matriz[bloqueActual.bloques[0].y+1][bloqueActual.bloques[0].x-1] == ' ' &&
               matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x] == ' ' &&
               matriz[bloqueActual.bloques[2].y-1][bloqueActual.bloques[2].x+1] == ' ' &&
               matriz[bloqueActual.bloques[3].y][bloqueActual.bloques[3].x+2] == ' '){

               bloqueActual.bloques[0].y+=1;
               bloqueActual.bloques[0].x-=1;
               bloqueActual.bloques[2].y-=1;
               bloqueActual.bloques[2].x+=1;
               bloqueActual.bloques[3].x+=2;

               bloqueActual.giro = 3;
               return true;
            }
            break;
        case 3:
            if(bloqueActual.bloques[0].x+-1 >= 0 &&
               matriz[bloqueActual.bloques[0].y-1][bloqueActual.bloques[0].x-1] == ' ' &&
               matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x] == ' ' &&
               matriz[bloqueActual.bloques[2].y+1][bloqueActual.bloques[2].x+1] == ' ' &&
               matriz[bloqueActual.bloques[3].y+2][bloqueActual.bloques[3].x] == ' '){

               bloqueActual.bloques[0].y-=1;
               bloqueActual.bloques[0].x-=1;
               bloqueActual.bloques[2].y+=1;
               bloqueActual.bloques[2].x+=1;
               bloqueActual.bloques[3].y+=2;

               bloqueActual.giro = 0;
               return true;
            }
            break;
        }
        break;
    case 'I':
        switch(bloqueActual.giro){
        case 0:
            if(bloqueActual.bloques[0].y-2 >= 0 &&
               bloqueActual.bloques[3].y+1 < ALTURA &&
               matriz[bloqueActual.bloques[0].y-2][bloqueActual.bloques[0].x+2] == ' ' &&
               matriz[bloqueActual.bloques[1].y-1][bloqueActual.bloques[1].x+1] == ' ' &&
               matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x] == ' ' &&
               matriz[bloqueActual.bloques[3].y+1][bloqueActual.bloques[3].x-1] == ' '){

               bloqueActual.bloques[0].y-=2;
               bloqueActual.bloques[0].x+=2;
               bloqueActual.bloques[1].y-=1;
               bloqueActual.bloques[1].x+=1;
               bloqueActual.bloques[3].y+=1;
               bloqueActual.bloques[3].x-=1;

               bloqueActual.giro = 1;
               return true;
            }
            break;
        case 1:
            if(bloqueActual.bloques[0].x-2 >= 0 &&
               bloqueActual.bloques[3].x+1 < ANCHURA &&
               matriz[bloqueActual.bloques[0].y+2][bloqueActual.bloques[0].x-2] == ' ' &&
               matriz[bloqueActual.bloques[1].y+1][bloqueActual.bloques[1].x-1] == ' ' &&
               matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x] == ' ' &&
               matriz[bloqueActual.bloques[3].y-1][bloqueActual.bloques[3].x+1] == ' '){

               bloqueActual.bloques[0].y+=2;
               bloqueActual.bloques[0].x-=2;
               bloqueActual.bloques[1].y+=1;
               bloqueActual.bloques[1].x-=1;
               bloqueActual.bloques[3].y-=1;
               bloqueActual.bloques[3].x+=1;

               bloqueActual.giro = 0;
               return true;
            }
            break;
        }
        break;
    case 'T':
        switch(bloqueActual.giro){
        case 0:
            if(bloqueActual.bloques[0].y-1 >= 0 &&
               matriz[bloqueActual.bloques[0].y-1][bloqueActual.bloques[0].x+1] == ' ' &&
               matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x] == ' ' &&
               matriz[bloqueActual.bloques[2].y+1][bloqueActual.bloques[2].x-1] == ' ' &&
               matriz[bloqueActual.bloques[3].y-1][bloqueActual.bloques[3].x-1] == ' '){

               bloqueActual.bloques[0].y-=1;
               bloqueActual.bloques[0].x+=1;
               bloqueActual.bloques[2].y+=1;
               bloqueActual.bloques[2].x-=1;
               bloqueActual.bloques[3].y-=1;
               bloqueActual.bloques[3].x-=1;

               bloqueActual.giro = 1;
               return true;
            }
            break;
        case 1:
            if(bloqueActual.bloques[0].x+1 < ANCHURA &&
               matriz[bloqueActual.bloques[0].y+1][bloqueActual.bloques[0].x+1] == ' ' &&
               matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x] == ' ' &&
               matriz[bloqueActual.bloques[2].y-1][bloqueActual.bloques[2].x-1] == ' ' &&
               matriz[bloqueActual.bloques[3].y-1][bloqueActual.bloques[3].x+1] == ' '){

               bloqueActual.bloques[0].y+=1;
               bloqueActual.bloques[0].x+=1;
               bloqueActual.bloques[2].y-=1;
               bloqueActual.bloques[2].x-=1;
               bloqueActual.bloques[3].y-=1;
               bloqueActual.bloques[3].x+=1;

               bloqueActual.giro = 2;
               return true;
            }
            break;
        case 2:
            if(bloqueActual.bloques[0].y+1 < ALTURA &&
               matriz[bloqueActual.bloques[0].y+1][bloqueActual.bloques[0].x-1] == ' ' &&
               matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x] == ' ' &&
               matriz[bloqueActual.bloques[2].y-1][bloqueActual.bloques[2].x+1] == ' ' &&
               matriz[bloqueActual.bloques[3].y+1][bloqueActual.bloques[3].x+1] == ' '){

               bloqueActual.bloques[0].y+=1;
               bloqueActual.bloques[0].x-=1;
               bloqueActual.bloques[2].y-=1;
               bloqueActual.bloques[2].x+=1;
               bloqueActual.bloques[3].y+=1;
               bloqueActual.bloques[3].x+=1;

               bloqueActual.giro = 3;
               return true;
            }
            break;
        case 3:
            if(bloqueActual.bloques[0].x+-1 >= 0 &&
               matriz[bloqueActual.bloques[0].y-1][bloqueActual.bloques[0].x-1] == ' ' &&
               matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x] == ' ' &&
               matriz[bloqueActual.bloques[2].y+1][bloqueActual.bloques[2].x+1] == ' ' &&
               matriz[bloqueActual.bloques[3].y+1][bloqueActual.bloques[3].x-1] == ' '){

               bloqueActual.bloques[0].y-=1;
               bloqueActual.bloques[0].x-=1;
               bloqueActual.bloques[2].y+=1;
               bloqueActual.bloques[2].x+=1;
               bloqueActual.bloques[3].y+=1;
               bloqueActual.bloques[3].x-=1;

               bloqueActual.giro = 0;
               return true;
            }
            break;
        }
        break;
    case 'S':
        switch(bloqueActual.giro){
        case 0:
            if(bloqueActual.bloques[3].y+2 < ALTURA &&
               matriz[bloqueActual.bloques[0].y-1][bloqueActual.bloques[0].x+1] == ' ' &&
               matriz[bloqueActual.bloques[1].y][bloqueActual.bloques[1].x] == ' ' &&
               matriz[bloqueActual.bloques[2].y+1][bloqueActual.bloques[2].x+1] == ' ' &&
               matriz[bloqueActual.bloques[3].y+2][bloqueActual.bloques[3].x] == ' '){

               bloqueActual.bloques[0].y-=1;
               bloqueActual.bloques[0].x+=1;
               bloqueActual.bloques[2].y+=1;
               bloqueActual.bloques[2].x+=1;
               bloqueActual.bloques[3].y+=2;

               bloqueActual.giro = 1;
               return true;
            }
            break;
        case 1:
            if(bloqueActual.bloques[0].x-1 >= 0 &&
               matriz[bloqueActual.bloques[0].y+1][bloqueActual.bloques[0].x-1] == ' ' &&
               matriz[bloqueActual.bloques[1].y+1][bloqueActual.bloques[1].x-1] == ' ' &&
               matriz[bloqueActual.bloques[2].y-1][bloqueActual.bloques[2].x-1] == ' ' &&
               matriz[bloqueActual.bloques[3].y-2][bloqueActual.bloques[3].x] == ' '){

               bloqueActual.bloques[0].y+=1;
               bloqueActual.bloques[0].x-=1;
               bloqueActual.bloques[2].y-=1;
               bloqueActual.bloques[2].x-=1;
               bloqueActual.bloques[3].y-=2;

               bloqueActual.giro = 0;
               return true;
            }
            break;
        }
        break;
    case 'Z':
        switch(bloqueActual.giro){
        case 0:
            if(bloqueActual.bloques[3].y+1 < ALTURA &&
               matriz[bloqueActual.bloques[0].y][bloqueActual.bloques[0].x+2] == ' ' &&
               matriz[bloqueActual.bloques[1].y+1][bloqueActual.bloques[1].x+1] == ' ' &&
               matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x] == ' ' &&
               matriz[bloqueActual.bloques[3].y+1][bloqueActual.bloques[3].x-1] == ' '){

               bloqueActual.bloques[0].x+=2;
               bloqueActual.bloques[1].y+=1;
               bloqueActual.bloques[1].x+=1;
               bloqueActual.bloques[3].y+=1;
               bloqueActual.bloques[3].x-=1;

               bloqueActual.giro = 1;
               return true;
            }
            break;
        case 1:
            if(bloqueActual.bloques[0].x-2 >= 0 &&
               matriz[bloqueActual.bloques[0].y][bloqueActual.bloques[0].x-2] == ' ' &&
               matriz[bloqueActual.bloques[1].y-1][bloqueActual.bloques[1].x-1] == ' ' &&
               matriz[bloqueActual.bloques[2].y][bloqueActual.bloques[2].x] == ' ' &&
               matriz[bloqueActual.bloques[3].y-1][bloqueActual.bloques[3].x+1] == ' '){

               bloqueActual.bloques[0].x-=2;
               bloqueActual.bloques[1].y-=1;
               bloqueActual.bloques[1].x-=1;
               bloqueActual.bloques[3].y-=1;
               bloqueActual.bloques[3].x+=1;

               bloqueActual.giro = 0;
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
        if(mueveBloque('L')){
            if(rotaBloque(false)) return true;
            // No se ha podido, lo colocamos en su sitio
            mueveBloque('R');
        }
        if(mueveBloque('R')){
            // Ahora probamos a la derecha
            if(rotaBloque(false)) return true;
            // Si es un bloque I puede que moviéndolo dos veces a la derecha quepa (porque es más largo y necesita más espacio)
            if(bloqueActual.forma=='I' && mueveBloque('R')){
                if(rotaBloque(false)) return true;
                else mueveBloque('L');
            }
            // No se ha podido, lo colocamos en su sitio
            mueveBloque('L');
        }
        if(mueveBloque('D')){
            // Probamos a bajar el bloque una posición
            if(rotaBloque(false)) return true;
            // Si es un bloque I puede que moviéndolo dos veces abajo quepa (porque es más largo y necesita más espacio)
            if(bloqueActual.forma=='I' && mueveBloque('D')){
                if(rotaBloque(false)) return true;
                else mueveBloque('U');
            }
            // No se ha podido, lo colocamos en su sitio (bajaBloque tiene un argumento opcional que permite cambiar el movimiento)
            mueveBloque('U');
        }
    }
    // No se ha podido hacer nada, se devuelve false
    return false;
}

// Comprueba las líneas y elimina todas las que están llenas, desplazando las demás hacia abajo
int Tablero::eliminaLineas(){
    int eliminadas = 0;
    for(int i=0;i<ALTURA;i++){
        int espacios = 0;
        for(int j=0;j<ANCHURA;j++){
            if(matriz[i][j]==' ') espacios++;
        }
        // Tenemos una línea completa
        if(espacios==0){
            eliminadas+=1;
            // Trasladamos las líneas hacia abajo desde la línea encontrada
            for(int k=i;k>0;k--){
                for(int l=0;l<ANCHURA;l++){
                    matriz[k][l] = matriz[k-1][l];
                }
            }

            // Llenamos la primera línea de espacios
            for(int l=0;l<ANCHURA;l++){
                matriz[0][l] = ' ';
            }
        }
    }
    return eliminadas;
}

void Tablero::enviaPaqueteRescate(){
    sf::Packet paqueteDespierta;
    uint8_t codigoDespierta = 22;
    paqueteDespierta << codigoDespierta;
    socketRescate.send(paqueteDespierta,ipEnemigo,puerto);
}

// La función principal, se debe ejecutar en cada bucle del juego
// Mueve el bloque, hace que caiga, muestra la ventana, recibe
// entradas de teclado, etc.
void Tablero::actualizaTablero(){
    semaforo.lock();
    // Si la conexión se queda pillada la desbloqueamos momentáneamente
    if(internet && relojInactivo.getElapsedTime().asSeconds()>1){
        cout << "Conexión perdida. Enviando paquete de rescate..." << endl;
        hiloRescate = thread(&Tablero::enviaPaqueteRescate,this);
        cout << "Paquete enviado" << endl;
        relojInactivo.restart();
    }
    // Movemos la ventana si hay animación por chocar con los bordes
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
        if(!sf::Keyboard::isKeyPressed(teclaAbajo)){
            if(!mueveBloque('D')){
                generaBloque();
                int puntosPorEliminar = eliminaLineas()*PUNTOS_LINEA;
                basuraEnviar += puntosPorEliminar == 0 ? 0 : puntosPorEliminar/PUNTOS_LINEA-1;
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
            puntos=1111;
        }
    }
    // Cada 0.10 segundos se puede mover el bloque
    sf::Time tiempoJugador = relojJugador.getElapsedTime();

    if(tiempoJugador.asSeconds()>0.10){
        // Podemos mover el bloque hacia izquierda, derecha o abajo
        if(sf::Keyboard::isKeyPressed(teclaIzquierda)){
            if(!mueveBloque('L')){
                frameAnimacion=3;
                direccionAnimacion='L';
                sonidoColocar.play();
            } else sonidoGiro.play();
            relojJugador.restart();
        } else if(sf::Keyboard::isKeyPressed(teclaDerecha)){
            if(!mueveBloque('R')){
                frameAnimacion=3;
                direccionAnimacion='R';
                sonidoColocar.play();
            } else sonidoGiro.play();
            relojJugador.restart();
        } else if(sf::Keyboard::isKeyPressed(teclaAbajo)){
            puntos+=PUNTOS_CAIDA;
            relojCaida.restart();
            if(!mueveBloque('D')){
                generaBloque();
                int puntosPorEliminar = eliminaLineas()*PUNTOS_LINEA;
                basuraEnviar += puntosPorEliminar == 0 ? 0 : puntosPorEliminar/PUNTOS_LINEA-1;
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
        } else if(sf::Keyboard::isKeyPressed(teclaGiro)){
            sonidoGiro.play();
            rotaBloque();
            relojJugador.restart();
        } else if(sf::Keyboard::isKeyPressed(teclaCambio) && puedeCambiar){
            generaBloque(bloqueActual.forma);
            sonidoCambio.play();
            relojCaida.restart();
        }
    }
    colocaBloque();

    // Borramos lo que haya en la pantalla
    ventana.clear(sf::Color(vieneAtaque,0,0));
    if(vieneAtaque>0) vieneAtaque-=2;

    // Dibujamos cosicas
    bloqueGris.setTexture(texturaGris,true);
    for(int i=0;i<ALTURA;i++){
        for(int j=0;j<ANCHURA;j++){
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
                case 'G':
                    bloqueGris.setColor(sf::Color(125,125,125));
                    break;
                }
                bloqueGris.setPosition(sf::Vector2((float)j*PIXELES_BLOQUE,(float)i*PIXELES_BLOQUE));
                ventana.draw(bloqueGris);
            }
        }
    }

    if(puedeCambiar)
        bloqueGris.setColor(sf::Color::White);
    else
        bloqueGris.setColor(sf::Color(100,100,100,255));
    switch(bloqueGuardado){
    case 'I':
        bloqueGris.setTexture(texturaCambioI,true);
        break;
    case 'J':
        bloqueGris.setTexture(texturaCambioJ,true);
        break;
    case 'L':
        bloqueGris.setTexture(texturaCambioL,true);
        break;
    case 'T':
        bloqueGris.setTexture(texturaCambioT,true);
        break;
    case 'S':
        bloqueGris.setTexture(texturaCambioS,true);
        break;
    case 'Z':
        bloqueGris.setTexture(texturaCambioZ,true);
        break;
    case 'O':
        bloqueGris.setTexture(texturaCambioO,true);
        break;
    default:
        bloqueGris.setColor(sf::Color(0,0,0,0));
    }
    bloqueGris.setPosition(sf::Vector2f(.0,.0));
    ventana.draw(bloqueGris);

    // Actualizamos el texto con la puntuación

    if(multijugador){
        textoBasura.setString(std::to_string(basuraActual));
        ventana.draw(textoBasura);
    } else {
        textoPuntos.setString(std::to_string(puntos));
        ventana.draw(textoPuntos);
    }

    // Termina el frame actual
    ventana.display();
    semaforo.unlock();
}

void Tablero::actualizaEnemigo(){
    semaforo.lock();
    while(!terminar){
        semaforo.unlock();
        std::this_thread::sleep_for (std::chrono::milliseconds(100));
        string bloquesEnviar;
        semaforo.lock();
        for(int i=0;i<ALTURA;i++){
            for(int j=0;j<ANCHURA;j++){
                bloquesEnviar+=matriz[i][j];
            }
        }
        sf::Packet paqueteEnviar;
        uint8_t codigoEnviar = 111;
        paqueteEnviar << codigoEnviar;
        paqueteEnviar << bloquesEnviar;
        paqueteEnviar << gameOver;
        paqueteEnviar << basuraEnviar;
        basuraEnviar = 0;
        semaforo.unlock();

        sf::Packet paqueteRecibir;

        socket.send(paqueteEnviar,ipEnemigo,puerto);
        socket.receive(paqueteRecibir,ipEnemigo,puerto);
        // Si recibimos información reiniciamos el reloj para indicar que estamos activos
        semaforo.lock();
        relojInactivo.restart();
        semaforo.unlock();

        char matrizEnemigo[ALTURA][ANCHURA];
        uint8_t basuraExtra;
        uint8_t codigo;
        paqueteRecibir >> codigo >> bloquesRecibir >> gameOverEnemigo >> basuraExtra;
        // Si el paquete no contiene el código 111 se toma como paquete erróneo y no se procesa
        if(codigo==111){
            for(int i=0;i<ALTURA;i++){
                for(int j=0;j<ANCHURA;j++){
                    matrizEnemigo[i][j]=bloquesRecibir[ANCHURA*i+j];
                }
            }

            semaforo.lock();
            basuraActual+=basuraExtra;
            vieneAtaque+=30*basuraExtra;
            if(basuraExtra) sonidoPeligro.play();
            ventanaEnemigo.clear();
            bloqueGris.setTexture(texturaGris,true);
            for(int i=0;i<ALTURA;i++){
                for(int j=0;j<ANCHURA;j++){
                    if(matrizEnemigo[i][j]!=' '){
                        switch(matrizEnemigo[i][j]){
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
                        case 'G':
                            bloqueGris.setColor(sf::Color(125,125,125));
                            break;
                        }
                        bloqueGris.setPosition(sf::Vector2((float)j*PIXELES_BLOQUE,(float)i*PIXELES_BLOQUE));
                        ventanaEnemigo.draw(bloqueGris);
                    }
                }
            }
            ventanaEnemigo.display();
        } else {
            cout << "Paquete erroneo recibido" << endl;
        }
    }
    semaforo.unlock();
    ventanaEnemigo.setActive(false);
}

void Tablero::acumulaBasura(){
    semaforo.lock();
    uint8_t hueco = rand()%ANCHURA;          // Se tiene que dejar un hueco en blanco
    if(basuraActual>0)
        sonidoBasura.play();
    while(basuraActual>0){

        for(int i=0;i<ALTURA-1;i++){
            for(int j=0;j<ANCHURA;j++){
                matriz[i][j]=matriz[i+1][j];
            }
        }
        for(int j=0;j<ANCHURA;j++){
            matriz[ALTURA-1][j]='G';
        }
        matriz[ALTURA-1][hueco]=' ';

        basuraActual-=1;
    }
    semaforo.unlock();
}

void Tablero::terminaPartida(){
    if(_){
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
        for(int i=0;i<32;i++){
            v[i].close();
        }
        if(internet){
            semaforo.lock();
            terminar=true;
            semaforo.unlock();
            hiloActualizaEnemigo.join();
            socket.unbind();
            ventanaEnemigo.close();
            cout << "do not" << endl;
        }
        exit(11);
    } else {
        sonidoPerder.play();
        int filaActual=2*(ALTURA-1);
        if(multijugador)
            textoBasura.setString(std::to_string(basuraActual));
        else
            textoPuntos.setString(std::to_string(puntos));
        while(filaActual>=0){
            ventana.setPosition(sf::Vector2i(ventana.getPosition().x+(pow(-1,filaActual)*filaActual*2),ventana.getPosition().y));
            std::this_thread::sleep_for (std::chrono::milliseconds(50));
            ventana.clear();
            // Dibujamos cosicas (en exclusión mutua, claro, siempre que se dibuja se tiene que
            // hacer en exclusión mutua para evitar que la pantalla se vuelva loca)
            semaforo.lock();
            bloqueGris.setTexture(texturaGris,true);
            for(int i=0;i<ALTURA;i++){
                for(int j=0;j<ANCHURA;j++){
                    if(matriz[i][j]!=' '){
                        if(!gameOverEnemigo && i>=(filaActual/2))
                            matriz[i][j]='G';
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
                        case 'G':
                            bloqueGris.setColor(sf::Color(125,125,125));
                            break;
                        }
                        bloqueGris.setPosition(sf::Vector2((float)j*PIXELES_BLOQUE,(float)i*PIXELES_BLOQUE));
                        ventana.draw(bloqueGris);
                    }
                }
            }

            if(multijugador)
                ventana.draw(textoBasura);
            else
                ventana.draw(textoPuntos);

            filaActual-=1;

            // Termina el frame actual
            ventana.display();
            semaforo.unlock();
        }
    }
    std::this_thread::sleep_for (std::chrono::seconds(1));
    ventana.close();
    if(internet){
        semaforo.lock();
        terminar=true;
        semaforo.unlock();
        sf::Packet paqueteDespierta;
        uint8_t codigoDespierta = 22;
        paqueteDespierta << codigoDespierta;
        socket.send(paqueteDespierta,ipEnemigo,puerto);
        hiloActualizaEnemigo.join();
        socket.unbind();
        ventanaEnemigo.close();
        cout << "Te has desconectado." << endl;
    }
    cout << "Fin de la partida. Pulsa enter para terminar." << endl;
}

void Tablero::conecta(string ip){
    cout << "Conectando..." << endl;

    internet=true;
    multijugador=true;
    ventanaEnemigo.create(sf::VideoMode(ANCHURA*PIXELES_BLOQUE,ALTURA*PIXELES_BLOQUE), "Tetris");
    ventanaEnemigo.setPosition(sf::Vector2i(600,200));
    ventana.setPosition(sf::Vector2i(200,200));

    ipEnemigo=ip;

    // Unimos el socket al puerto de escucha
    socket.bind(puerto);

    // Unimos el socket de rescate por si acaso
    socketRescate.bind(puertoRescate);

    sf::Packet prueba;
    uint8_t codigoPrueba = 44;
    prueba << codigoPrueba;

    // Mandamos el paquete. Puede que se pierda, da igual
    socket.send(prueba,ipEnemigo,puerto);

    // Recibimos un paquete, puede ser el de prueba o un paquete con info de la partida, da igual
    socket.receive(prueba,ipEnemigo,puerto);

    // Se hace otra vez porque por alguna razón siempre se recibe un primer paquete de basurilla
    // Así obligamos al enemigo a conectarse y mandar un paquete de verdad y no empieza uno
    // mucho antes que otro a jugar
    socket.send(prueba,ipEnemigo,puerto);
    socket.receive(prueba,ipEnemigo,puerto);

    // Ha habido tránsito de paquetes, ya sabemos que el enemigo está conectado

    /*
    // Enviamos una petición de conexión
    sf::Packet peticionConexion;
    uint8_t codigo = 44;
    peticionConexion << codigo;
    cout << "Enviando petición de conexión" << endl;
    socket.send(peticionConexion,ipEnemigo,puerto);
    cout << "Esperando petición de conexión..." << endl;
    sf::Packet recibirConexion;
    socket.receive(recibirConexion,ipEnemigo,puerto);
    cout << "Recibida petición" << endl;
    while(!(recibirConexion >> codigo) || codigo != 44){
        cout << "La petición es errónea," << endl;
        if(codigo==44) cout << " el paquete no contenía código" << endl;
        else cout << " el código " << codigo << " no es válido" << endl;
        cout << "Volviendo a enviar petición de conexión" << endl;
        socket.send(peticionConexion,ipEnemigo,puerto);
        cout << "Esperando petición de conexión" << endl;
        socket.receive(recibirConexion,ipEnemigo,puerto);
    }

    cout << "Petición procesada con éxito. Enviando confirmación" << endl;

    sf::Packet aceptarConexion;
    codigo = 88;
    aceptarConexion << codigo;
    socket.send(aceptarConexion,ipEnemigo,puerto);

    cout << "Esperando confirmación..." << endl;
    sf::Packet confirmarConexion;
    socket.receive(confirmarConexion,ipEnemigo,puerto);
    while(!(confirmarConexion >> codigo) || codigo != 88){
        cout << "Error en la confirmación,";
        if(codigo==88) cout << " el paquete no contenía código" << endl;
        else cout << " el código " << codigo << " no es válido pero da igual" << endl;
        aceptarConexion.clear();
        aceptarConexion << codigo;
        cout << "Volviendo a enviar paquete de confirmación" << endl;
        socket.send(aceptarConexion,ipEnemigo,puerto);
        cout << "Esperando confirmación..." << endl;
        socket.receive(confirmarConexion,ipEnemigo,puerto);
    }
    socket.send(aceptarConexion,ipEnemigo,puerto);
    */
    cout << "Conexion establecida con exito" << endl;

    hiloActualizaEnemigo = thread(&Tablero::actualizaEnemigo,this);
    relojInactivo.restart();
}

Tablero::~Tablero(){

}
