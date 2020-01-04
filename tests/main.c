/* SPDX-License-Identifier: MIT */

#include <stdio.h>
#include <setjmp.h>
#include "cmocka.h"

#include "simplecborrpc.h"

typedef enum {
    RPC_FUNCTION_PING = 0,
    RPC_FUNCTION_ECHO,
    RPC_FUNCTION_ALWAYS_ERROR,

    NUM_FUNCTION_HANDLES
} rpc_function_handle_t;

static rpc_call_result_t rpc_ping(const CborValue *args_iterator, CborEncoder *result, int64_t *error_code, const char **error_msg, void *user_ptr) {
    cbor_encode_text_stringz(result, "pong");

    return RPC_CALL_OK;
}

static rpc_call_result_t rpc_echo(const CborValue *args_iterator, CborEncoder *result, int64_t *error_code, const char **error_msg, void *user_ptr) {
    size_t string_length = 0;
    cbor_value_get_string_length(args_iterator, &string_length);
    if (string_length > 64) {
        *error_msg = "String too long";
        return RPC_CALL_ERROR_ARGUMENT_VALIDATION_FAILED;
    }

    char echobuf[64];
    size_t echobuflen = sizeof(echobuf);
    cbor_value_copy_text_string(args_iterator, echobuf, &echobuflen, NULL);
    cbor_encode_text_string(result, echobuf, echobuflen);

    return RPC_CALL_OK;
}

static rpc_call_result_t rpc_always_error(const CborValue *args_iterator, CborEncoder *result, int64_t *error_code, const char **error_msg, void *user_ptr) {

    *error_code = -3;
    *error_msg = "this is a test error";

    return RPC_CALL_INTERNAL_ERROR;
}

static const rpc_function_entry_t rpc_functions[NUM_FUNCTION_HANDLES] = {
        {"ping", rpc_ping,RPC_ARGS()},
        {"echo", rpc_echo,RPC_ARGS(CBOR_TYPE_TEXT_STRING)},
        {"always_error", rpc_always_error, RPC_ARGS()}
};

//////////////////////////////////

static void ping_test(void **state) {
    // request: {"v": 1, "id": 12, "func": "ping", "args":[]}
    uint8_t request[] = {0xA4, 0x61, 0x76, 0x01, 0x62,
                         0x69, 0x64, 0x0C, 0x64, 0x66,
                         0x75, 0x6E, 0x63, 0x64, 0x70,
                         0x69, 0x6E, 0x67, 0x64, 0x61,
                         0x72, 0x67, 0x73, 0x80};

    // response: {"v": 1, "id": 12, "res": "pong"}
    uint8_t expected_response[] = {0xA3, 0x61, 0x76, 0x01, 0x62,
                                   0x69, 0x64, 0x0C, 0x63, 0x72,
                                   0x65, 0x73, 0x64, 0x70, 0x6F,
                                   0x6E, 0x67};

    uint8_t response_buffer[512];
    memset(response_buffer, 0, sizeof(response_buffer));
    size_t response_size = sizeof(response_buffer);

    rpc_error_t err = execute_rpc_call(rpc_functions, NUM_FUNCTION_HANDLES, request, sizeof(request), response_buffer, &response_size, NULL);
    assert_true(err == RPC_OK);
    assert_int_equal(response_size, sizeof(expected_response));

    assert_memory_equal(expected_response, response_buffer, sizeof(expected_response));
}

static void echo_test(void **state) {
    // request: {"v": 1, "id": 12, "func": "echo", "args":["cake"]}
    uint8_t request[] = {0xA4, 0x61, 0x76, 0x01, 0x62,
                         0x69, 0x64, 0x0C, 0x64, 0x66,
                         0x75, 0x6E, 0x63, 0x64, 0x65,
                         0x63, 0x68, 0x6F, 0x64, 0x61,
                         0x72, 0x67, 0x73, 0x81, 0x64,
                         0x63, 0x61, 0x6B, 0x65};

    // response: {"v": 1, "id": 12, "res": "cake"}
    uint8_t expected_response[] = {0xA3, 0x61, 0x76, 0x01, 0x62,
                                   0x69, 0x64, 0x0C, 0x63, 0x72,
                                   0x65, 0x73, 0x64, 0x63, 0x61,
                                   0x6B, 0x65};

    uint8_t response_buffer[512];
    memset(response_buffer, 0, sizeof(response_buffer));
    size_t response_size = sizeof(response_buffer);

    rpc_error_t err = execute_rpc_call(rpc_functions, NUM_FUNCTION_HANDLES, request, sizeof(request), response_buffer, &response_size, NULL);
    assert_true(err == RPC_OK);
    assert_int_equal(response_size, sizeof(expected_response));

    assert_memory_equal(expected_response, response_buffer, response_size);
}

