#include <cstdlib>
#include <vector>


using std::vector;

class Game {
	void do_game() {
		Manager i;
		while (true) {
			i.do_();
		}
	}
};

class PhysObj {
	vec3 returnQart();
	vec3 returnPos();
};

class Sound {
	vec3 place;
	void soundfrom(vec3 place);
};

class GraphObj {
	vec3 place;
	Quat quat;
};


class Model {
public:
	PhysObj* phys;
	Sound sound;
	GraphObj graph;
	virtual void do_m() = 0;
};

class house : public Model {
	void do_m() override {
		printf("gh\n");
	}
};

class cat : public Model {
	void do_m() override {
		printf("ggfdgd\n");
	}
};

class physicworld;

class Manager {
public:
	physicworld* physworld;
	vector<Model&> g;
	
	void do_() {

		physworld->dosteep(time);
		for (auto i : collidenum()) {

		}
		for (auto it : g) {
			it.do_m();
		}
	}
};