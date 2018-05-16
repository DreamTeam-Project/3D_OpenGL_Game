#ifndef PHYSICS_H
#define PHYSICS_H

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <GLFW/glfw3.h>
#include "System.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

enum Physicalobj {
	standart,
	character,
	enemy,
	wall,
	bullet
};

enum Status{
	EXIST,
	ALIFE,
	DEAD
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
	void do_step(btScalar time);
	~phys_world();
};

class phys_body {
public:
	btRigidBody* body;
	int type_;
	phys_body(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass, int type = standart);
	void set_velosity(btVector3& vel);
	glm::vec3 get_pos();
	phys_body();
	virtual void collidedwith();
	void hit();
	virtual char get_status();
};

class Character: public phys_body {
public:
	int health;
	char get_status();
	void jump();
	int getHealth();
	void moving(glm::vec3& direction);
	void collidedwith() override;
	Character(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, character),
		health(50)
	{}
};

bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2);
#endif