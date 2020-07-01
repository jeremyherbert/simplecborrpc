from enum import Enum, auto, unique
import os
import jinja2
from perfect_hash import generate_hash, Hash2


@unique
class CborTypes(Enum):
    CBOR_TYPE_UNDEFINED = 0
    CBOR_TYPE_NULL = 1
    CBOR_TYPE_BOOL = 2
    CBOR_TYPE_SIMPLE = 3
    CBOR_TYPE_SIGNED_INTEGER = 4
    CBOR_TYPE_UNSIGNED_INTEGER = 5
    CBOR_TYPE_NEGATIVE_INTEGER = 6
    CBOR_TYPE_HALF_FLOAT = 7
    CBOR_TYPE_FLOAT = 8
    CBOR_TYPE_DOUBLE = 9
    CBOR_TYPE_TEXT_STRING = 10
    CBOR_TYPE_BYTE_STRING = 11
    CBOR_TYPE_ARRAY = 12
    CBOR_TYPE_MAP = 13


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
        f.write("\n")  # add a new line for pedantic warnings

    with open(os.path.join(path, "rpc_api.h"), 'w') as f:
        f.write(h_template.render(**template_args))
        f.write("\n")
