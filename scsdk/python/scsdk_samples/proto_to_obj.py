"""
Convert Proto Mesh to OBJ
    $ python proto_to_obj.py --proto /path/to/mesh.proto

Output will be /path/to/mesh.obj

"""
from conversion import proto_to_obj


def parse_args():
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--proto',
                        required=True,
                        help='Path to obj proto')

    return parser.parse_args()


def main(args):
    proto_to_obj(args.proto)


if __name__ == '__main__':
    _args = parse_args()
    print('args: {}'.format(_args))
    main(_args)