static void error_test(void **state) {
    // request: {"v": 1, "id": 12, "func": "always_error", "args":[]}
    uint8_t request[] = {0xA4, 0x61, 0x76, 0x01, 0x62,
                         0x69, 0x64, 0x0C, 0x64, 0x66,
                         0x75, 0x6E, 0x63, 0x6C, 0x61,
                         0x6C, 0x77, 0x61, 0x79, 0x73,
                         0x5F, 0x65, 0x72, 0x72, 0x6F,
                         0x72, 0x64, 0x61, 0x72, 0x67,
                         0x73, 0x80};

    // response: {"v": 1, "id": 12, "err": {"c": -3, "msg": "this is a test error"}}
    uint8_t expected_response[] = {0xA3, 0x61, 0x76, 0x01, 0x62,
                                   0x69, 0x64, 0x0C, 0x63, 0x65,
                                   0x72, 0x72, 0xA2, 0x61, 0x63,
                                   0x22, 0x63, 0x6D, 0x73, 0x67,
                                   0x74, 0x74, 0x68, 0x69, 0x73,
                                   0x20, 0x69, 0x73, 0x20, 0x61,
                                   0x20, 0x74, 0x65, 0x73, 0x74,
                                   0x20, 0x65, 0x72, 0x72, 0x6F,
                                   0x72};

    uint8_t response_buffer[512];
    memset(response_buffer, 0, sizeof(response_buffer));
    size_t response_size = sizeof(response_buffer);

    rpc_error_t err = execute_rpc_call(rpc_functions, NUM_FUNCTION_HANDLES, request, sizeof(request), response_buffer, &response_size, NULL);
    assert_true(err == RPC_OK);
    assert_int_equal(response_size, sizeof(expected_response));

    assert_memory_equal(expected_response, response_buffer, response_size);
}

static void method_not_found_test(void **state) {
    // request: {"v": 1, "id": 12, "func": "missing_function", "args":[]}
    uint8_t request[] = {0xA4, 0x61, 0x76, 0x01, 0x62,
                         0x69, 0x64, 0x0C, 0x64, 0x66,
                         0x75, 0x6E, 0x63, 0x70, 0x6D,
                         0x69, 0x73, 0x73, 0x69, 0x6E,
                         0x67, 0x5F, 0x66, 0x75, 0x6E,
                         0x63, 0x74, 0x69, 0x6F, 0x6E,
                         0x64, 0x61, 0x72, 0x67, 0x73,
                         0x80};

    // response: {"v": 1, "id": 12, "err":{"c": -32601, "msg": "Method not found"}}
    uint8_t expected_response[] = {0xA3, 0x61, 0x76, 0x01, 0x62,
                                   0x69, 0x64, 0x0C, 0x63, 0x65,
                                   0x72, 0x72, 0xA2, 0x61, 0x63,
                                   0x39, 0x7F, 0x58, 0x63, 0x6D,
                                   0x73, 0x67, 0x70, 0x4D, 0x65,
                                   0x74, 0x68, 0x6F, 0x64, 0x20,
                                   0x6E, 0x6F, 0x74, 0x20, 0x66,
                                   0x6F, 0x75, 0x6E, 0x64};

    uint8_t response_buffer[512];
    memset(response_buffer, 0, sizeof(response_buffer));
    size_t response_size = sizeof(response_buffer);

    rpc_error_t err = execute_rpc_call(rpc_functions, NUM_FUNCTION_HANDLES, request, sizeof(request), response_buffer, &response_size, NULL);
    assert_true(err == RPC_ERROR_METHOD_NOT_FOUND);
    assert_int_equal(response_size, sizeof(expected_response));

    assert_memory_equal(expected_response, response_buffer, response_size);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(ping_test),
        cmocka_unit_test(echo_test),
        cmocka_unit_test(error_test),
        cmocka_unit_test(method_not_found_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
