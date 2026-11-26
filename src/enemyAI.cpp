#include "enemyAI.hpp"
#include <cmath>
#include <cstdlib>

namespace {
float DistanceBetween(Vector2 a, Vector2 b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}
} // namespace

EnemyAI::EnemyAI(float startX, float startY, AssetManager& assets, Difficulty diff) {
    size = {115.0f, 200.0f};
    currentDifficulty = diff;
    activeBackVel = {0.0f, 0.0f};

    switch (currentDifficulty) {
        case Difficulty::EASY:
            speed = {3.5f, 3.5f};
            dashSpeed = {6.5f, 6.5f};
            decisionInterval = 0.35f;
            attackRange = 130.0f;
            alertRange = 420.0f;
            break;

        case Difficulty::MEDIUM:
            speed = {4.0f, 4.0f};
            dashSpeed = {7.5f, 7.5f};
            decisionInterval = 0.25f;
            attackRange = 135.0f;
            alertRange = 520.0f;
            break;

        case Difficulty::HARD:
            speed = {5.0f, 5.0f};
            dashSpeed = {9.0f, 9.0f};
            decisionInterval = 0.15f;
            attackRange = 145.0f;
            alertRange = 600.0f;
            break;
    }

    decisionTimer = 0.0f;
    playerAttackCounter = 0;
    consecutiveAttacks = 0;
    pendingPostAttackRetreat = false;

    CPUPosition = {startX, startY};
    position = CPUPosition;
    lastTargetPos = CPUPosition;

    moveSet = moveSetData.CharacterMoveSetDefault(assets, &CPUPosition);
    cpuCombat.SetCurrentMove(moveSet.Standing);

    decisionTreeRoot = BuildDecisionTree(currentDifficulty);
}

EnemyAI::~EnemyAI() {
    MTreeNode<DecisionKey>::destroy_tree(decisionTreeRoot);
}

MTreeNode<DecisionKey>* EnemyAI::BuildDecisionTree(Difficulty diff) {
    switch (diff) {
        case Difficulty::EASY:   return BuildEasyTree();
        case Difficulty::MEDIUM: return BuildMediumTree();
        case Difficulty::HARD:   return BuildHardTree();
    }
    return BuildMediumTree();
}

MTreeNode<DecisionKey>* EnemyAI::BuildEasyTree() {
    const float atkRange = attackRange;

    // Si está cerca: ~80% ataca, ~20% hace un BackWard de amague/deslizamiento
    auto* closeMixCond = new MTreeNode<DecisionKey>(
        DecisionKey([](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 20;
        })
    );
    closeMixCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT)));
    closeMixCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT)));

    // Raíz: ¿Está a rango de ataque? Si no, se acerca caminando
    auto* root = new MTreeNode<DecisionKey>(
        DecisionKey([atkRange](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= atkRange;
        })
    );
    root->append_child(closeMixCond);
    root->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH)));

    return root;
}

MTreeNode<DecisionKey>* EnemyAI::BuildMediumTree() {
    const float atkRange = attackRange;
    const float alertDist = alertRange;

    // 1. A corta distancia: ~65% ataca, ~35% retrocede con BackWard
    auto* closeProbCond = new MTreeNode<DecisionKey>(
        DecisionKey([](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 65;
        })
    );
    closeProbCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT)));
    closeProbCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT)));

    // 2. A larga distancia (> alertDist): ~35% DASH_FORWARD, ~65% APPROACH
    auto* farProbCond = new MTreeNode<DecisionKey>(
        DecisionKey([](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 35;
        })
    );
    farProbCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::DASH_FORWARD)));
    farProbCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH)));

    // 3. Rango medio vs lejano
    auto* alertRangeCond = new MTreeNode<DecisionKey>(
        DecisionKey([alertDist](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= alertDist;
        })
    );
    alertRangeCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH)));
    alertRangeCond->append_child(farProbCond);

    // 4. ¿Está a rango cuerpo a cuerpo?
    auto* attackRangeCond = new MTreeNode<DecisionKey>(
        DecisionKey([atkRange](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= atkRange;
        })
    );
    attackRangeCond->append_child(closeProbCond);
    attackRangeCond->append_child(alertRangeCond);

    // 5. Retroceso por presión de ataques del jugador
    auto* pressureRetreatCond = new MTreeNode<DecisionKey>(
        DecisionKey([](Vector2, Vector2, bool, int atkCount) {
            return atkCount >= 3;
        })
    );
    pressureRetreatCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT)));
    pressureRetreatCond->append_child(attackRangeCond);

    // 6. Raíz: ¿El jugador está atacando muy cerca?
    auto* root = new MTreeNode<DecisionKey>(
        DecisionKey([atkRange](Vector2 pos, Vector2 targetPos, bool isTargetAttacking, int) {
            return isTargetAttacking && DistanceBetween(pos, targetPos) <= atkRange * 1.25f;
        })
    );
    root->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::BLOCK)));
    root->append_child(pressureRetreatCond);

    return root;
}

