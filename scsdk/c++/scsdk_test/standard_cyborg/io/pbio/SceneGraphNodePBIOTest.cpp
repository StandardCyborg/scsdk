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

#include <gtest/gtest.h>

#include <exception>

#include <protobag/Utils/PBUtils.hpp>

#include "standard_cyborg/sc3d/Polyline.hpp"
#include "standard_cyborg/sc3d/ColorImage.hpp"
#include "standard_cyborg/sc3d/DepthImage.hpp"
#include "standard_cyborg/sc3d/PerspectiveCamera.hpp"
#include "standard_cyborg/sc3d/Geometry.hpp"
#include "standard_cyborg/sc3d/Plane.hpp"
#include "standard_cyborg/sc3d/Landmark.hpp"

#include "standard_cyborg/io/pbio/SceneGraphNodePBIO.hpp"

#include "standard_cyborg/io/pbio/TestUtils.hpp"

using namespace standard_cyborg;

template <typename MT>
void myCheckToFromPBText(const std::shared_ptr<scene_graph::Node> &value, const std::string &expected_pbtxt, const MT &msg) {
    
    /// Check value -> pbtxt
    auto maybe_txt =
    protobag::PBFactory::ToTextFormatString(msg, /*newlines=*/true);
    ASSERT_TRUE(maybe_txt.IsOk()) <<
    std::string("Failed to serialze to pb text: ") + maybe_txt.error;
    auto actual_pbtxt = *maybe_txt.value;
    EXPECT_EQ(expected_pbtxt, actual_pbtxt) <<
    "value -> pbtext failure: \n" <<
    "Expected: \n" <<
    expected_pbtxt <<
    "\nActual: \n" <<
    actual_pbtxt;
    
    /// Check pbtxt -> value
    auto maybe_decoded =
    protobag::PBFactory::LoadFromString<MT>(expected_pbtxt);
    EXPECT_TRUE(maybe_decoded.IsOk()) <<
    std::string("Failed to deserialze from pb binary: ") + maybe_decoded.error;
    
    auto maybe_decoded_value = io::pbio::FromPB(*maybe_decoded.value);
    EXPECT_TRUE(maybe_decoded_value.IsOk()) <<
    std::string("Failed to convert from PB: ") + maybe_decoded_value.error;
    auto decoded_value = *maybe_decoded_value.value;
    
    bool b = value->equals(*decoded_value);
    EXPECT_TRUE(b) <<
    "pbtext -> value failure: \n" <<
    "Expected: \n" << SCToString(value) <<
    "\n\nDecoded:\n" << SCToString(decoded_value);
}

template <typename T>
void myCheckToFromPBText(const T &value, const std::string &expected_pbtxt) {
    
    /// Check value -> pbtxt
    auto maybe_msg = io::pbio::ToPB(value);
    EXPECT_TRUE(maybe_msg.IsOk()) <<
    std::string("Failed to convert to PB: ") + maybe_msg.error;
    const auto &msg = *maybe_msg.value;
    myCheckToFromPBText(value, expected_pbtxt, msg);
}

template <typename T, typename MT>
void myCheckToFromPBBinaryRoundTrip(const T &value, const MT &msg) {
    auto maybe_buf = protobag::PBFactory::ToBinaryString(msg);
    EXPECT_TRUE(maybe_buf.IsOk()) <<
    std::string("Failed to serialze to pb binary: ") + maybe_buf.error;
    
    auto maybe_decoded =
    protobag::PBFactory::LoadFromContainer<MT>(*maybe_buf.value);
    EXPECT_TRUE(maybe_decoded.IsOk()) <<
    std::string("Failed to deserialze from pb binary: ") + maybe_decoded.error;
    
    auto maybe_decoded_value = io::pbio::FromPB(*maybe_decoded.value);
    EXPECT_TRUE(maybe_decoded_value.IsOk()) <<
    std::string("Failed to convert from PB: ") + maybe_decoded_value.error;
    auto decoded_value = *maybe_decoded_value.value;
    
    bool b = value->equals(*decoded_value);
    
    EXPECT_TRUE(b) <<
    "CheckToFromPBBinaryRoundTrip Failure \n" <<
    "Expected: \n" << SCToString(value) <<
    "\n\nDecoded:\n" << SCToString(decoded_value);
}

