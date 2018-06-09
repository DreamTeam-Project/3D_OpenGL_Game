#include "Physics.h"

vector<phys_body*>&  get_bullets() {
	static vector<phys_body*> bullets;
	return bullets;
}

phys_body* get_camera(phys_body* tmp) {
	static phys_body* camera;
	if (tmp == nullptr) {
		return camera;
	}
	camera = tmp;
	return camera;
}

void phys_body::set_velosity(btVector3 vel) {
	body->setLinearVelocity(vel);
}

vector<struct collided_id>& get_collided() {
	static vector<struct collided_id> collided;
	return collided;
}

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

void phys_world::clear_world() {
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

	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}
}

phys_world::phys_world(uint* count_enemies_) {
	count_enemies = count_enemies_;
	//printf("%ud\n", *count_enemies);
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

	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	delete dynamicsWorld;

	delete solver;

	delete overlappingPairCache;

	delete dispatcher;

	delete collisionConfiguration;

}

glm::vec3 phys_body::get_pos() {
	btVector3 tmp = body->getCenterOfMassPosition();
	return glm::vec3(tmp.getX(), tmp.getY(), tmp.getZ());
}

bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, 
		const btCollisionObjectWrapper* obj2, int id2, int index2) {

	const btCollisionObject* col_obj1 =obj1->getCollisionObject();
	phys_body* body_1 = (phys_body*)col_obj1->getUserPointer();
	char type_1 = body_1->type_;
	
	
	const btCollisionObject* col_obj2 = obj2->getCollisionObject();
	phys_body* body_2 = (phys_body*)col_obj2->getUserPointer();
	char type_2 = body_2->type_;
	body_2->hit(type_1, body_1);
	body_1->hit(type_2, body_2);
	return false;
}

void phys_world::do_step(btScalar time, phys_world& world) {
	dynamicsWorld->stepSimulation(time);
	int i = get_collided().size();
	while (i > 0) {
		get_collided()[i-1].your->collidedwith(get_collided()[i-1].type_ag, get_collided()[i-1].with);
		i--;
		get_collided().pop_back();
	}
	i = enemies.size();
	while (i > 0) {
		int res;
		res = enemies[i - 1]->do_something(world);
		if (res == 1) {
			enemies.erase(enemies.begin()+i - 1);
			(*count_enemies)--;
			//printf("%ud\n", *count_enemies);
		}
		i--;
	}

}

void phys_body::hit(char type, phys_body* with) {
	struct collided_id a;
	a.type_ag = type;
	a.your = this;
	a.with = with;
	get_collided().push_back(a);
}

phys_body::phys_body() :
	body(nullptr) {}

void Character::jump() {
	btScalar len = body->getLinearVelocity().length();
	btScalar h = body->getCenterOfMassPosition().getY();

	if (inair == false && len < 13 && h< 10 ) {
		body->setLinearVelocity(body->getLinearVelocity() + btVector3(0, 4, 0));
		inair = true;
	}
	body->setActivationState(ACTIVE_TAG);
}

int Character::getHealth() {
	return health;
}

phys_body* find_free_bullet(phys_world& real_world) {
	if (get_bullets().empty()) {
		Bullet* tmp = new Bullet(real_world, btVector3(1, 1, 1), btVector3(1, 1, 1), btScalar(1));
		return tmp;
	}
	int i = get_bullets().size();
	while (i > 0) {
		if (get_bullets()[i - 1]->get_able() == 0) {
			get_bullets()[i - 1]->set_able(1);
			return get_bullets()[i - 1];
		}
			
		i--;
	}
	Bullet* tmp = new Bullet(real_world, btVector3(1,1,1), btVector3(1, 1, 1), btScalar(1));
	get_bullets().push_back(tmp);
	return tmp;
}

phys_body* Character::aim(phys_world& real_world) {
	if (bullets > 0) {
		bullets--;
		shoot = 1;
		phys_body* tmp = find_free_bullet(real_world);

		btTransform btt;
		
		tmp->body->getMotionState()->getWorldTransform(btt);
		btt.setOrigin(body->getCenterOfMassPosition()+ 10*btVector3(camera.Front.x, camera.Front.y, camera.Front.z)+ btVector3(0, -1, 0)); // move body to the scene node new position


		btt.setRotation(btQuaternion(glm::atan(camera.Front.x / camera.Front.z)-1.57,0, 0));

		tmp->body->getMotionState()->setWorldTransform(btt);
		tmp->body->setCenterOfMassTransform(btt);
		
		tmp->set_velosity(100 * btVector3(camera.Front.x, camera.Front.y, camera.Front.z).normalize());
		tmp->body->setAngularVelocity(btVector3(0, 0, 0));

		return tmp;
	}
	return nullptr;

}

