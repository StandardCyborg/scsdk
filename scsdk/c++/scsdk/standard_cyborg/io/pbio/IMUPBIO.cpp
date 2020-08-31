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

#include "standard_cyborg/io/pbio/IMUPBIO.hpp"

#include "standard_cyborg/io/pbio/CorePBIO.hpp"

namespace standard_cyborg {
namespace io {
namespace pbio {

// From CoreMotion ============================================================

extern Result<::standard_cyborg::proto::sc3d::CMDeviceMotionMsg> 
CoreMotionToPB(
               const std::string &frame,
               
               const ::google::protobuf::Timestamp &timestamp,
               
               double rotationX,
               double rotationY,
               double rotationZ,
               
               double gravityX,
               double gravityY,
               double gravityZ,
               
               double accelerationX,
               double accelerationY,
               double accelerationZ,
               
               double magneticFieldAccuracy,
               double magneticFieldX,
               double magneticFieldY,
               double magneticFieldZ,
               
               double attitudeX,
               double attitudeY,
               double attitudeZ,
               double attitudeW) {
    
    ::standard_cyborg::proto::sc3d::CMDeviceMotionMsg msg;
    msg.set_frame(frame);
    
    *msg.mutable_timestamp() = timestamp;
    
#define FillVec3(member, x, y, z) do { \
auto maybeVec = ToPB(math::Vec3(x, y, z)); \
if (!maybeVec.IsOk()) { return {.error = maybeVec.error}; } \
*member = std::move(*maybeVec.value); \
} while (0) \

    FillVec3(msg.mutable_rotation(), rotationX, rotationY, rotationZ);
    FillVec3(msg.mutable_gravity(), gravityX, gravityY, gravityZ);
    FillVec3(msg.mutable_acceleration(), accelerationX, accelerationY, accelerationZ);
    msg.set_magnetic_field_accuracy(magneticFieldAccuracy);
    FillVec3(msg.mutable_magnetic_field(), magneticFieldX, magneticFieldY, magneticFieldZ);
    
    
#undef FillVec3
    
    {
        auto maybeQuat = ToPB(
                              math::Quaternion(attitudeX, attitudeY, attitudeZ, attitudeW));
        if (!maybeQuat.IsOk()) { return {.error = maybeQuat.error}; }
        *msg.mutable_attitude() = std::move(*maybeQuat.value);
    }
    
    return {.value = msg};
}



} // namespace pbio
} // namespace io
} // namespace standard_cyborg
