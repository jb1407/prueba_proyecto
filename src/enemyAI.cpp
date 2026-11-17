#include "enemyAI.hpp"
#include <cmath>
#include <raymath.h>

EnemyAI::EnemyAI(float startX, float startY, Difficulty diff) : hurtbox(startX, startY, 115.0f, 200.0f, true),
    attackHitbox(startX, startY, 45.0f, 30.0f, false) {
    size = { 115.0f, 200.0f };
    currentDifficulty = diff;
    health = 100.0f;
    maxHealth = 100.0f;
    position = { startX, startY };
    initialPosition = { startX, startY };

    switch (diff) {
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
            attackRange = 70.0f;
            alertRange = 600.0f;
            damageMultiplier = 1.25f;
            break;
    }

    currentAction = AIAction::IDLE;
    playerAttackCounter = 0;

    // Construir el árbol correspondiente
    decisionTreeRoot = BuildDecisionTree(diff);
}

MTreeNode<DecisionKey>* EnemyAI::BuildDecisionTree(Difficulty diff) {
    switch (diff) {
        case Difficulty::EASY:   return BuildEasyTree();
        case Difficulty::MEDIUM: return BuildMediumTree();
        case Difficulty::HARD:   return BuildHardTree();
    }
    return BuildMediumTree();
}

EnemyAI::~EnemyAI() {
    MTreeNode<DecisionKey>::destroy_tree(decisionTreeRoot);
}

MTreeNode<DecisionKey>* EnemyAI::BuildEasyTree() {
    auto leafAttack   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto leafApproach = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    // Nodo: ¿Está a rango de ataque?
    auto nodeCheckAttack = new MTreeNode<DecisionKey>(DecisionKey(
        [this](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= this->attackRange;
        }
    ));
    
    nodeCheckAttack->append_child(leafAttack);
    leafAttack->add_right_sibling(leafApproach); // Si está lejos -> Se acerca caminando

    return nodeCheckAttack;
}

MTreeNode<DecisionKey>* EnemyAI::BuildMediumTree() {
    auto leafBlock       = new MTreeNode<DecisionKey>(DecisionKey(AIAction::BLOCK));
    auto leafRetreat1    = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto leafRetreat2    = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto leafAttack      = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto leafDashForward = new MTreeNode<DecisionKey>(DecisionKey(AIAction::DASH_FORWARD));
    auto leafApproach    = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    // 1. Probabilidad a corta distancia: 60% Ataca, 40% Retrocede (amague táctico)
    auto nodeCloseProb = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return (rand() % 100) < 60;
        }
    ));
    nodeCloseProb->append_child(leafAttack);
    leafAttack->add_right_sibling(leafRetreat1);

    // 2. Probabilidad a larga distancia: 30% Dash explosivo hacia adelante, 70% Camina (APPROACH)
    auto nodeFarProb = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return (rand() % 100) < 30;
        }
    ));
    nodeFarProb->append_child(leafDashForward);
    leafDashForward->add_right_sibling(leafApproach);

    // 3. Nodo de Distancia: ¿Está a rango cuerpo a cuerpo?
    auto nodeCheckDistance = new MTreeNode<DecisionKey>(DecisionKey(
        [this](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= this->attackRange;
        }
    ));
    nodeCheckDistance->append_child(nodeCloseProb);     
    nodeCloseProb->add_right_sibling(nodeFarProb); // Si está lejos -> Evalúa Dash o Approach

    // 4. Retroceso por presión del jugador
    auto nodeCheckRetreat = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return atkCount >= 3;
        }
    ));
    nodeCheckRetreat->append_child(leafRetreat2);
    leafRetreat2->add_right_sibling(nodeCheckDistance);   

    // 5. Bloqueo defensivo
    auto nodeCheckBlock = new MTreeNode<DecisionKey>(DecisionKey(
        [this](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return attacking && Vector2Distance(pos, pPos) <= (this->attackRange * 1.3f);
        }
    ));
    nodeCheckBlock->append_child(leafBlock);
    leafBlock->add_right_sibling(nodeCheckRetreat);

    return nodeCheckBlock;
}

