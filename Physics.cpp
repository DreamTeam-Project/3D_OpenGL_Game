#include "Physics.h"

void phys_body::set_velosity(btVector3& vel) {
	body->setLinearVelocity(vel);
}
vector<phys_body*> collided;

phys_body::phys_body(phys_world& world, btVector3 position, btVector3 col_shape, btScalar mass, int type):
type_(type) {
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
	body->setUserPointer(this);

	world.dynamicsWorld->addRigidBody(body);
}

phys_world::phys_world() {
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	gContactAddedCallback = callbackFunc;
	dynamicsWorld->setGravity(btVector3(0, -10, 0));
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

bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2) {
	//std::cout << "collision" << std::endl;
	const btCollisionObject* col_obj1 =obj1->getCollisionObject();
	phys_body* body_1 = (phys_body*)col_obj1->getUserPointer();
	body_1->hit();
	const btCollisionObject* col_obj2 = obj2->getCollisionObject();
	phys_body* body_2 = (phys_body*)col_obj2->getUserPointer();
	body_2->hit();
	return false;
}

char phys_body::get_status() {
	return EXIST;
}

void phys_body::collidedwith() {

	return;
}

void Character::collidedwith() {
	printf("Collide\n");
	health-=100;
	body->setLinearVelocity(btVector3(0.0, 10.0, 0.0));
}
void phys_world::do_step(btScalar time) {
	dynamicsWorld->stepSimulation(time);
	int i = collided.size();
	printf("i = %d\n", i);
	while (i > 0) {
		collided[i-1]->collidedwith();
		i--;
		collided.pop_back();
	}
	

}

void phys_body::hit() {
	//std::cout << "hello" << std::endl;
	collided.push_back(this);
}

phys_body::phys_body() :
	body(nullptr) {}

char Character::get_status() {
	if (health > 0) {
		printf("ALIFE\n");
		return ALIFE;
	}
	printf("deadddd\n");
	return DEAD;
}

void Character::jump() {
	body->setLinearVelocity(body->getLinearVelocity() + btVector3(0, 10, 0));
}

int Character::getHealth() {
	return health;
}

void Character::moving(glm::vec3& didection) {

}