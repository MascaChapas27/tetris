#ifndef _TABLERO
#define _TABLERO

#include <thread>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

// Constantes de importancia
const int ALTURA = 15;
const int ANCHURA = 9;
const int PIXELES_BLOQUE = 32;
const int PUNTOS_CAIDA = 1;
const int PUNTOS_LINEA = 30;

// Un tablero es un espacio de juego en el que se van acumulando piezas que caen
class Tablero{
    private:
        // Una coordenada es un par (x,y)
        class Coordenada{
            friend class Tablero;
            private:
                int x;
                int y;
        };

        // Un bloque es un tetromino
        class Bloque {
            friend class Tablero;
            private:
                char forma;
                int giro;
                Coordenada bloques[4];
        };

        // Para las animaciones guapisimas de cuando la ventana se mueve cuando pones piezas fua que guapo nene de verdad esto es el futuro
        // 0 significa que no hay animacion. 3 es que sí que hay (cada animación tiene 3 "frames")
        uint8_t frameAnimacion = 0;

        // L significa izquierda, D significa abajo, R significa derecha, B signfica romper (cuando completas una línea)
        uint8_t direccionAnimacion = 'B';

        uint8_t matriz[ALTURA][ANCHURA];    // Matriz que guarda el tablero actual
        std::string bloquesRecibir;         // Aquí se recibe la información del enemigo
        bool gameOver = false;              // Informa de si la partida ha terminado
        uint16_t puntos = 0;                // Puntos del jugador
        bool _ = false;                     // _
        bool internet = false;              // Informa de si se ha activado multijugador por internet
        bool gameOverEnemigo = false;       // Informa de si el enemigo ha perdido
        sf::IpAddress ipEnemigo;            // Dirección IP del enemigo
        sf::Mutex semaforo;                 // Semáforo para la exclusión mutua de actualización de la matriz y de la pantalla
        bool terminar=false;                // Indica al thread de actualización de enemigo que se va a acabar ya el juego
        uint8_t vieneAtaque = 0;            // Para la animación de destello rojo cuando viene un ataque
        uint8_t basuraActual = 0;           // Contador de la basura que se añadirá en cuanto pongas la siguiente pieza
        uint8_t basuraEnviar = 0;           // Contador de la basura que se enviará al enemigo
        bool multijugador = false;          // Indica si se muestra el contador de basura (multijugador local o por red)
        char bloqueGuardado = ' ';          // Bloque que se queda guardado para sacarlo después
        bool puedeCambiar = true;           // Indica si se puede cambiar el bloque. Inicialmente sí, pero al cambiar hay que colocar el bloque.

        sf::Texture texturaGris;
        sf::Texture texturaCambioI;
        sf::Texture texturaCambioJ;
        sf::Texture texturaCambioL;
        sf::Texture texturaCambioS;
        sf::Texture texturaCambioZ;
        sf::Texture texturaCambioO;
        sf::Texture texturaCambioT;
        sf::Texture those;
        sf::Texture responsible;

        sf::Sprite bystander;
        sf::Sprite bloqueGris;

        sf::Font fuente;

        sf::Text in;
        sf::Text textoPuntos;           // Texto que indica cuántos puntos tienes
        sf::Text textoBasura;           // Texto que indica la basura que va a llegar al poner el siguiente bloque

        sf::Music _1111;
        sf::Music _sudden;

        sf::SoundBuffer bufferCaer;
        sf::SoundBuffer bufferColocar;
        sf::SoundBuffer bufferGiro;
        sf::SoundBuffer bufferRomper;
        sf::SoundBuffer bufferPerder;
        sf::SoundBuffer bufferBasura;
        sf::SoundBuffer bufferCambio;
        sf::SoundBuffer bufferPeligro;

        sf::Sound sonidoCaer;
        sf::Sound sonidoColocar;
        sf::Sound sonidoGiro;
        sf::Sound sonidoRomper;
        sf::Sound sonidoPerder;
        sf::Sound sonidoBasura;
        sf::Sound sonidoCambio;
        sf::Sound sonidoPeligro;

        sf::Clock relojCaida;
        sf::Clock relojJugador;

        // Cuenta el tiempo que ha pasado desde la última actualización del enemigo
        // Si supera cierto tiempo, se considera que ambos ordenadores se han quedado deadlockeados esperando
        // información, por lo que envía un paquete para despertarlos
        sf::Clock relojInactivo;

        sf::Keyboard::Key teclaIzquierda;
        sf::Keyboard::Key teclaAbajo;
        sf::Keyboard::Key teclaDerecha;
        sf::Keyboard::Key teclaGiro;
        sf::Keyboard::Key teclaCambio;

        sf::UdpSocket socket;
        unsigned short puerto = 54000;

        Bloque bloqueActual;
        sf::RenderWindow ventana;
        sf::RenderWindow ventanaEnemigo;

        void actualizaEnemigo();
        std::thread hiloActualizaEnemigo;
        std::thread hiloRescate;
        void enviaPaqueteRescate();
        sf::UdpSocket socketRescate;
        unsigned short puertoRescate = 60800;

    public:
        Tablero(int tipo);
        ~Tablero();
        bool isGameOver(){ return gameOver || gameOverEnemigo; }
        bool is_(){ return _; }
        void generaBloque(char bloqueAntiguo = ' ');
        void colocaBloque();
        void eliminaBloque();
        bool mueveBloque(char dir);
        bool rotaBloque(bool debug = true);
        int eliminaLineas();
        void actualizaTablero();
        void terminaPartida();
        void conecta(std::string ip);
        void acumulaBasura();
};
#endif