MTreeNode<DecisionKey>* EnemyAI::BuildHardTree() {
    const float atkRange = attackRange;
    const float closeTacticalDist = 175.0f;
    const float wideRetreatDist = 600.0f;

    // 1. Elección de golpe en rango cercano: alterna ataque pesado y ligero
    auto* attackTypeCond = new MTreeNode<DecisionKey>(
        DecisionKey([](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 40;
        })
    );
    attackTypeCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_HEAVY)));
    attackTypeCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT)));

    // 2. Mezcla táctica cercana (~50% RETREAT / ~50% atacar si está en rango, o acercarse si está al borde)
    auto* inStrikeRangeCond = new MTreeNode<DecisionKey>(
        DecisionKey([atkRange](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= atkRange;
        })
    );
    inStrikeRangeCond->append_child(attackTypeCond);
    inStrikeRangeCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH)));

    auto* closeMixCond = new MTreeNode<DecisionKey>(
        DecisionKey([](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 50;
        })
    );
    closeMixCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT)));
    closeMixCond->append_child(inStrikeRangeCond);

    // 3. Persecución / movimiento a media-larga distancia: DASH_FORWARD, RETREAT (incluso a ~600px) o APPROACH
    auto* farDashOrRetreatCond = new MTreeNode<DecisionKey>(
        DecisionKey([](Vector2, Vector2, bool, int) {
            int roll = std::rand() % 100;
            if (roll < 55) return true; // DASH_FORWARD
            return false;
        })
    );

    auto* farRetreatOrApproachCond = new MTreeNode<DecisionKey>(
        DecisionKey([](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 45; // RETREAT deslizante a distancia amplia
        })
    );
    farRetreatOrApproachCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT)));
    farRetreatOrApproachCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH)));

    farDashOrRetreatCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::DASH_FORWARD)));
    farDashOrRetreatCond->append_child(farRetreatOrApproachCond);

    // 4. ¿Está en zona táctica cercana o dentro del rango amplio de presión (~600px)?
    auto* closeZoneCond = new MTreeNode<DecisionKey>(
        DecisionKey([closeTacticalDist](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= closeTacticalDist;
        })
    );
    closeZoneCond->append_child(closeMixCond);
    closeZoneCond->append_child(farDashOrRetreatCond);

    auto* withinWidePressureCond = new MTreeNode<DecisionKey>(
        DecisionKey([wideRetreatDist](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= wideRetreatDist;
        })
    );
    withinWidePressureCond->append_child(closeZoneCond);
    withinWidePressureCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::DASH_FORWARD)));

    // 5. Presión por ataques del rival
    auto* pressureCond = new MTreeNode<DecisionKey>(
        DecisionKey([](Vector2, Vector2, bool, int atkCount) {
            return atkCount >= 2;
        })
    );
    pressureCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT)));
    pressureCond->append_child(withinWidePressureCond);

    // 6. Raíz: reacción si el jugador ataca cerca
    auto* root = new MTreeNode<DecisionKey>(
        DecisionKey([atkRange](Vector2 pos, Vector2 targetPos, bool isTargetAttacking, int) {
            return isTargetAttacking && DistanceBetween(pos, targetPos) <= atkRange * 1.35f;
        })
    );
    root->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::BLOCK)));
    root->append_child(pressureCond);

    return root;
}

