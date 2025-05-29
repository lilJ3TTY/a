#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;

class Pokemon {
private:
    string nombre;
    int hp;
    int ataque;
    int defensa;
    string tipoAtaque;
    string texturePath;

public:
    Pokemon(string n, int h, int a, int d, string t, string path)
        : nombre(n), hp(h), ataque(a), defensa(d), tipoAtaque(t), texturePath(path) {
    }

    void recibirDano(int dano) {
        hp -= dano;
        if (hp < 0) hp = 0;
    }

    bool estaDerrotado() const {
        return hp <= 0;
    }

    int calcularDano(int poderAtaque) const {
        int dano = (ataque + poderAtaque) - defensa;
        return dano > 0 ? dano : 1;
    }

    string getNombre() const { return nombre; }
    int getHp() const { return hp; }
    string getTipoAtaque() const { return tipoAtaque; }
    string getTexturePath() const { return texturePath; }
};

class Juego {
private:
    RenderWindow window;
    Font font;
    Texture fondoTexture;
    Sprite fondo;
    vector<Pokemon> pokemons;
    Pokemon* jugador;
    Pokemon* enemigo;
    Texture texJugador, texEnemigo;
    Sprite jugadorSprite;
    Sprite enemigoSprite;
    bool turnoJugador;
    int reduccionDano;
    bool enBatalla;
    string mensajeBatalla;

public:
    Juego() : window(VideoMode(800, 600), "Batalla Pokémon"), jugador(nullptr), enemigo(nullptr),
        turnoJugador(true), reduccionDano(0), enBatalla(false) {

        if (!font.loadFromFile("arial/arial.ttf")) {
            cerr << "Error cargando fuente\n";
        }

        if (!fondoTexture.loadFromFile("imagenes/fondo.jpg")) {
            cerr << "Error cargando fondo.jpg\n";
        }
        fondo.setTexture(fondoTexture);
        fondo.setScale(800.0f / fondoTexture.getSize().x, 600.0f / fondoTexture.getSize().y);

        pokemons.push_back(Pokemon("Pikachu", 100, 20, 15, "Eléctrico", "imagenes/pikachu.png"));
        pokemons.push_back(Pokemon("Charmander", 90, 25, 10, "Fuego", "imagenes/charmander.png"));
        pokemons.push_back(Pokemon("Squirtle", 110, 18, 20, "Agua", "imagenes/squirtle.png"));
        pokemons.push_back(Pokemon("Bulbasaur", 120, 15, 18, "Planta", "imagenes/bulbasaur.png"));
    }

    void cargarTexturas() {
        if (!texJugador.loadFromFile(jugador->getTexturePath())) {
            cerr << "Error cargando textura del jugador\n";
        }
        if (!texEnemigo.loadFromFile(enemigo->getTexturePath())) {
            cerr << "Error cargando textura del enemigo\n";
        }

        jugadorSprite.setTexture(texJugador);
        enemigoSprite.setTexture(texEnemigo);

        jugadorSprite.setScale(0.4f, 0.4f);
        enemigoSprite.setScale(0.4f, 0.4f);

        jugadorSprite.setPosition(50, 300);
        enemigoSprite.setPosition(450, 80);
    }

    void seleccionarPokemon() {
        cout << "Elige tu Pokemon:\n";
        for (size_t i = 0; i < pokemons.size(); ++i) {
            cout << i + 1 << ". " << pokemons[i].getNombre() << "\n";
        }

        int eleccion = 0;
        while (true) {
            cin >> eleccion;
            eleccion--;
            if (eleccion >= 0 && eleccion < (int)pokemons.size()) break;
            cout << "Opción no válida. Intenta de nuevo: ";
        }

        jugador = &pokemons[eleccion];

        do {
            enemigo = &pokemons[rand() % pokemons.size()];
        } while (enemigo->getNombre() == jugador->getNombre());

        cargarTexturas();
        enBatalla = true;
        mensajeBatalla = "¡Comienza la batalla!";
    }

