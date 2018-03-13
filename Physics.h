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

using std::vector;
using glm::vec3;

class Physics {
public:
	Physics (int num, vector<Vertex>& Pos);
	~Physics ();

private:
	btBroadphaseInterface * broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btConvexHullShape* shape;
};

Physics::Physics (int num, vector<Vertex>& Pos) {
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
	btConvexHullShape* shape = new btConvexHullShape((const btScalar*)&(Pos.at(0)), num, sizeof(vec3));
	btVector3 color(1, 1, 1);
	btVector3 scaling(0.1, 0.1, 0.1);
	shape->setLocalScaling(scaling);


}

Physics::~Physics () {
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;
}

#endif