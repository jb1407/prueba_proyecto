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
            attackRange = 50.0f;     // Rango de ataque cuerpo a cuerpo para fácil
            alertRange = 300.0f;     // Rango de visión de la IA
            damageMultiplier = 1.25f;
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
            alertRange = 600.0f;     // Un bot más peligroso te detecta desde más lejos
            damageMultiplier = 0.75f;
            break;
    }

    currentAction = AIAction::IDLE;
    
    patrolCenter = initialPosition;
    patrolCornerIndex = 0;
    rectWidth = 150.0f;   
    rectHeight = 90.0f;  
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
    auto leafAttack   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto leafApproach = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    // Nodo: ¿Está a rango de ataque?
    auto nodeCheckAttack = new MTreeNode<DecisionKey>(DecisionKey(
        [this](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= this->attackRange;
        }
    ));
    
    nodeCheckAttack->append_child(leafAttack);
    leafAttack->add_right_sibling(leafApproach); // Si está lejos -> ¡Se acerca directamente!

    return nodeCheckAttack; // Ahora la raíz es directamente la condición de ataque/aproximación
}

MTreeNode<DecisionKey>* EnemyAI::BuildMediumTree() {
    auto leafBlock    = new MTreeNode<DecisionKey>(DecisionKey(AIAction::BLOCK));
    auto leafRetreat1 = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto leafRetreat2 = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto leafAttack   = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto leafApproach = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    // 1. Probabilidad a corta distancia: 60% Ataca, 40% Retrocede (amague táctico / backward)
    auto nodeProbability = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            int randomChoice = rand() % 10;
            return (randomChoice < 6); 
        }
    ));
    nodeProbability->append_child(leafAttack);
    leafAttack->add_right_sibling(leafRetreat1); 

    // 2. Nodo de Distancia: ¿Está a rango cuerpo a cuerpo?
    auto nodeCheckDistance = new MTreeNode<DecisionKey>(DecisionKey(
        [this](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= this->attackRange;
        }
    ));
    nodeCheckDistance->append_child(nodeProbability);     
    nodeProbability->add_right_sibling(leafApproach);     // Si está lejos -> ¡Lo persigue sin dudar!

    // 3. Retroceso por presión del jugador
    auto nodeCheckRetreat = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return atkCount >= 3;
        }
    ));
    nodeCheckRetreat->append_child(leafRetreat2);
    leafRetreat2->add_right_sibling(nodeCheckDistance);   

    // 4. Bloqueo defensivo
    auto nodeCheckBlock = new MTreeNode<DecisionKey>(DecisionKey(
        [this](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return attacking && Vector2Distance(pos, pPos) <= (this->attackRange * 1.3f);
        }
    ));
    nodeCheckBlock->append_child(leafBlock);
    leafBlock->add_right_sibling(nodeCheckRetreat);

    return nodeCheckBlock; // Raíz directa de combate y persecución
}

