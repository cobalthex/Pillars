#pragma once

#include "game.h"

void EnemyThink(Actor* enemy);
void ChangeDir(Actor* enemy);
bool CanSee(Actor* enemy, Direction dir);
bool EnemyInteract(Actor* enemy, Actor* obj);