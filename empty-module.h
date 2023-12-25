#include <emacs-module.h>
int plugin_is_GPL_compatible;

#include "utils.h"

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int emacs_module_init (struct emacs_runtime *runtime);
