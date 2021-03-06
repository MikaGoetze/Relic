#ifndef TEST_H
#define TEST_H

#include <Core/RelicBehaviour.h>
#include <vector>

class Test : public RelicBehaviour
{
private:
	static BehaviourRegistrar<Test> registrar;

protected:

	void Serialize() override;
	void Deserialize() override;

public:
	Test();
	~Test();

	virtual void Update();
	virtual void Start();


	bool b;
	float f;
	std::vector<float*> ia;
	std::vector<Test*> others;
};

#endif
