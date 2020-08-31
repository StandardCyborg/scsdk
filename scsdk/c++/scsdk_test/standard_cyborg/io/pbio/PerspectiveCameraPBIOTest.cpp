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


#include "standard_cyborg/sc3d/PerspectiveCamera.hpp"
#include "standard_cyborg/io/pbio/PerspectiveCameraPBIO.hpp"
#include "standard_cyborg/io/pbio/TestUtils.hpp"

using namespace standard_cyborg;

const std::string kTestBasicPBTXT =
R"(frame: "test"
focal_length {
  x: 1
  y: 1
}
principal_point {
}
sensor_transform {
  rotation {
    w: 1
  }
  translation {
  }
}
)";


TEST(PerspectiveCameraPBIOTest, TestBasic) {
    sc3d::PerspectiveCamera camera;
    camera.setFrame("test");
    CheckToFromPBBinaryRoundTrip(camera);
    CheckToFromPBText(camera, kTestBasicPBTXT);
}


const std::string kTestComplicatedPBTXT =
R"(frame: "test"
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
)";

TEST(PerspectiveCameraPBIOTest, TestComplicated) {
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
    
    CheckToFromPBBinaryRoundTrip(camera);
    CheckToFromPBText(camera, kTestComplicatedPBTXT);
}

