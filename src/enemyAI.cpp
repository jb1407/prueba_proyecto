#include "enemyAI.hpp"
#include <cmath>
#include <cstdlib>

namespace {
float DistanceBetween(Vector2 a, Vector2 b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool IsWithinAttackRange(Vector2 pos, Vector2 targetPos, float atkRange) {
    return DistanceBetween(pos, targetPos) <= atkRange;
}
} // namespace

EnemyAI::EnemyAI(float startX, float startY, AssetManager& assets, Difficulty diff) {
    size = { 115.0f, 200.0f };
    currentDifficulty = diff;
    health = 100.0f;
    maxHealth = 100.0f;
    damageMultiplier = 1.0f;

    switch (currentDifficulty) {
        case Difficulty::EASY:
            speed = { 3.0f, 3.0f };
            reactionDelay = 0.35f;
            attackRange = 50.0f;
            alertRange = 300.0f;
            damageMultiplier = 0.80f;
            break;

        case Difficulty::MEDIUM:
            speed = { 4.0f, 4.0f };
            reactionDelay = 0.25f;
            attackRange = 60.0f;
            alertRange = 450.0f;
            damageMultiplier = 1.00f;
            break;

        case Difficulty::HARD:
            speed = { 5.0f, 5.0f };
            reactionDelay = 0.15f;
            attackRange = 180.0f;
            alertRange = 600.0f;
            damageMultiplier = 1.25f;
            break;
    }

    currentAction = AIAction::IDLE;
    playerAttackCounter = 0;
    reactionTimer = 0.0f;
    actionDurationTimer = 0.0f;
    moveDir = { 0.0f, 0.0f };

    CPUPosition = { startX, startY };
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
    auto* leafAttack   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto* leafIdle     = new MTreeNode<DecisionKey>(DecisionKey(AIAction::IDLE));
    auto* leafApproach = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    const float atkRange = attackRange;

    // 1. Dentro del rango: 65% ATTACK_LIGHT, 35% IDLE
    auto* nodeCloseAttackChance = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 65;
        }
    ));
    nodeCloseAttackChance->append_child(leafAttack);
    leafAttack->add_right_sibling(leafIdle);

    // 2. Fuera del rango: APPROACH
    auto* nodeCheckRange = new MTreeNode<DecisionKey>(DecisionKey(
        [atkRange](Vector2 pos, Vector2 pPos, bool, int) {
            return IsWithinAttackRange(pos, pPos, atkRange);
        }
    ));
    nodeCheckRange->append_child(nodeCloseAttackChance);
    nodeCloseAttackChance->add_right_sibling(leafApproach);

    return nodeCheckRange;
}

MTreeNode<DecisionKey>* EnemyAI::BuildMediumTree() {
    auto* leafRetreatClose    = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto* leafRetreatPressure = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto* leafAttackLight     = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto* leafAttackHeavy     = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_HEAVY));
    auto* leafDashForward     = new MTreeNode<DecisionKey>(DecisionKey(AIAction::DASH_FORWARD));
    auto* leafApproach        = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    const float atkRange = attackRange;

    // 1. Elección de ataque: 75% golpe ligero, 25% golpe pesado en rango
    auto* nodeAttackChoice = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 75;
        }
    ));
    nodeAttackChoice->append_child(leafAttackLight);
    leafAttackLight->add_right_sibling(leafAttackHeavy);

    // 2. A corta distancia: 25% retrocede para dar aire, 75% ataca
    auto* nodeCloseMix = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 25;
        }
    ));
    nodeCloseMix->append_child(leafRetreatClose);
    leafRetreatClose->add_right_sibling(nodeAttackChoice);

    // 3. A mayor distancia: prioriza APPROACH (75%) sobre DASH_FORWARD (25%)
    auto* nodeFarMovement = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 25;
        }
    ));
    nodeFarMovement->append_child(leafDashForward);
    leafDashForward->add_right_sibling(leafApproach);

    // 4. Rango cercano táctico
    auto* nodeCloseRange = new MTreeNode<DecisionKey>(DecisionKey(
        [atkRange](Vector2 pos, Vector2 pPos, bool, int) {
            float closeLimit = (atkRange > 150.0f) ? atkRange : 150.0f;
            return DistanceBetween(pos, pPos) <= closeLimit;
        }
    ));
    nodeCloseRange->append_child(nodeCloseMix);
    nodeCloseMix->add_right_sibling(nodeFarMovement);

    // 5. Retroceso solo si encadenas varios ataques seguidos (atkCount >= 5)
    auto* nodeCheckPressure = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2, Vector2, bool, int atkCount) {
            return atkCount >= 5;
        }
    ));
    nodeCheckPressure->append_child(leafRetreatPressure);
    leafRetreatPressure->add_right_sibling(nodeCloseRange);

    return nodeCheckPressure;
}

