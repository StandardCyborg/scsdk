"""
Convert Proto Image to JPG
    $ python proto_to_jpg.py --proto /path/to/image.proto

Output will be /path/to/image.jpg

"""
from conversion import proto_to_jpg


def parse_args():
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--proto',
                        required=True,
                        help='Path to image proto')

    return parser.parse_args()


def main(args):
    proto_to_jpg(args.proto)


if __name__ == '__main__':
    _args = parse_args()
    print('args: {}'.format(_args))
    main(_args)
