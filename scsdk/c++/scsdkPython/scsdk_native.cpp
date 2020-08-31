/*
Copyright 2020 Standard Cyborg

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <limits>
#include <sstream>

#include <fmt/format.h>

#include "standard_cyborg/util/Pybind11Defs.hpp"

#include "standard_cyborg/sc3d/Geometry.hpp"
#include "standard_cyborg/sc3d/Polyline.hpp"
#include "standard_cyborg/sc3d/PerspectiveCamera.hpp"
#include "standard_cyborg/sc3d/ColorImage.hpp"
#include "standard_cyborg/sc3d/DepthImage.hpp"

#include "standard_cyborg/io/ply/RawFrameDataIO_PLY.hpp"
#include "standard_cyborg/io/ply/GeometryFileIO_PLY.hpp"

#include "standard_cyborg/io/gltf/SceneGraphFileIO_GLTF.hpp"

#include "standard_cyborg/scene_graph/SceneGraph.hpp"

#include "standard_cyborg/io/pbio/ProtobagIO.hpp"
#include "standard_cyborg/util/DebugHelpers.hpp"



using namespace standard_cyborg;
namespace sg = standard_cyborg::scene_graph;
namespace py = pybind11;

auto a = (bool (*)(std::shared_ptr<sg::Node> sceneGraph, const std::string& path)) &io::ply::WriteGeometryToPLYFile;

NPFloat Vec3sToNpFloat(const std::vector<math::Vec3>& vectors) {
   std::vector<float> floats;
   for (int i = 0; i < vectors.size(); i++) {
      floats.push_back(vectors[i].x);
      floats.push_back(vectors[i].y);
      floats.push_back(vectors[i].z);
   }       

   return NPFloat(std::vector<ptrdiff_t>{(ptrdiff_t)vectors.size(), 3}, &floats[0]);
}

NPFloat Vec3ToNpFloat(const math::Vec3& v) {
   std::vector<float> floats = {
      v.x, v.y, v.z
   };
   return NPFloat(std::vector<ptrdiff_t>{3}, &floats[0]);
}

NPFloat Vec2sToNpFloat(const std::vector<math::Vec2>& vectors) {
   std::vector<float> floats;
   for (int i = 0; i < vectors.size(); i++) {
      floats.push_back(vectors[i].x);
      floats.push_back(vectors[i].y);
   }       

   return NPFloat(std::vector<ptrdiff_t>{(ptrdiff_t)vectors.size(), 2}, &floats[0]);
}

NPFloat Vec2ToNpFloat(const math::Vec2& v) {
   std::vector<float> floats(2);
   floats[0] = v.x; floats[1] = v.y;
   return NPFloat(std::vector<ptrdiff_t>{2}, &floats[0]);
}

NPFloat Mat3x3ToNpFloat(const math::Mat3x3& m) {
   std::vector<float> floats = {
      m.m00, m.m01, m.m02,
      m.m10, m.m11, m.m12,
      m.m20, m.m21, m.m22,
   };
   return NPFloat(std::vector<ptrdiff_t>{3, 3}, &floats[0]);
}

NPFloat Mat3x4ToNpFloat(const math::Mat3x4& m) {
   std::vector<float> floats = {
      m.m00, m.m01, m.m02, m.m03,
      m.m10, m.m11, m.m12, m.m13,
      m.m20, m.m21, m.m22, m.m23,
   };
   return NPFloat(std::vector<ptrdiff_t>{3, 4}, &floats[0]);
}

NPFloat Mat4x4ToNpFloat(const math::Mat4x4& m) {
   std::vector<float> floats = {
      m.m00, m.m01, m.m02, m.m03,
      m.m10, m.m11, m.m12, m.m13,
      m.m20, m.m21, m.m22, m.m23,
      m.m30, m.m31, m.m32, m.m33,
   };
   return NPFloat(std::vector<ptrdiff_t>{4, 4}, &floats[0]);
}

NPInt Size2DToNpInt(const sc3d::Size2D &s) {
   std::vector<int> ints = {s.width, s.height};
   return NPInt(std::vector<ptrdiff_t>{2}, &ints[0]);
}

std::vector<math::Vec3> NpFloatToVec3s(const NPFloat& positions) {
   auto r = positions.unchecked<2>();
   std::vector<math::Vec3> vectors;
   for (int i = 0; i < positions.shape(0); i++) {
      vectors.push_back(math::Vec3(r(i, 0), r(i, 1), r(i, 2)));
   }     
   return vectors;
}

std::vector<math::Vec2> NpFloatToVec2s(const NPFloat& positions) {
   auto r = positions.unchecked<2>();
   std::vector<math::Vec2> vectors;
   for (int i = 0; i < positions.shape(0); i++) {
      vectors.push_back(math::Vec2(r(i, 0), r(i, 1)));
   }     
   return vectors;
}

math::Mat3x4 NpFloatToMat3x4(const NPFloat& vs) {
   auto r = vs.unchecked<2>();
   math::Mat3x4 m(
      r(0, 0), r(0, 1), r(0, 2), r(0, 3),
      r(1, 0), r(1, 1), r(1, 2), r(1, 3),
      r(2, 0), r(2, 1), r(2, 2), r(2, 3));
   return m;
}

math::Mat3x3 NpFloatToMat3x3(const NPFloat& vs) {
   auto r = vs.unchecked<2>();
   math::Mat3x3 m(
      r(0, 0), r(0, 1), r(0, 2),
      r(1, 0), r(1, 1), r(1, 2),
      r(2, 0), r(2, 1), r(2, 2));
   return m;
}

NPInt FacesToNpInt(const std::vector<sc3d::Face3>& faces) {
   std::vector<int> ints;
   for (int i = 0; i < faces.size(); i++) {
      ints.push_back(faces[i][0]);
      ints.push_back(faces[i][1]);
      ints.push_back(faces[i][2]);
   }       
   return NPInt(std::vector<ptrdiff_t>{(ptrdiff_t)faces.size(), 3}, &ints[0]);
}

std::vector<sc3d::Face3> NpIntToFaces(const NPInt& ints) {
   auto r = ints.unchecked<2>();
   std::vector<sc3d::Face3> faces;
   for (int i = 0; i < ints.shape(0); i++) {
      faces.push_back(sc3d::Face3(r(i, 0), r(i, 1), r(i, 2)));
   }     
   return faces;
}

template <typename T>
std::string ToString(const T &v) {
   std::stringstream ss;
   ss << v; // Uses DebugHelpers
   return ss.str();
}

inline std::shared_ptr<sg::Node>
IterSGGetNext(io::pbio::IterSceneGraphsFromProtobag &v) {
   auto maybeNext = v.GetNext();
   if (maybeNext.IsEndOfSequence()) {
      throw pybind11::stop_iteration();
   } else if (!maybeNext.IsOk()) {
      throw std::runtime_error(maybeNext.error);
   } else {
      return *maybeNext.value;
   }
}

PYBIND11_MODULE(scsdk_native, m) {
   m.doc() = "StandardCyborgSDK Python wrappers";

   py::class_<sc3d::Geometry>(m, "Geometry", R"foo(This class contains either a triangle mesh if faces are present or a point cloud otherwise. Each vertex can have the attributes
   
       - positions
       - normals
       - colors
       - texCoords
       
       If not set, these attributes are assumed empty. 
       If a Geometry does not have any faces, it is a point cloud. Otherwise, it is a triangle mesh)foo")

      .def(py::init<>(), "Create an empty geometry, with no faces nor vertices.")
      .def("__str__", [](const sc3d::Geometry &v) { return ToString(v); })

      .def("vertexCount", &sc3d::Geometry::vertexCount, "The number of vertices of this geometry.")

      .def("getClosestVertexIndex", 
      [](const sc3d::Geometry &geometry, const NPFloat& queryPosition) {
         auto r = queryPosition.unchecked<1>();
         return geometry.getClosestVertexIndex(math::Vec3(r(0), r(1), r(2)));
         },
         "", py::arg("queryPosition") )

      .def_property("positions",      
         // getter
         [](const sc3d::Geometry &geometry) {
            return Vec3sToNpFloat(geometry.getPositions());
         },

         // setter
         [](sc3d::Geometry &geometry, const NPFloat& positions) {
            if(!(positions.shape(1) == 3)) {
               throw pybind11::value_error(fmt::format("positions must be nx3, has 2nd dimension of size {}", positions.shape(1)));
            }
            return geometry.setPositions(NpFloatToVec3s(positions));
         }, R"foo(This property sets and gets the position attribute of all vertices.
         This attribute is represented as an ndarray of float32s, of dimensions nx3, where n is the number of vertices. 
         
         Raises: 
            ValueError: When attempting to set to an ndarray that does not have the dimensions nx3. 
         )foo")

      .def_property("normals",       
         // getter
         [](const sc3d::Geometry &geometry) {
            return Vec3sToNpFloat(geometry.getNormals());
         },

         // setter
         [](sc3d::Geometry &geometry, const NPFloat& normals) {
            if(!(normals.shape(1) == 3)) {
               throw pybind11::value_error(fmt::format("normals must be nx3, has 2nd dimension of size {}", normals.shape(1)));
            }
            return geometry.setNormals(NpFloatToVec3s(normals));
         }, R"foo(This property sets and gets the normal attribute of all vertices.
         This attribute is represented as an ndarray of float32s, of dimensions nx3, where n is the number of vertices. 
         
         Raises: 
            ValueError: When attempting to set to an ndarray that does not have the dimensions nx3. 
         )foo")

      .def_property("colors",      
         // getter
         [](const sc3d::Geometry &geometry) {
            return Vec3sToNpFloat(geometry.getColors());
         },

         // setter
         [](sc3d::Geometry &geometry, const NPFloat& colors) {
            if(!(colors.shape(1) == 3)) {
               throw pybind11::value_error(fmt::format("colors must be nx3, has 2nd dimension of size {}", colors.shape(1)));
            }
            return geometry.setColors(NpFloatToVec3s(colors));
         }, R"foo(This property sets and gets the color attribute of all vertices.
         This attribute is represented as an ndarray of float32s, of dimensions nx3, where n is the number of vertices. 
         The colors are specified as three colors channel R, G, B, in linear color space, where each value is in the range [0, 1].
         
         Raises: 
            ValueError: When attempting to set to an ndarray that does not have the dimensions nx3. 
         )foo")

      .def_property("texCoords",      
         // getter
         [](const sc3d::Geometry &geometry) {
            return Vec2sToNpFloat(geometry.getTexCoords());
         },

         // setter
         [](sc3d::Geometry &geometry, const NPFloat& texCoords) {
            if(!(texCoords.shape(1) == 2)) {
               throw pybind11::value_error(fmt::format("texCoords must be nx2, has 2nd dimension of size {}", texCoords.shape(1)));
            }
            return geometry.setTexCoords(NpFloatToVec2s(texCoords));
         }, R"foo(This property sets and gets the texture coordinate attribute of all vertices.
         This attribute is represented as an ndarray of float32s, of dimensions nx2, where n is the number of vertices. 
         The texture coordinates are specified as two values U and V.
         
         Raises: 
            ValueError: When attempting to set to an ndarray that does not have the dimensions nx2. 
         )foo")

      .def_property("faces",      
         // getter
         [](const sc3d::Geometry &geometry) {
            return FacesToNpInt(geometry.getFaces());
         },

         // setter
         [](sc3d::Geometry &geometry, const NPInt& faces) {
            if(!(faces.shape(1) == 3)) {
               throw pybind11::value_error(fmt::format("faces must be nx3, has 2nd dimension of size {}", faces.shape(1)));
            }
            return geometry.setFaces(NpIntToFaces(faces));
         },  R"foo(This property sets and gets the indices of all the triangles of the geometry.
         This attribute is represented as an ndarray of ints, of dimensions nx3, where n is the number of triangles. 
         
         Raises: 
            ValueError: When attempting to set to an ndarray that does not have the dimensions nx3. 
         )foo")
         
      .def_property("texture", &sc3d::Geometry::getTexture, &sc3d::Geometry::setTexture, 
      R"foo(This property sets and gets the texture of this mesh, as a ColorImage instance.
         )foo");

   py::class_<sg::Node, std::shared_ptr<sg::Node>>(m, "Node",
      R"foo(A generic Node. You can add other nodes as children to this Node.
         )foo")
      .def(py::init<>(), "Creates an empty generic node.")   
      .def("__str__", [](const sg::Node &v) { return ToString(v); })

      .def("isGeometryNode", &sg::Node::isGeometryNode, "Returns true if this is a GeometryNode.")
      .def("isColorImageNode", &sg::Node::isColorImageNode, "Returns true if this is a ColorImageNode.")
      .def("isDepthImageNode", &sg::Node::isDepthImageNode, "Returns true if this is a DepthImageNode.")
      .def("isPointNode", &sg::Node::isPointNode, "Returns true if this is a PointNode.")
      .def("isCoordinateFrameNode", &sg::Node::isCoordinateFrameNode, "Returns true if this is a CoordinateFrameNode.")
      .def("isPolylineNode", &sg::Node::isPolylineNode, "Returns true if this is a PolylineNode.")
      .def("isPerspectiveCameraNode", &sg::Node::isPerspectiveCameraNode, "Returns true if this is a PerspectiveCameraNode.")

      .def("numChildren", &sg::Node::numChildren, "Returns how many children this node has.")
      
      .def("removeChild", [](sg::Node &node, std::shared_ptr<sg::Node> targetNode) { 
         node.removeChild(targetNode, nullptr);
      }, "Removes `targetNode` as a child of this node.", py::arg("targetNode"))

      .def_static("remove", &sg::Node::remove, 
         "From the SceneGraph with the specified root node, remove the target node", py::arg("rootNode"), py::arg("targetNode")  )

      .def("children", [](sg::Node &node) { return py::make_iterator(node.begin(), node.end()); },
               py::keep_alive<0, 1>(), "Returns an iterator over all children of this node.")

      .def_property("name",  
         (std::string (sg::Node::*)())  &sg::Node::getName, 
      
         [](sg::Node &node, const std::string& name) {
            node.setName(name, nullptr);
         }, "Sets and gets the name of this node.")

      .def_property("visibility",  
         (bool (sg::Node::*)())  &sg::Node::isVisible, 
      
         [](sg::Node &node, bool newVisibility) {
            node.setVisibility(newVisibility, nullptr);
         }, "Sets and gets the visibility of this node.")

      .def_property("material",  
         (sg::Material& (sg::Node::*)())  &sg::Node::getMaterial, 
         [](sg::Node &node, const sg::Material& material) {
            node.setMaterial(material, nullptr);
         }, "Set and gets the material of this node.")

      .def("appendChild", 
         [](sg::Node &node, std::shared_ptr<sg::Node> child) {
            return node.appendChild(child, nullptr);
         }, "Adds a node under this node, as a child. Returns true, on success", py::arg("child"))

      .def_property("transform",  
         // getter
         [](const sg::Node &a) {
            math::Mat3x4 m = math::Mat3x4::fromTransform(a.getTransform());
            return NPFloat(std::vector<ptrdiff_t>{3, 4}, &m.m00);
         },

         // setter
         [](sg::Node &node, const NPFloat& transform) {
            if(!(transform.shape(0) == 3 && transform.shape(1) == 4)) {
               throw pybind11::value_error(fmt::format(
                  "setting transform failed. Invalid shape: ({}, {}). Should be (3, 4)",
                  (int)transform.shape(0), (int)transform.shape(1)));
            }
            auto r = transform.unchecked<2>();
    
            math::Mat3x4 m(
               r(0, 0), r(0, 1), r(0, 2), r(0, 3),
               r(1, 0), r(1, 1), r(1, 2), r(1, 3),
               r(2, 0), r(2, 1), r(2, 2), r(2, 3));
            node.setTransform(math::Transform::fromMat3x4(m));
         }, R"foo(This property sets and gets the transform of the node.
         This attribute is represented as an ndarray of float32s, of dimensions 3x4.
         
         Raises: 
            ValueError: When attempting to set to an ndarray that does not have the dimensions 3x4. 
         )foo");

   py::class_<sg::GeometryNode, sg::Node, std::shared_ptr<sg::GeometryNode>>(m, "GeometryNode",
      "A node that contains a Geometry")
      .def(py::init<>(), "Creates an empty GeometryNode")
      .def_property("geometry",      
         (sc3d::Geometry& (sg::GeometryNode::*)())    &sg::GeometryNode::getGeometry,
         (void (sg::GeometryNode::*)(const sc3d::Geometry&))    &sg::GeometryNode::setGeometry,
         "The geometry of this node.");     

   py::class_<sg::ColorImageNode, sg::Node, std::shared_ptr<sg::ColorImageNode>>(m, "ColorImageNode", "A node that contains a ColorImage")
      .def(py::init<>(), "Creates an empty ColorImageNode")
      .def_property("colorImage",      
         (sc3d::ColorImage& (sg::ColorImageNode::*)()) &sg::ColorImageNode::getColorImage,
         (void (sg::ColorImageNode::*)(const sc3d::ColorImage&)) &sg::ColorImageNode::setColorImage,
         "The ColorImage of this node.");   
           
   py::class_<sc3d::ColorImage>(m, "ColorImage", "An RGBA Image")
      .def(py::init<>(), "Creates an empty ColorImage")
      .def(py::init<int, int, const NPFloat&>(), R"foo(
         Construct a ColorImage
         
         - width: The width of the image.
         - height: The height of the image.
         - rgba: The raw rgba data of the image. Is represented as an ndarray of float32s, of dimensions nx4, with color values in the unit interval [0, 1].
         
         Note that the constructor expects RGBA data in a linear color space. Please ensure it is not in sRGB, or any other non-linear color space.)foo", 
      py::arg("width"), py::arg("height"), py::arg("rgba"))
      .def("__str__", [](const sc3d::ColorImage &v) { return ToString(v); })
      .def_property_readonly("width", &sc3d::ColorImage::getWidth, "The width of the image")
      .def_property_readonly("height", &sc3d::ColorImage::getHeight, "The height of the image")

      .def_property_readonly("data",  
         // getter
         [](const sc3d::ColorImage &ci) {

            std::vector<float> floats;
            floats.reserve(ci.getHeight()*ci.getWidth()*4);
  
            for (int ii = 0; ii < ci.getData().size(); ii++) {
               floats.push_back(ci.getData()[ii].x);
               floats.push_back(ci.getData()[ii].y);
               floats.push_back(ci.getData()[ii].z);
               floats.push_back(ci.getData()[ii].w);
            }       

            return NPFloat(std::vector<ptrdiff_t>{(ptrdiff_t)(ci.getHeight()*ci.getWidth()), 4}, &floats[0]);
            
         }, R"foo(Gets the raw RGBA data of the image. 
         
         It is a represented as an ndarray of float32s, with the dimensions dx4, where d=width*height. 
         
         Each color is represent as a combination of four color channels, in the order R, G, B, A, and is a value in the unit interval [0, 1])foo")

      .def("getAsHWCFloatImage",
         [](const sc3d::ColorImage &ci) {

            const std::vector<ptrdiff_t> shape = {ci.getHeight(), ci.getWidth(), 4};
            NPFloat arr = NPFloat(shape);
            auto view = arr.mutable_unchecked();
            for (size_t r = 0; r < ci.getHeight(); ++r) {
               for (size_t c = 0; c < ci.getWidth(); ++c) {
                  const auto pixel = ci.getPixelAtColRow(c, r);
                  view(r, c, 0) = pixel.x;
                  view(r, c, 1) = pixel.y;
                  view(r, c, 2) = pixel.z;
                  view(r, c, 3) = pixel.w;
               }
            }
            return arr;

         },
         "Create and return an HWC float image-- a 3-d numpy array with "
         "dimensions height, width, and channels (four channels for RGBA)")

      .def("getAsHWC8BitColorImage",
         [](const sc3d::ColorImage &ci) {
            
            auto ToUChar = [](float v) -> uint8_t {
               return (uint8_t)std::max(0, std::min(255, (int)(v * 255.0)));
            };

            const std::vector<ptrdiff_t> shape = {ci.getHeight(), ci.getWidth(), 4};
            NPUInt8 arr = NPUInt8(shape);
            auto view = arr.mutable_unchecked();
            for (size_t r = 0; r < ci.getHeight(); ++r) {
               for (size_t c = 0; c < ci.getWidth(); ++c) {
                  const auto pixel = ci.getPixelAtColRow(c, r);
                  math::Vec3 rgb = sc3d::LinearToApproximateSRGB(pixel.xyz());
                  float alpha = pixel.w;
                  view(r, c, 0) = ToUChar(rgb.x);
                  view(r, c, 1) = ToUChar(rgb.y);
                  view(r, c, 2) = ToUChar(rgb.z);
                  view(r, c, 3) = ToUChar(alpha);
               }
            }
            return arr;

         },
         "Create and return an HWC 8-bit color image-- a 3-d numpy array "
         "with dimensions height, width, and channels (four channels for RGBA).  "
         "Each color is a value in [0, 255] and the image has sRGB gamma.")
      
      .def_static("fromHWC8BitArr",
         [](const NPFloat& arr) { 
         
         auto toFloat = [](uint8_t v) -> float {
            return static_cast<float>(v) / 255.f;
         };
         
         if (arr.ndim() != 3) {
            throw pybind11::value_error(fmt::format("Input array must be HWC with 3 dimensions, given has dimensions {}", arr.ndim()));
         }
         
         const int height = arr.shape(0);
         const int width = arr.shape(1);
         const int nChan = arr.shape(2);
         if (nChan < 3) {
            throw pybind11::value_error(fmt::format("Input array must be 4-channel RGBA or 3-channel RGB, given has dimensions {}", nChan));
         }
         sc3d::ColorImage ci(width, height);
         auto view = arr.unchecked<3>();
         for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
               ci.setPixelAtColRow(
                     c, r,
                     math::Vec4(
                        sc3d::ApproximateSRGBGammaToLinear({
                           toFloat(view(r, c, 0)),
                           toFloat(view(r, c, 1)), 
                           toFloat(view(r, c, 2)),
                        }),
                        nChan == 4 ? toFloat(view(r, c, 3)) : 0));
            }
         }

         return ci;
      },
      "Create and return a ColorImage from a HWC 8-bit color Numpy array -- a"
      "3-d numpy array with dimensions height, width, and channels (three channels "
      "for RGB *OR* four channels for RGBA). The input array should be in the "
      "sRGB color space");

   py::class_<sg::DepthImageNode, sg::Node, std::shared_ptr<sg::DepthImageNode>>(m, "DepthImageNode", "A node that contains a DepthImage")
      .def(py::init<>(), "Creates an empty DepthImageNode")
      .def_property("depthImage",      
         (sc3d::DepthImage& (sg::DepthImageNode::*)()) &sg::DepthImageNode::getDepthImage,
         (void (sg::DepthImageNode::*)(const sc3d::DepthImage&)) &sg::DepthImageNode::setDepthImage,
         "The DepthImage of this node.");
         
   py::class_<sc3d::DepthImage>(m, "DepthImage", "An Depth Image. Every pixel is a depth value.")
      .def(py::init<>(), "Creates an empty DepthImage")
      .def(py::init<int, int, const NPFloat&>(), R"foo(
         Construct a DepthImage
         
         - width: The width of the image.
         - height: The height of the image.
         - depth: The raw depth data of the image. Is represented as an ndarray of float32s, of dimensions n, where the values are in meters.)foo", 
      py::arg("width"), py::arg("height"), py::arg("depth"))
      .def("__str__", [](const sc3d::DepthImage &v) { return ToString(v); })
      .def_property_readonly("width", &sc3d::DepthImage::getWidth, "The width of the image")
      .def_property_readonly("height", &sc3d::DepthImage::getHeight, "The height of the image")

      .def_property_readonly("data",  
         // getter
         [](const sc3d::DepthImage &di) {
            std::vector<float> floats;
  
            for (int ii = 0; ii < di.getData().size(); ii++) {
               floats.push_back(di.getData()[ii]);
            }       

            return NPFloat(std::vector<ptrdiff_t>{(ptrdiff_t)(di.getHeight()*di.getWidth())}, &floats[0]);
            
         }, "Gets the raw depth data of the image, as a flat array of depth values")

      .def("getAsHWCFloatImage",
         [](const sc3d::DepthImage &di) {
            const std::vector<ptrdiff_t> shape = {di.getHeight(), di.getWidth(), 1};
            NPFloat arr = NPFloat(shape);
            auto view = arr.mutable_unchecked();
            for (size_t r = 0; r < di.getHeight(); ++r) {
               for (size_t c = 0; c < di.getWidth(); ++c) {
                  view(r, c, 0) = di.getPixelAtColRow(c, r);
               }
            }
            return arr;

         }, "Create and return an HWC float image-- a 3-d numpy array with dimensions height, width, and channels (one channel for depth)");


   py::class_<sg::PerspectiveCameraNode, sg::Node, std::shared_ptr<sg::PerspectiveCameraNode>>(m, "PerspectiveCameraNode", "A node that contains a PerspectiveCamera")
      .def(py::init<>(), "Creates an empty PerspectiveCamera")
      .def_property("perspectiveCamera",      
         (sc3d::PerspectiveCamera& (sg::PerspectiveCameraNode::*)()) &sg::PerspectiveCameraNode::getPerspectiveCamera,
         (void (sg::PerspectiveCameraNode::*)(const sc3d::PerspectiveCamera&)) &sg::PerspectiveCameraNode::setPerspectiveCamera,
         "The PerspectiveCamera of this node.");   

   py::class_<sg::PointNode, sg::Node, std::shared_ptr<sg::PointNode>>(m, "PointNode", "Represents a 3D position. Used to represent landmarks")
      .def(py::init<>(), "Create a PointNode");    
         
   py::class_<sg::CoordinateFrameNode, sg::Node, std::shared_ptr<sg::CoordinateFrameNode>>(m, "CoordinateFrameNode", "Represents a coordinate frame; that is, a position, and x-,y- and z-axes.")
      .def(py::init<>(), "Create a CoordinateFrameNode");    

   py::class_<sg::PolylineNode, sg::Node, std::shared_ptr<sg::PolylineNode>>(m, "PolylineNode", "A node that contains a Polyline")
      .def(py::init<>(), "Creates an empty PolylineNode")
      .def_property("polyline",      
         (sc3d::Polyline& (sg::PolylineNode::*)()) &sg::PolylineNode::getPolyline,
         (void (sg::PolylineNode::*)(const sc3d::Polyline&)) &sg::PolylineNode::setPolyline,
         "The Polyline of this node.");     

   py::class_<sc3d::Polyline>(m, "Polyline", "This class represents a polyline. Which is a shape that consits of only lines, that are connected by a several points. These points are specified in the positions property.")

      .def(py::init<>(), "Create an empty polyline, with no vertices.")
      .def("__str__", [](const sc3d::Polyline &v) { return ToString(v); })
 
	   .def("vertexCount", &sc3d::Polyline::vertexCount, "The number of vertices of this polyline.")
      .def("length", &sc3d::Polyline::length, "Gets the length of the polyline.")

      .def_property("positions",      
         // getter
         [](const sc3d::Polyline &polyline) {
            return Vec3sToNpFloat(polyline.getPositions());
         },

         // setter
         [](sc3d::Polyline &polyline, const NPFloat& positions) {
            if(!(positions.shape(1) == 3)) {
               throw pybind11::value_error(fmt::format("positions must be nx3, has 2nd dimension of size {}", positions.shape(1)));
            }
            polyline.setPositions(NpFloatToVec3s(positions));
         }, 
         R"foo(This property sets and gets the positions of the points that describe the polyline.
         This property is represented as an ndarray of float32s, of dimensions nx3, where n is the number of vertices. 
         
         Raises: 
            ValueError: When attempting to set to an ndarray that does not have the dimensions nx3. 
         )foo");

   py::enum_<sg::MaterialModel>(m, "MaterialModel")
      .value("Unlit", sg::MaterialModel::Unlit, "A material model that has no shading")
      .value("Phong", sg::MaterialModel::Phong, "A material model that uses phong shading");
    
   py::class_<sg::Material>(m, "Material", "Describes the material of a node")
      .def(py::init<>(), "Creates a default material")   

      .def_property("objectColor",  
         // getter
         [](const sg::Material &m) {
            std::vector<float> dat({m.objectColor.x, m.objectColor.y, m.objectColor.z});
            return NPFloat(std::vector<ptrdiff_t>{3}, &dat[0]);
         },

         // setter
         [](sg::Material &material, const NPFloat& objectColor) {
            if(!(objectColor.shape(0) == 3)) {
               throw pybind11::value_error(fmt::format("setting material color failed. Invalid shape: (%d). Should be (3)", (int)objectColor.shape(0)));
            }
            auto r = objectColor.unchecked<1>();
   
            material.objectColor = math::Vec3(r(0), r(1), r(2));

         }, R"foo(Gets and sets the color of the material. 
         
         A color is represented as an ndarray of float32s, of dimension 3. The three values represent the values R, G, B, and are in the range [0, 1]

         )foo")
         
      .def_property("materialModel",  
         // getter
         [](const sg::Material &m) {
            return m.materialModel;
         },

         // setter
         [](sg::Material &material, sg::MaterialModel materialModel) {
            material.materialModel = materialModel;
         }, "The material model of the node.");


   py::class_<sc3d::PerspectiveCamera>(m, "PerspectiveCamera", "Stores camera data.")
      .def(py::init<>(), "Create an empty PerspectiveCamera.")
      .def("__str__", [](const sc3d::PerspectiveCamera &v) { return ToString(v); })
      .def("getIntrinsicMatrixReferenceSize",
            [](sc3d::PerspectiveCamera &camera) { 
               return Vec2ToNpFloat(camera.getIntrinsicMatrixReferenceSize());
            }, "Get the intrinsic matrix reference size")
      .def("setIntrinsicMatrixReferenceSize",
            [](sc3d::PerspectiveCamera &camera, float width, float height) { 
               camera.setIntrinsicMatrixReferenceSize(math::Vec2(width, height));
            })
      .def("getNominalIntrinsicMatrix",
            [](sc3d::PerspectiveCamera &camera) { 
               return Mat3x3ToNpFloat(camera.getNominalIntrinsicMatrix());
            }, "Return the unmodified nominal intrinsic matrix")
      .def("setNominalIntrinsicMatrix",
            [](sc3d::PerspectiveCamera &camera, const NPFloat& ext) { 
               if (ext.ndim() != 2) {
                  throw pybind11::value_error(fmt::format("Nominal Intrinsic matrix must be 2d, given has dimensions {}", ext.ndim()));
               }
               if (!(ext.shape(0) == 3 && ext.shape(1) == 3)) {
                  throw pybind11::value_error(fmt::format("Nominal Intrinsic matrix must be 3x3, given is {}x{}", ext.shape(0), ext.shape(1)));
               }
               camera.setNominalIntrinsicMatrix(NpFloatToMat3x3(ext));
            })
      .def("getIntrinsicMatrix",
            [](sc3d::PerspectiveCamera &camera) { 
               return Mat3x3ToNpFloat(camera.getIntrinsicMatrix());
            }, "Get the calibrated intrinsic matrix")
      .def("getExtrinsicMatrix",
            [](sc3d::PerspectiveCamera &camera) { 
               return Mat3x4ToNpFloat(camera.getExtrinsicMatrix());
            }, "Get the extrinsic matrix")
      .def("setExtrinsicMatrix",
            [](sc3d::PerspectiveCamera &camera, const NPFloat& ext) { 
               if (ext.ndim() != 2) {
                  throw pybind11::value_error(fmt::format("Extrinsic matrix must be 2d, given has dimensions {}", ext.ndim()));
               }
               if (!(ext.shape(0) == 3 && ext.shape(1) == 4)) {
                  throw pybind11::value_error(fmt::format("Extrinsic matrix must be 3x4, given is {}x{}", ext.shape(0), ext.shape(1)));
               }
               camera.setExtrinsicMatrix(NpFloatToMat3x4(ext));
            })
      .def("getProjectionViewMatrix",
            [](sc3d::PerspectiveCamera &camera, float near, float far) { 
               return Mat4x4ToNpFloat(camera.getProjectionViewMatrix(near, far));
            }, "Get the projection view matrix",
            py::arg("near") = 0.001f, py::arg("far") = 100.0f)
      .def("getFocalLengthScaleFactor",
            &sc3d::PerspectiveCamera::getFocalLengthScaleFactor,
            "Get the factor by which the focal lengths are scaled")
      .def("setFocalLengthScaleFactor",
            &sc3d::PerspectiveCamera::setFocalLengthScaleFactor)
      .def("getOrientationMatrix",
            [](sc3d::PerspectiveCamera &camera) { 
               return Mat3x4ToNpFloat(camera.getOrientationMatrix());
            }, "Get the orientation transform matrix")
      .def("setOrientationMatrix",
            [](sc3d::PerspectiveCamera &camera, const NPFloat& ext) { 
               if (ext.ndim() != 2) {
                  throw pybind11::value_error(fmt::format("Orientation matrix must be 2d, given has dimensions {}", ext.ndim()));
               }
               if (!(ext.shape(0) == 3 && ext.shape(1) == 4)) {
                  throw pybind11::value_error(fmt::format("Orientation matrix must be 3x4, given is {}x{}", ext.shape(0), ext.shape(1)));
               }
               camera.setOrientationMatrix(NpFloatToMat3x4(ext));
            })
      .def("getLensDistortionCalibration",
            &sc3d::PerspectiveCamera::getLensDistortionCalibration,
            "Get the lens distortion calibration lookup table")
      .def("getLegacyImageSize",
            [](sc3d::PerspectiveCamera &camera) { 
               return Size2DToNpInt(camera.getLegacyImageSize());
            }, "Get image size")
      .def("setLegacyImageSize",
            [](sc3d::PerspectiveCamera &camera, int width, int height) { 
               camera.setLegacyImageSize({width, height});
            })
      .def("unprojectDepthSample",
            [](sc3d::PerspectiveCamera &camera, int imageWidth, int imageHeight, float pixelCol, float pixelRow, float depth) { 
               return Vec3ToNpFloat(camera.unprojectDepthSample(imageWidth, imageHeight, pixelCol, pixelRow, depth));
            },
            "Unproject a single pixel at (`pixelCol`, `pixelRow`) with depth `depth` "
            "to a point in 3-d space (in the frame of this camera)")
      .def("unprojectFrame",
            &sc3d::PerspectiveCamera::unprojectFrame,
            "Unproject a `color` and `depth` image into a colored point cloud `Geometry`.  "
            "Optionally filter depth by `minDepth` and `maxDepth`.",
            py::arg("depth"), py::arg("color"), py::arg("minDepth") = 0, py::arg("maxDepth") = std::numeric_limits<float>::max());

   m.def("WriteSceneGraphToGltf", (bool (*)(std::shared_ptr<sg::Node>, const std::string&)) &io::gltf::WriteSceneGraphToGltf,
      "Write a Scene graph to a path", py::arg("sceneGraph"), py::arg("path"));
   m.def("ReadSceneGraphFromGltf",  &io::gltf::ReadSceneGraphFromGltf, 
      "Read a Scene graph from a string formatted as a gltf-file", py::arg("gltfSource"));

   m.def("WriteGeometryToPLYFile", (  bool (*)(std::string, const sc3d::Geometry&)  ) &io::ply::WriteGeometryToPLYFile,
      "Write a Geometry to a ply file", py::arg("filename"), py::arg("geometry"));

   m.def("ReadGeometryFromPLYFile", (  bool (*)(sc3d::Geometry&, std::string filename)  ) &io::ply::ReadGeometryFromPLYFile,
      "Read a Geometry from a ply file", py::arg("geometryOut"), py::arg("filename"));

   py::class_<io::ply::RawFrameMetadata>(m, "RawFrameMetadata", "Stores extra metadata for a raw frame.")
      .def(py::init<>(), "Create an empty RawFrameMetadata.")
      .def_readwrite("timestamp", &io::ply::RawFrameMetadata::timestamp);

   // In order to extract the timestamp with backward compatibility in the function signature,
   // this function is overloaded. Pybind is unable to infer the signature so that we need to
   // select it ourselves.
   bool (*ReadRawFrameDataFromPLYFileFuncPtr)(
       sc3d::ColorImage&, sc3d::DepthImage&, sc3d::PerspectiveCamera&, io::ply::RawFrameMetadata&, std::string
    ) = &io::ply::ReadRawFrameDataFromPLYFile;

   m.def("ReadRawFrameDataFromPLYFile", ReadRawFrameDataFromPLYFileFuncPtr,
      "Read a PLY file and extract the embedded camera and depth data.",
      py::arg("imageOut"), py::arg("depthOut"), py::arg("cameraOut"), py::arg("metadataOut"), py::arg("filename"));

   py::class_<io::pbio::IterSceneGraphsFromProtobag>(m, "IterSceneGraphsFromProtobag", "Iterate over SceneGraphs from a given protobag")
      .def(py::init<const std::string &>())
      .def("__str__",
         [](const io::pbio::IterSceneGraphsFromProtobag &v) {
             return fmt::format("IterSceneGraphsFromProtobag(\"{}\")", v.path);
         })
      .def("__iter__",
         [](io::pbio::IterSceneGraphsFromProtobag &v) -> io::pbio::IterSceneGraphsFromProtobag& { return v; })
      .def("next",
         [](io::pbio::IterSceneGraphsFromProtobag &v) { return IterSGGetNext(v); })
      .def("__next__",
         [](io::pbio::IterSceneGraphsFromProtobag &v) { return IterSGGetNext(v); });
}