MTreeNode<DecisionKey>* EnemyAI::BuildHardTree() {
    auto* leafBlock         = new MTreeNode<DecisionKey>(DecisionKey(AIAction::BLOCK));
    auto* leafRetreat       = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto* leafRetreatGlobal = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto* leafAttackLight   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto* leafAttackHeavy   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_HEAVY));
    auto* leafDashForward   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::DASH_FORWARD));
    auto* leafApproach      = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    const float atkRange = attackRange;

    // 1. Elección de ataque (Ligero en rango corto, Pesado al límite)
    auto* nodeAttackChoice = new MTreeNode<DecisionKey>(DecisionKey(
        [atkRange](Vector2 pos, Vector2 pPos, bool, int) {
            return IsWithinAttackRange(pos, pPos, atkRange);
        }
    ));
    nodeAttackChoice->append_child(leafAttackLight);
    leafAttackLight->add_right_sibling(leafAttackHeavy);

    // 2. Probabilidad táctica a corta distancia: solo 10% RETREAT, 90% ataca
    auto* nodeTacticalMix = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 10;
        }
    ));
    nodeTacticalMix->append_child(leafRetreat);
    leafRetreat->add_right_sibling(nodeAttackChoice);

    // 3. Persecución (> 150px): 60% DASH_FORWARD, 40% APPROACH
    auto* nodeFarAggression = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 60;
        }
    ));
    nodeFarAggression->append_child(leafDashForward);
    leafDashForward->add_right_sibling(leafApproach);

    // 4. Rango táctico cercano (<= 150.0f)
    auto* nodeCloseRange = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool, int) {
            return DistanceBetween(pos, pPos) <= 150.0f;
        }
    ));
    nodeCloseRange->append_child(nodeTacticalMix);
    nodeTacticalMix->add_right_sibling(nodeFarAggression);

    // 5. Presión por ataques sostenidos del jugador (atkCount >= 4)
    auto* nodeCheckRetreat = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2, Vector2, bool, int atkCount) {
            return atkCount >= 4;
        }
    ));
    nodeCheckRetreat->append_child(leafRetreatGlobal);
    leafRetreatGlobal->add_right_sibling(nodeCloseRange);

    // 6. Bloqueo defensivo solo si el jugador ataca cerca
    auto* nodeCheckBlock = new MTreeNode<DecisionKey>(DecisionKey(
        [atkRange](Vector2 pos, Vector2 pPos, bool attacking, int) {
            return attacking && DistanceBetween(pos, pPos) <= (atkRange * 1.25f);
        }
    ));
    nodeCheckBlock->append_child(leafBlock);
    leafBlock->add_right_sibling(nodeCheckRetreat);

    return nodeCheckBlock;
}

