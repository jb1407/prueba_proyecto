#include "enemyAI.hpp"
#include <cmath>
#include <raymath.h>

EnemyAI::EnemyAI(float startX, float startY, Difficulty diff) {
    position = { startX, startY };
    initialPosition = position;
    size = { 50.0f, 50.0f };
    currentDifficulty = diff;

    // Ajustar velocidad base según la dificultad
    if (diff == Difficulty::EASY) {
        speed = { 1.8f, 1.8f };
    } else if (diff == Difficulty::MEDIUM) {
        speed = { 2.5f, 2.5f };
    } else if (diff == Difficulty::HARD) {
        speed = { 3.5f, 3.5f }; // Bot mucho más rápido y letal
    }

    health = 100;
    maxHealth = 100;
    currentAction = AIAction::IDLE;
    
    patrolCenter = initialPosition;
    patrolCornerIndex = 0;
    rectWidth = 60.0f;   
    rectHeight = 40.0f;  
    patrolDirection = { 1.0f, 0.5f };
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
    // Si la librería DeSiGNAR provee destroy_tree, puedes usarla:
    MTreeNode<DecisionKey>::destroy_tree(decisionTreeRoot);
}

MTreeNode<DecisionKey>* EnemyAI::BuildEasyTree() {
    auto leafPatrol   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::PATROL));
    auto leafAttack   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto leafApproach = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    // Nodo interno: ¿En rango de ataque? (Sí -> Atacar, No -> Acercarse)
    auto nodeCheckAttack = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= 60.0f;
        }
    ));
    nodeCheckAttack->append_child(leafAttack);
    leafAttack->add_right_sibling(leafApproach);

    // Raíz Fácil: ¿Jugador dentro de 150px? (Sí -> Evaluar ataque, No -> Patrullar)
    auto rootNode = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= 150.0f;
        }
    ));
    rootNode->append_child(nodeCheckAttack);
    nodeCheckAttack->add_right_sibling(leafPatrol);

    return rootNode;
}

MTreeNode<DecisionKey>* EnemyAI::BuildMediumTree() {
    auto leafPatrol   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::PATROL));
    auto leafBlock    = new MTreeNode<DecisionKey>(DecisionKey(AIAction::BLOCK));
    auto leafRetreat  = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto leafAttack   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto leafApproach = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    auto nodeCheckAttack = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= 60.0f;
        }
    ));
    nodeCheckAttack->append_child(leafAttack);
    leafAttack->add_right_sibling(leafApproach);

    auto nodeCheckRetreat = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return atkCount >= 3;
        }
    ));
    nodeCheckRetreat->append_child(leafRetreat);
    leafRetreat->add_right_sibling(nodeCheckAttack);

    auto nodeCheckBlock = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return attacking && Vector2Distance(pos, pPos) <= 80.0f;
        }
    ));
    nodeCheckBlock->append_child(leafBlock);
    leafBlock->add_right_sibling(nodeCheckRetreat);

    auto rootNode = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= 250.0f;
        }
    ));
    rootNode->append_child(nodeCheckBlock);
    nodeCheckBlock->add_right_sibling(leafPatrol);

    return rootNode;
}

MTreeNode<DecisionKey>* EnemyAI::BuildHardTree() {
    auto leafPatrol      = new MTreeNode<DecisionKey>(DecisionKey(AIAction::PATROL));
    auto leafBlock       = new MTreeNode<DecisionKey>(DecisionKey(AIAction::BLOCK));
    auto leafRetreat     = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto leafAttackLight = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto leafAttackHeavy = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_HEAVY)); // Nuevo estado duro
    auto leafApproach    = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    // Nodo: ¿Distancia ideal para ataque pesado?
    auto nodeCheckHeavy = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            float dist = Vector2Distance(pos, pPos);
            return dist > 60.0f && dist <= 120.0f; // Rango medio para golpe fuerte
        }
    ));
    nodeCheckHeavy->append_child(leafAttackHeavy);
    leafAttackHeavy->add_right_sibling(leafApproach);

    // Nodo: ¿En rango de golpe ligero?
    auto nodeCheckLight = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= 60.0f;
        }
    ));
    nodeCheckLight->append_child(leafAttackLight);
    leafAttackLight->add_right_sibling(nodeCheckHeavy);

    // Nodo de presión / retroceso estricto
    auto nodeCheckRetreat = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return atkCount >= 2; // Reacciona más rápido (a los 2 ataques)
        }
    ));
    nodeCheckRetreat->append_child(leafRetreat);
    leafRetreat->add_right_sibling(nodeCheckLight);

    // Nodo de bloqueo avanzado
    auto nodeCheckBlock = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return attacking; // Bloquea casi cualquier ataque inminente sin importar tanto la distancia exacta
        }
    ));
    nodeCheckBlock->append_child(leafBlock);
    leafBlock->add_right_sibling(nodeCheckRetreat);

    // Raíz Difícil: Rango de alerta masivo (400px)
    auto rootNode = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= 400.0f; // ¡Te detecta desde lejos!
        }
    ));
    rootNode->append_child(nodeCheckBlock);
    nodeCheckBlock->add_right_sibling(leafPatrol);

    return rootNode;
}

