//#include "Physics.h"
//
//
//
//Physics_t::Physics_t() {
//
//	// Build the broadphase
//	broadphase = new btDbvtBroadphase();
//	// Set up the collision configuration and dispatcher
//	collisionConfiguration = new btDefaultCollisionConfiguration();
//	dispatcher = new btCollisionDispatcher(collisionConfiguration);
//
//	// The actual physics solver
//	solver = new btSequentialImpulseConstraintSolver;
//
//	// The world.
//	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
//	dynamicsWorld->setGravity(btVector3(0, -10.0f, 0));
//
//
//
//	// In this example, all monkeys will use the same collision shape : 
//	// A box of 2m*2m*2m (1.0 is the half-extent !)
//
//
//	//boxCollisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
//	//glm::quat orientation = glm::normalize(glm::quat(glm::vec3(180, 0, 90)));
//	//motionstate = new btDefaultMotionState(
//	//	btTransform(
//	//		btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w),
//	//		btVector3(0.0f, 100.0f, 0.0f)
//	//	)
//	//);
//	//btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
//	//	1,                  // mass, in kg. 0 -> Static object, will never move.
//	//	motionstate,
//	//	boxCollisionShape,  // collision shape of body
//	//	btVector3(0, 0, 0)    // local inertia
//	//);
//	//rigidBody = new btRigidBody(rigidBodyCI);
//	//dynamicsWorld->addRigidBody(rigidBody);
//
//}
//void Physics_t::step_do()
//{
//	dynamicsWorld->stepSimulation(1.0f / 60.0f, 10);
//}
//Physics_t::~Physics_t() {
//	delete dynamicsWorld;
//	delete solver;
//	delete dispatcher;
//	delete collisionConfiguration;
//	delete broadphase;
//	/*delete boxCollisionShape;
//	delete motionstate;
//	delete rigidBody;*/
//}
////btVector3 Physics::get_rig() {
////	btVector3 mot;
////	mot = rigidBody->getCenterOfMassPosition();
////	return mot;
////}
////glm::vec3 get_glm_vec(btVector3 a) {
////	glm::vec3 tmp;
////
////	tmp.x = a.getX();
////	tmp.y = a.getY();
////	tmp.z = a.getZ();
////	return tmp;
////}
//
//int Physics_t::addRigidBody(btRigidBody* rigidBody) {
//	dynamicsWorld->addRigidBody(rigidBody);
//	return 0;
//}
//
//int Physics_t::removeRigidBody(btRigidBody* rigidBody) {
//	dynamicsWorld->removeRigidBody(rigidBody);
//	return 0;
//}