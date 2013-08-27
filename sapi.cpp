#include <iostream>
#include <main/php.h>
#include <main/php_main.h>
#include <main/php_variables.h>
#include "sapi.h"
#include "extension.h"

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

static int embed_startup(sapi_module_struct* sapi_module)
{
#ifdef HAVE_SIGNAL_H
#if defined(SIGPIPE) && defined(SIG_IGN)
	signal(SIGPIPE, SIG_IGN);
#endif
#endif

	return php_module_startup(sapi_module, &wolf_module_entry, 1);
}

static int embed_deactivate(TSRMLS_D)
{
	std::cout.flush();
	return SUCCESS;
}

static int embed_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	std::cout.write(str, str_length);
	return str_length;
}

static void embed_flush(void* server_context)
{
	std::cout.flush();
}

static void embed_send_header(sapi_header_struct* sapi_header, void* server_context TSRMLS_DC)
{
}

static char* embed_read_cookies(TSRMLS_D)
{
	return 0;
}

static void embed_register_variables(zval* track_vars_array TSRMLS_DC)
{
	php_import_environment_variables(track_vars_array TSRMLS_CC);
}

static void embed_log_message(char* message)
{
	std::cerr << message << std::endl;
}

static char name[]        = "php-embed";
static char pretty_name[] = "Embedded PHP";

sapi_module_struct embed_module = {
	name,                        /* name */
	pretty_name,                 /* pretty name */

	embed_startup,               /* startup */
	php_module_shutdown_wrapper, /* shutdown */

	NULL,                        /* activate */
	embed_deactivate,            /* deactivate */

	embed_ub_write,              /* unbuffered write */
	embed_flush,                 /* flush */
	NULL,                        /* get uid */
	NULL,                        /* getenv */

	php_error,                   /* error handler */

	NULL,                        /* header handler */
	NULL,                        /* send headers handler */
	embed_send_header,           /* send header handler */

	NULL,                        /* read POST data */
	embed_read_cookies,          /* read Cookies */

	embed_register_variables,    /* register server variables */
	embed_log_message,           /* Log message */
	NULL,                        /* Get request time */
	NULL,                        /* Child terminate */

	STANDARD_SAPI_MODULE_PROPERTIES
};
