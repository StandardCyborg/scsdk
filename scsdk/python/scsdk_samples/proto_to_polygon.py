"""
Convert Proto Polygon Label to json
    $ python proto_to_polygon.py --proto /path/to/polygon.proto

Output will be /path/to/polygon.json

"""
from conversion import proto_to_polygon


def parse_args():
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--proto',
                        required=True,
                        help='Path to polygon proto')

    return parser.parse_args()


def main(args):
    proto_to_polygon(args.proto)


if __name__ == '__main__':
    _args = parse_args()
    print('args: {}'.format(_args))
    main(_args)
