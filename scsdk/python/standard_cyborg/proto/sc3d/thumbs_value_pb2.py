# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: standard_cyborg/proto/sc3d/thumbs_value.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='standard_cyborg/proto/sc3d/thumbs_value.proto',
  package='standard_cyborg.proto.sc3d',
  syntax='proto3',
  serialized_options=None,
  serialized_pb=b'\n-standard_cyborg/proto/sc3d/thumbs_value.proto\x12\x1astandard_cyborg.proto.sc3d\"\x1c\n\x0bThumbsValue\x12\r\n\x05value\x18\x01 \x01(\x08\x62\x06proto3'
)




_THUMBSVALUE = _descriptor.Descriptor(
  name='ThumbsValue',
  full_name='standard_cyborg.proto.sc3d.ThumbsValue',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='value', full_name='standard_cyborg.proto.sc3d.ThumbsValue.value', index=0,
      number=1, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
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
  serialized_start=77,
  serialized_end=105,
)

DESCRIPTOR.message_types_by_name['ThumbsValue'] = _THUMBSVALUE
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

ThumbsValue = _reflection.GeneratedProtocolMessageType('ThumbsValue', (_message.Message,), {
  'DESCRIPTOR' : _THUMBSVALUE,
  '__module__' : 'standard_cyborg.proto.sc3d.thumbs_value_pb2'
  # @@protoc_insertion_point(class_scope:standard_cyborg.proto.sc3d.ThumbsValue)
  })
_sym_db.RegisterMessage(ThumbsValue)


# @@protoc_insertion_point(module_scope)
