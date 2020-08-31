# scsdk

### **Open-source cross-platform data structures, serialization, and algorithms for expressing, storing, and processing spatial data.**

SCSDK is a cross-platform package that provides fast, flexible, and expressive data structures designed to make working with spatial data both easy and intuitive. It aims to be the fundamental high-level building block for solving computer vision and perception problems. SCSDK also provides non-library dependent data structures through our protobuf definitions. Lastly SCSDK aims to have a robust catalog of core processing algorithms for spatial data.

The core SDK is written in highly performant C++ with minimal dependencies.

`Status:` This library is early in its development but has a solid well tested foundation.

## Supports:
- C++ - [Get started with C++](LibStandardCyborg/c++/README.md)
- Python (tested on Mac and Unix/Linux) - [Get started with Python](LibStandardCyborg/python/README.md)
- Javascript (Node/browser) - *Coming Soon*

## Example
For example, you can capture a stream of images on iOS, open them in the browser to view, and then easily fetch them via python and load them into a [pandas dataframe](https://pandas.pydata.org/pandas-docs/stable/reference/api/pandas.DataFrame.html). You can do this across not just trivial data like images, but also meshes, point clouds, accelerometer data and more.
[more example scripts](LibStandardCyborg/python/scsdk_samples)

## Motivations and Goals
- Have a consistent set of high-performant data structures and fast IO for working with spatial data across platforms
- Have very few dependencies and reliable build instructions for easy breezy usage
- Easy interoperability with other data structures: for example tfrecords, COCO, pascal VOC, etc. 
- Easy interoperability with other geometry processing libraries: for example libigl
- Open format and intrinsically cross-platform data structures that do not require the library using protobuf messages

If you need an archive compressed format for at-rest storage or moving things around, we suggest checking out [protobag](https://github.com/StandardCyborg/protobag)

## Use cases:
- Static scenes of data from a single sensor
- Static scenes of data from multiple inputs/sensors
- Dynamic time-series data from one or more sensors

## Data types
- RGB-D: Images and depth frames
- Primitives: Points, polylines, planes, bounding boxes, 
- Cameras
- 3D assets: point clouds and meshes
- Scene graph

## Documentation
While documentation is early compared to our goals for the project, you can find example code as well as header comments throughout the project. 

## Getting help
We intend to use Github Discussions, Gitter and other tools, but please just use Issues for now. 

## Contributions
Work on `scsdk` started at Standard Cyborg in 2018 and has been under active development since then. We welcome contributions though we don't have a contribution guide yet.

## License
[Apache License 2.0]()
