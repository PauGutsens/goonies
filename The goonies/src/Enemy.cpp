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
	for (i = 0; i < 8; ++i) sprite->AddKeyFrame((int)EnemyAnim::WALKING_RIGHT, { (float)i*n, 4*n, n, n });
	sprite->SetAnimationDelay((int)EnemyAnim::WALKING_LEFT, ANIM_DELAY);
	for (i = 0; i < 8; ++i) sprite->AddKeyFrame((int)EnemyAnim::WALKING_LEFT, { (float)i*n, 4*n, -n, n });
	sprite->SetAnimation((int)EnemyAnim::IDLE_RIGHT);
	return AppStatus::OK;
}
void Enemy::SetScore(int n)
{
	if(State::state == DEAD)
	{
	score +=  // MIRAR CANTIDAD DE SCORE DADA POR RL ENEMIGO Y SUMARSELA AL Enemy
	}
}

void Enemy::SetTileMap(TileMap* tilemap)
{
	map = tilemap;
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
void Enemy::WalkingLeft()
{
	state = State::WALKING;
	SetAnimation((int)EnemyAnim::WALKING_LEFT);
}

void Enemy::WalkingRight()
{
	state = State::WALKING;
	SetAnimation((int)EnemyrAnim::WALKING_RIGHT);
}
void Enemy::ChangeAnimRight()
{
	switch (state)
	{
		case State::IDLE:	 SetAnimation((int)EnemyAnim::IDLE_RIGHT);    break; 
		case State::WALKING: SetAnimation((int)EnemyAnim::WALKING_RIGHT); break;
	}
}
void Enemy::ChangeAnimLeft()
{
	switch (state)
	{
		case State::IDLE: SetAnimation((int)EnemyAnim::IDLE_LEFT);    break;
		case State::WALKING: SetAnimation((int)EnemyAnim::WALKING_LEFT); break;
	}
}
void Enemy::Update()
{

	

	MoveX();

	MoveY();



	Sprite* sprite = dynamic_cast<Sprite*>(render);

	sprite->Update();

}

void Enemy::MoveX()

{
	AABB box;
	int prev_x = pos.x;
		box = GetHitbox();

		if (map->TestCollisionWallLeft(box))

		{

			pos.x = prev_x;

			if (state == State::WALKING)ChangeAnimRight() ;

		}
		else if (map->TestCollisionWallRight(box))
		{
			pos.x = prev_x;
			if (state == State::WALKING)ChangeAnimLeft() ;

		}

		pos.x += ENEMY_SPEED;

		if (state == State::IDLE) StartWalkingRight();

		else

		{

			if (IsLookingLeft()) ChangeAnimRight();

		}



		box = GetHitbox();
	}

}
void Enemy::DrawDebug(const Color& col) const

{	
	Entity::DrawHitbox(pos.x, pos.y, width, height, col);
	DrawText(TextFormat("Position: (%d,%d)\nSize: %dx%d\nFrame: %dx%d", pos.x, pos.y, width, height, frame_width, frame_height), 8*8, 0, 8, LIGHTGRAY);
	DrawPixel(pos.x, pos.y, WHITE);
}
void Enemy::Release()
{
	ResourceManager& data = ResourceManager::Instance();
	data.ReleaseTexture(Resource::IMG_Enemy);
	render->Release();
}
