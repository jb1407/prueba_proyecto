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
            attackRange = 155.0f;
            alertRange = 350.0f;
            break;

        case Difficulty::MEDIUM:
            speed = {4.0f, 4.0f};
            dashSpeed = {7.5f, 7.5f};
            decisionInterval = 0.25f;
            attackRange = 170.0f;
            alertRange = 500.0f;
            break;

        case Difficulty::HARD:
            speed = {6.0f, 6.0f};
            dashSpeed = {10.0f, 10.0f};
            decisionInterval = 0.15f;
            attackRange = 190.0f;
            alertRange = 650.0f;
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
    const float dashThreshold = alertRange; // 350.0f

    // Si supera 300 px: DASH_FORWARD; si no: APPROACH
    auto* nodeFarDash = new MTreeNode<DecisionKey>(DecisionKey(
        [dashThreshold](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= dashThreshold;
        }
    ));
    auto* leafDashForward = new MTreeNode<DecisionKey>(DecisionKey(AIAction::DASH_FORWARD));
    auto* leafApproach    = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));
    nodeFarDash->append_child(leafDashForward);
    leafDashForward->add_right_sibling(leafApproach);

    // Raíz: ¿Está a rango de ataque?
    auto* leafAttack = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto* nodeCheckAttack = new MTreeNode<DecisionKey>(DecisionKey(
        [atkRange](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= atkRange;
        }
    ));
    nodeCheckAttack->append_child(leafAttack);
    leafAttack->add_right_sibling(nodeFarDash);

    return nodeCheckAttack;
}

MTreeNode<DecisionKey>* EnemyAI::BuildMediumTree() {
    const float atkRange = attackRange;
    const float dashThreshold = alertRange; // 500.0f

    // 1. Corta distancia: 70% ataca, 30% retrocede (amague táctico)
    auto* nodeCloseProb = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 70;
        }
    ));
    auto* leafAttackClose = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto* leafRetreatClose = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    nodeCloseProb->append_child(leafAttackClose);
    leafAttackClose->add_right_sibling(leafRetreatClose);

    // 2. Larga distancia (> 450 px): 35% DASH_FORWARD, 65% APPROACH (o 100% DASH si supera umbral)
    auto* nodeFarDashThreshold = new MTreeNode<DecisionKey>(DecisionKey(
        [dashThreshold](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= dashThreshold;
        }
    ));
    auto* nodeFarProb = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 35;
        }
    ));
    auto* leafDashFar = new MTreeNode<DecisionKey>(DecisionKey(AIAction::DASH_FORWARD));
    auto* leafApproachFar = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));
    nodeFarProb->append_child(leafDashFar);
    leafDashFar->add_right_sibling(leafApproachFar);

    auto* leafApproachMid = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));
    nodeFarDashThreshold->append_child(nodeFarProb);
    nodeFarProb->add_right_sibling(leafApproachMid);

    // 3. ¿Está a rango cuerpo a cuerpo?
    auto* nodeCheckDistance = new MTreeNode<DecisionKey>(DecisionKey(
        [atkRange](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= atkRange;
        }
    ));
    nodeCheckDistance->append_child(nodeCloseProb);
    nodeCloseProb->add_right_sibling(nodeFarDashThreshold);

    // 4. Retroceso por presión del jugador
    auto* nodeCheckRetreat = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2, Vector2, bool, int atkCount) {
            return atkCount >= 3;
        }
    ));
    auto* leafRetreatPressure = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    nodeCheckRetreat->append_child(leafRetreatPressure);
    leafRetreatPressure->add_right_sibling(nodeCheckDistance);

    // 5. Bloqueo defensivo cerca si el jugador ataca
    auto* nodeCheckBlock = new MTreeNode<DecisionKey>(DecisionKey(
        [atkRange](Vector2 pos, Vector2 targetPos, bool attacking, int) {
            return attacking && DistanceBetween(pos, targetPos) <= (atkRange * 1.3f);
        }
    ));
    auto* leafBlock = new MTreeNode<DecisionKey>(DecisionKey(AIAction::BLOCK));
    nodeCheckBlock->append_child(leafBlock);
    leafBlock->add_right_sibling(nodeCheckRetreat);

    return nodeCheckBlock;
}