template <typename T>
void myCheckToFromPBBinaryRoundTrip(const T &value) {
    auto maybe_msg = io::pbio::ToPB(value);
    EXPECT_TRUE(maybe_msg.IsOk()) <<
    std::string("Failed to convert to PB: ") + maybe_msg.error;
    const auto &msg = *maybe_msg.value;
    myCheckToFromPBBinaryRoundTrip(value, msg);
}


TEST(SceneGraphNodePBIOTest, TestPolyline) {
    
    std::vector<math::Vec3> positions;
    positions.push_back(math::Vec3(1.1, 2.2, 3.2));
    sc3d::Polyline polyline(positions);
    
    std::shared_ptr<scene_graph::PolylineNode> node(std::make_shared<scene_graph::PolylineNode>());
    
    node->setName("node0");
    node->setPolyline(polyline);
    node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
    node->getMaterial().materialModel = scene_graph::MaterialModel::Phong;
    
    myCheckToFromPBBinaryRoundTrip(node);
    
    myCheckToFromPBText(node, R"(nodeName: "node0"
objectColor {
  x: 0.9
  y: 0.2
  z: 0.1
}
materialModel: 1
polyline {
  positions {
    x: 1.1
    y: 2.2
    z: 3.2
  }
}
)");
    
    
}


TEST(SceneGraphNodePBIOTest, TestColorImage) {
    sc3d::ColorImage ci = sc3d::ColorImage(1, 1, std::vector<math::Vec4>{
        {0.0, 0.25, 0.5, 0.75},
    });
    ci.setFrame("test");
    
    std::shared_ptr<scene_graph::ColorImageNode> node(std::make_shared<scene_graph::ColorImageNode>());
    node->setName("test");
    
    node->setColorImage(ci);
    node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
    node->getMaterial().materialModel = scene_graph::MaterialModel::Phong;
    
    myCheckToFromPBBinaryRoundTrip(node);

    myCheckToFromPBText(node, R"(nodeName: "test"
objectColor {
  x: 0.9
  y: 0.2
  z: 0.1
}
materialModel: 1
image {
  frame: "test"
  pixels {
    properties {
      numeric_type: NUMERIC_TYPE_FLOAT
      shape {
        size: 1
        name: "height"
      }
      shape {
        size: 1
        name: "width"
      }
      shape {
        size: 4
        name: "channels"
        field_names: "r"
        field_names: "g"
        field_names: "b"
        field_names: "a"
      }
    }
    float_values: 0
    float_values: 0.25
    float_values: 0.5
    float_values: 0.75
  }
  color_space: COLOR_SPACE_LINEAR
}
)");

    
}


TEST(SceneGraphNodePBIOTest, TestDepthImage) {
    sc3d::DepthImage ci = sc3d::DepthImage(1, 1, std::vector<float>{ 0.5, });
    ci.setFrame("test");
    
    std::shared_ptr<scene_graph::DepthImageNode> node(std::make_shared<scene_graph::DepthImageNode>());
    node->setName("test");
    
    node->setDepthImage(ci);
    node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
    myCheckToFromPBBinaryRoundTrip(node);

    myCheckToFromPBText(node, R"(nodeName: "test"
objectColor {
  x: 0.9
  y: 0.2
  z: 0.1
}
image {
  frame: "test"
  pixels {
    properties {
      numeric_type: NUMERIC_TYPE_FLOAT
      shape {
        size: 1
        name: "height"
      }
      shape {
        size: 1
        name: "width"
      }
      shape {
        size: 1
        name: "channels"
        field_names: "depth"
      }
    }
    float_values: 0.5
  }
  color_space: COLOR_SPACE_DEPTH
}
)");
}


