#ifndef TEST_H
#define TEST_H

#include <Core/RelicBehaviour.h>

class Test : public RelicBehaviour
{
public:
	void Serialize() override;
	void Deserialize() override;
	Test();
	~Test();

	virtual void Update();
	virtual void Start();
};

#endif
