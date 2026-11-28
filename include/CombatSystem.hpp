#ifndef CombatSystem_HPP
#define CombatSystem_HPP

#include "PlayerAndWeapons.hpp"

class MoveSetData
{
    private:
    std::vector<Move*> AllMoves;

    public:
    MoveSetData();
    MoveSetData(const MoveSetData&) = delete;
    MoveSetData& operator=(const MoveSetData&) = delete;

    MoveSetBase CharacterMoveSetDefault(AssetManager&, Vector2*); //Está preparado para recibir tanto el vector del jugador como el del Bot
    MoveSetWeapons NeonSwordRedMoves(AssetManager&, Vector2*, Vector2*, Vector2*);

    ~MoveSetData();
};

class Combat //Las animaciones tienen multiples propiedades. Las mismas asi como la informacion de los inputs se manejarán aquí (uno por jugador)
{
    private:
    Player* P; //Todos los datos del jugador

    int DepthSwapStart; //Propiedades clave de la animacion al momento de ejecutarse
    int DepthSwapEnd;
    bool FlipX;
    float Rotation;

    int FrameCounterMove = 0; //Se maneja internamente

    bool MoveEndOrReset; //Maneja el finished o reseteo de la animacion
    int FrameMoveEnd; //Indica en qué frame la animacion debe terminar (en caso de ser cancelable)

    CombatManager PlayerCombat{nullptr, nullptr, CombatMode::UnArmed};

    bool IsBackward = false;
    Vector2 DashVelocity = {8, 8};

    public:
    Combat() = delete;
    Combat(const Combat&) = delete;
    Combat& operator=(const Combat&) = delete;

    Combat(Player*, int, int, bool, float, bool, int);

    void SetDepthSwapStart(int);
    void SetDepthSwapEnd(int);
    void SetMoveEnd(bool);
    void SetFrameMoveEnd(int);
    void SetFlipX(bool);
    void SetRotation(float);

    int GetDepthSwapStart() const;
    int GetDepthSwapEnd() const;
    bool GetMoveEnd() const;
    int GetFrameMoveEnd() const;
    bool GetFlipX() const;
    float GetRotation() const;

    void UpdateMoveBasePlayer1();
    void UpdateMoveNSRPlayer1();
};
#endif