#include "emacs-odbc.h"
#include <stdio.h>
#include <stdbool.h>

emacs_value odbc_alloc_env(emacs_env *env,
                           ptrdiff_t nargs,
                           emacs_value args[],
                           void *data) EMACS_NOEXCEPT {
  SQLRETURN ret;
  SQLHENV *sql_env_p;

  sql_env_p = malloc(sizeof(SQLHENV));
  ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, sql_env_p);

  if (SQL_SUCCEEDED(ret)) {
    SQLSetEnvAttr(*sql_env_p, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
    emacs_value res = env->make_user_ptr(env, NULL, (void *) sql_env_p);
    return res;
  } else {
    return env->make_integer(env, ret);
  }
}


const char* odbc_alloc_env_doc =
  "Allocate an ODBC environment handle.\n";


emacs_value odbc_alloc_dbc(emacs_env *env,
                           ptrdiff_t nargs,
                           emacs_value args[],
                           void *data) EMACS_NOEXCEPT {
  SQLRETURN ret;
  SQLHENV sql_env;
  SQLHDBC *dbc_p;

  sql_env = *((SQLHENV *) env->get_user_ptr(env, args[0]));

  dbc_p = malloc(sizeof(SQLHDBC));
  ret = SQLAllocHandle(SQL_HANDLE_DBC, sql_env, dbc_p);

  if (SQL_SUCCEEDED(ret)) {
    emacs_value res = env->make_user_ptr(env, NULL, (void *) dbc_p);
    return res;
  } else {
    return env->make_integer(env, ret);
  }
}

const char* odbc_alloc_dbc_doc =
  "Allocate an ODBC database connection handle.\n";


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



void extract_error(char *fn,
                   SQLHANDLE handle,
                   SQLSMALLINT type)
{
  SQLINTEGER   i = 0;
  SQLINTEGER   native;
  SQLCHAR      state[ 7 ];
  SQLCHAR      text[256];
  SQLSMALLINT  len;
  SQLRETURN    ret;

  fprintf(stderr,
          "\n"
          "The driver reported the following diagnostics whilst running "
          "%s\n\n",
          fn);

  do {
    ret = SQLGetDiagRec(type, handle, ++i, state, &native, text,
                        sizeof(text), &len);
    if (SQL_SUCCEEDED(ret)) {
      printf("%s:%d:%d:%s\n", state, i, native, text);
    }
  } while( ret == SQL_SUCCESS );
}



emacs_value odbc_connect(emacs_env *env,
                         ptrdiff_t nargs,
                         emacs_value args[],
                         void *data) EMACS_NOEXCEPT {
  SQLHDBC dbc;
  SQLRETURN ret; /* ODBC API return status */
  SQLCHAR outstr[1024];
  SQLSMALLINT outstrlen;
  bool string_ret;
  char connection_string[256];
  ptrdiff_t size = 256;

  dbc = *((SQLHDBC *) env->get_user_ptr(env, args[0]));

  string_ret = env->copy_string_contents(env, args[1], connection_string, &size);
  if (!string_ret) {
    return nil(env);
  }

  /* Connect to the DSN mydsn */
  ret = SQLDriverConnect(dbc, NULL, (SQLCHAR *) connection_string, SQL_NTS,
                         outstr, sizeof(outstr), &outstrlen,
                         SQL_DRIVER_COMPLETE);
  if (SQL_SUCCEEDED(ret)) {
    printf("Connected\n");
    printf("Returned connection string was:\n\t%s\n", outstr);
    if (ret == SQL_SUCCESS_WITH_INFO) {
      printf("Driver reported the following diagnostics\n");
      extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
    }
  } else {
    fprintf(stderr, "Failed to connect\n");
    extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
  }

  return nil(env);
}


const char* odbc_connect_doc =
  "Make a plain ODBC connection.\n";


emacs_value odbc_alloc_stmt(emacs_env *env,
                            ptrdiff_t nargs,
                            emacs_value args[],
                            void *data) EMACS_NOEXCEPT {
  SQLRETURN ret;
  SQLHDBC dbc;
  SQLHSTMT *stmt_p;

  dbc = *((SQLHDBC *) env->get_user_ptr(env, args[0]));

  stmt_p = malloc(sizeof(SQLHSTMT));
  ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, stmt_p);

  if (SQL_SUCCEEDED(ret)) {
    emacs_value res = env->make_user_ptr(env, NULL, (void *) stmt_p);
    return res;
  } else {
    return env->make_integer(env, ret);
  }
}


