#include <emacs-module.h>
#include "utils.h"

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sql.h>
#include <sqlext.h>

#if __GNUC__ >= 4
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT
#endif

EXPORT int plugin_is_GPL_compatible;

emacs_value odbc_drivers(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data) EMACS_NOEXCEPT;
emacs_value odbc_data_sources(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data) EMACS_NOEXCEPT;
EXPORT int emacs_module_init(struct emacs_runtime *runtime);
