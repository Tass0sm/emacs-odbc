#include <emacs-module.h>
#include <stdarg.h>
#include <stdlib.h>

emacs_value nil(emacs_env *env);
emacs_value intern(emacs_env *env, const char *name);
emacs_value funcall(emacs_env *env, const char* fn, ptrdiff_t nargs, ...);
emacs_value cons(emacs_env *env, emacs_value x, emacs_value y);

void define_function(emacs_env *env, const char *name,
                     ptrdiff_t min_arity, ptrdiff_t max_arity,
                     emacs_value (*function) (emacs_env *env,
                                              ptrdiff_t nargs,
                                              emacs_value* args,
                                              void *data) EMACS_NOEXCEPT,
                     const char *documentation);
void provide(emacs_env *env, const char *feature);
