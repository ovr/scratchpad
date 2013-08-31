#ifndef TEST_PZVAL_H_
#define TEST_PZVAL_H_

#include <cpptest.h>

class Test_PZVal : public Test::Suite {
public:
	Test_PZVal(void);

protected:
	virtual void setup(void);
	virtual void tear_down(void);

private:
	void testCreation(void);
	void testReferences(void);
	void testSeparation(void);
	void testOperators(void);
};

#endif /* TEST_PZVAL_H_ */
