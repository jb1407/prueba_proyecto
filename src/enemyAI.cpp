#include "enemyAI.hpp"
#include <cmath>
#include <memory>

namespace {
float DistanceBetween(Vector2 a, Vector2 b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}
} // namespace

EnemyAI::EnemyAI(float startX, float startY, AssetManager& assets) {
    size = {115.0f, 200.0f};
    speed = {5.0f, 5.0f};
    dashSpeed = {8.0f, 8.0f};
    activeBackVel = {0.0f, 0.0f};

    decisionTimer = 0.0f;
    decisionInterval = 0.25f;

    CPUPosition = {startX, startY};
    position = CPUPosition;

    moveSet = moveSetData.CharacterMoveSetDefault(assets, &CPUPosition);
    cpuCombat.SetCurrentMove(moveSet.Standing);

    decisionTreeRoot = BuildDecisionTree();
}

EnemyAI::~EnemyAI() {
    MTreeNode<DecisionKey>::destroy_tree(decisionTreeRoot);
}

MTreeNode<DecisionKey>* EnemyAI::BuildDecisionTree() {
    const float atkRange = attackRange;
    const float alertDist = alertRange;

    auto patternStep = std::make_shared<int>(0);

    // 1. Raíz: ¿El objetivo está atacando y está muy cerca?
    auto* root = new MTreeNode<DecisionKey>(
        DecisionKey([atkRange](Vector2 pos, Vector2 targetPos, bool isTargetAttacking) {
            return isTargetAttacking && DistanceBetween(pos, targetPos) <= atkRange * 1.15f;
        })
    );

    // Rama TRUE: en 2 de cada 3 ocasiones retrocede deslizándose; en 1 de cada 3 bloquea/aguanta
    auto* evadeOrBlockCond = new MTreeNode<DecisionKey>(
        DecisionKey([patternStep](Vector2, Vector2, bool) {
            int step = ++(*patternStep);
            return (step % 3) != 0;
        })
    );
    evadeOrBlockCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT)));
    evadeOrBlockCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::BLOCK)));

    // Rama FALSE: ¿Está dentro del rango de ataque?
    auto* inAttackRangeNode = new MTreeNode<DecisionKey>(
        DecisionKey([atkRange](Vector2 pos, Vector2 targetPos, bool) {
            return DistanceBetween(pos, targetPos) <= atkRange;
        })
    );

    // Dentro de rango de ataque: 1 de cada 6 veces hace un deslizamiento corto para confundir, el resto ataca
    auto* baitOrAttackCond = new MTreeNode<DecisionKey>(
        DecisionKey([patternStep](Vector2, Vector2, bool) {
            int step = ++(*patternStep);
            return (step % 6) == 0;
        })
    );
    baitOrAttackCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT)));

    // Alternar ataque pesado y ligero en rango de ataque
    auto* heavyAttackCond = new MTreeNode<DecisionKey>(
        DecisionKey([patternStep](Vector2, Vector2, bool) {
            int step = ++(*patternStep);
            return (step % 3) == 0;
        })
    );
    heavyAttackCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_HEAVY)));
    heavyAttackCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT)));

    baitOrAttackCond->append_child(heavyAttackCond);

    // Fuera de rango de ataque: ¿Está dentro del rango de alerta?
    auto* inAlertRangeNode = new MTreeNode<DecisionKey>(
        DecisionKey([alertDist](Vector2 pos, Vector2 targetPos, bool) {
            return DistanceBetween(pos, targetPos) <= alertDist;
        })
    );

    // En rango medio: 1 de cada 8 veces hace un deslizamiento táctico, el resto se acerca (APPROACH)
    auto* midRangeRepositionCond = new MTreeNode<DecisionKey>(
        DecisionKey([patternStep](Vector2, Vector2, bool) {
            int step = ++(*patternStep);
            return (step % 8) == 0;
        })
    );
    midRangeRepositionCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT)));
    midRangeRepositionCond->append_child(new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH)));

    // Lejos (> alertDist): avanzar con DASH_FORWARD
    auto* dashForwardNode = new MTreeNode<DecisionKey>(DecisionKey(AIAction::DASH_FORWARD));

    inAlertRangeNode->append_child(midRangeRepositionCond);
    inAlertRangeNode->append_child(dashForwardNode);

    inAttackRangeNode->append_child(baitOrAttackCond);
    inAttackRangeNode->append_child(inAlertRangeNode);

    root->append_child(evadeOrBlockCond);
    root->append_child(inAttackRangeNode);

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

        bool condResult = key.condition ? key.condition(currentPos, targetPos, isTargetAttacking) : false;

        if (condResult) {
            current = trueBranch;
        } else {
            current = (falseBranch != nullptr) ? falseBranch : trueBranch;
        }
    }

    return AIAction::IDLE;
}

void EnemyAI::Update(Vector2 targetPos, bool isTargetAttacking, int screenWidth, int screenHeight) {
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

    if (cpuCombat.GetAnimState().CurrentFrame1 == anim1->totalFrames - 1) {
        cpuCombat.ExecuteMove(depthSwapStart, depthSwapEnd, flipX, rotation, true);
        if (isAttacking) {
            isAttacking = false;
            decisionTimer = 0.0f;
        }
        if (isBacking) {
            isBacking = false;
            activeBackVel = {0.0f, 0.0f};
            decisionTimer = 0.0f;
        }
    }

    CPUPosition.x += dx;
    CPUPosition.y += dy;
    position = CPUPosition;

    cpuCombat.ExecuteMove(depthSwapStart, depthSwapEnd, flipX, rotation, false);
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
            if (std::fabs(dy) > std::fabs(dx)) {
                if (dy < 0.0f) MoveUp(screenWidth, screenHeight);
                else MoveDown(screenWidth, screenHeight);
            } else {
                if (dx < 0.0f) MoveLeft(screenWidth, screenHeight);
                else if (dx > 0.0f) MoveRight(screenWidth, screenHeight);
                else StandIdle(screenWidth, screenHeight);
            }
            currentAction = AIAction::IDLE;
            break;

        case AIAction::DASH_FORWARD:
            DashBackward(dirX, dirY, screenWidth, screenHeight);
            currentAction = AIAction::IDLE;
            break;

        case AIAction::RETREAT:
            DashBackward(-dirX, -dirY, screenWidth, screenHeight);
            currentAction = AIAction::IDLE;
            break;

        case AIAction::ATTACK_LIGHT:
        case AIAction::ATTACK_HEAVY:
            FaceTarget(targetPos);
            AttackPunch(screenWidth, screenHeight);
            currentAction = AIAction::IDLE;
            break;

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