    void dibujarInterfaz() {
        window.clear();
        window.draw(fondo);
        window.draw(jugadorSprite);
        window.draw(enemigoSprite);

        Text textoJugador(jugador->getNombre() + ": HP " + to_string(jugador->getHp()), font, 14);
        textoJugador.setPosition(50, 480);
        textoJugador.setFillColor(Color::Black);
        window.draw(textoJugador);

        Text textoEnemigo(enemigo->getNombre() + ": HP " + to_string(enemigo->getHp()), font, 14);
        textoEnemigo.setPosition(550, 50);
        textoEnemigo.setFillColor(Color::Black);
        window.draw(textoEnemigo);

        RectangleShape cuadroTexto(Vector2f(700, 90));
        cuadroTexto.setFillColor(Color(0, 0, 0, 150));
        cuadroTexto.setPosition(50, 500);
        window.draw(cuadroTexto);

        Text textoMensaje(mensajeBatalla, font, 16);
        textoMensaje.setPosition(60, 510);
        textoMensaje.setFillColor(Color::White);
        window.draw(textoMensaje);

        if (turnoJugador && enBatalla) {
            Text textoMenu("1. Ataque normal   2. Ataque fuerte   3. Defender   4. Huir", font, 14);
            textoMenu.setPosition(60, 535);
            textoMenu.setFillColor(Color::White);
            window.draw(textoMenu);

            Text turnoInd(">> Tu turno", font, 14);
            turnoInd.setPosition(650, 560);
            turnoInd.setFillColor(Color::Yellow);
            window.draw(turnoInd);
        }

        window.display();
    }

    void animarAtaque(Sprite& sprite, int dx = 10) {
        sprite.move(dx, 0);
        dibujarInterfaz();
        sleep(milliseconds(100));
        sprite.move(-dx, 0);
        dibujarInterfaz();
    }

    void procesarAtaque(int opcion) {
        if (!turnoJugador || !enBatalla) return;

        switch (opcion) {
        case 1: {
            int dano = jugador->calcularDano(10);
            enemigo->recibirDano(dano);
            mensajeBatalla = "¡" + jugador->getNombre() + " ataca y causa " + to_string(dano) + " de daño!";
            animarAtaque(jugadorSprite);
            break;
        }
        case 2: {
            int dano = jugador->calcularDano(20);
            jugador->recibirDano(5);
            enemigo->recibirDano(dano);
            mensajeBatalla = "¡Ataque fuerte! Causas " + to_string(dano) + " de daño (pierdes 5 HP).";
            animarAtaque(jugadorSprite);
            break;
        }
        case 3: {
            reduccionDano = 8;
            mensajeBatalla = "¡Te preparas para defender!";
            break;
        }
        case 4: {
            mensajeBatalla = "¡Has huido de la batalla!";
            enBatalla = false;
            break;
        }
        }

        if (enemigo->estaDerrotado()) {
            mensajeBatalla = "¡Has derrotado a " + enemigo->getNombre() + "!";
            dibujarInterfaz();
            sleep(seconds(2));
            enBatalla = false;
            window.close();
            return;
        }
        else {
            turnoJugador = false;
            turnoEnemigo();
        }
    }

    void turnoEnemigo() {
        if (!enBatalla) return;

        int dano = (rand() % 3 == 0) ? enemigo->calcularDano(15) : enemigo->calcularDano(8);
        if (reduccionDano > 0) {
            dano -= reduccionDano;
            if (dano < 0) dano = 0;
            reduccionDano = 0;
        }

        jugador->recibirDano(dano);
        mensajeBatalla = "¡" + enemigo->getNombre() + " ataca y causa " + to_string(dano) + " de daño!";
        animarAtaque(enemigoSprite);

        if (jugador->estaDerrotado()) {
            mensajeBatalla = "¡Tu " + jugador->getNombre() + " ha sido derrotado!";
            dibujarInterfaz();
            sleep(seconds(2));
            enBatalla = false;
            window.close();
        }
        else {
            turnoJugador = true;
        }
    }

    void ejecutar() {
        seleccionarPokemon();

        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::KeyPressed && turnoJugador && enBatalla) {
                    if (event.key.code == Keyboard::Num1) procesarAtaque(1);
                    else if (event.key.code == Keyboard::Num2) procesarAtaque(2);
                    else if (event.key.code == Keyboard::Num3) procesarAtaque(3);
                    else if (event.key.code == Keyboard::Num4) procesarAtaque(4);
                }
            }
            dibujarInterfaz();
        }
    }
};

int main() {
    srand(static_cast<unsigned int>(time(NULL)));
    Juego juego;
    juego.ejecutar();
    return 0;
}
