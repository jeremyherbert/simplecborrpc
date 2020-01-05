from enum import Enum, auto, unique
import os
import jinja2
from perfect_hash import generate_hash, Hash2

@unique
class CborTypes(Enum):
    CBOR_TYPE_NULL = auto()
    CBOR_TYPE_BOOL = auto()
    CBOR_TYPE_SIMPLE = auto()
    CBOR_TYPE_SIGNED_INTEGER = auto()
    CBOR_TYPE_UNSIGNED_INTEGER = auto()
    CBOR_TYPE_NEGATIVE_INTEGER = auto()
    CBOR_TYPE_HALF_FLOAT = auto()
    CBOR_TYPE_FLOAT = auto()
    CBOR_TYPE_DOUBLE = auto()
    CBOR_TYPE_TEXT_STRING = auto()
    CBOR_TYPE_BYTE_STRING = auto()
    CBOR_TYPE_ARRAY = auto()
    CBOR_TYPE_MAP = auto()


def split_hex(num: int):
    output = []
    while num:
        output.append(num & 0xFF)
        num >>= 8

    return output


def generate_api(path, rpc_table):
    current_path = os.path.dirname(os.path.realpath(__file__))

    key_index = list(zip(rpc_table.keys(), range(len(rpc_table))))
    f1, f2, G = generate_hash(dict(key_index), Hash2)

    env = jinja2.Environment(loader=jinja2.FileSystemLoader(current_path), trim_blocks=True,block_start_string='@@',block_end_string='@@',variable_start_string='@=', variable_end_string='=@')

    c_template = env.get_template("rpc_api.c.jinja2")
    h_template = env.get_template("rpc_api.h.jinja2")

    rpc_functions = []
    for key, index in key_index:
        val = rpc_table[key]
        tmp = [key, ', '.join(x.name for x in val)]
        rpc_functions.append(tmp)

    template_args = {
        'rpc_functions': rpc_functions,
        'salt1': ', '.join("0x{:02X}".format(x) for x in f1.salt),
        'salt2': ', '.join("0x{:02X}".format(x) for x in f2.salt),
        'graph': ', '.join(str(x) for x in G),
        'keys': ', '.join('"{}"'.format(x) for x, _ in key_index)
    }

    with open(os.path.join(path, "rpc_api.c"), 'w') as f:
        f.write(c_template.render(**template_args))

    with open(os.path.join(path, "rpc_api.h"), 'w') as f:
        f.write(h_template.render(**template_args))
