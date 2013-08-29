#ifndef ZVAL_ADOPTIONPOLICIES_H_
#define ZVAL_ADOPTIONPOLICIES_H_

#include <Zend/zend.h>

class ZValAdoptionPolicyRef {
public:
	static void adopt(zval*& target, zval* other)
	{
		Z_ADDREF_P(other);
		target = other;
	}
};

class ZValAdoptionPolicyCopy {
public:
	static void adopt(zval*& target, zval* other)
	{
		Z_ADDREF_P(other);
		INIT_PZVAL(target);
		target->value    = other->value;
		Z_TYPE_P(target) = Z_TYPE_P(other);
		if (Z_TYPE_P(other) > IS_BOOL) {
			zval_copy_ctor(target);
		}
	}
};

class ZValAdoptionPolicyWriteThrough {
public:
	static void adopt(zval*& target, zval* other)
	{
		target = other;
	}
};

#endif /* ZVAL_ADOPTIONPOLICIES_H_ */
