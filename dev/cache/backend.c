
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2012 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_phalcon.h"
#include "phalcon.h"

#include "Zend/zend_operators.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#include "kernel/main.h"
#include "kernel/memory.h"

#include "kernel/exception.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "kernel/concat.h"
#include "kernel/fcall.h"

/**
 * Phalcon\Cache\Backend
 *
 * This class implements common functionality for backend adapters. All the backend cache adapter must
 * extend this class
 */

/**
 * Phalcon\Cache\Backend constructor
 *
 * @param mixed $frontendObject
 * @param array $backendOptions
 */
PHP_METHOD(Phalcon_Cache_Backend, __construct){

	zval *frontend_object = NULL, *backend_options = NULL;
	zval *a0 = NULL;
	zval *r0 = NULL;
	int eval_int;

	PHALCON_MM_GROW();
	
	PHALCON_ALLOC_ZVAL_MM(a0);
	array_init(a0);
	zend_update_property(phalcon_cache_backend_ce, this_ptr, SL("_backendOptions"), a0 TSRMLS_CC);
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &frontend_object, &backend_options) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!backend_options) {
		PHALCON_INIT_VAR(backend_options);
		array_init(backend_options);
	}
	
	if (Z_TYPE_P(frontend_object) != IS_OBJECT) {
		PHALCON_THROW_EXCEPTION_STR(phalcon_cache_exception_ce, "Frontend must be an Object");
		return;
	}
	eval_int = phalcon_array_isset_string(backend_options, SL("prefix")+1);
	if (eval_int) {
		PHALCON_ALLOC_ZVAL_MM(r0);
		phalcon_array_fetch_string(&r0, backend_options, SL("prefix"), PH_NOISY_CC);
		phalcon_update_property_zval(this_ptr, SL("_prefix"), r0 TSRMLS_CC);
	}
	
	phalcon_update_property_zval(this_ptr, SL("_frontendObject"), frontend_object TSRMLS_CC);
	phalcon_update_property_zval(this_ptr, SL("_backendOptions"), backend_options TSRMLS_CC);
	
	PHALCON_MM_RESTORE();
}

/**
 * Starts a cache. The $keyname allow to identify the created fragment
 *
 * @param int|string $keyName
 * @return  mixed
 */
PHP_METHOD(Phalcon_Cache_Backend, start){

	zval *key_name = NULL, *backend = NULL, *front_end = NULL, *prefixed_key = NULL;
	zval *existing_cache = NULL, *fresh = NULL;
	zval *t0 = NULL, *t1 = NULL;
	zval *r0 = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &key_name) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(backend);
	phalcon_read_property(&backend, this_ptr, SL("_backendOptions"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(front_end);
	phalcon_read_property(&front_end, this_ptr, SL("_frontendObject"), PH_NOISY_CC);
	
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_prefix"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(prefixed_key);
	PHALCON_CONCAT_VV(prefixed_key, t0, key_name);
	
	PHALCON_INIT_VAR(existing_cache);
	PHALCON_CALL_METHOD_PARAMS_1(existing_cache, this_ptr, "get", prefixed_key, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(t1);
	ZVAL_NULL(t1);
	
	PHALCON_ALLOC_ZVAL_MM(r0);
	is_equal_function(r0, existing_cache, t1 TSRMLS_CC);
	if (zend_is_true(r0)) {
		PHALCON_INIT_VAR(fresh);
		ZVAL_BOOL(fresh, 1);
		PHALCON_CALL_METHOD_NORETURN(front_end, "start", PH_NO_CHECK);
	} else {
		PHALCON_INIT_VAR(fresh);
		ZVAL_BOOL(fresh, 0);
	}
	
	phalcon_update_property_zval(this_ptr, SL("_fresh"), fresh TSRMLS_CC);
	phalcon_update_property_bool(this_ptr, SL("_started"), 1 TSRMLS_CC);
	
	RETURN_CCTOR(existing_cache);
}

/**
 * Returns front-end instance adapter related to the back-end
 *
 * @return mixed
 */
PHP_METHOD(Phalcon_Cache_Backend, getFrontend){

	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_frontendObject"), PH_NOISY_CC);
	
	RETURN_CCTOR(t0);
}

/**
 * Checks whether the last cache is fresh or cached
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Cache_Backend, isFresh){

	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_fresh"), PH_NOISY_CC);
	
	RETURN_CCTOR(t0);
}

/**
 * Checks whether the cache has started buffering or not
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Cache_Backend, isStarted){

	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_started"), PH_NOISY_CC);
	
	RETURN_CCTOR(t0);
}

/**
 * Gets the last key stored by the cache
 *
 * @return string
 */
PHP_METHOD(Phalcon_Cache_Backend, getLastKey){

	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_lastKey"), PH_NOISY_CC);
	
	RETURN_CCTOR(t0);
}

