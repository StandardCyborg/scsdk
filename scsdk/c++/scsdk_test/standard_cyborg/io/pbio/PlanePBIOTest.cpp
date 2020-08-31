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

#include "standard_cyborg/sc3d/Plane.hpp"
#include "standard_cyborg/io/pbio/PlanePBIO.hpp"
#include "standard_cyborg/io/pbio/TestUtils.hpp"

using namespace standard_cyborg;

const std::string kTestPlanePBTXT =
R"(frame: "testFrame"
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
)";

TEST(PlanePBIOTest, Plane) {
    sc3d::Plane plane {math::Vec3(1.8, 1.3, 1.1), math::Vec3(0.1, 1.1, 2.1) };
    plane.setFrame("testFrame");
    CheckToFromPBBinaryRoundTrip(plane);
    CheckToFromPBText(plane, kTestPlanePBTXT);
}