MTreeNode<DecisionKey>* EnemyAI::BuildHardTree() {
    const float atkRange = attackRange;
    const float dashThreshold = alertRange; // 650.0f en HARD

    // 1. Elección de golpe en rango de ataque: ligero o pesado
    auto* nodeAttackChoice = new MTreeNode<DecisionKey>(DecisionKey(
        [atkRange](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= (atkRange * 0.75f);
        }
    ));
    auto* leafAttackLight = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto* leafAttackHeavy = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_HEAVY));
    nodeAttackChoice->append_child(leafAttackLight);
    leafAttackLight->add_right_sibling(leafAttackHeavy);

    // 2. En rango de ataque: 60% golpea, 40% hace amague hacia adelante con DASH_FORWARD
    auto* nodeCloseMix = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2, Vector2, bool, int) {
            return (std::rand() % 100) < 60;
        }
    ));
    auto* leafFeintDashForward = new MTreeNode<DecisionKey>(DecisionKey(AIAction::DASH_FORWARD));
    nodeCloseMix->append_child(nodeAttackChoice);
    nodeAttackChoice->add_right_sibling(leafFeintDashForward);

    // 3. Fuera de rango de ataque: si supera el umbral de dash hace DASH_FORWARD directo; si no, APPROACH directo
    auto* nodeFarDash = new MTreeNode<DecisionKey>(DecisionKey(
        [dashThreshold](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= dashThreshold;
        }
    ));
    auto* leafDashForward = new MTreeNode<DecisionKey>(DecisionKey(AIAction::DASH_FORWARD));
    auto* leafApproach    = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));
    nodeFarDash->append_child(leafDashForward);
    leafDashForward->add_right_sibling(leafApproach);

    // 4. Raíz: ¿Está en rango de ataque?
    auto* nodeCheckAttackRange = new MTreeNode<DecisionKey>(DecisionKey(
        [atkRange](Vector2 pos, Vector2 targetPos, bool, int) {
            return DistanceBetween(pos, targetPos) <= atkRange;
        }
    ));
    nodeCheckAttackRange->append_child(nodeCloseMix);
    nodeCloseMix->add_right_sibling(nodeFarDash);

    return nodeCheckAttackRange;
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

    if (currentDifficulty == Difficulty::HARD) {
        if (consecutiveAttacks >= maxConsecutiveAttacks) {
            consecutiveAttacks = 0;
            pendingPostAttackRetreat = false;
            currentAction = AIAction::IDLE;
            decisionTimer = 0.0f;
            DashBackward(dirX, dirY, screenWidth, screenHeight);
            return;
        }

        pendingPostAttackRetreat = false;
        currentAction = AIAction::IDLE;
        decisionTimer = 0.0f;
        return;
    }

    if (consecutiveAttacks >= maxConsecutiveAttacks) {
        consecutiveAttacks = 0;
        pendingPostAttackRetreat = false;
        currentAction = AIAction::IDLE;
        decisionTimer = decisionInterval;
        DashBackward(-dirX, -dirY, screenWidth, screenHeight);
        return;
    }

    if (pendingPostAttackRetreat) {
        consecutiveAttacks = 0;
        pendingPostAttackRetreat = false;
        currentAction = AIAction::IDLE;
        decisionTimer = decisionInterval;
        DashBackward(-dirX, -dirY, screenWidth, screenHeight);
        return;
    }

    int secondHitChance = 0;
    if (currentDifficulty == Difficulty::MEDIUM) secondHitChance = 25;

    if (dist <= attackRange * 1.15f && (std::rand() % 100) < secondHitChance) {
        pendingPostAttackRetreat = true;
        currentAction = AIAction::IDLE;
        decisionTimer = 0.05f;
        FaceTarget(lastTargetPos);
        AttackPunch(screenWidth, screenHeight);
        return;
    }

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

        case AIAction::APPROACH: {
            const float deadZone = 5.0f;
            float vx = (std::fabs(dx) > deadZone) ? (dirX * speed.x) : 0.0f;
            float vy = (std::fabs(dy) > deadZone) ? (dirY * speed.y) : 0.0f;

            if (std::fabs(vx) <= 0.01f && std::fabs(vy) <= 0.01f) {
                StandIdle(screenWidth, screenHeight);
                break;
            }

            if (std::fabs(vx) > 0.01f) {
                // En diagonal o movimiento horizontal usa la caminata horizontal según X
                flipX = (vx < 0.0f);
                RunMove(moveSet.Walk, vx, vy);
                ClampToScreen(screenWidth, screenHeight);
            } else if (vy < 0.0f) {
                MoveUp(screenWidth, screenHeight);
            } else {
                MoveDown(screenWidth, screenHeight);
            }
            break;
        }

        case AIAction::DASH_FORWARD:
            DashBackward(dirX, dirY, screenWidth, screenHeight);
            currentAction = AIAction::IDLE;
            break;

        case AIAction::RETREAT:
            pendingPostAttackRetreat = false;
            consecutiveAttacks = 0;

            DashBackward(-dirX, -dirY, screenWidth, screenHeight);
            currentAction = AIAction::IDLE;
            break;

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