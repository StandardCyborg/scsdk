# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: standard_cyborg/proto/sc3d/point_cloud.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from standard_cyborg.proto.math import tensor_pb2 as standard__cyborg_dot_proto_dot_math_dot_tensor__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='standard_cyborg/proto/sc3d/point_cloud.proto',
  package='standard_cyborg.proto.sc3d',
  syntax='proto3',
  serialized_options=None,
  serialized_pb=b'\n,standard_cyborg/proto/sc3d/point_cloud.proto\x12\x1astandard_cyborg.proto.sc3d\x1a\'standard_cyborg/proto/math/tensor.proto\"\xbb\x01\n\nPointCloud\x12\r\n\x05\x66rame\x18\x01 \x01(\t\x12\x35\n\tpositions\x18\x02 \x01(\x0b\x32\".standard_cyborg.proto.math.Tensor\x12\x33\n\x07normals\x18\x03 \x01(\x0b\x32\".standard_cyborg.proto.math.Tensor\x12\x32\n\x06\x63olors\x18\x04 \x01(\x0b\x32\".standard_cyborg.proto.math.Tensorb\x06proto3'
  ,
  dependencies=[standard__cyborg_dot_proto_dot_math_dot_tensor__pb2.DESCRIPTOR,])




_POINTCLOUD = _descriptor.Descriptor(
  name='PointCloud',
  full_name='standard_cyborg.proto.sc3d.PointCloud',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='frame', full_name='standard_cyborg.proto.sc3d.PointCloud.frame', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='positions', full_name='standard_cyborg.proto.sc3d.PointCloud.positions', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='normals', full_name='standard_cyborg.proto.sc3d.PointCloud.normals', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='colors', full_name='standard_cyborg.proto.sc3d.PointCloud.colors', index=3,
      number=4, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=118,
  serialized_end=305,
)

_POINTCLOUD.fields_by_name['positions'].message_type = standard__cyborg_dot_proto_dot_math_dot_tensor__pb2._TENSOR
_POINTCLOUD.fields_by_name['normals'].message_type = standard__cyborg_dot_proto_dot_math_dot_tensor__pb2._TENSOR
_POINTCLOUD.fields_by_name['colors'].message_type = standard__cyborg_dot_proto_dot_math_dot_tensor__pb2._TENSOR
DESCRIPTOR.message_types_by_name['PointCloud'] = _POINTCLOUD
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

PointCloud = _reflection.GeneratedProtocolMessageType('PointCloud', (_message.Message,), {
  'DESCRIPTOR' : _POINTCLOUD,
  '__module__' : 'standard_cyborg.proto.sc3d.point_cloud_pb2'
  # @@protoc_insertion_point(class_scope:standard_cyborg.proto.sc3d.PointCloud)
  })
_sym_db.RegisterMessage(PointCloud)


# @@protoc_insertion_point(module_scope)
