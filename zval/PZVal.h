#ifndef PZVAL_H_
#define PZVAL_H_

#include <cassert>

extern "C" {
#include <Zend/zend.h>
#include <Zend/zend_operators.h>
}

#include "zval/AllocationPolicies.h"
#include "zval/FreePolicies.h"
#include "zval/AdoptionPolicies.h"
#include "HashTable.h"

template<typename AllocationPolicy, typename FreePolicy, typename AdoptionPolicy>
class PZVal {
public:
	static PZVal create(void)
	{
		zval* z = AllocationPolicy::allocate();
		ZVAL_NULL(z);
		return PZVal(z, 1);
	}

	static PZVal create(int v)
	{
		zval* z = AllocationPolicy::allocate();
		ZVAL_LONG(z, v);
		return PZVal(z, 1);
	}

	static PZVal create(long int v)
	{
		zval* z = AllocationPolicy::allocate();
		ZVAL_LONG(z, v);
		return PZVal(z, 1);
	}

	static PZVal create(double v)
	{
		zval* z = AllocationPolicy::allocate();
		ZVAL_DOUBLE(z, v);
		return PZVal(z, 1);
	}

	static PZVal create(bool v)
	{
		zval* z = AllocationPolicy::allocate();
		ZVAL_BOOL(z, v);
		return PZVal(z, 1);
	}

	static PZVal createFromResource(long int v)
	{
		zval* z = AllocationPolicy::allocate();
		ZVAL_RESOURCE(z, v);
		return PZVal(z, 1);
	}

	static PZVal create(const char* s)
	{
		zval* z = AllocationPolicy::allocate();
		ZVAL_STRING(z, s, 1);
		return PZVal(z, 1);
	}

	static PZVal create(const char* s, std::size_t len)
	{
		zval* z = AllocationPolicy::allocate();
		ZVAL_STRINGL(z, s, len, 1);
		return PZVal(z, 1);
	}

	static PZVal createFromString(const char* s)
	{
		zval* z = AllocationPolicy::allocate();
		ZVAL_STRING(z, s, 0);
		return PZVal(z, 1);
	}

	static PZVal createFromString(const char* s, uint len)
	{
		zval* z = AllocationPolicy::allocate();
		ZVAL_STRINGL(z, s, len, 0);
		return PZVal(z, 1);
	}

	static PZVal createFromInternedString(const char* s)
	{
		zval* z = AllocationPolicy::allocate();
#if PHP_VERSION_ID < 50400
		ZVAL_STRING(z, s, 1);
#else
		if (IS_INTERNED(s)) {
			ZVAL_STRINGL(z, s, INTERNED_LEN(s)-1, 0);
		}
		else {
			ZVAL_STRING(z, s, 1);
		}
#endif
		return PZVal(z, 1);
	}

	static PZVal createArray(uint size = 0)
	{
		zval* z = AllocationPolicy::allocate();
		array_init_size(z, size);
		return PZVal(z, 1);
	}

	static PZVal createStdClass(TSRMLS_D)
	{
		zval* z = AllocationPolicy::allocate();
		object_init(z);
		return PZVal(z, 1);
	}

	static PZVal createObject(zend_class_entry* ce TSRMLS_DC)
	{
		zval* z = AllocationPolicy::allocate();
		object_init_ex(z, ce);
		return PZVal(z, 1);
	}

	PZVal(void)
		: m_z(0)
	{
	}

	explicit PZVal(zval* z)
		: m_z(0)
	{
		assert(z != 0);
		AdoptionPolicy::adopt(this->m_z, z);
	}

	template<typename I, typename D, typename A>
	PZVal(const PZVal<I, D, A>& other)
		: m_z(0)
	{
		zval* tmp = const_cast<zval*>(other.m_z);
		A::adopt(this->m_z, tmp);
	}

	PZVal(const PZVal& other)
		: m_z(0)
	{
		zval* tmp = const_cast<zval*>(other.m_z);
		AdoptionPolicy::adopt(this->m_z, tmp);
	}

