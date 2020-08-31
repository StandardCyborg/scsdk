
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

#include "standard_cyborg/sc3d/Geometry.hpp"
#include "standard_cyborg/io/pbio/GeometryPBIO.hpp"
#include "standard_cyborg/io/pbio/TestUtils.hpp"

using namespace standard_cyborg;

const std::string kTestGeometryPointCloudTxt =
R"(positions {
  properties {
    numeric_type: NUMERIC_TYPE_FLOAT
    shape {
      size: 2
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
}
normals {
  properties {
    numeric_type: NUMERIC_TYPE_FLOAT
    shape {
      size: 2
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
  float_values: 0.8
  float_values: 0.1
  float_values: 0.4
}
colors {
  properties {
    numeric_type: NUMERIC_TYPE_FLOAT
    shape {
      size: 2
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
  float_values: 0.5
  float_values: 0.2
  float_values: 0.85
}
)";

TEST(GeometryPBIOTest, TestPointCloud) {
    std::vector<math::Vec3> geoPositions;
    std::vector<math::Vec3> geoNormals;
    std::vector<math::Vec3> geoColors;
    
    geoPositions.push_back(math::Vec3(1.1f,2.2f,3.2f));
    geoPositions.push_back(math::Vec3(10.1f,20.2f,30.2f));
    
    geoNormals.push_back(math::Vec3(0.1f,0.6f,0.2f));
    geoNormals.push_back(math::Vec3(0.8f,0.1f,0.4f));
    
    geoColors.push_back(math::Vec3(0.95f,0.5f,0.9f));
    geoColors.push_back(math::Vec3(0.5f,0.2f,0.85f));
    
    sc3d::Geometry geometry(geoPositions, geoNormals, geoColors);
    
    
    {
        auto maybe_msg = io::pbio::ToPB(geometry);
        EXPECT_TRUE(maybe_msg.IsOk()) <<
        std::string("Failed to convert to PB: ") + maybe_msg.error;
        
        EXPECT_TRUE(maybe_msg.value->point_cloud.has_value()) <<
        std::string("ToPB result contains no expected point cloud");
        
        const auto &msg = *(maybe_msg.value->point_cloud);
        
        CheckToFromPBBinaryRoundTrip(geometry, msg);
    }
    
    {
        auto maybe_msg = io::pbio::ToPB(geometry);
        EXPECT_TRUE(maybe_msg.IsOk()) <<
        std::string("Failed to convert to PB: ") + maybe_msg.error;
        
        EXPECT_TRUE(maybe_msg.value->point_cloud.has_value()) <<
        std::string("ToPB result contains no expected point cloud");
        
        const auto &msg = *maybe_msg.value->point_cloud;
        CheckToFromPBText(geometry, kTestGeometryPointCloudTxt, msg);
    }
    
}

const std::string kTestGeometryTriangleMeshTxt =
R"(frame: "testframe"
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
)";

static sc3d::ColorImage CreateTestImg() {
    sc3d::ColorImage ci(1, 1);
    ci.setFrame("test");
    ci.setPixelAtColRow(0, 0, math::Vec4(0.2f, 0.1f, 0.7f, 0.5f));
    
    return ci;
}

TEST(GeometryPBIOTest, TestTriangleMesh) {
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
    
    geometry.setFrame("testframe");
    
    {
        auto maybe_msg = io::pbio::ToPB(geometry);
        EXPECT_TRUE(maybe_msg.IsOk()) <<
        std::string("Failed to convert to PB: ") + maybe_msg.error;
        
        EXPECT_TRUE(maybe_msg.value->triangle_mesh.has_value()) <<
        std::string("ToPB result contains no expected triangle mesh");
        
        const auto &msg = *maybe_msg.value->triangle_mesh;
        
        CheckToFromPBBinaryRoundTrip(geometry, msg);
    }
    
    {
        auto maybe_msg = io::pbio::ToPB(geometry);
        EXPECT_TRUE(maybe_msg.IsOk()) <<
        std::string("Failed to convert to PB: ") + maybe_msg.error;
        
        EXPECT_TRUE(maybe_msg.value->triangle_mesh.has_value()) <<
        std::string("ToPB result contains no expected triangle mesh");
        
        const auto &msg = *maybe_msg.value->triangle_mesh;
        CheckToFromPBText(geometry, kTestGeometryTriangleMeshTxt, msg);
    }
}
