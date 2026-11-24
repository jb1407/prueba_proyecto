#include "PlayerAndWeapons.hpp"

Weapon::Weapon(const std::string& Name, MoveSetWeapons MSet, int Dur, bool Equip) : WeapName(Name), MoveSet(MSet), Duration(Dur), Equippable(Equip){}
Weapon::Weapon(const Weapon& NewWeap)
{ 
    WeapName = NewWeap.GetWeaponName();
    MoveSet = NewWeap.GetMoveSet();
    Duration = NewWeap.GetDuration();
    Equippable = NewWeap.GetEquippable();
}

void Weapon::SetEquippable(bool Equip){Equippable = Equip;}
void Weapon::SetDuration(int Dur){Duration = Dur;}

std::string Weapon::GetWeaponName() const{return WeapName;}
bool Weapon::GetEquippable() const {return Equippable;}
int Weapon::GetDuration() const {return Duration;}
MoveSetWeapons Weapon::GetMoveSet() const{return MoveSet;}

Player::Player(int health, int energy, int Shield, int Stun, Weapon* arm1, Weapon* arm2, bool isAttack, MoveSetBase moveBase) : Health(health), Energy(energy), ShieldEnergy(Shield), TimeStun(Stun), Arm1(arm1), Arm2(arm2), IsAttack(isAttack), MoveBase(moveBase){}

void Player::DamageReceived(int Damage)
{
    if(Health > 0)Health -= Damage;
    else{Health = 0;}
}

void Player::SetHealth(int NewHealth){if(NewHealth >= 0)Health = NewHealth;}
void Player::SetEnergy(int NewEnergy){if(Energy >= 0)Energy = NewEnergy;}
void Player::SetIsAttack(bool isAttack){IsAttack = isAttack;}
void Player::SetTimeStun(int Stun){TimeStun = Stun;}
void Player::SetShieldEnergy(int Shield){ShieldEnergy = Shield;}
void Player::SetActualWeapon(ActualWeapon Act){ActArm = Act;}

int Player::GetHealth() const{return Health;}
int Player::GetEnergy() const{return Energy;}
bool Player::GetIsAttack() const{return IsAttack;}
int Player::GetTimeStun() const{return TimeStun;}
int Player::GetShieldEnergy() const{return ShieldEnergy;}
ActualWeapon Player::GetActualWeapon() const{return ActArm;}
MoveSetBase Player::GetMoveBase() const{return MoveBase;}
Weapon* Player::GetWeapon1() const{return Arm1;}
Weapon* Player::GetWeapon2() const{return Arm2;}