#include <emacs-module.h>
#include <stdarg.h>
#include <stdlib.h>

emacs_value intern(emacs_env *env, const char *name);
emacs_value funcall(emacs_env *env, const char* fn, ptrdiff_t nargs, ...);
void provide(emacs_env *env, const char *feature);
