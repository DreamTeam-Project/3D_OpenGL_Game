#ifndef PHYSICS_H
#define PHYSICS_H
#define GLEW_STATIC
//#include "Manager.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "Camera.h"



#include "System.h"


#include <glm/glm.hpp>



//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>
#include <vector>

//#include <GLFW/glfw3.h>
class phys_body;
extern phys_body* persona;
extern vector<phys_body*> to_create;

vector<phys_body*> get_creation();



enum Physicalobj {
	character,
	standart,
	enemy_dis,
	enemy_close,
	wall,
	bullet
};

enum Status{
EXIST,
ALIFE,
DEAD};

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
	btRigidBody* getRigidBody() {
		return body;
	}
	virtual int do_something(phys_world& world) {
		return 0;
	};
	virtual void collidedwith(char type);
	void hit(char type);
	glm::vec3 get_quat() {
		return glm::vec3(body->getOrientation().getX(), body->getOrientation().getY(), body->getOrientation().getZ());
	}
	virtual char get_status();
};

class Bullet;

class Character: public phys_body {
public:
	int health;
	bool inair;
	char get_status();
	void jump();
	phys_body* aim(phys_world& real_world);
	void legs();
	int getHealth();
	void collidedwith(char type) override;
	Character(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, character),
		health(50), 
		inair(false)
	{
		persona = this;
	}
};

class Enemy_dis :public phys_body {
public:
	int health;
	int do_something(phys_world& world) override;
	char get_status() {
		return 0;
	}
	void collidedwith(char type) override;
	Enemy_dis(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, bullet),
		health(50)
	{}

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
};

class Bullet : public phys_body {
public:
	bool status;
	void collidedwith(char type) override;
	Bullet(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass) :
		phys_body(world, position, col_shape, mass, character),
		status(true)
	{}
};

bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2);




#endif