const char* odbc_alloc_stmt_doc =
  "Allocate an ODBC statement handle for an active database connection.\n";


emacs_value odbc_tables(emacs_env *env,
                        ptrdiff_t nargs,
                        emacs_value args[],
                        void *data) EMACS_NOEXCEPT {
  SQLHSTMT *stmt_p;
  SQLHSTMT stmt_handle;
  SQLRETURN ret;

  stmt_p = (SQLHSTMT *) env->get_user_ptr(env, args[0]);
  stmt_handle = *stmt_p;

  ret = SQLTables(stmt_handle,
                  NULL, 0,  /* no specific catalog */
                  NULL, 0,  /* no specific schema */
                  NULL, 0,  /* no specific table */
                  NULL, 0); /* no specific type - table or view */

  return env->make_integer(env, ret);
}


const char* odbc_tables_doc =
  "Query for tables through ODBC statement handle.\n";


emacs_value odbc_exec_direct(emacs_env *env,
                             ptrdiff_t nargs,
                             emacs_value args[],
                             void *data) EMACS_NOEXCEPT {
  SQLHSTMT *stmt_p;
  SQLHSTMT stmt_handle;
  SQLRETURN ret;
  bool string_ret;
  char statement[256];
  ptrdiff_t size = 256;

  stmt_p = (SQLHSTMT *) env->get_user_ptr(env, args[0]);
  stmt_handle = *stmt_p;

  string_ret = env->copy_string_contents(env, args[1], statement, &size);
  if (!string_ret) {
    return nil(env);
  }

  ret = SQLExecDirect(stmt_handle, (SQLCHAR *) statement, SQL_NTS);

  return env->make_integer(env, ret);
}


const char* odbc_exec_direct_doc =
  "Execute statement directly.\n";


emacs_value odbc_fetch_results(emacs_env *env,
                               ptrdiff_t nargs,
                               emacs_value args[],
                               void *data) EMACS_NOEXCEPT {
  SQLHSTMT stmt_handle;
  SQLSMALLINT columns; /* number of columns in result-set */
  SQLRETURN ret;
  int row = 0;

  stmt_handle = *((SQLHSTMT *) env->get_user_ptr(env, args[0]));

  SQLNumResultCols(stmt_handle, &columns);

  /* Loop through the rows in the result-set */
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt_handle))) {
    SQLUSMALLINT i;
    printf("Row %d\n", row++);
    /* Loop through the columns */
    for (i = 1; i <= columns; i++) {
      long int indicator;
      char buf[512];
      /* retrieve column data as a string */
      ret = SQLGetData(stmt_handle, i, SQL_C_CHAR,
                       buf, sizeof(buf), &indicator);
      if (SQL_SUCCEEDED(ret)) {
        /* Handle null columns */
        if (indicator == SQL_NULL_DATA) strcpy(buf, "NULL");
        printf("  Column %u : %s\n", i, buf);
      }
    }
  }

  return env->make_integer(env, ret);
}


const char* odbc_fetch_results_doc =
  "Fetch results from ODBC statement handle.\n";


int emacs_module_init (struct emacs_runtime *runtime) {
  emacs_env *env = runtime->get_environment(runtime);

  define_function(env, "odbc-alloc-env", 0, 0, &odbc_alloc_env, odbc_alloc_env_doc);
  define_function(env, "odbc-alloc-dbc", 1, 1, &odbc_alloc_dbc, odbc_alloc_dbc_doc);
  define_function(env, "odbc-alloc-stmt", 1, 1, &odbc_alloc_stmt, odbc_alloc_stmt_doc);

  define_function(env, "odbc-drivers", 1, 1, &odbc_drivers, odbc_drivers_doc);
  define_function(env, "odbc-data-sources", 1, 1, &odbc_data_sources, odbc_data_sources_doc);
  define_function(env, "odbc-tables", 1, 1, &odbc_tables, odbc_tables_doc);
  define_function(env, "odbc-fetch-results", 1, 1, &odbc_fetch_results, odbc_fetch_results_doc);

  define_function(env, "odbc-connect", 2, 2, &odbc_connect, odbc_connect_doc);

  define_function(env, "odbc-exec-direct", 2, 2, &odbc_exec_direct, odbc_exec_direct_doc);

  provide(env, "emacs-odbc");
  return 0;
}