TEST(SceneGraphNodePBIOTest, TestPerspectiveCamera) {
    sc3d::PerspectiveCamera camera;
    camera.setFrame("test");
    camera.setIntrinsicMatrixReferenceSize(math::Vec2(3088.f, 2316.f));
    camera.setNominalIntrinsicMatrix(math::Mat3x3(
                                                  2881., 0.0,   1536.,
                                                  0.0,   2881., 1149.,
                                                  0.0,   0.0,   1.0
                                                  ));
    camera.setExtrinsicMatrix(math::Mat3x4(
                                           1.0, 0.0, 0.0, 1.0,
                                           0.0, 1.0, 0.0, 2.0,
                                           0.0, 0.0, 1.0, 3.0
                                           ));
    camera.setLensDistortionLookupTable({
        1, 2, 3, 4, 5
    });
    
    std::shared_ptr<scene_graph::PerspectiveCameraNode> node(std::make_shared<scene_graph::PerspectiveCameraNode>());
    
    node->setPerspectiveCamera(camera);
    node->setName("test");
    node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
    myCheckToFromPBBinaryRoundTrip(node);

    myCheckToFromPBText(node, R"(nodeName: "test"
objectColor {
  x: 0.9
  y: 0.2
  z: 0.1
}
pinholeCamera {
  frame: "test"
  focal_length {
    x: 2881
    y: 2881
  }
  principal_point {
    x: 1536
    y: 1149
  }
  image_height: 2316
  image_width: 3088
  sensor_transform {
    rotation {
      w: 1
    }
    translation {
      x: 1
      y: 2
      z: 3
    }
  }
  radial_distortion_coefficients: 1
  radial_distortion_coefficients: 2
  radial_distortion_coefficients: 3
  radial_distortion_coefficients: 4
  radial_distortion_coefficients: 5
}
)");
}

TEST(SceneGraphNodePBIOTest, TestPointCloud) {
    std::vector<math::Vec3> geoPositions;
    std::vector<math::Vec3> geoNormals;
    std::vector<math::Vec3> geoColors;
    
    geoPositions.push_back(math::Vec3(1.1f,2.2f,3.2f));
    
    geoNormals.push_back(math::Vec3(0.1f,0.6f,0.2f));
    
    geoColors.push_back(math::Vec3(0.95f,0.5f,0.9f));
    
    sc3d::Geometry geometry(geoPositions, geoNormals, geoColors);
    geometry.setFrame("testFrame");
    
    std::shared_ptr<scene_graph::GeometryNode> node(std::make_shared<scene_graph::GeometryNode>());
    
    node->setGeometry(geometry);
    node->setName("test");
    node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
    
    myCheckToFromPBBinaryRoundTrip(node);

    myCheckToFromPBText(node, R"(nodeName: "test"
objectColor {
  x: 0.9
  y: 0.2
  z: 0.1
}
pointCloud {
  frame: "testFrame"
  positions {
    properties {
      numeric_type: NUMERIC_TYPE_FLOAT
      shape {
        size: 1
        name: "positionCount"
      }
      shape {
        size: 3
        name: "components"
        field_names: "x"
        field_names: "y"
        field_names: "z"
      }
    }
    float_values: 1.1
    float_values: 2.2
    float_values: 3.2
  }
  normals {
    properties {
      numeric_type: NUMERIC_TYPE_FLOAT
      shape {
        size: 1
        name: "normalCount"
      }
      shape {
        size: 3
        name: "components"
        field_names: "x"
        field_names: "y"
        field_names: "z"
      }
    }
    float_values: 0.1
    float_values: 0.6
    float_values: 0.2
  }
  colors {
    properties {
      numeric_type: NUMERIC_TYPE_FLOAT
      shape {
        size: 1
        name: "colorCount"
      }
      shape {
        size: 3
        name: "components"
        field_names: "r"
        field_names: "g"
        field_names: "b"
      }
    }
    float_values: 0.95
    float_values: 0.5
    float_values: 0.9
  }
}
)");
}

