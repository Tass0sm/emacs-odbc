#include "emacs-odbc.h"
#include <stdio.h>

emacs_value odbc_alloc_env(emacs_env *env,
                           ptrdiff_t nargs,
                           emacs_value args[],
                           void *data) EMACS_NOEXCEPT {
  SQLHENV *sql_env_p;

  sql_env_p = malloc(sizeof(SQLHENV));
  SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, sql_env_p);
  SQLSetEnvAttr(*sql_env_p, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);

  emacs_value res = env->make_user_ptr(env, NULL, (void *) sql_env_p);
  return res;
}


const char* odbc_alloc_env_doc =
  "Allocate an ODBC environment handle.\n";


emacs_value odbc_drivers(emacs_env *env,
                         ptrdiff_t nargs,
                         emacs_value args[],
                         void *data) EMACS_NOEXCEPT {
  SQLHENV sql_env;
  SQLCHAR driver[256];
  SQLCHAR attr[256];
  SQLSMALLINT driver_ret;
  SQLSMALLINT attr_ret;
  SQLUSMALLINT direction;
  SQLRETURN ret;

  emacs_value list = nil(env);

  sql_env = *((SQLHENV *) env->get_user_ptr(env, args[0]));

  direction = SQL_FETCH_FIRST;
  while(SQL_SUCCEEDED(ret = SQLDrivers(sql_env, direction,
                                       driver, sizeof(driver), &driver_ret,
                                       attr, sizeof(attr), &attr_ret))) {
    direction = SQL_FETCH_NEXT;

    emacs_value driver_str = env->make_string(env, (const char*) driver, strlen((const char*) driver));
    emacs_value attr_str = env->make_string(env, (const char*) attr, strlen((const char*) attr));
    emacs_value driver_attr_cell = cons(env, driver_str, attr_str);
    list = cons(env, driver_attr_cell, list);

    if (ret == SQL_SUCCESS_WITH_INFO) printf("\tdata truncation\n");
  }

  return list;
}


const char* odbc_drivers_doc =
  "List installed ODBC drivers.\n";


emacs_value odbc_data_sources(emacs_env *env,
                              ptrdiff_t nargs,
                              emacs_value args[],
                              void *data) EMACS_NOEXCEPT {
  SQLHENV sql_env;
  SQLCHAR dsn[256];
  SQLCHAR desc[256];
  SQLSMALLINT dsn_ret;
  SQLSMALLINT desc_ret;
  SQLUSMALLINT direction;
  SQLRETURN ret;

  emacs_value list = nil(env);

  sql_env = *((SQLHENV *) env->get_user_ptr(env, args[0]));

  direction = SQL_FETCH_FIRST;
  while(SQL_SUCCEEDED(ret = SQLDataSources(sql_env, direction,
                                           dsn, sizeof(dsn), &dsn_ret,
                                           desc, sizeof(desc), &desc_ret))) {
    direction = SQL_FETCH_NEXT;

    emacs_value dsn_str = env->make_string(env, (const char*) dsn, strlen((const char*) dsn));
    emacs_value desc_str = env->make_string(env, (const char*) desc, strlen((const char*) desc));
    emacs_value dsn_desc_cell = cons(env, dsn_str, desc_str);
    list = cons(env, dsn_desc_cell, list);

    if (ret == SQL_SUCCESS_WITH_INFO) printf("\tdata truncation\n");
  }

  return list;
}


const char* odbc_data_sources_doc =
  "List installed ODBC data sources.\n";


int emacs_module_init (struct emacs_runtime *runtime) {
  emacs_env *env = runtime->get_environment(runtime);

  define_function(env, "odbc-alloc-env", 0, 0, &odbc_alloc_env, odbc_alloc_env_doc);
  define_function(env, "odbc-drivers", 1, 1, &odbc_drivers, odbc_drivers_doc);
  define_function(env, "odbc-data-sources", 1, 1, &odbc_data_sources, odbc_data_sources_doc);

  provide(env, "emacs-odbc");
  return 0;
}
