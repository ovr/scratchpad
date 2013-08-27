/*
 * PZVal.h
 *
 *  Created on: 26 серп. 2013
 *      Author: vladimir
 */

#ifndef PZVAL_H_
#define PZVAL_H_

extern "C" {
#include <Zend/zend.h>
#include <Zend/zend_operators.h>
}

#include <cstddef>

#include "HashTable.h"

class PZVal {
public:
	enum Type {
		isNull     = IS_NULL,
		isLong     = IS_LONG,
		isDouble   = IS_DOUBLE,
		isBool     = IS_BOOL,
		isArray    = IS_ARRAY,
		isObject   = IS_OBJECT,
		isString   = IS_STRING,
		isResource = IS_RESOURCE,
		isConstant = IS_CONSTANT,
		isConstantArray = IS_CONSTANT_ARRAY,
		isCallable = IS_CALLABLE
	};

	PZVal(void)
	{
		ALLOC_INIT_ZVAL(this->m_z);
	}

	PZVal(long int v)
	{
		MAKE_STD_ZVAL(this->m_z);
		ZVAL_LONG(this->m_z, v);
	}

	PZVal(double v)
	{
		MAKE_STD_ZVAL(this->m_z);
		ZVAL_DOUBLE(this->m_z, v);
	}

	explicit PZVal(bool v)
	{
		MAKE_STD_ZVAL(this->m_z);
		ZVAL_BOOL(this->m_z, v ? 1 : 0);
	}

	PZVal(const char* s)
	{
		MAKE_STD_ZVAL(this->m_z);
		ZVAL_STRING(this->m_z, s, 1);
	}

	explicit PZVal(zval* other)
		: m_z(other)
	{
		Z_ADDREF_P(this->m_z);
	}

	PZVal(const PZVal& other)
		: m_z(other.m_z)
	{
		Z_ADDREF_P(this->m_z);
	}

	PZVal(const char* s, std::size_t len)
	{
		MAKE_STD_ZVAL(this->m_z);
		ZVAL_STRINGL(this->m_z, s, len, 1);
	}

	~PZVal(void)
	{
		zval_ptr_dtor(&this->m_z);
	}

	operator zval*(void)
	{
		return this->m_z;
	}

	operator const zval*(void) const
	{
		return this->m_z;
	}

	void addRef(void)
	{
		Z_ADDREF_P(this->m_z);
	}

	void delRef(void)
	{
		Z_DELREF_P(this->m_z);
	}

	zend_uint refCount(void) const
	{
		return Z_REFCOUNT_P(this->m_z);
	}

	void setRefCount(zend_uint rc)
	{
		Z_SET_REFCOUNT_P(this->m_z, rc);
	}

	bool isRef(void) const
	{
		return Z_ISREF_P(this->m_z);
	}

	void setIsRef(bool ref)
	{
		Z_SET_ISREF_TO_P(this->m_z, ref);
	}

	void separate(void)
	{
		SEPARATE_ZVAL(&this->m_z);
	}

	void separateIfNotRef(void)
	{
		SEPARATE_ZVAL_IF_NOT_REF(&this->m_z);
	}

	void separateToMakeIsRef(void)
	{
		SEPARATE_ZVAL_TO_MAKE_IS_REF(&this->m_z);
	}

	bool isSimple(void) const
	{
		return Z_TYPE_P(this->m_z) <= IS_BOOL;
	}

	PZVal& operator=(const PZVal& other)
	{
		if (this->refCount() != 1) {
			this->delRef();
			MAKE_STD_ZVAL(this->m_z);
		}
		else {
			zval_dtor(this->m_z);
			INIT_PZVAL(this->m_z);
		}

		ZVAL_COPY_VALUE(this->m_z, other.m_z);
		if (!other.isSimple()) {
			zval_copy_ctor(this->m_z);
		}

		return *this;
	}

	PZVal& operator=(const zval* other)
	{
		if (this->refCount() != 1) {
			this->delRef();
			MAKE_STD_ZVAL(this->m_z);
		}
		else {
			zval_dtor(this->m_z);
			INIT_PZVAL(this->m_z);
		}

		ZVAL_COPY_VALUE(this->m_z, other);
		if (Z_TYPE_P(other) > IS_BOOL) {
			zval_copy_ctor(this->m_z);
		}

		return *this;
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

	char* asString(void)
	{
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



private:
	zval* m_z;
};


#endif /* PZVAL_H_ */
