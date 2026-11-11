#include "enemyAI.hpp"
#include <cmath>
#include <raymath.h>

EnemyAI::EnemyAI(float startX, float startY, Difficulty diff) : hurtbox(startX, startY, 40.0f, 40.0f, true),
    attackHitbox(startX, startY, 30.0f, 20.0f, false) {
    size = { 40.0f, 40.0f };
    currentDifficulty = diff;
    health = 100;
    maxHealth = 100;
    position = { startX, startY };
    initialPosition = { startX, startY };

    switch (diff) {
        case Difficulty::EASY:
            speed = { 1.8f, 1.8f };
            reactionDelay = 0.35f;
            attackRange = 50.0f;     // Rango de ataque cuerpo a cuerpo para fácil
            alertRange = 250.0f;     // Rango de visión de la IA
            damageMultiplier = 1.25f;
            break;

        case Difficulty::MEDIUM:
            speed = { 2.5f, 2.5f };
            reactionDelay = 0.25f;
            attackRange = 60.0f;
            alertRange = 200.0f;
            damageMultiplier = 1.00f;

            break;

        case Difficulty::HARD:
            speed = { 3.5f, 3.5f };
            reactionDelay = 0.15f;
            attackRange = 70.0f;
            alertRange = 150.0f;     // Un bot más peligroso te detecta desde más lejos
            damageMultiplier = 0.75f;
            break;
    }

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
    auto leafRetreat1 = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto leafRetreat2 = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto leafAttack   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto leafApproach = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    // 1. Nodo de probabilidad (Solo se ejecuta si está cerca): 60% Ataca, 40% Retrocede
    auto nodeProbability = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            int randomChoice = rand() % 10;
            return (randomChoice < 6); // 60% true (Ataca), 40% false (Retrocede)
        }
    ));
    nodeProbability->append_child(leafAttack);
    leafAttack->add_right_sibling(leafRetreat1); // Si el azar da false -> Retrocede (C_Backward)

    // 2. Nodo de Distancia: ¿Está el jugador a rango cuerpo a cuerpo (<= 60px)?
    auto nodeCheckDistance = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= 60.0f;
        }
    ));
    nodeCheckDistance->append_child(nodeProbability);     // Si SÍ está cerca (TRUE) -> Evalúa la probabilidad de atacar/retroceder
    nodeProbability->add_right_sibling(leafApproach);     // Si NO está cerca (FALSE) -> ¡Se acerca (APPROACH / C_Walk)!

    // 3. Nodo de retroceso por presión del jugador (si el jugador ataca 3 veces seguidas)
    auto nodeCheckRetreat = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return atkCount >= 3;
        }
    ));
    nodeCheckRetreat->append_child(leafRetreat2);
    leafRetreat2->add_right_sibling(nodeCheckDistance);   // Si no hay presión -> Pasa a evaluar la distancia y el combate

    // 4. Nodo de bloqueo defensivo
    auto nodeCheckBlock = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return attacking && Vector2Distance(pos, pPos) <= 80.0f;
        }
    ));
    nodeCheckBlock->append_child(leafBlock);
    leafBlock->add_right_sibling(nodeCheckRetreat);

    // 5. Raíz: Rango de alerta general (250px)
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
    // 1. Acumulamos el tiempo del frame para respetar el retraso de reacción de la dificultad
    reactionTimer += GetFrameTime();

    // 2. Solo reevaluamos el árbol de decisiones cuando se cumpla el tiempo (evita el parpadeo / flickering)
    if (reactionTimer >= reactionDelay) {
        EvaluateTree(playerPosition, isPlayerAttacking);
        reactionTimer = 0.0f; // Reiniciamos el temporizador de decisión
    }

    // 3. Ejecutar el movimiento o comportamiento físico correspondiente a la acción elegida
    switch (currentAction) {
        case AIAction::APPROACH: {
            Vector2 dir = { playerPosition.x - position.x, playerPosition.y - position.y };
            float len = std::hypot(dir.x, dir.y);
            if (len > 0) {
                moveDir = { dir.x / len, dir.y / len }; // Guardamos la dirección normalizada
                position.x += moveDir.x * speed.x;
                position.y += moveDir.y * speed.y;
            } else {
                moveDir = { 0.0f, 0.0f };
            }
            break;
        }

        case AIAction::RETREAT: {
            // Retroceder alejándose del jugador tácticamente
            Vector2 dir = { position.x - playerPosition.x, position.y - playerPosition.y };
            float len = std::hypot(dir.x, dir.y);
            if (len > 0) {
                moveDir = { dir.x / len, dir.y / len };
                position.x += moveDir.x * (speed.x * 0.8f);
                position.y += moveDir.y * (speed.x * 0.8f);
            } else {
                moveDir = { 0.0f, 0.0f };
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
                moveDir = { dir.x / dist, dir.y / dist };
                position.x += moveDir.x * patrolSpeed;
                position.y += moveDir.y * patrolSpeed;
            } else {
                moveDir = { 0.0f, 0.0f };
                patrolCornerIndex = (patrolCornerIndex + 1) % 4;
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

    hurtbox.UpdatePosition(position.x, position.y, 0.0f, 0.0f);

    if (currentAction == AIAction::ATTACK_LIGHT || currentAction == AIAction::ATTACK_HEAVY) {
        attackHitbox.SetActive(true);
        // Desplazar la hitbox de ataque hacia el lado donde esté mirando el enemigo
        float offsetX = (playerPosition.x < position.x) ? -30.0f : size.x;
        attackHitbox.UpdatePosition(position.x, position.y, offsetX, 10.0f);
    } else {
        attackHitbox.SetActive(false);
    }
}

void EnemyAI::TakeDamage(float amount) {
    // Calculamos el daño real aplicando el multiplicador de la dificultad
    float finalDamage = amount * damageMultiplier;
    
    health -= finalDamage;
    
    if (health < 0.0f) {
        health = 0.0f; // Evitamos números negativos
    }
}

void EnemyAI::Draw(AssetManager& manager, Vector2 playerPosition) {
    std::string animKey = "C_Standing"; // Animación por defecto

    // Mapear la acción del árbol de decisión eligiendo entre caminar vertical u horizontal
    switch (currentAction) {
        case AIAction::APPROACH:
        case AIAction::PATROL:
            // Si el desplazamiento vertical es mayor que el horizontal, usa arriba o abajo
            if (std::abs(moveDir.y) > std::abs(moveDir.x)) {
                if (moveDir.y < 0) {
                    animKey = "C_WalkUP";   // Subiendo (respeta tu clave de asset)
                } else {
                    animKey = "C_WalkDown"; // Bajando
                }
            } else {
                animKey = "C_Walk";         // Movimiento lateral (izquierda / derecha)
            }
            break;

        case AIAction::RETREAT:
            animKey = "C_Backward";
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

    // Buscar la animación en el AssetManager
    Animation* anim = manager.Search(animKey);

    // Determinar si debe voltearse (flipX):
    // - En PATROL: mira hacia la dirección de movimiento (moveDir.x)
    // - En combate / otras acciones: fija la vista en el jugador
    bool flipX = false;
    if (currentAction == AIAction::PATROL) {
        if (moveDir.x < 0) {
            flipX = true;  // Mirando a la izquierda
        } else if (moveDir.x > 0) {
            flipX = false; // Mirando a la derecha
        }
    } else {
        flipX = (playerPosition.x < position.x);
    }

    if (anim != nullptr) {
        // Dibujar usando la misma función de animación fluida del jugador
        UpdateAndDrawAnimation(*anim, position, flipX, 0.0f);

        // Reiniciar el loop de la animación si llega al final (útil para ataques)
        if (anim->currentFrame >= anim->totalFrames - 1) {
            anim->currentFrame = 0;
            anim->tickCounter = 0;
        }
    } else {
        // Fallback de seguridad por si la textura llegara a faltar
        DrawRectangleV(position, size, RED);
    }
}