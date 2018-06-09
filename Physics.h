#ifndef PHYSICS_H
#define PHYSICS_H

#define GLEW_STATIC

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "Camera.h"
#include "System.h"
#include <glm/glm.hpp>
#include "Sound.h"
#include <vector>

class phys_body;

enum Physicalobj {
	character,
	standart,
	enemy_dis,
	enemy_close,
	light,
	bullet,
	box_bullet,
	hp_box
};
struct collided_id {
	phys_body* your;
	char type_ag;
	phys_body* with;
};
phys_body* get_camera(phys_body* tmp = nullptr);
vector<struct collided_id>& get_collided();
vector<phys_body*>&  get_bullets();

class phys_world {
public:
	vector<phys_body*> bullets;
	vector<phys_body*> enemies;
	btDiscreteDynamicsWorld* dynamicsWorld;
	~phys_world();
	phys_world(uint* count_enemies_);
	void do_step(btScalar time, phys_world& world);
	uint* count_enemies;
	void clear_world();
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
};

class phys_body {
public:
	btRigidBody* body;
	int type_;
	phys_body(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass, int type = standart);
	void set_velosity(btVector3 vel);
	glm::vec3 get_pos();
	phys_body();
	virtual int get_damage() {
		return 0;
	}
	virtual phys_body* aim(phys_world& real_world) {
		return nullptr;
	}
	virtual void jump() {
		return;
	}
	virtual int get_bullets() {
		return 0;
	}
	virtual int getHealth() {
		return 0;
	}
	virtual bool get_able() {
		return false;
	}
	virtual void set_able(bool b) {
		return;
	}
	glm::vec3 set_angle_vel(btVector3 vel) {
		body->setAngularVelocity(vel);
	}

	btRigidBody* getRigidBody() {
		return body;
	}
	virtual int do_something(phys_world& world) {
		return 0;
	};
	virtual void collidedwith(char type, phys_body* with);
	void hit(char type, phys_body* with);
	glm::vec3 get_quat() {
		btScalar x,y, z;
		body->getOrientation().getEulerZYX(x, y,z);
		return glm::vec3(x,y,z);
	}
	virtual int get_status() {
		return ALIVE_Sound;
	};
};

class Bullet;

phys_body* find_free_bullet(phys_world& real_world);

class Character: public phys_body {
public:
	int health;
	bool inair;
	int bullets;
	int shoot;
	int get_bullets() {
		return bullets;
	}
	void jump() override;
	phys_body* aim(phys_world& real_world) override;
	void legs();
	int getHealth();
	int get_status() override {
		if (health <= 0) {
			return DEAD_Sound;
		}
		if (shoot == 1) {
			shoot = 0;
			return ATTACK_Sound;
		}
		if (inair == 1) {
			return JUMP_Sound;
		}
		if (body->getLinearVelocity().norm() > 0) {
			return WALK_Sound;
		}
		return ALIVE_Sound;
	}
	void collidedwith(char type, phys_body* with) override;
	Character(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, character),
		health(500),
		bullets(50),
		inair(false)
	{
		get_camera(this);
	}
};

class Enemy_dis :public phys_body {
public:
	int health;
	int do_something(phys_world& world) override;
	void collidedwith(char type, phys_body* with) override;
	Enemy_dis(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, enemy_dis),
		health(50)
	{
		world.enemies.push_back(this);
	}
	int get_status() override {
		if (health < 0) {
			return DEAD_Sound;
		}
		if (body->getLinearVelocity().norm() > 0) {
			return WALK_Sound;
		}
		return ALIVE_Sound;
	}
	int getHealth() override {
		return health;
	}

};

class Enemy_close : public phys_body {
public:
	int health;
	int do_something(phys_world& world) override;
	void collidedwith(char type, phys_body* with) override;
	Enemy_close(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, enemy_close),
		health(50)
	{
		world.enemies.push_back(this);
	}
	int getHealth() override {
		return health;
	}
	int get_status() override {
		if (health < 0) {
			return DEAD_Sound;
		}
		if (body->getLinearVelocity().norm() > 0) {
			return WALK_Sound;
		}
		return ALIVE_Sound;
	}
};

class Bullet : public phys_body {
public:
	bool able;
	void set_able(bool b) override {
		able = b;
	}
	void collidedwith(char type, phys_body* able) override;
	Bullet(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, bullet),
		able(true)
	{
		world.bullets.push_back(this);
	}
	bool get_able() override {
		return able;
	}
	int get_status() override {
		if(able == true)
			return ALIVE_Sound;
		return DEAD_Sound;
	}
};

class Box_bullet : public phys_body {
public:
	bool able;
	Box_bullet(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, box_bullet),
		able(1)
	{}
	bool get_able() override {
		return able;
	}
	void set_able(bool a) override {
		able = a;
	}
	void collidedwith(char type, phys_body* with) override;
	int get_status() override {
		if (able == 1) {
			return ALIVE_Sound;
		}
		return DEAD_Sound;
	}
};

class HP_box : public phys_body {
public:
	int resive_HP;
	bool able;

	HP_box(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, 0, hp_box),
		able(true),
		resive_HP(15)
	{}
	bool get_able() override {
		return able;
	}
	void set_able(bool a) override {
		able = a;
	}
	void collidedwith(char type, phys_body* with) override;
};

bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2);

#endif