static sc3d::ColorImage CreateTestImg() {
    sc3d::ColorImage ci(1, 1);
    ci.setFrame("test");
    ci.setPixelAtColRow(0, 0, math::Vec4(0.2f, 0.1f, 0.7f, 0.5f));
    
    return ci;
}
TEST(SceneGraphNodePBIOTest, TestTriangleMesh) {
    std::vector<math::Vec3> geoPositions;
    std::vector<math::Vec3> geoNormals;
    std::vector<math::Vec3> geoColors;
    std::vector<math::Vec2> geoTexCoords;
    std::vector<sc3d::Face3> geoFaces;
    
    geoPositions.push_back(math::Vec3(1.1f,2.2f,3.2f));
    geoPositions.push_back(math::Vec3(10.1f,20.2f,30.2f));
    geoPositions.push_back(math::Vec3(100.1f,200.2f,300.2f));
    
    geoNormals.push_back(math::Vec3(0.1f,0.2f,0.2f));
    geoNormals.push_back(math::Vec3(0.2f,0.9f,0.4f));
    geoNormals.push_back(math::Vec3(0.3f,0.2f,0.2f));
    
    geoColors.push_back(math::Vec3(0.9f,0.5f,0.3f));
    geoColors.push_back(math::Vec3(0.7f,0.7f,0.5f));
    geoColors.push_back(math::Vec3(0.6f,0.3f,0.7f));
    
    geoTexCoords.push_back(math::Vec2(0.22f,0.14f));
    geoTexCoords.push_back(math::Vec2(0.24f,0.34f));
    geoTexCoords.push_back(math::Vec2(0.12f,0.38f));
    
    geoFaces.push_back(sc3d::Face3(2, 12, 14));
    geoFaces.push_back(sc3d::Face3(7, 3, 5));
    
    sc3d::Geometry geometry;
    geometry.setPositions(geoPositions);
    geometry.setNormals(geoNormals);
    geometry.setColors(geoColors);
    
    geometry.setTexCoords(geoTexCoords);
    geometry.setFaces(geoFaces);
    geometry.setTexture(CreateTestImg());
    geometry.setFrame("testFrame");
    
    std::shared_ptr<scene_graph::GeometryNode> node(std::make_shared<scene_graph::GeometryNode>());
    
    node->setGeometry(geometry);
    node->setName("test");
    node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
    
    myCheckToFromPBBinaryRoundTrip(node);

    myCheckToFromPBText(node, R"(nodeName: "test"
objectColor {
  x: 0.9
  y: 0.2
  z: 0.1
}
triangleMesh {
  frame: "testFrame"
  positions {
    properties {
      numeric_type: NUMERIC_TYPE_FLOAT
      shape {
        size: 3
        name: "positionCount"
      }
      shape {
        size: 3
        name: "components"
        field_names: "x"
        field_names: "y"
        field_names: "z"
      }
    }
    float_values: 1.1
    float_values: 2.2
    float_values: 3.2
    float_values: 10.1
    float_values: 20.2
    float_values: 30.2
    float_values: 100.1
    float_values: 200.2
    float_values: 300.2
  }
  normals {
    properties {
      numeric_type: NUMERIC_TYPE_FLOAT
      shape {
        size: 3
        name: "normalCount"
      }
      shape {
        size: 3
        name: "components"
        field_names: "x"
        field_names: "y"
        field_names: "z"
      }
    }
    float_values: 0.1
    float_values: 0.2
    float_values: 0.2
    float_values: 0.2
    float_values: 0.9
    float_values: 0.4
    float_values: 0.3
    float_values: 0.2
    float_values: 0.2
  }
  colors {
    properties {
      numeric_type: NUMERIC_TYPE_FLOAT
      shape {
        size: 3
        name: "colorCount"
      }
      shape {
        size: 3
        name: "components"
        field_names: "r"
        field_names: "g"
        field_names: "b"
      }
    }
    float_values: 0.9
    float_values: 0.5
    float_values: 0.3
    float_values: 0.7
    float_values: 0.7
    float_values: 0.5
    float_values: 0.6
    float_values: 0.3
    float_values: 0.7
  }
  tex_coords {
    properties {
      numeric_type: NUMERIC_TYPE_FLOAT
      shape {
        size: 3
        name: "texCoordCount"
      }
      shape {
        size: 2
        name: "components"
        field_names: "u"
        field_names: "v"
      }
    }
    float_values: 0.22
    float_values: 0.14
    float_values: 0.24
    float_values: 0.34
    float_values: 0.12
    float_values: 0.38
  }
  faces {
    properties {
      numeric_type: NUMERIC_TYPE_INT32
      shape {
        size: 2
        name: "faceCount"
      }
      shape {
        size: 3
        name: "components"
        field_names: "i0"
        field_names: "i1"
        field_names: "i2"
      }
    }
    int32_values: 2
    int32_values: 12
    int32_values: 14
    int32_values: 7
    int32_values: 3
    int32_values: 5
  }
  texture {
    frame: "test"
    pixels {
      properties {
        numeric_type: NUMERIC_TYPE_FLOAT
        shape {
          size: 1
          name: "height"
        }
        shape {
          size: 1
          name: "width"
        }
        shape {
          size: 4
          name: "channels"
          field_names: "r"
          field_names: "g"
          field_names: "b"
          field_names: "a"
        }
      }
      float_values: 0.2
      float_values: 0.1
      float_values: 0.7
      float_values: 0.5
    }
    color_space: COLOR_SPACE_LINEAR
  }
}
)");
}