MTreeNode<DecisionKey>* EnemyAI::BuildHardTree() {
    auto leafBlock         = new MTreeNode<DecisionKey>(DecisionKey(AIAction::BLOCK));
    auto leafRetreat       = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto leafRetreatGlobal = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto leafAttackLight   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto leafAttackHeavy   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_HEAVY)); 
    auto leafDashForward   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::DASH_FORWARD));
    auto leafApproach      = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    // 1. Elección de ataque (Ligero en rango corto, Pesado al límite)
    auto nodeAttackChoice = new MTreeNode<DecisionKey>(DecisionKey(
        [this](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= this->attackRange;
        }
    ));
    nodeAttackChoice->append_child(leafAttackLight);
    leafAttackLight->add_right_sibling(leafAttackHeavy);

    // 2. Probabilidad Táctica a corta distancia (<= 150px): 50% Amague (RETREAT), 50% Ataca
    auto nodeTacticalMix = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return (rand() % 100) < 50;
        }
    ));
    nodeTacticalMix->append_child(leafRetreat);
    leafRetreat->add_right_sibling(nodeAttackChoice);

    // 3. Probabilidad de Persecución Agresiva (> 150px): 60% Dash hacia adelante, 40% Camina
    auto nodeFarAggression = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return (rand() % 100) < 60;
        }
    ));
    nodeFarAggression->append_child(leafDashForward);
    leafDashForward->add_right_sibling(leafApproach);

    // 4. Nodo de Rango Táctico Cercano (<= 150px)
    auto nodeCloseRange = new MTreeNode<DecisionKey>(DecisionKey(
        [this](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= 150.0f;
        }
    ));
    nodeCloseRange->append_child(nodeTacticalMix);
    nodeTacticalMix->add_right_sibling(nodeFarAggression); // Si está lejos -> Evalúa Dash agresivo

    // 5. Nodo de Presión por ataques del jugador (atkCount >= 2)
    auto nodeCheckRetreat = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return atkCount >= 2; 
        }
    ));
    nodeCheckRetreat->append_child(leafRetreatGlobal);
    leafRetreatGlobal->add_right_sibling(nodeCloseRange);

    // 6. Nodo de Bloqueo Defensivo avanzado
    auto nodeCheckBlock = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return attacking; 
        }
    ));
    nodeCheckBlock->append_child(leafBlock);
    leafBlock->add_right_sibling(nodeCheckRetreat);

    return nodeCheckBlock;
}