void Character::legs() {

}

void Enemy_dis::collidedwith(char type, phys_body* with) {
	switch (type) {
	case standart: 
		
		break;
	case character:
		break;
	case bullet:
		if (with->get_able() == true) {
			with->set_able(false);
			health -= 10;
		}
		if (health < 0) {
			body->setActivationState(DISABLE_SIMULATION);
		}
		break;
	default:
		break;
	}
}

void Character::collidedwith(char type, phys_body* with) {
	switch (type) {
	case standart:
		inair = false;
		break;
	case character:
		break;
	case bullet:
		if (with->get_able() == 1)
		{
			with->set_able(false);
			health -= 10;
		}
			
		break;
	case box_bullet:
		if (with->get_able() == true) {
			with->set_able(false);
			bullets += 10;
		}
		
		break;
	case enemy_close:
		if(with->getHealth() > 0)
			health -= 20;
		break;
	case hp_box:
		if (with->get_able() == 1) {
			with->set_able(0);
			health += 10;
		}
		break;
	case enemy_dis:
		if (with->getHealth() > 0)
			health -= 40;
		break;
	default:
		break;
	}
}

void phys_body::collidedwith(char type, phys_body* with) {
	return;
}

void Enemy_close::collidedwith(char type, phys_body* with) {
	switch (type) {
	case standart:
		break;
	case character:
		break;
	case bullet:
		if (with->get_able() == 1) {
			with->set_able(false);
			health -= 10;
		}
		
		break;
	case enemy_close:
		break;
	default:
		break;
	}
	return;
}

void Bullet::collidedwith(char type, phys_body* with) {
	return;
}

int Enemy_close::do_something(phys_world& world) {
	btScalar len = (body->getCenterOfMassPosition() - get_camera()->body->getCenterOfMassPosition()).length();
	if (health > 0 && len < 100) {
		body->setActivationState(DISABLE_DEACTIVATION);
		body->setLinearVelocity(-4*(body->getCenterOfMassPosition() - get_camera()->body->getCenterOfMassPosition()).normalize());

		btTransform btt;
		body->getMotionState()->getWorldTransform(btt);
		btt.setOrigin(body->getCenterOfMassPosition());
		btVector3 used = -(body->getCenterOfMassPosition() - get_camera()->body->getCenterOfMassPosition());
		if (used.getZ() > 0) {
			btt.setRotation(btQuaternion(glm::atan(used.getX() / used.getZ()), 0, 0));
		}
		else {
			btt.setRotation(btQuaternion(-3.14 +(glm::atan(used.getX() / used.getZ())), 0, 0 ));
		}

		body->getMotionState()->setWorldTransform(btt);
		body->setCenterOfMassTransform(btt);


	}
	else {
		body->setActivationState(DISABLE_SIMULATION);
		body->setAngularVelocity(btVector3(0, 0, 0));
		body->setLinearVelocity(btVector3(0, 0, 0));
		return 1;
	}
	
	return 0;
}

int Enemy_dis::do_something(phys_world& world) {
	btScalar len = (body->getCenterOfMassPosition() - get_camera()->body->getCenterOfMassPosition()).length();
	if (health > 0 && len < 100) {
		body->setActivationState(DISABLE_DEACTIVATION);
		body->setLinearVelocity(-4 * (body->getCenterOfMassPosition() - get_camera()->body->getCenterOfMassPosition()).normalize());

		btTransform btt;
		body->getMotionState()->getWorldTransform(btt);
		btt.setOrigin(body->getCenterOfMassPosition());
		btVector3 used = -(body->getCenterOfMassPosition() - get_camera()->body->getCenterOfMassPosition());
		if (used.getZ() > 0) {
			btt.setRotation(btQuaternion(glm::atan(used.getX() / used.getZ()), 0, 0));
		}
		else {
			btt.setRotation(btQuaternion(-3.14 + (glm::atan(used.getX() / used.getZ())), 0, 0));
		}

		body->getMotionState()->setWorldTransform(btt);
		body->setCenterOfMassTransform(btt);


	}
	else {
		body->setActivationState(DISABLE_SIMULATION);
		body->setAngularVelocity(btVector3(0, 0, 0));
		body->setLinearVelocity(btVector3(0, 0, 0));
		return 1;
	}

	return 0;
}

void Box_bullet::collidedwith(char type, phys_body* with) {
	switch (type) {
	case standart:
		break;
	case character:
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

void HP_box::collidedwith(char type, phys_body* with) {
	switch (type) {
	case standart:
		break;
	case character:
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