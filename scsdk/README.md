# scsdk

There are a few subdirectories 

- *c++/* The C++ source code of our Core SDK. It is included in the open source release for documentation purposes, and for
allowing outside developers to fix minor bugs they come across. But the C++ SDK is not the official recommended and supported way of using our SDK

- *python/* Our python language bindings around our C++ core SDK. This is the officially recommended way of using our SDK.

- *proto/* Protobuf message definitions that are used throughout our SDK, for data exchange purposes. 

Before even trying to run anything in the below directories, please do the below, in the root of the repo. 

```
$ git submodule update --init
$ git lfs install && git lfs fetch && git lfs checkout
```