TEST(SceneGraphNodePBIOTest, TestPlane) {
    
    sc3d::Plane plane {math::Vec3(1.8, 1.3, 1.1), math::Vec3(0.1, 1.1, 2.1) };
    plane.setFrame("testFrame");
    std::shared_ptr<scene_graph::PlaneNode> node(std::make_shared<scene_graph::PlaneNode>());
    node->setPlane(plane);
    node->setName("test");
    
    node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
    
    myCheckToFromPBBinaryRoundTrip(node);
    myCheckToFromPBText(node, R"(nodeName: "test"
objectColor {
  x: 0.9
  y: 0.2
  z: 0.1
}
plane {
  frame: "testFrame"
  position {
    x: 1.8
    y: 1.3
    z: 1.1
  }
  normal {
    x: 0.1
    y: 1.1
    z: 2.1
  }
}
)");

}


TEST(SceneGraphNodePBIOTest, TestNode) {
    
    
    std::shared_ptr<scene_graph::Node> node(std::make_shared<scene_graph::Node>());
    
    node->setName("node0");
    node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
    node->getMaterial().materialModel = scene_graph::MaterialModel::Phong;
    
    myCheckToFromPBBinaryRoundTrip(node);
    
    myCheckToFromPBText(node, R"(nodeName: "node0"
objectColor {
  x: 0.9
  y: 0.2
  z: 0.1
}
materialModel: 1
isGenericNode: true
)");
    
}

TEST(SceneGraphNodePBIOTest, CoordinateFrameNode) {
    
    
    std::shared_ptr<scene_graph::CoordinateFrameNode> node(std::make_shared<scene_graph::CoordinateFrameNode>());
    
    node->setName("node0");
    node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
    node->getMaterial().materialModel = scene_graph::MaterialModel::Phong;
    
    myCheckToFromPBBinaryRoundTrip(node);
    
    myCheckToFromPBText(node, R"(nodeName: "node0"
objectColor {
  x: 0.9
  y: 0.2
  z: 0.1
}
materialModel: 1
isCoordinateFrameNode: true
)");
    
}


TEST(SceneGraphNodePBIOTest, PointNode) {
    
    std::shared_ptr<scene_graph::PointNode> node(std::make_shared<scene_graph::PointNode>());
    
    node->setPosition(math::Vec3(1.2, 2.2, 3.2));
    
    node->setName("node0");
    node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
    node->getMaterial().materialModel = scene_graph::MaterialModel::Phong;
    
    myCheckToFromPBBinaryRoundTrip(node);
    
    myCheckToFromPBText(node, R"(nodeName: "node0"
objectColor {
  x: 0.9
  y: 0.2
  z: 0.1
}
materialModel: 1
pointPosition {
  x: 1.2
  y: 2.2
  z: 3.2
}
)");
    
}

TEST(SceneGraphNodePBIOTest, TestLandmark) {
    
    sc3d::Landmark landmark {"landmark2", math::Vec3(0.1, 1.1, 2.1) };
    landmark.setFrame("test");
    
    std::shared_ptr<scene_graph::LandmarkNode> node(std::make_shared<scene_graph::LandmarkNode>());
    node->setLandmark(landmark);
    node->setName("test");
    node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
    
    myCheckToFromPBBinaryRoundTrip(node);
    myCheckToFromPBText(node, R"(nodeName: "test"
objectColor {
  x: 0.9
  y: 0.2
  z: 0.1
}
landmark {
  frame: "test"
  position {
    x: 0.1
    y: 1.1
    z: 2.1
  }
  name: "landmark2"
}
)");

}