AIAction EnemyAI::EvaluateTree(Vector2 targetPos, bool isTargetAttacking) const {
    if (decisionTreeRoot == nullptr) {
        return AIAction::IDLE;
    }

    MTreeNode<DecisionKey>* current = decisionTreeRoot;
    Vector2 currentPos = CPUPosition;

    while (current != nullptr) {
        const DecisionKey& key = current->get_key();

        if (key.type == NodeType::ACTION) {
            return key.action;
        }

        MTreeNode<DecisionKey>* trueBranch = current->get_first_child();
        MTreeNode<DecisionKey>* falseBranch = (trueBranch != nullptr) ? trueBranch->get_right_sibling() : nullptr;

        bool condResult = key.condition
            ? key.condition(currentPos, targetPos, isTargetAttacking, playerAttackCounter)
            : false;

        if (condResult) {
            current = trueBranch;
        } else {
            current = (falseBranch != nullptr) ? falseBranch : trueBranch;
        }
    }

    return AIAction::IDLE;
}

void EnemyAI::HandlePostAttack(int screenWidth, int screenHeight) {
    float dx = lastTargetPos.x - CPUPosition.x;
    float dy = lastTargetPos.y - CPUPosition.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    float dirX = (dist > 0.0001f) ? (dx / dist) : (flipX ? -1.0f : 1.0f);
    float dirY = (dist > 0.0001f) ? (dy / dist) : 0.0f;

    // Si ya alcanzó el tope de 2 golpes seguidos, fuerza el retroceso para salir del rango/cambiar ritmo
    if (consecutiveAttacks >= maxConsecutiveAttacks) {
        consecutiveAttacks = 0;
        pendingPostAttackRetreat = false;
        currentAction = AIAction::IDLE;
        decisionTimer = decisionInterval;
        DashBackward(-dirX, -dirY, screenWidth, screenHeight);
        return;
    }

    // Si tras el primer golpe se decidió retroceder (Caso 1: golpe -> retroceso)
    if (pendingPostAttackRetreat) {
        consecutiveAttacks = 0;
        pendingPostAttackRetreat = false;
        currentAction = AIAction::IDLE;
        decisionTimer = decisionInterval;

        // En MEDIUM/HARD puede deslizarse alejándose o hacer un cruce lateral corto
        bool crossSide = (currentDifficulty == Difficulty::HARD) && ((std::rand() % 100) < 35);
        if (crossSide) {
            float slideY = (dirY >= 0.0f) ? -0.65f : 0.65f;
            DashBackward(-dirX * 0.7f, slideY, screenWidth, screenHeight);
        } else {
            DashBackward(-dirX, -dirY, screenWidth, screenHeight);
        }
        return;
    }

    // Con 1 golpe conectado hasta ahora, probabilidad de encadenar un 2º golpe rápido (Caso 2: golpe -> golpe -> retroceso)
    int secondHitChance = 0;
    if (currentDifficulty == Difficulty::MEDIUM) secondHitChance = 25;
    else if (currentDifficulty == Difficulty::HARD) secondHitChance = 55;

    if (dist <= attackRange * 1.15f && (std::rand() % 100) < secondHitChance) {
        pendingPostAttackRetreat = true; // Tras este 2º golpe retrocederá
        currentAction = AIAction::IDLE;
        decisionTimer = 0.05f;
        FaceTarget(lastTargetPos);
        AttackPunch(screenWidth, screenHeight);
        return;
    }

    // Si no encadena segundo golpe ni retrocede de inmediato, queda listo para reevaluar o quedarse ahí
    decisionTimer = 0.0f;
}

void EnemyAI::Update(Vector2 targetPos, bool isTargetAttacking, int screenWidth, int screenHeight) {
    lastTargetPos = targetPos;

    if (isTargetAttacking) {
        playerAttackCounter++;
    } else if (playerAttackCounter > 0) {
        playerAttackCounter--;
    }

    if (isAttacking || isBacking) {
        UpdateFrame(screenWidth, screenHeight);
        return;
    }

    decisionTimer -= GetFrameTime();
    if (decisionTimer <= 0.0f) {
        currentAction = EvaluateTree(targetPos, isTargetAttacking);
        decisionTimer = decisionInterval;
    }

    ExecuteAction(currentAction, targetPos, screenWidth, screenHeight);
}

