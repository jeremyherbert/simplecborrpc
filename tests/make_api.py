import os
from api_gen import generate_api, CborTypes

current_path = os.path.dirname(os.path.realpath(__file__))

generate_api(current_path, {
    "ping": [],
    "echo": [CborTypes.CBOR_TYPE_TEXT_STRING],
    "always_error": []
})