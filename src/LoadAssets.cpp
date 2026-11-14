#include "LoadAssets.hpp"

// Implementación de la función que actualiza y dibuja
void UpdateAndDrawAnimation(Animation& anim, Vector2 position, bool flipX, float rotation) {
    anim.tickCounter++;
    
    if (anim.tickCounter >= anim.frameSpeed) {
        anim.tickCounter = 0;
        anim.currentFrame = (anim.currentFrame + 1) % anim.totalFrames;
    }

    float widthMultiplier = static_cast<float>(anim.frameWidth);
    if (flipX) {
        widthMultiplier = -widthMultiplier;
    }

    Rectangle sourceRec = {
        (float)(anim.currentFrame * anim.frameWidth), 
        0.0f,                                         
        widthMultiplier,                              
        (float)anim.frameHeight                       
    };

    Rectangle destRec = {
        position.x, 
        position.y, 
        (float)anim.frameWidth, 
        (float)anim.frameHeight
    };

    // Por defecto, dibujamos desde la esquina superior izquierda (comportamiento original)
    Vector2 origin = { 0.0f, 0.0f };

    // SÓLO si se especifica una rotación (como en el Backward), centramos el origen para que gire sobre su propio eje
    if (rotation != 0.0f) {
        origin = { (float)anim.frameWidth / 2.0f, (float)anim.frameHeight / 2.0f };
        destRec.x += origin.x;
        destRec.y += origin.y;
    }

    DrawTexturePro(anim.texture, sourceRec, destRec, origin, rotation, WHITE);
}


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
    manager.insert("C_Backward", LoadAsset(Img, 9, 5));

    Img = LoadImage("assets/sprites/character/C_MoveSet_Base/C_Walk/C_Walk.png");
    manager.insert("C_Walk", LoadAsset(Img, 12, 5));

    Img = LoadImage("assets/sprites/character/C_MoveSet_Base/C_Walk_UP/C_WalkUP.png");
    manager.insert("C_WalkUP", LoadAsset(Img, 8, 8));

    Img = LoadImage("assets/sprites/character/C_MoveSet_Base/C_Walk_Down/C_WalkDown.png");
    manager.insert("C_WalkDown", LoadAsset(Img, 8, 8));

    Img = LoadImage("assets/sprites/character/C_MoveSet_Base/C_De_Pie/C_De_Pie.png");
    manager.insert("C_Standing", LoadAsset(Img, 7, 9));

    Img = LoadImage("assets/sprites/character/C_MoveSet_Base/C_Punch/C_Punch.png");
    manager.insert("C_Punch", LoadAsset(Img, 10, 2));
}

Animation LoadAsset(Image& img, int TextureFrames, int FrameSpeed)
{
    float HeightObj = (float)GetScreenHeight()/4; 

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