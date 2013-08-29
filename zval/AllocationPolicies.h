#ifndef ZVAL_ALLOCATIONPOLICIES_H_
#define ZVAL_ALLOCATIONPOLICIES_H_

extern "C" {
#include <Zend/zend.h>
}

class ZValAllocationPolicyPerRequest {
public:
	static zval* allocate(void)
	{
		zval* result;
		MAKE_STD_ZVAL(result);
		return result;
	}
};

class ZValAllocationPolicyPermanent {
public:
	static zval* allocate(void)
	{
		zval* result;
		ALLOC_PERMANENT_ZVAL(result);
		INIT_PZVAL(result);
		return result;
	}
};

#endif /* ZVAL_ALLOCATIONPOLICIES_H_ */