	~PZVal(void)
	{
		FreePolicy::destroy(this->m_z);
	}

	operator zval*(void)
	{
		return this->m_z;
	}

	operator zval*(void) const
	{
		return this->m_z;
	}

	void addRef(void)
	{
		assert(this->m_z != NULL);
		Z_ADDREF_P(this->m_z);
	}

	void delRef(void)
	{
		assert(this->m_z != NULL);
		Z_DELREF_P(this->m_z);
	}

	zend_uint refCount(void) const
	{
		assert(this->m_z != NULL);
		return Z_REFCOUNT_P(this->m_z);
	}

	void setRefCount(zend_uint rc)
	{
		assert(this->m_z != NULL);
		Z_SET_REFCOUNT_P(this->m_z, rc);
	}

	bool isRef(void) const
	{
		assert(this->m_z != NULL);
		return Z_ISREF_P(this->m_z);
	}

	void setIsRef(bool ref)
	{
		assert(this->m_z != NULL);
		Z_SET_ISREF_TO_P(this->m_z, ref);
	}

	void separate(void)
	{
		assert(this->m_z != NULL);
		SEPARATE_ZVAL(&this->m_z);
	}

	void separateIfNotRef(void)
	{
		assert(this->m_z != NULL);
		if (!PZVAL_IS_REF(this->m_z)) {
			SEPARATE_ZVAL(&this->m_z);
		}
	}

	void separateToMakeIsRef(void)
	{
		assert(this->m_z != NULL);
		if (!PZVAL_IS_REF(this->m_z)) {
			SEPARATE_ZVAL(&this->m_z);
			Z_SET_ISREF_P(this->m_z);
		}
	}

	bool isSimple(void) const
	{
		assert(this->m_z != NULL);
		return Z_TYPE_P(this->m_z) <= IS_BOOL;
	}

	template<typename I, typename D, typename A>
	PZVal& operator=(const PZVal<I, D, A>& other)
	{
		assert(this->m_z != NULL);

		if (this->refCount() != 1) {
			this->delRef();
			this->m_z = AllocationPolicy::allocate();
		}
		else {
			zval_dtor(this->m_z);
		}

		zval* tmp = const_cast<zval*>(other);
		AdoptionPolicy::adopt(this->m_z, tmp);
		return *this;
	}

	PZVal& operator=(const PZVal& other)
	{
		assert(this->m_z != NULL);

		if (this->refCount() != 1) {
			this->delRef();
			this->m_z = AllocationPolicy::allocate();
		}
		else {
			zval_dtor(this->m_z);
		}

		zval* tmp = const_cast<zval*>(other.m_z);
		AdoptionPolicy::adopt(this->m_z, tmp);
		return *this;
	}

	PZVal& operator=(zval* other)
	{
		assert(this->m_z != NULL);

		if (this->refCount() != 1) {
			this->delRef();
			this->m_z = AllocationPolicy::allocate();
		}
		else {
			zval_dtor(this->m_z);
			INIT_PZVAL(this->m_z);
		}

		AdoptionPolicy::adopt(this->m_z, other);
		return *this;
	}

	PZVal& operator=(int v)
	{
		zval tmp = zval_used_for_init;
		ZVAL_LONG(&tmp, v);
		return this->operator=(&tmp);
	}

	PZVal& operator=(long int v)
	{
		zval tmp = zval_used_for_init;
		ZVAL_LONG(&tmp, v);
		return this->operator=(&tmp);
	}

	PZVal& operator=(double v)
	{
		zval tmp = zval_used_for_init;
		ZVAL_DOUBLE(&tmp, v);
		return this->operator=(&tmp);
	}

	PZVal& operator=(bool v)
	{
		zval tmp = zval_used_for_init;
		ZVAL_BOOL(&tmp, v);
		return this->operator=(&tmp);
	}

	PZVal& operator=(const char* s)
	{
		zval tmp = zval_used_for_init;
		ZVAL_STRING(&tmp, s, 0);
		return this->operator=(&tmp);
	}

