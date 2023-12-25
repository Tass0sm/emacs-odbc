#include "empty-module.h"

int emacs_module_init (struct emacs_runtime *runtime) {
  emacs_env *env = runtime->get_environment(runtime);

  provide(env, "empty-module");
  return 0;
}
