#ifndef PLAYERANDWEAPONS_HPP
#define PLAYERANDWEAPONS_HPP

#include "Mechanics.hpp"

struct MoveSetWeapons
{
    Move* Normal1 = nullptr;
    Move* Normal2 = nullptr;
    Move* Special1 = nullptr;
    Move* Special2 = nullptr;
    Move* Ultimate = nullptr;
};

class Weapon
{
    private:
    std::string WeapName;
    MoveSetWeapons MoveSet;
    int Duration; //Cada arma tendrá una duración antes de desaparecer. Si está en el suelo sin equiparse, su tiempo será mucho más bajo en comparación a su tiempo en inventario. La duracion será en segundos (Duration*60)
    bool Equippable; //Detecta si un arma es equipable o no (negativo si las dos ranuras están llenas o ya hay un arma igual equipada)

    public:
    Weapon() = delete;
    //el operador = y el constructor por copia los mantendré, creo que para este caso si resulta conveniente
    Weapon(const std::string&, MoveSetWeapons, int, bool);
    Weapon(const Weapon&);

    void SetEquippable(bool); //Dado que el jugador puede estar tomando y desechando armas a lo largo del combate, es bueno que esto se pueda cambiar en tiempo real
    void SetDuration(int); //Por supuesto, esto tambien tendrá que cambiar, aunque si veo la forma de que lo haga sin necesidad de esto, lo quitaré

    std::string GetWeaponName() const;
    MoveSetWeapons GetMoveSet() const;
    int GetDuration() const;
    bool GetEquippable() const;
};

struct MoveSetBase
{
    Move* Standing = nullptr;
    Move* Walk = nullptr;
    Move* WalkUP = nullptr;
    Move* WalkDown = nullptr;
    Move* Punch = nullptr;
    Move* BackWard = nullptr;
};
 
enum ActualWeapon
{
    Weapon1 = 0,
    Weapon2
};

class Player
{
    private:
    int Health;
    int Energy; //Usado Exclusivamente para activar las ultis, si un jugador logra un combo maximo, llega al 100 y puede activar el ultimate (solo en modo de combate 2)
    int ShieldEnergy; //El personaje podrá defenderse de ataques físicos, bloqueando todo el daño, pero ésta defensa se desgasta
    int TimeStun; //Tiempo de Stun que druará el persoaje si su defensa es atravesada (se cancela si recibe un golpe despues)

    Weapon* Arm1;
    Weapon* Arm2;

    ActualWeapon ActArm = Weapon1;

    bool IsAttack;

    MoveSetBase MoveBase;

    public:
    Player() = delete;
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;
    
    Player(int, int, int, int, Weapon*, Weapon*, bool, MoveSetBase);

    void DamageReceived(int);

    void SetHealth(int);
    void SetEnergy(int);
    void SetIsAttack(bool);
    void SetTimeStun(int);
    void SetShieldEnergy(int);
    void SetActualWeapon(ActualWeapon);

    int GetHealth() const;
    int GetEnergy() const;
    bool GetIsAttack() const;
    int GetTimeStun() const;
    int GetShieldEnergy() const;
    ActualWeapon GetActualWeapon() const;
    MoveSetBase GetMoveBase() const;

    Weapon* GetWeapon1() const;
    Weapon* GetWeapon2() const;
};

#endif