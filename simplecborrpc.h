/* SPDX-License-Identifier: MIT */

#ifndef SIMPLECBORRPC_SIMPLECBORRPC_H
#define SIMPLECBORRPC_SIMPLECBORRPC_H

#include <stddef.h>
#include "cbor.h"

typedef enum {
    CBOR_TYPE_NULL = 0,
    CBOR_TYPE_BOOL,
    CBOR_TYPE_SIMPLE,

    CBOR_TYPE_SIGNED_INTEGER,
    CBOR_TYPE_UNSIGNED_INTEGER,
    CBOR_TYPE_NEGATIVE_INTEGER,

    CBOR_TYPE_HALF_FLOAT,
    CBOR_TYPE_FLOAT,
    CBOR_TYPE_DOUBLE,

    CBOR_TYPE_TEXT_STRING,
    CBOR_TYPE_BYTE_STRING,

    CBOR_TYPE_ARRAY,
    CBOR_TYPE_MAP
} rpc_argument_type_t;

typedef enum {
    RPC_CALL_OK = 0,

    RPC_CALL_ERROR_FORCE_METHOD_NOT_FOUND,
    RPC_CALL_ERROR_ARGUMENT_VALIDATION_FAILED,

    RPC_CALL_INTERNAL_ERROR
} rpc_call_result_t;

typedef rpc_call_result_t (*rpc_function_t)(const CborValue *args_iterator, CborEncoder *result, int64_t *error_code, const char **error_msg, void *user_ptr);

typedef struct {
    const char *name;

    const rpc_function_t function_ptr;

    const rpc_argument_type_t *argument_types;
    const size_t number_of_arguments;
} rpc_function_entry_t;

typedef enum {
    RPC_OK = 0,
    RPC_ERROR_FAILED_CBOR_PARSER_INIT,

    RPC_ERROR_PARSER_FAILED,
    RPC_ERROR_UNEXPECTED_KEY_IN_REQUEST,

    RPC_ERROR_PARSE_ERROR = -32700,
    RPC_ERROR_INVALID_REQUEST = -32600,
    RPC_ERROR_METHOD_NOT_FOUND = -32601,
    RPC_ERROR_INVALID_ARGS = -32602
} rpc_error_t;


#define RPC_ARGS(...) (rpc_argument_type_t[]){ __VA_ARGS__ }, sizeof((rpc_argument_type_t[]) { __VA_ARGS__ })/sizeof(rpc_argument_type_t)

rpc_error_t execute_rpc_call(const rpc_function_entry_t *rpc_functions, size_t rpc_functions_count, const uint8_t *input_buffer,
                             size_t input_buffer_size, uint8_t *output_buffer, size_t *output_buffer_size,
                             void *user_ptr);

#endif //SIMPLECBORRPC_SIMPLECBORRPC_H