void EnemyAI::ClampToScreen(int screenWidth, int screenHeight) {
    if (CPUPosition.x < 0.0f) CPUPosition.x = 0.0f;
    if (CPUPosition.y < 0.0f) CPUPosition.y = 0.0f;
    if (CPUPosition.x + size.x > (float)screenWidth) CPUPosition.x = (float)screenWidth - size.x;
    if (CPUPosition.y + size.y > (float)screenHeight) CPUPosition.y = (float)screenHeight - size.y;

    position = CPUPosition;
}

void EnemyAI::FaceTarget(Vector2 targetPos) {
    if (isAttacking || isBacking) return;
    if (targetPos.x < CPUPosition.x) flipX = true;
    else if (targetPos.x > CPUPosition.x) flipX = false;
}

void EnemyAI::RunMove(Move* targetMove, float dx, float dy) {
    if (targetMove == nullptr) return;

    Move* current = cpuCombat.GetCurrentMove();
    Animation* anim1 = targetMove->GetAnimation1();
    if (anim1 == nullptr) return;

    if (current != targetMove) {
        cpuCombat.ExecuteMove(depthSwapStart, depthSwapEnd, flipX, rotation, true);
        cpuCombat.SetCurrentMove(targetMove);
    }

    if (!cpuCombat.ValidateMove()) return;

    bool finishedAnim = (cpuCombat.GetAnimState().CurrentFrame1 == anim1->totalFrames - 1);

    CPUPosition.x += dx;
    CPUPosition.y += dy;
    position = CPUPosition;

    cpuCombat.ExecuteMove(depthSwapStart, depthSwapEnd, flipX, rotation, false);

    if (finishedAnim) {
        cpuCombat.ExecuteMove(depthSwapStart, depthSwapEnd, flipX, rotation, true);

        if (isAttacking) {
            isAttacking = false;
            HandlePostAttack(GetScreenWidth(), GetScreenHeight());
            return;
        }

        if (isBacking) {
            isBacking = false;
            activeBackVel = {0.0f, 0.0f};
            decisionTimer = 0.0f;
        }
    }
}

void EnemyAI::ExecuteAction(AIAction action, Vector2 targetPos, int screenWidth, int screenHeight) {
    if (isAttacking || isBacking) {
        UpdateFrame(screenWidth, screenHeight);
        return;
    }

    float dx = targetPos.x - CPUPosition.x;
    float dy = targetPos.y - CPUPosition.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    float dirX = (dist > 0.0001f) ? (dx / dist) : 0.0f;
    float dirY = (dist > 0.0001f) ? (dy / dist) : 0.0f;

    switch (action) {
        case AIAction::IDLE:
            StandIdle(screenWidth, screenHeight);
            break;

        case AIAction::APPROACH:
            // Se mantiene APPROACH mientras dure el intervalo para que el caminado sea fluido
            if (std::fabs(dy) > std::fabs(dx)) {
                if (dy < 0.0f) MoveUp(screenWidth, screenHeight);
                else MoveDown(screenWidth, screenHeight);
            } else {
                if (dx < 0.0f) MoveLeft(screenWidth, screenHeight);
                else if (dx > 0.0f) MoveRight(screenWidth, screenHeight);
                else StandIdle(screenWidth, screenHeight);
            }
            break;

        case AIAction::DASH_FORWARD:
            DashBackward(dirX, dirY, screenWidth, screenHeight);
            currentAction = AIAction::IDLE;
            break;

        case AIAction::RETREAT: {
            int retreatChanceAfterHit = 0;
            if (currentDifficulty == Difficulty::EASY) retreatChanceAfterHit = 15;
            else if (currentDifficulty == Difficulty::MEDIUM) retreatChanceAfterHit = 45;
            else retreatChanceAfterHit = 65;

            pendingPostAttackRetreat = false;
            consecutiveAttacks = 0;
            (void)retreatChanceAfterHit;

            DashBackward(-dirX, -dirY, screenWidth, screenHeight);
            currentAction = AIAction::IDLE;
            break;
        }

        case AIAction::ATTACK_LIGHT:
        case AIAction::ATTACK_HEAVY: {
            if (consecutiveAttacks < maxConsecutiveAttacks) {
                consecutiveAttacks++;
            }

            // Decide si tras este primer golpe retrocederá (Caso 1: golpe -> retroceso)
            int retreatAfterHitChance = 0;
            if (currentDifficulty == Difficulty::EASY) retreatAfterHitChance = 15;
            else if (currentDifficulty == Difficulty::MEDIUM) retreatAfterHitChance = 40;
            else retreatAfterHitChance = 55;

            pendingPostAttackRetreat = (consecutiveAttacks == 1) && ((std::rand() % 100) < retreatAfterHitChance);

            FaceTarget(targetPos);
            AttackPunch(screenWidth, screenHeight);
            currentAction = AIAction::IDLE;
            break;
        }

        case AIAction::BLOCK:
            FaceTarget(targetPos);
            StandIdle(screenWidth, screenHeight);
            currentAction = AIAction::IDLE;
            break;
    }
}

