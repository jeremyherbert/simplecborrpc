#include <stddef.h>
#include "cbor.h"
#include "simplecborrpc.h"

rpc_error_t
rpc___funcs(const CborValue *args_iterator, CborEncoder *result, const char **error_msg, void *user_ptr) {
    size_t count = 0;
    for (size_t i=0; i<rpc_get_key_count(); i++) {
        const char *key = rpc_lookup_key_by_index(i);
        if (key == NULL || key[0] == '_') continue;
        count++;
    }

    CborEncoder map_encoder;
    cbor_encoder_create_map(result, &map_encoder, count);

    for (size_t i=0; i<rpc_get_key_count(); i++) {
        const char *key = rpc_lookup_key_by_index(i);
        if (key == NULL || key[0] == '_') continue;

        cbor_encode_text_stringz(&map_encoder, key);
        cbor_encode_int(&map_encoder, i);
    }

    if (cbor_encoder_close_container(result, &map_encoder) != CborNoError) {
        return RPC_ERROR_ENCODE_ERROR;
    }

    return RPC_OK;
}

rpc_error_t
rpc___ping(const CborValue *args_iterator, CborEncoder *result, const char **error_msg, void *user_ptr) {
    cbor_encode_text_stringz(result, "pong");

    return RPC_OK;
}

rpc_error_t
rpc___version(const CborValue *args_iterator, CborEncoder *result, const char **error_msg, void *user_ptr) {
    cbor_encode_int(result, 1);

    return RPC_OK;
}
