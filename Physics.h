
#ifndef PHYSICS_H
#define PHYSICS_H
#define GLEW_STATIC
//#include "Manager.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "Camera.h"



#include "System.h"


#include <glm/glm.hpp>

#include "Sound.h"


//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>
#include <vector>

//#include <GLFW/glfw3.h>
class phys_body;
extern phys_body* persona;
extern vector<phys_body*> to_create;
extern vector<phys_body*> bullets;

vector<phys_body*> get_creation();



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

class phys_world {
public:
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	btDefaultCollisionConfiguration* collisionConfiguration; //= new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher;// = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlappingPairCache;// = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver;// = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld;// = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	phys_world();
	void do_step(btScalar time, phys_world& world);
	~phys_world();
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
	glm::vec3 set_angle_vel(btVector3 vel) {
		body->setAngularVelocity(vel);
	}

	btRigidBody* getRigidBody() {
		return body;
	}
	virtual int do_something(phys_world& world) {
		return 0;
	};
	virtual void collidedwith(char type);
	void hit(char type);
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
	void collidedwith(char type) override;
	Character(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, character),
		health(50),
		bullets(50),
		inair(false)
	{
		persona = this;
	}
};

class Enemy_dis :public phys_body {
public:
	int health;
	int shoot;
	int do_something(phys_world& world) override;
	int get_status() override {
		if (health <= 0) {
			return DEAD_Sound;
		}
		if (shoot == 1) {
			shoot = 0;
			return ATTACK_Sound;
		}
		if (body->getLinearVelocity().norm() > 0) {
			return WALK_Sound;
		}
		return ALIVE_Sound;
		return 0;
	}
	void collidedwith(char type) override;
	Enemy_dis(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, bullet),
		health(50)
	{
		printf("enemy_dis created\n");
	}

};





class Enemy_close : public phys_body {
public:
	int health;
	int do_something(phys_world& world) override;
	void collidedwith(char type) override;
	Enemy_close(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, character),
		health(50)
	{}
	int get_status() override {
		if (health <= 0) {
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
	bool status;
	void collidedwith(char type) override;
	Bullet(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, bullet),
		status(true)
	{
		bullets.push_back(this);
	}
	int get_status() override {
		return ALIVE_Sound;
	}
};

class Box_bullet : public phys_body {
public:
	int status;

	Box_bullet(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, box_bullet),
		status(1)
	{}
	void collidedwith(char type) override;
	int get_status() override {
		if (status == 1) {
			return ALIVE_Sound;
		}
		return DEAD_Sound;
	}
};

class HP_box : public phys_body {
public:
	int resive_HP;
	int status;

	HP_box(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, hp_box),
		status(1),
		resive_HP(15)
	{}
	void collidedwith(char type) override;
};

bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2);




#endif