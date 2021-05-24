// https://nodejs.org/api/addons.html
// https://github.com/JaneaSystems/nodejs-mobile-cordova/blob/unstable/src/common/cordova-bridge/cordova-bridge.cpp

#include <string>
#include "node_api.h"
#include "uv.h"

#define NM_F_BUILTIN 0x1
#define NM_F_LINKED 0x2

#define GET_AND_THROW_LAST_ERROR(env)                                    \
  do {                                                                   \
    const napi_extended_error_info *error_info;                          \
    napi_get_last_error_info((env), &error_info);                        \
    bool is_pending;                                                     \
    napi_is_exception_pending((env), &is_pending);                       \
    /* If an exception is already pending, don't rethrow it */           \
    if (!is_pending) {                                                   \
      const char* error_message = error_info->error_message != NULL ?    \
        error_info->error_message :                                      \
        "empty error message";                                           \
      napi_throw_error((env), NULL, error_message);                      \
    }                                                                    \
  } while (0)

#define NAPI_CALL_BASE(env, the_call, ret_val)                           \
  do {                                                                   \
    if ((the_call) != napi_ok) {                                         \
      GET_AND_THROW_LAST_ERROR((env));                                   \
      return ret_val;                                                    \
    }                                                                    \
  } while (0)

// Returns NULL if the_call doesn't return napi_ok.
#define NAPI_CALL(env, the_call)                                         \
  NAPI_CALL_BASE(env, the_call, NULL)

// Returns empty if the_call doesn't return napi_ok.
#define NAPI_CALL_RETURN_VOID(env, the_call)                             \
  NAPI_CALL_BASE(env, the_call, NAPI_RETVAL_NOTHING)

#define NAPI_ASSERT_BASE(env, assertion, message, ret_val)               \
  do {                                                                   \
    if (!(assertion)) {                                                  \
      napi_throw_error(                                                  \
          (env),                                                         \
          NULL,                                                          \
          "assertion (" #assertion ") failed: " message);                \
      return ret_val;                                                    \
    }                                                                    \
  } while (0)

// Returns NULL on failed assertion.
// This is meant to be used inside napi_callback methods.
#define NAPI_ASSERT(env, assertion, message)                             \
  NAPI_ASSERT_BASE(env, assertion, message, NULL)

napi_value Method_HelloWorld(napi_env env, napi_callback_info info) {
    // NAPI_ASSERT(env, Method_GetDataDir!=NULL, "Data directory not set from native side.");
    napi_value hello_world;
    const char* msg = "Hello World";
    size_t str_len = strlen(msg);
    NAPI_CALL(env, napi_create_string_utf8(env, msg, str_len, &hello_world));
    return hello_world;
}

napi_value Method_Add_Ints(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[argc];

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));
    NAPI_ASSERT(env, argc == 2, "Wrong number of arguments.");

    // args[0] is value 1
    napi_value arg0_val1 = args[0];
    napi_valuetype valuetype0;
    NAPI_CALL(env, napi_typeof(env, arg0_val1, &valuetype0));
    NAPI_ASSERT(env, valuetype0 == napi_number, "Expected a number (arg1).");

    int64_t val1;
    NAPI_CALL(env, napi_get_value_int64(env, arg0_val1, &val1));

    // args[1] is value 2
    napi_value arg1_val2 = args[1];
    napi_valuetype valuetype1;
    NAPI_CALL(env, napi_typeof(env, arg1_val2, &valuetype1));
    NAPI_ASSERT(env, valuetype0 == napi_number, "Expected a number (arg2).");

    int64_t val2;
    NAPI_CALL(env, napi_get_value_int64(env, arg1_val2, &val2));

    // NAPI_ASSERT(env, Method_GetDataDir!=NULL, "Data directory not set from native side.");
    int64_t result = val1 + val2;

    napi_value addInts;
    NAPI_CALL(env, napi_create_int64(env, result, &addInts));
    return addInts;
}

#define DECLARE_NAPI_METHOD(name, func) { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
    napi_status status;
    napi_property_descriptor properties[] = {
            DECLARE_NAPI_METHOD("helloWorld", Method_HelloWorld),
            DECLARE_NAPI_METHOD("addInts", Method_Add_Ints)
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(*properties), properties));
    return exports;
}


// Register the native module at libnode startup
NAPI_MODULE_X(sbgck_bridge, Init, NULL, NM_F_LINKED)