AIAction EnemyAI::EvaluateTree(Vector2 targetPos, bool isTargetAttacking) {
    if (isTargetAttacking) {
        playerAttackCounter++;
    } else {
        if (playerAttackCounter > 0) {
            playerAttackCounter--;
        }
    }

    if (decisionTreeRoot == nullptr) {
        return AIAction::IDLE;
    }

    MTreeNode<DecisionKey>* current = decisionTreeRoot;
    Vector2 currentPos = {
        CPUPosition.x + size.x * 0.5f,
        CPUPosition.y + size.y * 0.5f
    };

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

void EnemyAI::Update(Vector2 targetPos, bool isTargetAttacking, int screenWidth, int screenHeight) {
    float dt = GetFrameTime();
    lastTargetPos = targetPos;

    reactionTimer += dt;
    if (actionDurationTimer > 0.0f) {
        actionDurationTimer -= dt;
    }

    if (reactionTimer >= reactionDelay && actionDurationTimer <= 0.0f) {
        currentAction = EvaluateTree(targetPos, isTargetAttacking);
        reactionTimer = 0.0f;

        if (currentAction == AIAction::RETREAT || currentAction == AIAction::BLOCK) {
            currentAction = AIAction::RETREAT;
            actionDurationTimer = 0.35f;
        } else if (currentAction == AIAction::DASH_FORWARD) {
            actionDurationTimer = 0.35f;
        }
    }

    ExecuteAction(currentAction, targetPos, dt, screenWidth, screenHeight);
}

void EnemyAI::ClampToScreen(int screenWidth, int screenHeight) {
    if (CPUPosition.x < 0.0f) CPUPosition.x = 0.0f;
    if (CPUPosition.y < 0.0f) CPUPosition.y = 0.0f;
    if (CPUPosition.x + size.x > (float)screenWidth) CPUPosition.x = (float)screenWidth - size.x;
    if (CPUPosition.y + size.y > (float)screenHeight) CPUPosition.y = (float)screenHeight - size.y;

    position = CPUPosition;
}

void EnemyAI::FaceTarget(Vector2 targetPos) {
    if (targetPos.x < CPUPosition.x - 0.5f) flipX = true;
    else if (targetPos.x > CPUPosition.x + 0.5f) flipX = false;
}

void EnemyAI::RunMove(Move* targetMove, float dx, float dy) {
    if (targetMove == nullptr) return;

    Move* current = cpuCombat.GetCurrentMove();
    Animation* anim1 = targetMove->GetAnimation1();
    if (anim1 == nullptr) return;

    if (current != targetMove) {
        cpuCombat.SetCurrentMove(targetMove);
        cpuCombat.ExecuteMove(depthSwapStart, depthSwapEnd, flipX, rotation, true);
    }

    if (!cpuCombat.ValidateMove()) return;

    bool finishedAnim = (cpuCombat.GetAnimState().CurrentFrame1 == anim1->totalFrames - 1);

    CPUPosition.x += dx;
    CPUPosition.y += dy;
    position = CPUPosition;

    cpuCombat.ExecuteMove(depthSwapStart, depthSwapEnd, flipX, rotation, false);

    if (finishedAnim) {
        cpuCombat.ExecuteMove(depthSwapStart, depthSwapEnd, flipX, rotation, true);
    }
}

void EnemyAI::ExecuteAction(AIAction action, Vector2 targetPos, float dt, int screenWidth, int screenHeight) {
    Vector2 enemyCenter = {
        CPUPosition.x + size.x * 0.5f,
        CPUPosition.y + size.y * 0.5f
    };
    float dx = targetPos.x - enemyCenter.x;
    float dy = targetPos.y - enemyCenter.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    switch (action) {
        case AIAction::APPROACH: {
            if (dist > 0.0001f) {
                moveDir = { dx / dist, dy / dist };
                if (std::fabs(dx) > 0.5f) {
                    flipX = (targetPos.x < enemyCenter.x);
                }
                float vx = moveDir.x * speed.x;
                float vy = moveDir.y * speed.y;
                RunMove(moveSet.Walk, vx, vy);
            } else {
                moveDir = { 0.0f, 0.0f };
                RunMove(moveSet.Standing, 0.0f, 0.0f);
            }
            ClampToScreen(screenWidth, screenHeight);
            break;
        }

        case AIAction::DASH_FORWARD: {
            if (dist > 0.0001f) {
                moveDir = { dx / dist, dy / dist };
                if (std::fabs(dx) > 0.5f) {
                    flipX = (targetPos.x < enemyCenter.x);
                }
                float vx = moveDir.x * (speed.x * 2.2f);
                float vy = moveDir.y * (speed.y * 2.2f);
                RunMove(moveSet.BackWard, vx, vy);
            } else {
                moveDir = { 0.0f, 0.0f };
                RunMove(moveSet.Standing, 0.0f, 0.0f);
            }
            ClampToScreen(screenWidth, screenHeight);
            break;
        }

        case AIAction::RETREAT: {
            float awayX = enemyCenter.x - targetPos.x;
            float awayY = enemyCenter.y - targetPos.y;
            float awayLen = std::sqrt(awayX * awayX + awayY * awayY);
            if (awayLen > 0.0001f) {
                moveDir = { awayX / awayLen, awayY / awayLen };
                if (std::fabs(dx) > 0.5f) {
                    flipX = (targetPos.x < enemyCenter.x);
                }
                float vx = moveDir.x * (speed.x * 1.4f);
                float vy = moveDir.y * (speed.y * 1.4f);
                RunMove(moveSet.BackWard, vx, vy);
            } else {
                moveDir = { 0.0f, 0.0f };
                RunMove(moveSet.Standing, 0.0f, 0.0f);
            }
            ClampToScreen(screenWidth, screenHeight);

            // Si ya pegó contra un borde, cortamos el retroceso para no quedarse patinando ahí
            if (CPUPosition.x <= 0.0f ||
                CPUPosition.x >= (float)screenWidth - size.x ||
                CPUPosition.y <= 0.0f ||
                CPUPosition.y >= (float)screenHeight - size.y) {
                actionDurationTimer = 0.0f;
            }
            break;
        }

        case AIAction::ATTACK_LIGHT:
        case AIAction::ATTACK_HEAVY: {
            moveDir = { 0.0f, 0.0f };
            FaceTarget(targetPos);
            RunMove(moveSet.Punch, 0.0f, 0.0f);
            ClampToScreen(screenWidth, screenHeight);
            break;
        }

        case AIAction::BLOCK:
        case AIAction::IDLE:
        default: {
            moveDir = { 0.0f, 0.0f };
            FaceTarget(targetPos);
            RunMove(moveSet.Standing, 0.0f, 0.0f);
            ClampToScreen(screenWidth, screenHeight);
            break;
        }
    }
}

void EnemyAI::TakeDamage(float amount) {
    float finalDamage = amount * damageMultiplier;
    health -= finalDamage;
    if (health < 0.0f) health = 0.0f;
}