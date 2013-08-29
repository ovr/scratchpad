#ifndef EMBED_H_
#define EMBED_H_

extern "C" {
#include <main/php.h>

#ifdef ZTS
extern void*** tsrm_ls;
#endif

extern zend_uint leaks;
extern int memclean_called;
#if PHP_VERSION_ID < 50400
ZEND_API void zend_message_dispatcher(long message, void* data TSRMLS_DC);
#else
ZEND_API void zend_message_dispatcher(long message, const void* data TSRMLS_DC);
#endif
}

#if !ZEND_DEBUG
static inline int _mem_block_check(void *ptr, int silent ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) { return ptr != NULL; }
#endif

int startup_php(void);
int startup_request(const char* func);
int shutdown_request(void);
int shutdown_php(void);

static inline int embed_startup(const char* func)
{
	if (SUCCESS == startup_php()) {
		if (SUCCESS == startup_request(func)) {
			return SUCCESS;
		}

		shutdown_php();
	}

	return FAILURE;
}

static inline void embed_shutdown(void)
{
	shutdown_request();
	shutdown_php();
}

#endif /* EMBED_H_ */
