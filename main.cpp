#include <ctime>
#include "Tablero.hpp"
using namespace std;

int main()
{
    // Iniciamos la semilla del rand() con el tiempo actual
    srand(time(NULL));

    // Variables para las respuestas
    char respuesta1;
    char respuesta2 = 'N';
    string ip;

    // Preguntamos si se quiere jugar con internet
    cout << "Quieres conectarte a Internet? S/N" << endl;

    cin >> respuesta1;

    if(respuesta1=='S'){
        cout << "Introduce la direccion IP a la que conectarse: ";
        cin >> ip;

    } else {
        cout << "Quieres jugar con alguien en el mismo teclado? S/N" << endl;
        cin >> respuesta2;
    }

    // En base a las respuestas construimos el tablero o los tableros necesarios
    Tablero t(1);

    if(respuesta1=='S'){
        t.conecta(ip);
    }
    if(respuesta2=='N'){
        // El programa se ejecuta mientras que la ventana esté abierta
        while(!t.isGameOver()){
            // Dormimos el proceso durante 50 milisegundos para conseguir 20 frames por segundo (no necesitamos más)
            std::this_thread::sleep_for (std::chrono::milliseconds(50));

            t.actualizaTablero();
        }

        // Acaba la partida
        t.terminaPartida();
    } else {
        Tablero s(0);
        // El programa se ejecuta mientras que las ventanas estén abiertas
        while(!t.isGameOver() && !s.isGameOver()){
            // Dormimos el proceso durante 50 milisegundos para conseguir 20 frames por segundo (no necesitamos más)
            std::this_thread::sleep_for (std::chrono::milliseconds(100));

            t.actualizaTablero();
            s.actualizaTablero();
        }

        // Acaba la partida
        t.terminaPartida();
        s.terminaPartida();
    }
    system("pause>0");
}
