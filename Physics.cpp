#include "Physics.h"
extern Camera camera;

static int timer = 0;
vector<phys_body*> to_create;
vector<phys_body*> bullets;

phys_body* persona;
vector<phys_body*> get_creation() {
	return to_create;
}

void phys_body::set_velosity(btVector3 vel) {
	body->setLinearVelocity(vel);
}

struct collided_id {
	phys_body* your;
	char type_ag;
};

vector<phys_body*> enemies;


vector<struct collided_id> collided;

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
	body->setActivationState(DISABLE_DEACTIVATION);

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
	//world_saver = this;
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

	//world_saver = nullptr;
}

glm::vec3 phys_body::get_pos() {
	//printf("%p\n", this);
	btVector3 tmp = body->getCenterOfMassPosition();
	return glm::vec3(tmp.getX(), tmp.getY(), tmp.getZ());
}

bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2) {
	//std::cout << "collision" << std::endl;
	const btCollisionObject* col_obj1 =obj1->getCollisionObject();
	phys_body* body_1 = (phys_body*)col_obj1->getUserPointer();
	char type_1 = body_1->type_;
	
	
	const btCollisionObject* col_obj2 = obj2->getCollisionObject();
	phys_body* body_2 = (phys_body*)col_obj2->getUserPointer();
	char type_2 = body_2->type_;
	body_2->hit(type_2);
	body_1->hit(type_2);
	return false;
}




void phys_world::do_step(btScalar time, phys_world& world) {
	dynamicsWorld->stepSimulation(time);
	int i = collided.size();
	//printf("i = %d\n", i);
	while (i > 0) {
		collided[i-1].your->collidedwith(collided[i-1].type_ag);
		i--;
		collided.pop_back();
	}
	i = enemies.size();
	while (i > 0) {
		enemies[i - 1]->do_something(world);
	}

}

void phys_body::hit(char type) {
	//std::cout << "hello" << std::endl;
	struct collided_id a;
	a.type_ag = type;
	a.your = this;
	collided.push_back(a);
}

phys_body::phys_body() :
	body(nullptr) {}


void Character::jump() {
	static int holloo = 0;
	holloo++;
	if (/*inair == false*/ holloo % 1000 == 101) {
		body->setLinearVelocity(body->getLinearVelocity() + btVector3(0, 10, 0));
		inair = true;
	}
	body->setActivationState(ACTIVE_TAG);
}

int Character::getHealth() {
	return health;
}

phys_body* find_free_bullet(phys_world& real_world) {
	if (bullets.empty()) {
		Bullet* tmp = new Bullet(real_world, btVector3(1, 1, 1), btVector3(1, 1, 1), btScalar(1));
		//bullets.push_back(tmp);
		return tmp;
	}
	int i = bullets.size();
	printf("%d\n", i);
	while (i > 0) {
		if (bullets[i-1]->get_status() == 0)
			return bullets[i - 1];
		i--;
	}
	printf("after all\n");
	Bullet* tmp = new Bullet(real_world, btVector3(1,1,1), btVector3(1, 1, 1), btScalar(1));
	bullets.push_back(tmp);
	return tmp;
}


phys_body* Character::aim(phys_world& real_world) {
	if (bullets > 0) {
		bullets--;
		shoot = 1;
		phys_body* tmp = find_free_bullet(real_world);
		btTransform btt;
		tmp->body->getMotionState()->getWorldTransform(btt);
		btt.setOrigin(body->getCenterOfMassPosition()); // move body to the scene node new position

													   // update _body according to CharacterDemo demo

		tmp->body->getMotionState()->setWorldTransform(btt);
		tmp->body->setCenterOfMassTransform(btt);
		tmp->set_velosity(100 * btVector3(camera.Front.x, camera.Front.y, camera.Front.z));
		return tmp;
	}
	return nullptr;
	
	//Bullet* tmp = new Bullet(world_saver, body->getCenterOfMassPosition(), btVector3(1, 1, 1), btScalar(1));

}

void Character::legs() {

}

void Enemy_dis::collidedwith(char type) {
	switch (type) {
	case standart: 
		
		break;
	case character:
		break;
	case bullet:
		health -= persona->get_damage();
		if (health < 0) {
			body->setActivationState(DISABLE_SIMULATION);
		}
		break;
	default:
		break;
	}
}

void Character::collidedwith(char type) {
	switch (type) {
	case standart:
		inair = false;
		break;
	case character:
		break;
	case bullet:
		health -= 10;
		break;
	case box_bullet:
		bullets += 10;
		break;
	case enemy_close:
		health -= 20;
	case hp_box:
		health += 15;
	default:
		break;
	}
}

void phys_body::collidedwith(char type) {

	return;
}

void Enemy_close::collidedwith(char type) {
	switch (type) {
	case standart:
		break;
	case character:
		health -= persona->get_damage();
		break;
	case bullet:
		health -= persona->get_damage();
		break;
	case enemy_close:
		health -= 20;
	default:
		break;
	}
	return;
}

void Bullet::collidedwith(char type) {
	status = false;
	return;
}

int Enemy_close::do_something(phys_world& world) {
	if (health > 0) {
		body->setActivationState(DISABLE_DEACTIVATION);
		body->setAngularVelocity(btVector3(0, 0, 1));
		body->setLinearVelocity(body->getCenterOfMassPosition() - persona->body->getCenterOfMassPosition());
	}
	
	return 0;
}


int Enemy_dis::do_something(phys_world& world) {
	timer++;
	if (timer % 1000 == 101 && health>0) {
		phys_body* tmp = find_free_bullet(world);
		to_create.push_back(tmp);
		body->setActivationState(DISABLE_DEACTIVATION);
		body->setLinearVelocity(body->getCenterOfMassPosition() - persona->body->getCenterOfMassPosition());
		body->setAngularVelocity(btVector3(0, 0, 1));
	}
	return 0;
}

void Box_bullet::collidedwith(char type) {
	switch (type) {
	case standart:
		break;
	case character:
		status = 0;
		body->setActivationState(DISABLE_SIMULATION);
		break;
	case bullet:
		break;
	case enemy_close:
		break;
	default:
		break;
	}
	return;
}

void HP_box::collidedwith(char type) {
	switch (type) {
	case standart:
		break;
	case character:
		status = 0;
		body->setActivationState(DISABLE_SIMULATION);
		break;
	case bullet:
		break;
	case enemy_close:
		break;
	default:
		break;
	}
	return;
}