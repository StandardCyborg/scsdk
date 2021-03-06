# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: standard_cyborg/proto/sc3d/label_pair.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from standard_cyborg.proto.math import core_pb2 as standard__cyborg_dot_proto_dot_math_dot_core__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='standard_cyborg/proto/sc3d/label_pair.proto',
  package='standard_cyborg.proto.sc3d',
  syntax='proto3',
  serialized_options=None,
  serialized_pb=b'\n+standard_cyborg/proto/sc3d/label_pair.proto\x12\x1astandard_cyborg.proto.sc3d\x1a%standard_cyborg/proto/math/core.proto\"w\n\tLabelPair\x12\x34\n\nposition2D\x18\x01 \x01(\x0b\x32 .standard_cyborg.proto.math.Vec2\x12\x34\n\nposition3D\x18\x02 \x01(\x0b\x32 .standard_cyborg.proto.math.Vec3b\x06proto3'
  ,
  dependencies=[standard__cyborg_dot_proto_dot_math_dot_core__pb2.DESCRIPTOR,])




_LABELPAIR = _descriptor.Descriptor(
  name='LabelPair',
  full_name='standard_cyborg.proto.sc3d.LabelPair',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='position2D', full_name='standard_cyborg.proto.sc3d.LabelPair.position2D', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='position3D', full_name='standard_cyborg.proto.sc3d.LabelPair.position3D', index=1,
      number=2, type=11, cpp_type=10, label=1,
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
  serialized_start=114,
  serialized_end=233,
)

_LABELPAIR.fields_by_name['position2D'].message_type = standard__cyborg_dot_proto_dot_math_dot_core__pb2._VEC2
_LABELPAIR.fields_by_name['position3D'].message_type = standard__cyborg_dot_proto_dot_math_dot_core__pb2._VEC3
DESCRIPTOR.message_types_by_name['LabelPair'] = _LABELPAIR
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

LabelPair = _reflection.GeneratedProtocolMessageType('LabelPair', (_message.Message,), {
  'DESCRIPTOR' : _LABELPAIR,
  '__module__' : 'standard_cyborg.proto.sc3d.label_pair_pb2'
  # @@protoc_insertion_point(class_scope:standard_cyborg.proto.sc3d.LabelPair)
  })
_sym_db.RegisterMessage(LabelPair)


# @@protoc_insertion_point(module_scope)
