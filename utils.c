#include "utils.h"

emacs_value nil(emacs_env *env) {
  return env->intern(env, "nil");
}

emacs_value intern(emacs_env *env, const char *name) {
  return env->intern(env, name);
}

emacs_value funcall(emacs_env *env, const char* fn, ptrdiff_t nargs, ...) {
  va_list argv;
  va_start(argv, nargs);
  emacs_value *args = (emacs_value *) malloc(nargs * sizeof(emacs_value));

  for (int idx = 0; idx < nargs; idx++) {
    args[idx] = va_arg (argv, emacs_value);
  }

  va_end(argv);
  emacs_value val = env->funcall(env, intern (env, fn), nargs, args);
  free(args);
  return val;
}

emacs_value cons(emacs_env *env, emacs_value x, emacs_value y) {
  return funcall(env, "cons", 2, x, y);
}

void define_function(emacs_env *env, const char *name,
                     ptrdiff_t min_arity, ptrdiff_t max_arity,
                     emacs_value (*function) (emacs_env *env,
                                              ptrdiff_t nargs,
                                              emacs_value* args,
                                              void *data) EMACS_NOEXCEPT,
                     const char *documentation)
{
  emacs_value fn = env->make_function(env, min_arity, max_arity, function,
                                      documentation, NULL);
  funcall(env, "fset", 2, intern(env, name), fn);
}


void provide(emacs_env *env, const char *feature) {
  funcall(env, "provide", 1, intern(env, feature));
}
