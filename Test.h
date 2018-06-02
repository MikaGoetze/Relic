#ifndef TEST_H
#define TEST_H

#include <Core/RelicBehaviour.h>
#include <vector>

class Test : public RelicBehaviour
{
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
	Test* other;
};

#endif
