#include "Enemy.h"
#include "Sprite.h"
#include "TileMap.h"
#include "Globals.h"
#include <raymath.h>
Ennemy::Enemy(const Point& p, State s, Look view) :
	Entity(p, ENEMY_PHYSICAL_WIDTH, ENEMY_PHYSICAL_HEIGHT, ENEMY_FRAME_SIZE, ENEMY_FRAME_SIZE)

{

	state = s;
	look = view;
	map = nullptr;
}

Enemy::~Enemy()

{

}

AppStatus Enemy::Initialise()

{

	int i;

	const int n = ENEMY_FRAME_SIZE;



	ResourceManager& data = ResourceManager::Instance();

	if (data.LoadTexture(Resource::IMG_ENEMY, "//PONER PNG DEL ENEMIGO") != AppStatus::OK)

	{

		return AppStatus::ERROR;

	}



	render = new Sprite(data.GetTexture(Resource::IMG_ENEMY));

	if (render == nullptr)

	{

		LOG("Failed to allocate memory for Enemy sprite");

		return AppStatus::ERROR;

	}



	Sprite* sprite = dynamic_cast<Sprite*>(render);

	sprite->SetNumberAnimations((int)EnemyAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)EnemyAnim::IDLE_RIGHT, ANIM_DELAY);

	sprite->AddKeyFrame((int)EnemyAnim::IDLE_RIGHT, { 0, 0, n, n }); //MIRAR SI LAS STATS SON CORRECTAS

	sprite->SetAnimationDelay((int)EnemyAnim::IDLE_LEFT, ANIM_DELAY);

	sprite->AddKeyFrame((int)EnemyAnim::IDLE_LEFT, { 0, 0, -n, n });

	sprite->SetAnimationDelay((int)EnemyAnim::WALKING_RIGHT, ANIM_DELAY);

	for (i = 0; i < 8; ++i)

		sprite->AddKeyFrame((int)EnemyAnim::WALKING_RIGHT, { (float)i*n, 4*n, n, n });

	sprite->SetAnimationDelay((int)EnemyAnim::WALKING_LEFT, ANIM_DELAY);

	for (i = 0; i < 8; ++i)

	sprite->AddKeyFrame((int)EnemyAnim::WALKING_LEFT, { (float)i*n, 4*n, -n, n });

	sprite->SetAnimation((int)EnemyAnim::IDLE_RIGHT);

	return AppStatus::OK;

}




void Enemy::IncrScore(int n)

{

	score = // MIRAR CANTIDAD DE SCORE DADA POR RL ENEMIGO Y SUMARSELA AL Enemy

}

int Enemy::GetScore()

{

	return score;

}

void Enemy::SetTileMap(TileMap* tilemap)

{

	map = tilemap;

}

bool Enemy::IsLookingRight() const

{

	return look == Look::RIGHT;

}

bool Enemy::IsLookingLeft() const

{

	return look == Look::LEFT;

}



bool Enemy::IsInFirstHalfTile() const

{

	return pos.y % TILE_SIZE < TILE_SIZE / 2;

}

bool Enemy::IsInSecondHalfTile() const

{

	return pos.y % TILE_SIZE >= TILE_SIZE/2;

}

void Enemy::SetAnimation(int id)

{

	Sprite* sprite = dynamic_cast<Sprite*>(render);

	sprite->SetAnimation(id);

}

EnemyAnim Enemy::GetAnimation()

{

	Sprite* sprite = dynamic_cast<Sprite*>(render);

	return (EnemyAnim)sprite->GetAnimation();

}

void Enemy::StartWalkingLeft()

{

	state = State::WALKING;

	look = Look::LEFT;

	SetAnimation((int)EnemyAnim::WALKING_LEFT);

}

void Enemy::StartWalkingRight()

{

	state = State::WALKING;

	look = Look::RIGHT;

	SetAnimation((int)EnemyrAnim::WALKING_RIGHT);

}


void Enemy::ChangeAnimRight()

{

	look = Look::RIGHT;

	switch (state)

	{

		case State::IDLE:	 SetAnimation((int)EnemyAnim::IDLE_RIGHT);    break; 

		case State::WALKING: SetAnimation((int)EnemyAnim::WALKING_RIGHT); break;

		

	}

}

void Enemy::ChangeAnimLeft()

{

	look = Look::LEFT;

	switch (state)

	{

		case State::IDLE: SetAnimation((int)EnemyAnim::IDLE_LEFT);    break;

		case State::WALKING: SetAnimation((int)EnemyAnim::WALKING_LEFT); break;

		

	}

}

void Enemy::Update()

{

	//Enemy doesn't use the "Entity::Update() { pos += dir; }" default behaviour.

	//Instead, uses an independent behaviour for each axis.

	MoveX();

	MoveY();



	Sprite* sprite = dynamic_cast<Sprite*>(render);

	sprite->Update();

}

void Enemy::MoveX()

{

	AABB box;

	int prev_x = pos.x;



	//We can only go up and down while climbing

	if (state == State::CLIMBING)	return;



	if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT))

	{

		pos.x += -Enemy_SPEED;

		if (state == State::IDLE) StartWalkingLeft();

		else

		{

			if (IsLookingRight()) ChangeAnimLeft();

		}



		box = GetHitbox();

		if (map->TestCollisionWallLeft(box))

		{

			pos.x = prev_x;

			if (state == State::WALKING) Stop();

		}

	}

	else if (IsKeyDown(KEY_RIGHT))

	{

		pos.x += Enemy_SPEED;

		if (state == State::IDLE) StartWalkingRight();

		else

		{

			if (IsLookingLeft()) ChangeAnimRight();

		}



		box = GetHitbox();

		if (map->TestCollisionWallRight(box))

		{

			pos.x = prev_x;

			if (state == State::WALKING) Stop();

		}

	}

	else

	{

		if (state == State::WALKING) Stop();

	}

}

