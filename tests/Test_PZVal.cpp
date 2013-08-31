#include <iostream>
#include <cstring>
#include <cpptest.h>
#include "embed.h"
#include "zval/PZVal.h"
#include "tests/Test_PZVal.h"

Test_PZVal::Test_PZVal(void)
{
	TEST_ADD(Test_PZVal::testCreation);
	TEST_ADD(Test_PZVal::testReferences);
	TEST_ADD(Test_PZVal::testSeparation);
	TEST_ADD(Test_PZVal::testOperators);
}

void Test_PZVal::setup(void)
{
	TEST_ASSERT(embed_startup(__func__) == SUCCESS);
}

void Test_PZVal::tear_down(void)
{
	embed_shutdown();
	TEST_ASSERT(leaks == 0);
}

void Test_PZVal::testCreation(void)
{
	zend_first_try {
		StdPZVal z_null(StdPZVal::create());
		TEST_ASSERT(z_null.type() == IS_NULL);
		TEST_ASSERT(z_null.refCount() == 1);
		TEST_ASSERT(z_null.isRef() == false);

		StdPZVal z_bool = StdPZVal::create(true);
		TEST_ASSERT(z_bool.type() == IS_BOOL);
		TEST_ASSERT(z_bool.asBool() == true);
		TEST_ASSERT(z_bool.asBool() == z_bool.getBool());
		TEST_ASSERT(z_bool.refCount() == 1);
		TEST_ASSERT(z_bool.isRef() == false);

		StdPZVal z_long = StdPZVal::create(12345678);
		TEST_ASSERT(z_long.type() == IS_LONG);
		TEST_ASSERT(z_long.asInteger() == 12345678);
		TEST_ASSERT(z_long.asInteger() == z_long.getLong());
		TEST_ASSERT(z_long.refCount() == 1);
		TEST_ASSERT(z_long.isRef() == false);

		StdPZVal z_double = StdPZVal::create(12345.5);
		TEST_ASSERT(z_double.type() == IS_DOUBLE);
		TEST_ASSERT(z_double.asDouble() == 12345.5);
		TEST_ASSERT(z_double.asDouble() == z_double.getDouble());
		TEST_ASSERT(z_double.refCount() == 1);
		TEST_ASSERT(z_double.isRef() == false);

		StdPZVal z_str1 = StdPZVal::create("test\0test");
		TEST_ASSERT(z_str1.type() == IS_STRING);
		TEST_ASSERT(!std::strcmp(z_str1.getString(), "test"));
		TEST_ASSERT(z_str1.getStringLength() == 4);
		TEST_ASSERT(z_str1.refCount() == 1);
		TEST_ASSERT(z_str1.isRef() == false);

		StdPZVal z_str2 = StdPZVal::create(ZEND_STRL("test\0test"));
		TEST_ASSERT(z_str2.type() == IS_STRING);
		TEST_ASSERT(!memcmp(z_str2.getString(), "test\0test", sizeof("test\0test")-1));
		TEST_ASSERT(z_str2.getStringLength() == 9);
		TEST_ASSERT(z_str2.refCount() == 1);
		TEST_ASSERT(z_str2.isRef() == false);

		StdPZVal z_array = StdPZVal::createArray();
		TEST_ASSERT(z_array.type() == IS_ARRAY);
		TEST_ASSERT(z_array.refCount() == 1);
		TEST_ASSERT(z_array.isRef() == false);
		TEST_ASSERT(zend_hash_num_elements(z_array.getHashTable()) == 0);
	}
	zend_catch {
		TEST_ASSERT(false);
	}
	zend_end_try();
}

void Test_PZVal::testReferences(void)
{
	zend_first_try {
		zval* x;

		{
			StdPZVal z = StdPZVal::create("test");
			z.addRef();
			TEST_ASSERT(z.refCount() == 2);
			z.addRef();
			TEST_ASSERT(z.refCount() == 3);
			z.delRef();
			TEST_ASSERT(z.refCount() == 2);
			z.setRefCount(10);
			TEST_ASSERT(z.refCount() == 10);
			z.setRefCount(2);
			TEST_ASSERT(z.refCount() == 2);

			z.setIsRef(true);
			TEST_ASSERT(z.isRef() == true);
			z.setIsRef(false);
			TEST_ASSERT(z.isRef() == false);
			z.setIsRef(true);
			TEST_ASSERT(z.isRef() == true);

			x = z;
		}

		TEST_ASSERT(Z_REFCOUNT_P(x) == 1);
		TEST_ASSERT(Z_TYPE_P(x) == IS_STRING);
		TEST_ASSERT(Z_STRLEN_P(x) == 4);
		TEST_ASSERT(!std::strcmp(Z_STRVAL_P(x), "test"));
		TEST_ASSERT(Z_ISREF_P(x) == 1);
		zval_ptr_dtor(&x);
	}
	zend_catch {
		TEST_ASSERT(false);
	}
	zend_end_try();
}

void Test_PZVal::testSeparation(void)
{
	zend_first_try {
		StdPZVal z = StdPZVal::create(123);
		zval* orig = z;

		TEST_ASSERT(z.refCount() == 1);
		z.separate();
		TEST_ASSERT(z.refCount() == 1);
		TEST_ASSERT(z == orig);

		z.addRef();
		TEST_ASSERT(z.refCount() == 2);
		z.separate();
		TEST_ASSERT(z.refCount() == 1);
		TEST_ASSERT(z != orig);
		zval_ptr_dtor(&orig);

		z.addRef();
		TEST_ASSERT(z.refCount() == 2);
		z.setIsRef(true);
		TEST_ASSERT(z.isRef() == true);
		z.separateIfNotRef();
		TEST_ASSERT(z.refCount() == 2);
		TEST_ASSERT(z.isRef() == true);

		z.setIsRef(false);
		TEST_ASSERT(z.isRef() == false);
		orig = z;
		z.separateIfNotRef();
		TEST_ASSERT(z.refCount() == 1);
		TEST_ASSERT(z.isRef() == false);
		zval_ptr_dtor(&orig);
	}
	zend_catch {
		TEST_ASSERT(false);
	}
	zend_end_try();
}

void Test_PZVal::testOperators(void)
{
	zend_first_try {
		zval* za;
		zval* zb;

		MAKE_STD_ZVAL(za);
		MAKE_STD_ZVAL(zb);

		{
			PZValWrapper a(za);
			PZValWrapper b(zb);
			TEST_ASSERT(a.refCount() == 1);
			TEST_ASSERT(b.refCount() == 1);

			a = 1;
			b = 2;
			StdPZVal c = StdPZVal::create();
			std::cerr << "RC: " << c.refCount() << std::endl;
			std::cerr << "Type: " << c.type() << std::endl;
			std::cout << "c = a+b" << std::endl;
			c = a + b;
			std::cerr << "RC: " << c.refCount() << std::endl;
			TEST_ASSERT(c.refCount() == 1);
			std::cerr << "Type: " << c.type() << std::endl;
			TEST_ASSERT(c.type() == IS_LONG);
		}

		TEST_ASSERT(Z_REFCOUNT_P(za) == 1);
		TEST_ASSERT(Z_REFCOUNT_P(zb) == 1);

		zval_ptr_dtor(&za);
		zval_ptr_dtor(&zb);
	}
	zend_catch {
		TEST_ASSERT(false);
	}
	zend_end_try();
}
