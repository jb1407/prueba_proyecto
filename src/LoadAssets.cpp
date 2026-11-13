#include "LoadAssets.hpp"

//Definiciones de AssetManager

AssetManager::AssetManager(size_t cap) : table(cap, AssetComparator{}, [](const AssetEntry& entry) {return Designar::super_fast_hash(entry.key);}){}

void AssetManager::insert(const std::string& _Key, const Animation& _anim)
{
    AssetEntry entry{_Key, _anim};
    table.insert(entry);
}

Animation* AssetManager::Search(const std::string& _Key)
{
    // Búsqueda heterogénea eficiente provista por openhash.hpp[cite: 11]
    AssetEntry* entry = table.search_by(_Key, [](const std::string& k) {return Designar::super_fast_hash(k);});

    if (entry != nullptr) 
    {
        return &(entry->animation);
    }

    return nullptr;
}

AssetManager::~AssetManager()
{
    // Iteramos sobre los elementos ocupados para liberar las texturas de la GPU en Raylib[cite: 11]
    for (AssetEntry& entry : table) 
    {
        UnloadTexture(entry.animation.texture);
    }
}

//Carga de assets
void LoadAssetsToTable(AssetManager& manager)
{
    // Cargar animaciones base del personaje
    Image Img = LoadImage("assets/sprites/character/C_MoveSet_Base/C_Backward/C_Backward.png");
    manager.insert("C_Backward", LoadAsset(Img, 9, 5, 2.0f));

    Img = LoadImage("assets/sprites/character/C_MoveSet_Base/C_Walk/C_Walk.png");
    manager.insert("C_Walk", LoadAsset(Img, 12, 5, 2.0f));

    Img = LoadImage("assets/sprites/character/C_MoveSet_Base/C_Walk_UP/C_WalkUP.png");
    manager.insert("C_WalkUP", LoadAsset(Img, 8, 8, 2.0f));

    Img = LoadImage("assets/sprites/character/C_MoveSet_Base/C_Walk_Down/C_WalkDown.png");
    manager.insert("C_WalkDown", LoadAsset(Img, 8, 8, 2.0f));

    Img = LoadImage("assets/sprites/character/C_MoveSet_Base/C_De_Pie/C_De_Pie.png");
    manager.insert("C_Standing", LoadAsset(Img, 7, 9, 2.0f));

    Img = LoadImage("assets/sprites/character/C_MoveSet_Base/C_Punch/C_Punch.png");
    manager.insert("C_Punch", LoadAsset(Img, 10, 4, 2.0f));

    // Cargar animaciones de las armas
    //Neon Sword Red

    Img = LoadImage("assets/sprites/character/Weapons/NSR/NSR_Normal1(Character).png");
    manager.insert("NSR_Normal1(Character)", LoadAsset(Img, 9, 4, 2.0f));

    Img = LoadImage("assets/sprites/character/Weapons/NSR/NSR_Normal1(Sword).png");
    manager.insert("NSR_Normal1(Sword)", LoadAsset(Img, 9, 4, 1.5f));

    Img = LoadImage("assets/sprites/character/Weapons/NSR/NSR_Normal2(Character).png");
    manager.insert("NSR_Normal2(Character)", LoadAsset(Img, 19, 2, 2.0f));

    Img = LoadImage("assets/sprites/character/Weapons/NSR/NSR_Normal2(Sword).png");
    manager.insert("NSR_Normal2(Sword)", LoadAsset(Img, 19, 2, 1.5f));

    Img = LoadImage("assets/sprites/character/Weapons/NSR/NSR_Special1(Character).png");
    manager.insert("NSR_Special1(Character)", LoadAsset(Img, 10, 4, 3.0f));

    Img = LoadImage("assets/sprites/character/Weapons/NSR/NSR_Special1(Sword).png");
    manager.insert("NSR_Special1(Sword)", LoadAsset(Img, 18, 3, 1.5f));

    Img = LoadImage("assets/sprites/character/Weapons/NSR/NSR_Special1(Circle).png");
    manager.insert("NSR_Special1(Circle)", LoadAsset(Img, 20, 3, 2.0f));
}

Animation LoadAsset(Image& img, int TextureFrames, int FrameSpeed, float ScaleModifier)
{
    // Ahora el tamaño base se multiplica por el modificador (ej: 0.5 para armas)
    float HeightObj = ((float)GetScreenHeight() / 4) * ScaleModifier; 

    float Scale = HeightObj / (float)img.height;
    int NewWidth = (int)(img.width * Scale);
    int NewHeight = (int)HeightObj;
    ImageResizeNN(&img, NewWidth, NewHeight);

    Texture2D AnimTexture = LoadTextureFromImage(img);
    SetTextureFilter(AnimTexture, TEXTURE_FILTER_POINT);

    UnloadImage(img);

    Animation Anim = {AnimTexture, TextureFrames, AnimTexture.width/TextureFrames, AnimTexture.height, FrameSpeed, 0, 0};

    return Anim;
}