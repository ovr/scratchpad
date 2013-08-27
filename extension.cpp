#include <iostream>
#include "php_wolf.h"
#include "PZVal.h"
#include "HashTable.h"
#include "HashTableIterator.h"
#include "EMallocAllocator.h"

static PHP_MINIT_FUNCTION(wolf)
{
	return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(wolf)
{
	return SUCCESS;
}

static PHP_RINIT_FUNCTION(wolf)
{
	return SUCCESS;
}

static PHP_RSHUTDOWN_FUNCTION(wolf)
{
	return SUCCESS;
}

zend_module_entry wolf_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	NULL,
	"Wolf",
	NULL,
	PHP_MINIT(wolf),
	PHP_MSHUTDOWN(wolf),
	PHP_RINIT(wolf),
	PHP_RSHUTDOWN(wolf),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES
};