MTreeNode<DecisionKey>* EnemyAI::BuildHardTree() {
    auto leafBlock       = new MTreeNode<DecisionKey>(DecisionKey(AIAction::BLOCK));
    auto leafRetreat     = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    auto leafAttackLight = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_LIGHT));
    auto leafAttackHeavy = new MTreeNode<DecisionKey>(DecisionKey(AIAction::ATTACK_HEAVY)); 
    auto leafApproach    = new MTreeNode<DecisionKey>(DecisionKey(AIAction::APPROACH));

    // 1. Elección de ataque (Ligero si está en rango normal, Pesado si está un poco más al límite)
    auto nodeAttackChoice = new MTreeNode<DecisionKey>(DecisionKey(
        [this](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= this->attackRange;
        }
    ));
    nodeAttackChoice->append_child(leafAttackLight);
    leafAttackLight->add_right_sibling(leafAttackHeavy); // Hermano FALSE de la condición de ataque

    // 2. Nodo de Probabilidad Táctica a distancia cercana (<= 150px):
    // 35% Retrocede (Backward / Amague), 65% Pasa a evaluar los ataques
    auto nodeTacticalMix = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            int roll = rand() % 100;
            return (roll < 50); // 35% true -> Retrocede
        }
    ));
    nodeTacticalMix->append_child(leafRetreat);
    leafRetreat->add_right_sibling(nodeAttackChoice); // Hermano FALSE -> Va a la elección de ataque

    // 3. Nodo de Rango Táctico Cercano (<= 150px)
    auto nodeCloseRange = new MTreeNode<DecisionKey>(DecisionKey(
        [this](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return Vector2Distance(pos, pPos) <= 150.0f;
        }
    ));
    nodeCloseRange->append_child(nodeTacticalMix);
    nodeTacticalMix->add_right_sibling(leafApproach); // Hermano FALSE (> 150px) -> APPROACH directo

    // 4. Nodo de Presión por ataques del jugador (atkCount >= 2)
    auto nodeCheckRetreat = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return atkCount >= 2; 
        }
    ));
    auto leafRetreatGlobal = new MTreeNode<DecisionKey>(DecisionKey(AIAction::RETREAT));
    nodeCheckRetreat->append_child(leafRetreatGlobal);
    leafRetreatGlobal->add_right_sibling(nodeCloseRange);

    // 5. Nodo de Bloqueo Defensivo avanzado (Prioridad si el jugador ataca)
    auto nodeCheckBlock = new MTreeNode<DecisionKey>(DecisionKey(
        [](Vector2 pos, Vector2 pPos, bool attacking, int atkCount) {
            return attacking; 
        }
    ));
    nodeCheckBlock->append_child(leafBlock);
    leafBlock->add_right_sibling(nodeCheckRetreat);

    return nodeCheckBlock;
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
    float dt = GetFrameTime();

    // 1. Acumulamos el tiempo del frame y descontamos el bloqueo de acción táctica si está activo
    reactionTimer += dt;
    if (actionDurationTimer > 0.0f) {
        actionDurationTimer -= dt;
    }

    // 2. Solo reevaluamos el árbol de decisiones cuando se cumpla el delay Y haya terminado el compromiso de la acción actual
    if (reactionTimer >= reactionDelay && actionDurationTimer <= 0.0f) {
        EvaluateTree(playerPosition, isPlayerAttacking);
        reactionTimer = 0.0f; // Reiniciamos el temporizador de decisión

        // Si el árbol decide hacer un retroceso/amague (RETREAT), lo sostenemos durante 0.45 segundos
        // para que el desplazamiento tenga buen alcance y se note visualmente
        if (currentAction == AIAction::RETREAT) {
            actionDurationTimer = 0.45f; 
        }
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
            // Retroceder con impulso sostenido (Backward / Amague táctico)
            Vector2 dir = { position.x - playerPosition.x, position.y - playerPosition.y };
            float len = std::hypot(dir.x, dir.y);
            if (len > 0) {
                moveDir = { dir.x / len, dir.y / len };
                position.x += moveDir.x * (speed.x * 1.4f); // Impulso dinámico para el amague
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

    hurtbox.UpdatePosition(position.x, position.y, 0.0f, 0.0f);

    if (currentAction == AIAction::ATTACK_LIGHT || currentAction == AIAction::ATTACK_HEAVY) {
        attackHitbox.SetActive(true);
        
        // El ancho de la hitbox de ataque del enemigo
        float attackWidth = 60.0f; 
        
        // Determinamos la orientación según dónde esté el jugador respecto al enemigo:
        float offsetX = (playerPosition.x < position.x) ? (-attackWidth - 10.0f) : (size.x + 55.0f);
        
        // Ajustamos la altura al porcentaje correspondiente (torso)
        float offsetY = size.y * 0.6f; 
        
        attackHitbox.UpdatePosition(position.x, position.y, offsetX, offsetY);
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
            // Si el desplazamiento vertical es mayor que el horizontal, usa arriba o abajo
            if (std::abs(moveDir.y) > std::abs(moveDir.x)) {
                if (moveDir.y < 0) {
                    animKey = "C_WalkUP";   // Subiendo
                } else {
                    animKey = "C_WalkDown"; // Bajando
                }
            } else {
                animKey = "C_Walk";         // Movimiento lateral (izquierda / derecha)
            }
            break;

        case AIAction::RETREAT:
            animKey = "C_Backward";         // Animación de amague / retroceso táctico
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
    // Como ya no hay patrullaje, el enemigo siempre mira al jugador en combate,
    // o se orienta según su dirección si está ejecutando un retroceso/amague.
    bool flipX = false;
    if (currentAction == AIAction::RETREAT) {
        // Si retrocede, puede mantener la mirada opuesta al jugador o según su vector de movimiento
        flipX = (moveDir.x < 0);
    } else {
        flipX = (playerPosition.x < position.x);
    }

    if (anim != nullptr) {
        // Dibujar usando la función de animación fluida
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