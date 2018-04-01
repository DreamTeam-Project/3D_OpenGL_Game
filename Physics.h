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
glm::vec3 get_glm_vec(btVector3 a);

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
//void Physics::print_cord() {
//	btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()(1);
//	btRigitBody* body = bt
//}
Physics::Physics () {
	
	/*collisionConfiguration = new btDefaultCollisionConfiguration();
	broadphase = new btDbvtBroadphase();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));*/
	//end initialisation
	//start loading

	/*btConvexHullShape* shape = new btConvexHullShape((const btScalar*)&(Pos.at(0)), num, sizeof(vec3));
	btVector3 color(1, 1, 1);
	btVector3 scaling(0.1, 0.1, 0.1);
	shape->setLocalScaling(scaling);*/
	/*btScalar mass(1);
	btDefaultMotionState* myMotionStage = new btDefaultMotionStage()*/

	// Build the broadphase
	broadphase = new btDbvtBroadphase();
	// Set up the collision configuration and dispatcher
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	solver = new btSequentialImpulseConstraintSolver;

	// The world.
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -0.81f, 0));

	// In this example, all monkeys will use the same collision shape : 
	// A box of 2m*2m*2m (1.0 is the half-extent !)
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
glm::vec3 get_glm_vec(btVector3 a) {
	glm::vec3 tmp;

	tmp.x = a.getX();
	tmp.y = a.getY();
	tmp.z = a.getZ();
	return tmp;
}

#endif