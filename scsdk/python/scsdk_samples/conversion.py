import json
from standard_cyborg.proto.sc3d import image_pb2
from standard_cyborg.proto.sc3d import triangle_mesh_pb2
from standard_cyborg.proto.sc3d import polygon_label_pb2


def write_json(obj, path):
    """
    Write json file from python Object.
    :param obj: Object to write
    :param path: Path to write json file
    """
    with open(path, 'w') as fp:
        json.dump(obj, fp, indent=2)


def proto_to_jpg(path: str):
    """
    Convert image protobuf file to JPG format
    """

    # output path
    output_path = '{}.jpg'.format(path.replace('.proto', ''))

    # initialize empty pb
    pb_image_input = image_pb2.Image()

    # Read proto
    with open(path, 'rb') as f:
        pb_image_input.ParseFromString(f.read())

    # Write image jpg
    with open(output_path, 'wb') as f:
        f.write(pb_image_input.pixels.jpeg_bytes)

    return output_path


def proto_to_polygon(path: str):
    """
    Convert polygon protobuf file to json
    """

    # output path
    output_path = '{}.json'.format(path.replace('.proto', ''))

    # initialize empty pb
    proto_polygon_label = polygon_label_pb2.PolygonLabel()

    # Read proto
    with open(path, "rb") as f:
        proto_polygon_label.ParseFromString(f.read())
        f.close()

    coordinates = [[(position.x, position.y)
                        for position in sub_polygon.positions]
                            for sub_polygon in proto_polygon_label.subPolygons]

    write_json(coordinates, output_path)


def proto_to_obj(path: str):
    """
    Convert mesh protobuf file to OBJ format
    """
    # output path
    output_path = '{}.obj'.format(path.replace('.proto', ''))

    # initialize empty pb
    pb_mesh = triangle_mesh_pb2.TriangleMesh()

    # Read proto
    with open(path, "rb") as f:
        pb_mesh.ParseFromString(f.read())

    faces = pb_mesh.faces.int32_values
    vertices = pb_mesh.positions.float_values
    normals = pb_mesh.normals.float_values
    NV = int(len(vertices) / 3)
    NF = int(len(faces) / 3)

    # Write mesh obj
    with open(output_path, 'w') as f:
        f.write('g object\n')
        for iv in range(NV):
            x = vertices[iv * 3 + 0]
            y = vertices[iv * 3 + 1]
            z = vertices[iv * 3 + 2]
            f.write('v ' + str(x) + " " + str(y) + " " + str(z) + '\n')
        for iv in range(NV):
            nx = normals[iv * 3 + 0]
            ny = normals[iv * 3 + 1]
            nz = normals[iv * 3 + 2]
            f.write('vn ' + str(nx) + " " + str(ny) + " " + str(nz) + '\n')
        for iface in range(NF):
            i0 = str(faces[iface * 3 + 0] + 1)
            i1 = str(faces[iface * 3 + 1] + 1)
            i2 = str(faces[iface * 3 + 2] + 1)
            f.write('f ' + i0 + "//" + i0 + " " + i1 + "//" + i1 + " " + i2 + "//" + i2 + "\n")

    return output_path