void EnemyAI::EvaluateTree(Vector2 playerPosition, bool isPlayerAttacking) {
    if (isPlayerAttacking) {
        playerAttackCounter++;
    } else {
        if (playerAttackCounter > 0) playerAttackCounter--;
    }

    MTreeNode<DecisionKey>* currentNode = decisionTreeRoot;

    while (currentNode && !currentNode->is_leaf()) {
        auto& key = currentNode->get_key();
        
        if (key.type == NodeType::CONDITION) {
            bool evalResult = key.condition(position, playerPosition, isPlayerAttacking, playerAttackCounter);
            
            MTreeNode<DecisionKey>* trueBranch = currentNode->get_first_child();
            if (trueBranch) {
                MTreeNode<DecisionKey>* falseBranch = trueBranch->get_right_sibling();
                
                if (evalResult) {
                    currentNode = trueBranch;
                } else {
                    currentNode = falseBranch ? falseBranch : trueBranch;
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }

    if (currentNode && currentNode->is_leaf()) {
        currentAction = currentNode->get_key().action;
    }
}

void EnemyAI::ClampToScreen(int screenWidth, int screenHeight) {
    if (position.x < 0) position.x = 0;
    if (position.y < 0) position.y = 0;
    if (position.x + size.x > screenWidth) position.x = screenWidth - size.x;
    if (position.y + size.y > screenHeight) position.y = screenHeight - size.y;
}

void EnemyAI::Update(Vector2 playerPosition, bool isPlayerAttacking, int screenWidth, int screenHeight) {
    float dt = GetFrameTime();

    // 1. Acumulamos el tiempo del frame y descontamos el compromiso táctico si está activo
    reactionTimer += dt;
    if (actionDurationTimer > 0.0f) {
        actionDurationTimer -= dt;
    }

    // 2. Reevaluamos el árbol cuando se cumpla el delay Y haya terminado la acción comprometida
    if (reactionTimer >= reactionDelay && actionDurationTimer <= 0.0f) {
        EvaluateTree(playerPosition, isPlayerAttacking);
        reactionTimer = 0.0f;

        if (currentAction == AIAction::RETREAT) {
            actionDurationTimer = 0.45f; // Sostiene el amague hacia atrás
        } else if (currentAction == AIAction::DASH_FORWARD) {
            actionDurationTimer = 0.35f; // Sostiene el impulso explosivo hacia adelante
        }
    }

    // 3. Ejecutar el movimiento o comportamiento físico correspondiente
    switch (currentAction) {
        case AIAction::APPROACH: {
            Vector2 dir = { playerPosition.x - position.x, playerPosition.y - position.y };
            float len = std::hypot(dir.x, dir.y);
            if (len > 0) {
                moveDir = { dir.x / len, dir.y / len };
                position.x += moveDir.x * speed.x;
                position.y += moveDir.y * speed.y;
            } else {
                moveDir = { 0.0f, 0.0f };
            }
            break;
        }

        case AIAction::DASH_FORWARD: {
            // Deslizamiento rápido hacia el jugador para recortar distancia agresivamente
            Vector2 dir = { playerPosition.x - position.x, playerPosition.y - position.y };
            float len = std::hypot(dir.x, dir.y);
            if (len > 0) {
                moveDir = { dir.x / len, dir.y / len };
                position.x += moveDir.x * (speed.x * 2.2f);
                position.y += moveDir.y * (speed.y * 2.2f);
            } else {
                moveDir = { 0.0f, 0.0f };
            }
            break;
        }

        case AIAction::RETREAT: {
            // Retroceso con impulso sostenido (Backward / Amague táctico)
            Vector2 dir = { position.x - playerPosition.x, position.y - playerPosition.y };
            float len = std::hypot(dir.x, dir.y);
            if (len > 0) {
                moveDir = { dir.x / len, dir.y / len };
                position.x += moveDir.x * (speed.x * 1.4f);
                position.y += moveDir.y * (speed.y * 1.4f);
            } else {
                moveDir = { 0.0f, 0.0f };
            }
            break;
        }

        case AIAction::ATTACK_LIGHT:
        case AIAction::ATTACK_HEAVY:
        case AIAction::BLOCK:
        case AIAction::IDLE:
        default:
            moveDir = { 0.0f, 0.0f };
            break;
    }

    // 4. Mantener al enemigo confinado dentro de los límites de la ventana
    ClampToScreen(screenWidth, screenHeight);

    // --- NUEVO: Ajustar tamaño y posición de la Hurtbox del enemigo según su acción ---
    float hurtboxWidth = size.x;
    float hurtboxHeight = size.y;
    float hurtboxOffsetY = 0.0f;

    if (currentAction == AIAction::RETREAT) {
        hurtboxHeight = size.y * 0.55f; // Reduce la altura al 55% al retroceder
        hurtboxOffsetY = size.y * 0.45f; // Desplaza la caja hacia abajo
    }

    hurtbox.SetSize(hurtboxWidth, hurtboxHeight);
    hurtbox.UpdatePosition(position.x, position.y, 0.0f, hurtboxOffsetY);

    if (currentAction == AIAction::ATTACK_LIGHT || currentAction == AIAction::ATTACK_HEAVY) {
        attackHitbox.SetActive(true);
        
        float attackWidth = 60.0f; 
        float offsetX = (playerPosition.x < position.x) ? (-attackWidth - 10.0f) : (size.x + 55.0f);
        float offsetY = size.y * 0.6f; 
        
        attackHitbox.UpdatePosition(position.x, position.y, offsetX, offsetY);
    } else {
        attackHitbox.SetActive(false);
    }
}

void EnemyAI::TakeDamage(float amount) {
    float finalDamage = amount * damageMultiplier;
    health -= finalDamage;
    if (health < 0.0f) health = 0.0f;
}

void EnemyAI::Draw(AssetManager& manager, Vector2 playerPosition) {
    std::string animKey = "C_Standing";

    switch (currentAction) {
        case AIAction::APPROACH:
            if (std::abs(moveDir.y) > std::abs(moveDir.x)) {
                animKey = (moveDir.y < 0) ? "C_WalkUP" : "C_WalkDown";
            } else {
                animKey = "C_Walk";
            }
            break;

        case AIAction::DASH_FORWARD:
        case AIAction::RETREAT:
            animKey = "C_Backward"; // Usa el sprite dinámico de deslizamiento
            break;

        case AIAction::ATTACK_LIGHT:
        case AIAction::ATTACK_HEAVY:
            animKey = "C_Punch";
            break;

        case AIAction::BLOCK:
        case AIAction::IDLE:
        default:
            animKey = "C_Standing";
            break;
    }

    Animation* anim = manager.Search(animKey);

    bool flipX = false;
    if (currentAction == AIAction::RETREAT) {
        flipX = (moveDir.x < 0);
    } else {
        // En DASH_FORWARD, APPROACH o ataques siempre mira directamente al jugador
        flipX = (playerPosition.x < position.x);
    }

    if (anim != nullptr) {
        UpdateAndDrawAnimation(*anim, position, flipX, 0.0f);

        if (anim->currentFrame >= anim->totalFrames - 1) {
            anim->currentFrame = 0;
            anim->tickCounter = 0;
        }
    } else {
        DrawRectangleV(position, size, RED);
    }
}