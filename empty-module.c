#include <emacs-module.h>
int plugin_is_GPL_compatible;

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int
emacs_module_init (struct emacs_runtime *runtime)
{
  emacs_env *env = runtime->get_environment(runtime);

  emacs_value Qfeat = env->intern(env, "empty-module");
  emacs_value Qprovide = env->intern(env, "provide");
  emacs_value args[] = { Qfeat };

  env->funcall(env, Qprovide, 1, args);
  return 0;
}