void Enemy::MoveY()

{

	AABB box;



	if (state == State::JUMPING)

	{

		LogicJumping();

	}

	else if (state == State::CLIMBING)

	{

		LogicClimbing();

	}

	else //idle, walking, falling

	{

		pos.y += Enemy_SPEED;

		box = GetHitbox();

		if (map->TestCollisionGround(box, &pos.y))

		{

			if (state == State::FALLING) Stop();



			if (IsKeyDown(KEY_UP))

			{

				box = GetHitbox();

				if (map->TestOnLadder(box, &pos.x))

					StartClimbingUp();

			}

			else if (IsKeyDown(KEY_DOWN))

			{

				//To climb up the ladder, we need to check the control point (x, y)

				//To climb down the ladder, we need to check pixel below (x, y+1) instead

				box = GetHitbox();

				box.pos.y++;

				if (map->TestOnLadderTop(box, &pos.x))

				{

					StartClimbingDown();

					pos.y += Enemy_LADDER_SPEED;

				}

					

			}

			else if (IsKeyPressed(KEY_SPACE))

			{

				StartJumping();

			}

		}

		else

		{

			if (state != State::FALLING) StartFalling();

		}

	}

}

void Enemy::LogicJumping()

{

	AABB box, prev_box;

	int prev_y;



	jump_delay--;

	if (jump_delay == 0)

	{

		prev_y = pos.y;

		prev_box = GetHitbox();



		pos.y += dir.y;

		dir.y += GRAVITY_FORCE;

		jump_delay = Enemy_JUMP_DELAY;



		//Is the jump finished?

		if (dir.y > Enemy_JUMP_FORCE)

		{

			dir.y = Enemy_SPEED;

			StartFalling();

		}

		else

		{

			//Jumping is represented with 3 different states

			if (IsAscending())

			{

				if (IsLookingRight())	SetAnimation((int)EnemyAnim::JUMPING_RIGHT);

				else					SetAnimation((int)EnemyAnim::JUMPING_LEFT);

			}

			else if (IsLevitating())

			{

				if (IsLookingRight())	SetAnimation((int)EnemyAnim::LEVITATING_RIGHT);

				else					SetAnimation((int)EnemyAnim::LEVITATING_LEFT);

			}

			else if (IsDescending())

			{

				if (IsLookingRight())	SetAnimation((int)EnemyAnim::FALLING_RIGHT);

				else					SetAnimation((int)EnemyAnim::FALLING_LEFT);

			}

		}

		//We check ground collision when jumping down

		if (dir.y >= 0)

		{

			box = GetHitbox();



			//A ground collision occurs if we were not in a collision state previously.

			//This prevents scenarios where, after levitating due to a previous jump, we found

			//ourselves inside a tile, and the entity would otherwise be placed above the tile,

			//crossing it.

			if (!map->TestCollisionGround(prev_box, &prev_y) &&

				map->TestCollisionGround(box, &pos.y))

			{

				Stop();

			}

		}

	}

}

void Enemy::LogicClimbing()

{

	AABB box;

	Sprite* sprite = dynamic_cast<Sprite*>(render);

	int tmp;



	if (IsKeyDown(KEY_UP))

	{

		pos.y -= Enemy_LADDER_SPEED;

		sprite->NextFrame();

	}

	else if (IsKeyDown(KEY_DOWN))

	{

		pos.y += Enemy_LADDER_SPEED;

		sprite->PrevFrame();

	}



	//It is important to first check LadderTop due to its condition as a collision ground.

	//By doing so, we ensure that we don't stop climbing down immediately after starting the descent.

	box = GetHitbox();

	if (map->TestOnLadderTop(box, &tmp))

	{

		if (IsInSecondHalfTile())		SetAnimation((int)EnemyAnim::CLIMBING_PRE_TOP);

		else if (IsInFirstHalfTile())	SetAnimation((int)EnemyAnim::CLIMBING_TOP);

		else					LOG("Internal error, tile should be a LADDER_TOP, coord: (%d,%d)", box.pos.x, box.pos.y);

	}

	else if (map->TestCollisionGround(box, &pos.y))

	{

		//Case leaving the ladder descending.

		Stop();

		sprite->SetAutomaticMode();

	}

	else if (!map->TestOnLadder(box, &tmp))

	{

		//Case leaving the ladder ascending.

		//If we are not in a LadderTop, colliding ground or in the Ladder it means we are leaving

		//ther ladder ascending.

		Stop();

		sprite->SetAutomaticMode();

	}

	else

	{

		if (GetAnimation() != EnemyAnim::CLIMBING)	SetAnimation((int)EnemyAnim::CLIMBING);

	}

}

void Enemy::DrawDebug(const Color& col) const

{	

	Entity::DrawHitbox(pos.x, pos.y, width, height, col);

	

	DrawText(TextFormat("Position: (%d,%d)\nSize: %dx%d\nFrame: %dx%d", pos.x, pos.y, width, height, frame_width, frame_height), 18*16, 0, 8, LIGHTGRAY);

	DrawPixel(pos.x, pos.y, WHITE);

}

void Enemy::Release()

{

	ResourceManager& data = ResourceManager::Instance();

	data.ReleaseTexture(Resource::IMG_Enemy);



	render->Release();

}
