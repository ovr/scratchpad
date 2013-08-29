#ifndef ZVAL_FREEPOLICIES_H_
#define ZVAL_FREEPOLICIES_H_

#include <cstdlib>

extern "C" {
#include <Zend/zend.h>
}

class ZValFreePolicyNone {
public:
	static void destroy(zval*)
	{
	}
};

class ZValFreePolicyStandard {
public:
	static void destroy(zval* z)
	{
		if (Z_REFCOUNT_P(z) > 1) {
			Z_DELREF_P(z);
		}
		else {
			zval_ptr_dtor(&z);
		}
	}
};

class ZValFreePolicyPermanent {
public:
	static void destroy(zval* z)
	{
		if (Z_REFCOUNT_P(z) > 1) {
			Z_DELREF_P(z);
		}
		else {
			std::free(z);
		}
	}
};

class ZValFreePolicyNullify {
public:
	static void destroy(zval* z)
	{
		if (Z_REFCOUNT_P(z) > 1) {
			Z_DELREF_P(z);
		}
		else {
			zval_dtor(z);
			ZVAL_NULL(z);
		}
	}
};

#endif /* ZVAL_FREEPOLICIES_H_ */