	void ref(const PZVal& other)
	{
		if (this->refCount() != 1) {
			this->delRef();
		}
		else {
			zval_dtor(this->m_z);
		}

		this->m_z = other.m_z;
		Z_ADDREF_P(this->m_z);
	}

	void ref(zval* other)
	{
		if (this->refCount() != 1) {
			this->delRef();
		}
		else {
			zval_dtor(this->m_z);
		}

		this->m_z = other;
		Z_ADDREF_P(this->m_z);
	}

	int type(void) const
	{
		assert(this->m_z != NULL);
		return Z_TYPE_P(this->m_z);
	}

	long int asInteger(void) const
	{
		switch (this->type()) {
			case IS_LONG:
			case IS_BOOL:
			case IS_RESOURCE:
				return Z_LVAL_P(this->m_z);

			case IS_DOUBLE:
				return zend_dval_to_lval(Z_DVAL_P(this->m_z));

			case IS_NULL:
				return 0;

			default: {
				zval copy;
				ZVAL_COPY_VALUE(&copy, this->m_z);
				zval_copy_ctor(&copy);
				convert_to_long(&copy);
				return Z_LVAL(copy);
			}
		}
	}

	double asDouble(void) const
	{
		switch (this->type()) {
			case IS_LONG:
			case IS_BOOL:
			case IS_RESOURCE:
				return static_cast<double>(Z_LVAL_P(this->m_z));

			case IS_DOUBLE:
				return Z_DVAL_P(this->m_z);

			case IS_NULL:
				return 0;

			default: {
				zval copy;
				ZVAL_COPY_VALUE(&copy, this->m_z);
				zval_copy_ctor(&copy);
				convert_to_double(&copy);
				return Z_DVAL(copy);
			}
		}
	}

	bool asBool(void) const
	{
		switch (this->type()) {
			case IS_LONG:
			case IS_BOOL:
			case IS_RESOURCE:
				return (Z_LVAL_P(this->m_z)) ? 1 : 0;

			case IS_DOUBLE:
				return (Z_DVAL_P(this->m_z)) ? 1 : 0;

			case IS_NULL:
				return 0;

			default: {
				zval copy;
				ZVAL_COPY_VALUE(&copy, this->m_z);
				zval_copy_ctor(&copy);
				convert_to_double(&copy);
				return Z_DVAL(copy);
			}
		}
	}

	char* asString(bool convert = true)
	{
		if (convert) {
			switch (this->type()) {
				case IS_NULL:
					return STR_EMPTY_ALLOC();

				case IS_STRING:
					return estrndup(Z_STRVAL_P(this->m_z), Z_STRLEN_P(this->m_z));

				default: {
					zval copy;
					ZVAL_COPY_VALUE(&copy, this->m_z);
					zval_copy_ctor(&copy);
					convert_to_string(&copy);
					char* res = estrndup(Z_STRVAL(copy), Z_STRLEN(copy));
					zval_dtor(&copy);
					return res;
				}
			}
		}
		else {
			return (this->type() == IS_STRING) ? Z_STRVAL_P(this->m_z) : 0;
		}
	}

	bool getBool(void) const
	{
		return Z_BVAL_P(this->m_z);
	}

	long int getLong(void) const
	{
		return Z_LVAL_P(this->m_z);
	}

	long int getResource(void) const
	{
		return Z_RESVAL_P(this->m_z);
	}

	double getDouble(void) const
	{
		return Z_DVAL_P(this->m_z);
	}

	char* getString(void)
	{
		return Z_STRVAL_P(this->m_z);
	}

	const char* getString(void) const
	{
		return Z_STRVAL_P(this->m_z);
	}

	int getStringLength(void) const
	{
		return Z_STRLEN_P(this->m_z);
	}

	HashTable* getHashTable(void)
	{
		return Z_ARRVAL_P(this->m_z);
	}

	void toInt(void)
	{
		convert_to_long_ex(&this->m_z);
	}

