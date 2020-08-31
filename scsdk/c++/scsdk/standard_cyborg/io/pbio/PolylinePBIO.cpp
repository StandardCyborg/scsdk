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

#include "standard_cyborg/io/pbio/PolylinePBIO.hpp"

#include "standard_cyborg/sc3d/Polyline.hpp"

using Tensor = ::standard_cyborg::proto::math::Tensor;
using NumericType = ::standard_cyborg::proto::math::NumericType;

namespace standard_cyborg {
namespace io {
namespace pbio {

Result<sc3d::Polyline> FromPB(const standard_cyborg::proto::sc3d::Polyline &msg) {
    std::vector<math::Vec3> positions;  
    
    for (int j = 0; j < msg.positions_size(); j++) {
        const auto& position = msg.positions(j);
        
        float x = position.x();
        float y = position.y();
        float z = position.z();
        
        math::Vec3 v(x, y, z);
        
        positions.push_back(v);
    }
    
    sc3d::Polyline polyline(positions);
    
    return {.value = polyline};
}

Result<standard_cyborg::proto::sc3d::Polyline> ToPB(const sc3d::Polyline &polyline) {
    
    standard_cyborg::proto::sc3d::Polyline protoPolyline;
    
    const std::vector<math::Vec3>& positions = polyline.getPositions();
    
    for (int iVertex = 0; iVertex < positions.size(); ++iVertex) {
        const auto &position = positions[iVertex];
        
        auto* newPosition = protoPolyline.add_positions();
        
        newPosition->set_x(position.x);
        newPosition->set_y(position.y);
        newPosition->set_z(position.z);
    }
    
    return {.value = protoPolyline};
    
}

} // namespace pbio
} // namespace io
} // namespace standard_cyborg
