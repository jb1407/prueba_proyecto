#ifndef LOADASSETS_HPP
#define LOADASSETS_HPP

#include "raylib.h"
#include <iostream>
#include <string>
#include <openhash.hpp>
#include <chainedhash.hpp>

// Estructura que almacena todos los datos necesarios para reproducir el spritesheet
struct Animation {
    Texture2D texture;
    int totalFrames;   // Frames totales del sprite
    int frameWidth;    // Ancho de cada frame
    int frameHeight;   // Altura de cada frame
    int frameSpeed;    // Ticks necesarios para cambiar de fotograma
    int tickCounter;   // Contador interno de ticks del juego
    int currentFrame;  // Fotograma actual que se está mostrando
};

// Clase que almacenará los assets cargados y se facilite su uso en otros archivos (Tabla Hash)
class AssetManager
{
    private:

    // Estructura para almacenar la clave y la animación en la tabla
    struct AssetEntry
    {
        std::string key;
        Animation animation;
    };

    // Comparador compatible con búsquedas heterogéneas de OpenAddressingHashTable
    struct AssetComparator 
    {
        bool operator()(const AssetEntry& a, const AssetEntry& b) const {return a.key == b.key;}
        bool operator()(const AssetEntry& a, const std::string& k) const {return a.key == k;}
        bool operator()(const std::string& k, const AssetEntry& a) const {return k == a.key; }
    };

    // Instancia de QuadraticHashTable (Direccionamiento abierto con sondeo cuadrático)
    Designar::QuadraticHashTable<AssetEntry, AssetComparator> table;

    public:

    AssetManager(size_t cap = 101);

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    void insert(const std::string&, const Animation&);
    Animation* Search(const std::string&);

    ~AssetManager();
};

// Cargar assets
void LoadAssetsToTable(AssetManager&);
Animation LoadAsset(Image&, int, int, float);

#endif