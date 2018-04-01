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
glm::vec3 get_glm_vec(const btVector3& a);

class Physics {
public:
	Physics ();
	~Physics ();
	void step_do();
	btVector3 get_rig();
private:
	btBroadphaseInterface * broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btConvexHullShape* shape;
	btCollisionShape* boxCollisionShape;
	btDefaultMotionState* motionstate;
	btRigidBody *rigidBody;
};

Physics::Physics () {
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -1.01, 0));

	boxCollisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	glm::quat orientation = glm::normalize(glm::quat(glm::vec3(180, 0, 90)));
	motionstate = new btDefaultMotionState(
		btTransform(
			btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), 
			btVector3(0.0f, 100.0f, 0.0f)
					)
	);
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
		1,                  // mass, in kg. 0 -> Static object, will never move.
		motionstate,
		boxCollisionShape,  // collision shape of body
		btVector3(0, 0, 0)    // local inertia
	);
	rigidBody = new btRigidBody(rigidBodyCI);
	dynamicsWorld->addRigidBody(rigidBody);

}
void Physics::step_do()
{
	dynamicsWorld->stepSimulation(1.0f / 60.0f, 10);
}

Physics::~Physics () {
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;
	delete boxCollisionShape;
	delete motionstate;
	delete rigidBody;
}

btVector3 Physics::get_rig() {
	btVector3 mot;
	mot = rigidBody->getCenterOfMassPosition();
	return mot;
}

glm::vec3 get_glm_vec(const btVector3& a) {
	glm::vec3 tmp;

	tmp.x = a.getX();
	tmp.y = a.getY();
	tmp.z = a.getZ();
	return tmp;
}

#endif