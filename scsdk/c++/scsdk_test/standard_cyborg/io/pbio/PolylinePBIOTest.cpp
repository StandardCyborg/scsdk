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
#include "standard_cyborg/io/pbio/PolylinePBIO.hpp"
#include "standard_cyborg/io/pbio/TestUtils.hpp"

using namespace standard_cyborg;

const std::string kTestPolylineTxt =
R"(positions {
  x: 1.1
  y: 2.2
  z: 3.2
}
positions {
  x: 10.1
  y: 20.2
  z: 30.2
}
positions {
  x: 12.1
  y: 26.2
  z: 39.2
}
positions {
  x: 17.1
  y: 84.2
  z: 30.2
}
)";

TEST(PolylinePBIOTest, TestPolyline) {
    std::vector<math::Vec3> positions;
    
    positions.push_back(math::Vec3(1.1, 2.2, 3.2));
    positions.push_back(math::Vec3(10.1, 20.2, 30.2));
    positions.push_back(math::Vec3(12.1, 26.2, 39.2));
    positions.push_back(math::Vec3(17.1, 84.2, 30.2));
    
    sc3d::Polyline polyline(positions);
    
    CheckToFromPBBinaryRoundTrip(polyline);
    CheckToFromPBText(polyline, kTestPolylineTxt);
}