void EnemyAI::UpdateFrame(int screenWidth, int screenHeight) {
    if (isAttacking) {
        RunMove(moveSet.Punch, 0.0f, 0.0f);
        ClampToScreen(screenWidth, screenHeight);
        return;
    }

    if (isBacking) {
        RunMove(moveSet.BackWard, activeBackVel.x, activeBackVel.y);
        ClampToScreen(screenWidth, screenHeight);
        return;
    }

    RunMove(cpuCombat.GetCurrentMove() ? cpuCombat.GetCurrentMove() : moveSet.Standing, 0.0f, 0.0f);
    ClampToScreen(screenWidth, screenHeight);
}

void EnemyAI::MoveUp(int screenWidth, int screenHeight) {
    if (isAttacking || isBacking) {
        UpdateFrame(screenWidth, screenHeight);
        return;
    }
    RunMove(moveSet.WalkUP, 0.0f, -speed.y);
    ClampToScreen(screenWidth, screenHeight);
}

void EnemyAI::MoveDown(int screenWidth, int screenHeight) {
    if (isAttacking || isBacking) {
        UpdateFrame(screenWidth, screenHeight);
        return;
    }
    RunMove(moveSet.WalkDown, 0.0f, speed.y);
    ClampToScreen(screenWidth, screenHeight);
}

void EnemyAI::MoveLeft(int screenWidth, int screenHeight) {
    if (isAttacking || isBacking) {
        UpdateFrame(screenWidth, screenHeight);
        return;
    }
    flipX = true;
    RunMove(moveSet.Walk, -speed.x, 0.0f);
    ClampToScreen(screenWidth, screenHeight);
}

void EnemyAI::MoveRight(int screenWidth, int screenHeight) {
    if (isAttacking || isBacking) {
        UpdateFrame(screenWidth, screenHeight);
        return;
    }
    flipX = false;
    RunMove(moveSet.Walk, speed.x, 0.0f);
    ClampToScreen(screenWidth, screenHeight);
}

void EnemyAI::AttackPunch(int screenWidth, int screenHeight) {
    if (isBacking) {
        UpdateFrame(screenWidth, screenHeight);
        return;
    }
    isAttacking = true;
    RunMove(moveSet.Punch, 0.0f, 0.0f);
    ClampToScreen(screenWidth, screenHeight);
}

void EnemyAI::DashBackward(float dirX, float dirY, int screenWidth, int screenHeight) {
    if (isAttacking) {
        UpdateFrame(screenWidth, screenHeight);
        return;
    }
    if (dirX < 0.0f) flipX = true;
    else if (dirX > 0.0f) flipX = false;

    activeBackVel = {dirX * dashSpeed.x, dirY * dashSpeed.y};
    isBacking = true;
    RunMove(moveSet.BackWard, activeBackVel.x, activeBackVel.y);
    ClampToScreen(screenWidth, screenHeight);
}

void EnemyAI::StandIdle(int screenWidth, int screenHeight) {
    if (isAttacking || isBacking) {
        UpdateFrame(screenWidth, screenHeight);
        return;
    }
    RunMove(moveSet.Standing, 0.0f, 0.0f);
    ClampToScreen(screenWidth, screenHeight);
}