	void toDouble(void)
	{
		convert_to_double_ex(&this->m_z);
	}

	void toBool(void)
	{
		convert_to_boolean_ex(&this->m_z);
	}

	void toNull(void)
	{
		convert_to_null_ex(&this->m_z);
	}

	void toString(void)
	{
		convert_to_string_ex(&this->m_z);
	}

	void toArray(void)
	{
		convert_to_array_ex(&this->m_z);
	}

	void toObject(void)
	{
		convert_to_object_ex(&this->m_z);
	}

	PZVal& operator++(void)
	{
		increment_function(this->m_z);
		return *this;
	}

	PZVal& operator--(void)
	{
		decrement_function(this->m_z);
		return *this;
	}

	PZVal operator++(int)
	{
		zval* tmp;
		ALLOC_ZVAL(tmp);
		MAKE_COPY_ZVAL(&this->m_z, tmp);
		increment_function(this->m_z);
		return PZVal(tmp);
	}

	PZVal operator--(int)
	{
		zval* tmp;
		ALLOC_ZVAL(tmp);
		MAKE_COPY_ZVAL(&this->m_z, tmp);
		return PZVal(tmp);
	}

	friend PZVal<ZValAllocationPolicyPerRequest, ZValFreePolicyStandard, ZValAdoptionPolicyRef> operator+(const PZVal& lhs, const PZVal& rhs)
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		zval* tmp = ZValAllocationPolicyPerRequest::allocate();
		TSRMLS_FETCH();
		add_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal<ZValAllocationPolicyPerRequest, ZValFreePolicyStandard, ZValAdoptionPolicyRef>(tmp, 1);
	}

	friend PZVal operator-(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		sub_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal(tmp, 1);
	}

	friend PZVal operator*(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		mul_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator/(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		div_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator%(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		mod_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator<<(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		shift_left_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator>>(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		shift_right_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator&&(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		MAKE_STD_ZVAL(tmp);
		ZVAL_BOOL(tmp, zend_is_true(const_cast<PZVal&>(lhs)) && zend_is_true(const_cast<PZVal&>(rhs)));
		return PZVal(tmp);
	}

	friend PZVal operator||(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		MAKE_STD_ZVAL(tmp);
		ZVAL_BOOL(tmp, zend_is_true(const_cast<PZVal&>(lhs)) || zend_is_true(const_cast<PZVal&>(rhs)));
		return PZVal(tmp);
	}

	friend PZVal operator&(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		bitwise_and_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator|(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		bitwise_or_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator==(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		is_equal_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator!=(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		is_not_equal_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator<(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		is_smaller_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator>(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		is_smaller_function(tmp, const_cast<PZVal&>(rhs), const_cast<PZVal&>(lhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator<=(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		is_smaller_or_equal_function(tmp, const_cast<PZVal&>(lhs), const_cast<PZVal&>(rhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator>=(const PZVal& lhs, const PZVal& rhs)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		is_smaller_or_equal_function(tmp, const_cast<PZVal&>(rhs), const_cast<PZVal&>(lhs) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator!(const PZVal& op)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		boolean_not_function(tmp, const_cast<PZVal&>(op) TSRMLS_CC);
		return PZVal(tmp);
	}

	friend PZVal operator~(const PZVal& op)
	{
		zval* tmp;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(tmp);
		bitwise_not_function(tmp, const_cast<PZVal&>(op) TSRMLS_CC);
		return PZVal(tmp);
	}

protected:
	zval* m_z;

	PZVal(zval* z, int)
		: m_z(z)
	{
	}
};

typedef PZVal<ZValAllocationPolicyPerRequest, ZValFreePolicyStandard, ZValAdoptionPolicyRef> StdPZVal;
typedef PZVal<ZValAllocationPolicyPerRequest, ZValFreePolicyNone, ZValAdoptionPolicyWriteThrough> PZValWrapper;

#endif /* PZVAL_H_ */
