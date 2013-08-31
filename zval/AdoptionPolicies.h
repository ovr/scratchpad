#ifndef ZVAL_ADOPTIONPOLICIES_H_
#define ZVAL_ADOPTIONPOLICIES_H_

#include <cassert>

extern "C" {
#include <Zend/zend.h>
}

class ZValAdoptionPolicyRef {
public:
	static void adopt(zval** target, zval* other)
	{
		assert(target && other);
		if (*target) {
			ZVAL_NULL(*target);
			zval_ptr_dtor(target);
		}

		Z_ADDREF_P(other);
		*target = other;
	}
};

class ZValAdoptionPolicyCopy {
public:
	static void adopt(zval** target, zval* other)
	{
		assert(target && *target && other);
		Z_ADDREF_P(other);
		INIT_PZVAL(*target);
		(*target)->value    = other->value;
		Z_TYPE_PP(target) = Z_TYPE_P(other);
		if (Z_TYPE_P(other) > IS_BOOL) {
			zval_copy_ctor(*target);
		}
	}
};

class ZValAdoptionPolicyWriteThrough {
public:
	static void adopt(zval** target, zval* other)
	{
		assert(target && other);
		if (*target) {
			ZVAL_NULL(*target);
			zval_ptr_dtor(target);
		}

		*target = other;
	}
};

#endif /* ZVAL_ADOPTIONPOLICIES_H_ */
