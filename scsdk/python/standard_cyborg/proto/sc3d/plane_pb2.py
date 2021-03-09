# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: standard_cyborg/proto/sc3d/plane.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from standard_cyborg.proto.math import core_pb2 as standard__cyborg_dot_proto_dot_math_dot_core__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='standard_cyborg/proto/sc3d/plane.proto',
  package='standard_cyborg.proto.sc3d',
  syntax='proto3',
  serialized_options=None,
  serialized_pb=b'\n&standard_cyborg/proto/sc3d/plane.proto\x12\x1astandard_cyborg.proto.sc3d\x1a%standard_cyborg/proto/math/core.proto\"|\n\x05Plane\x12\r\n\x05\x66rame\x18\x01 \x01(\t\x12\x32\n\x08position\x18\x02 \x01(\x0b\x32 .standard_cyborg.proto.math.Vec3\x12\x30\n\x06normal\x18\x03 \x01(\x0b\x32 .standard_cyborg.proto.math.Vec3b\x06proto3'
  ,
  dependencies=[standard__cyborg_dot_proto_dot_math_dot_core__pb2.DESCRIPTOR,])




_PLANE = _descriptor.Descriptor(
  name='Plane',
  full_name='standard_cyborg.proto.sc3d.Plane',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='frame', full_name='standard_cyborg.proto.sc3d.Plane.frame', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='position', full_name='standard_cyborg.proto.sc3d.Plane.position', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='normal', full_name='standard_cyborg.proto.sc3d.Plane.normal', index=2,
      number=3, type=11, cpp_type=10, label=1,
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
  serialized_start=109,
  serialized_end=233,
)

_PLANE.fields_by_name['position'].message_type = standard__cyborg_dot_proto_dot_math_dot_core__pb2._VEC3
_PLANE.fields_by_name['normal'].message_type = standard__cyborg_dot_proto_dot_math_dot_core__pb2._VEC3
DESCRIPTOR.message_types_by_name['Plane'] = _PLANE
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

Plane = _reflection.GeneratedProtocolMessageType('Plane', (_message.Message,), {
  'DESCRIPTOR' : _PLANE,
  '__module__' : 'standard_cyborg.proto.sc3d.plane_pb2'
  # @@protoc_insertion_point(class_scope:standard_cyborg.proto.sc3d.Plane)
  })
_sym_db.RegisterMessage(Plane)


# @@protoc_insertion_point(module_scope)