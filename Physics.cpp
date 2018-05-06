#include "Physics.h"

void phys_body::set_velosity(btVector3& vel) {
	body->setLinearVelocity(vel);
}

phys_body::phys_body(phys_world& world, btVector3& position, btVector3& col_shape, btScalar mass) {
	btCollisionShape* colShape = new btBoxShape(col_shape);
	world.collisionShapes.push_back(colShape);
	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(position);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	body = new btRigidBody(rbInfo);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	body->setUserPointer(body);

	world.dynamicsWorld->addRigidBody(body);
}

phys_world::phys_world() {
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	gContactAddedCallback = callbackFunc;
}

phys_world::~phys_world() {

	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;
}

glm::vec3 phys_body::get_pos() {
	btVector3 tmp = body->getCenterOfMassPosition();
	return glm::vec3(tmp.getX(), tmp.getY(), tmp.getZ());
}

bool callbackFunc(btManifoldPoint& cp, btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2) {
	std::cout << "collision" << std::endl;
	const btCollisionObject* tmp =obj1->getCollisionObject();
	phys_body* newone = (phys_body*)tmp->getUserPointer();
	//((bulletObject*)obj1->getUserPointer())->hit=true;
	return false;
}

void phys_world::do_step(btScalar time) {
	dynamicsWorld->stepSimulation(time);

}