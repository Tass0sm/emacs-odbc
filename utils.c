#include "utils.h"

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

void provide(emacs_env *env, const char *feature) {
  funcall(env, "provide", 1, intern(env, feature));
}
