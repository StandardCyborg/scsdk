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

#include "standard_cyborg/io/pbio/CorePBIO.hpp"
#include "standard_cyborg/io/pbio/TestUtils.hpp"


using namespace standard_cyborg;

TEST(CorePBIOTest, TestToFromPBVec2) {
    CheckToFromPBBinaryRoundTrip(math::Vec2(1, 2));
    
    CheckToFromPBText(
                      math::Vec2(1.5, 2.5),
                      "x: 1.5\ny: 2.5\n");
}

TEST(CorePBIOTest, TestToFromPBVec3) {
    CheckToFromPBBinaryRoundTrip(math::Vec3(1, 2, 3));
    
    CheckToFromPBText(
                      math::Vec3(1.5, 2.5, 3.5),
                      "x: 1.5\ny: 2.5\nz: 3.5\n");
}

TEST(CorePBIOTest, TestToFromPBQuaternion) {
    CheckToFromPBBinaryRoundTrip(math::Quaternion(1, 2, 3, 4));
    
    CheckToFromPBText(
                      math::Quaternion(1.5, 2.5, 3.5, 4.5),
                      "x: 1.5\ny: 2.5\nz: 3.5\nw: 4.5\n");
}
