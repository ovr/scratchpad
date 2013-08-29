#include <iostream>
#include <cstring>
#include <sapi/embed/php_embed.h>
#include "embed.h"
#include "sapi.h"
#include "extension.h"

#ifdef ZTS
void*** tsrm_ls;
#endif

zend_uint leaks;
int memclean_called;

static const char default_ini[] =
	"html_errors=0\n"
	"register_argc_argv=0\n"
	"implicit_flush=1\n"
	"output_buffering=0\n"
	"max_execution_time=0\n"
	"max_input_time=-1\n\0";

extern "C" {
#if PHP_VERSION_ID < 50400
void zend_message_dispatcher(long message, void* data TSRMLS_DC)
#else
void zend_message_dispatcher(long message, const void* data TSRMLS_DC)
#endif
{
	switch (message) {
		case ZMSG_MEMORY_LEAK_DETECTED: {
			zend_leak_info* leak = (zend_leak_info*)data;
			std::cerr << leak->addr << " " << leak->size << " [" << leak->filename << ":" << leak->lineno << "] [" << (leak->orig_filename ? leak->orig_filename : "N/A") << ":" << leak->orig_lineno << "]" << std::endl;
			break;
		}

		case ZMSG_MEMORY_LEAK_REPEATED: {
			zend_uintptr_t repeated = (zend_uintptr_t)data;
			std::cerr << "Last leak repeated " << repeated << " times" << std::endl;
			break;
		}

		case ZMSG_MEMORY_LEAKS_GRAND_TOTAL: {
			zend_uint total = *(zend_uint*)data;
			leaks = total;
			std::cerr << total << " leaks detected" << std::endl;
			break;
		}
	}
}

}

int startup_php(void)
{
#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);
	tsrm_ls   = static_cast<void***>(ts_resource(0));
#endif
	char location[] = "-";

	sapi_startup(&embed_module);
	embed_module.ini_entries = new char[sizeof(default_ini)];
	std::memcpy(embed_module.ini_entries, default_ini, sizeof(default_ini));

	embed_module.executable_location = location;
	return embed_module.startup(&embed_module);
}

int startup_request(const char* func)
{
	const char* argv[2]    = { func, NULL };
	char PHP_SELF[]        = "PHP_SELF";
	char dash[]            = "-";

	SG(options)           |= SAPI_OPTION_NO_CHDIR;
	SG(request_info).argc  = 1;
	SG(request_info).argv  = const_cast<char**>(argv);
	PG(report_memleaks)    = 1;

	if (php_request_startup(TSRMLS_C) == FAILURE) {
		return FAILURE;
	}

	SG(headers_sent)            = 1;
	SG(request_info).no_headers = 1;

	leaks           = 0;
	memclean_called = 0;

	php_register_variable(PHP_SELF, dash, NULL TSRMLS_CC);

	return SUCCESS;
}

int shutdown_request(void)
{
	php_request_shutdown(NULL);
	return SUCCESS;
}

int shutdown_php(void)
{
	php_module_shutdown(TSRMLS_C);
	sapi_shutdown();
#ifdef ZTS
	tsrm_shutdown();
#endif

	if (embed_module.ini_entries) {
		delete[] php_embed_module.ini_entries;
		embed_module.ini_entries = 0;
	}

	return SUCCESS;
}
