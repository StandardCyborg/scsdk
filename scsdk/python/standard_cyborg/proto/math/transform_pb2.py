# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: standard_cyborg/proto/math/transform.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from standard_cyborg.proto.math import core_pb2 as standard__cyborg_dot_proto_dot_math_dot_core__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='standard_cyborg/proto/math/transform.proto',
  package='standard_cyborg.proto.math',
  syntax='proto3',
  serialized_options=None,
  serialized_pb=b'\n*standard_cyborg/proto/math/transform.proto\x12\x1astandard_cyborg.proto.math\x1a%standard_cyborg/proto/math/core.proto\"\x85\x02\n\tTransform\x12\x38\n\x08rotation\x18\x01 \x01(\x0b\x32&.standard_cyborg.proto.math.Quaternion\x12\x35\n\x0btranslation\x18\x02 \x01(\x0b\x32 .standard_cyborg.proto.math.Vec3\x12/\n\x05scale\x18\x05 \x01(\x0b\x32 .standard_cyborg.proto.math.Vec3\x12/\n\x05shear\x18\x06 \x01(\x0b\x32 .standard_cyborg.proto.math.Vec3\x12\x11\n\tsrc_frame\x18\x03 \x01(\t\x12\x12\n\ndest_frame\x18\x04 \x01(\tb\x06proto3'
  ,
  dependencies=[standard__cyborg_dot_proto_dot_math_dot_core__pb2.DESCRIPTOR,])




_TRANSFORM = _descriptor.Descriptor(
  name='Transform',
  full_name='standard_cyborg.proto.math.Transform',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='rotation', full_name='standard_cyborg.proto.math.Transform.rotation', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='translation', full_name='standard_cyborg.proto.math.Transform.translation', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='scale', full_name='standard_cyborg.proto.math.Transform.scale', index=2,
      number=5, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='shear', full_name='standard_cyborg.proto.math.Transform.shear', index=3,
      number=6, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='src_frame', full_name='standard_cyborg.proto.math.Transform.src_frame', index=4,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='dest_frame', full_name='standard_cyborg.proto.math.Transform.dest_frame', index=5,
      number=4, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
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
  serialized_end=375,
)

_TRANSFORM.fields_by_name['rotation'].message_type = standard__cyborg_dot_proto_dot_math_dot_core__pb2._QUATERNION
_TRANSFORM.fields_by_name['translation'].message_type = standard__cyborg_dot_proto_dot_math_dot_core__pb2._VEC3
_TRANSFORM.fields_by_name['scale'].message_type = standard__cyborg_dot_proto_dot_math_dot_core__pb2._VEC3
_TRANSFORM.fields_by_name['shear'].message_type = standard__cyborg_dot_proto_dot_math_dot_core__pb2._VEC3
DESCRIPTOR.message_types_by_name['Transform'] = _TRANSFORM
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

Transform = _reflection.GeneratedProtocolMessageType('Transform', (_message.Message,), {
  'DESCRIPTOR' : _TRANSFORM,
  '__module__' : 'standard_cyborg.proto.math.transform_pb2'
  # @@protoc_insertion_point(class_scope:standard_cyborg.proto.math.Transform)
  })
_sym_db.RegisterMessage(Transform)


# @@protoc_insertion_point(module_scope)