// --- EVALUACIÓN DEL ÁRBOL RECORRIendo MTreeNode ---
void EnemyAI::EvaluateTree(Vector2 playerPosition, bool isPlayerAttacking) {
    if (isPlayerAttacking) {
        playerAttackCounter++;
    } else {
        if (playerAttackCounter > 0) playerAttackCounter--;
    }

    MTreeNode<DecisionKey>* currentNode = decisionTreeRoot;

    // Navegar por el árbol mientras el nodo tenga hijos (no sea una hoja)
    while (currentNode && !currentNode->is_leaf()) {
        auto& key = currentNode->get_key();
        
        if (key.type == NodeType::CONDITION) {
            bool evalResult = key.condition(position, playerPosition, isPlayerAttacking, playerAttackCounter);
            
            // first_child es la rama TRUE (Sí)
            MTreeNode<DecisionKey>* trueBranch = currentNode->get_first_child();
            if (trueBranch) {
                // El hermano derecho del first_child es la rama FALSE (No)
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

    // Al llegar a la hoja, extraemos la acción resultante
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
    EvaluateTree(playerPosition, isPlayerAttacking);

    switch (currentAction) {
        case AIAction::APPROACH: {
            Vector2 dir = { playerPosition.x - position.x, playerPosition.y - position.y };
            float len = std::hypot(dir.x, dir.y);
            if (len > 0) {
                position.x += (dir.x / len) * speed.x;
                position.y += (dir.y / len) * speed.y;
            }
            break;
        }

        case AIAction::RETREAT: {
            // Retroceder alejándose del jugador sin perderlo de vista
            Vector2 dir = { position.x - playerPosition.x, position.y - playerPosition.y };
            float len = std::hypot(dir.x, dir.y);
            if (len > 0) {
                position.x += (dir.x / len) * (speed.x * 0.8f);
                position.y += (dir.y / len) * (speed.y * 0.8f);
            }
            break;
        }

        case AIAction::PATROL: {
            Vector2 targetCorner;
            switch (patrolCornerIndex) {
                case 0: targetCorner = { patrolCenter.x - rectWidth/2, patrolCenter.y - rectHeight/2 }; break;
                case 1: targetCorner = { patrolCenter.x + rectWidth/2, patrolCenter.y - rectHeight/2 }; break;
                case 2: targetCorner = { patrolCenter.x + rectWidth/2, patrolCenter.y + rectHeight/2 }; break;
                case 3: targetCorner = { patrolCenter.x - rectWidth/2, patrolCenter.y + rectHeight/2 }; break;
            }

            Vector2 dir = { targetCorner.x - position.x, targetCorner.y - position.y };
            float dist = std::hypot(dir.x, dir.y);
            float patrolSpeed = speed.x * 0.4f;

            if (dist > 2.0f) {
                position.x += (dir.x / dist) * patrolSpeed;
                position.y += (dir.y / dist) * patrolSpeed;
            } else {
                patrolCornerIndex = (patrolCornerIndex + 1) % 4;
            }
            break;
        }

        default:
            break;
    }

    ClampToScreen(screenWidth, screenHeight);
}

void EnemyAI::Draw() {
    Color botColor = RED;
    const char* actionText = "PATRULLANDO";

    if (currentAction == AIAction::APPROACH) { actionText = "PERSIGUIENDO"; botColor = ORANGE; }
    else if (currentAction == AIAction::RETREAT) { actionText = "RETROCEDIENDO"; botColor = PURPLE; }
    else if (currentAction == AIAction::ATTACK_LIGHT) { actionText = "ATACANDO"; botColor = RED; }
    else if (currentAction == AIAction::BLOCK) { actionText = "BLOQUEANDO"; botColor = BLUE; }

    DrawRectangleV(position, size, botColor);
}