#pragma once
#include "Entity.h"
#include "TileMap.h"

//Representation model size: 32x32
#define ENEMY_FRAME_SIZE		32
//Logical model size: 12x28
#define ENEMY_PHYSICAL_WIDTH	12
#define ENEMY_PHYSICAL_HEIGHT	2

//Logic states
enum class State { IDLE, WALKING, DEAD };
enum class Look { RIGHT, LEFT };

//Rendering states

enum class EnemyAnim {

	IDLE_LEFT, IDLE_RIGHT,
	WALKING_LEFT, WALKING_RIGHT,
	NUM_ANIMATIONS

};



class Enemy: public Entity

{

public:

	Enemy(const Point& p, State s, Look view);
	~Enemy();
	AppStatus Initialise();
	void SetTileMap(TileMap* tilemap);
	void IncrScore(int n);
	void Update();
	void DrawDebug(const Color& col) const;
	void Release();



private:

	bool IsLookingRight() const;

	bool IsLookingLeft() const;



	//Player mechanics

	void MoveX();
	void MoveY();

	//Animation management
	void SetAnimation(int id);

	EnemyAnim GetAnimation();
	void StartWalkingLeft();
	void StartWalkingRight();
	void ChangeAnimRight();
	void ChangeAnimLeft();

	State state;
	TileMap *map;
	int score;

};

