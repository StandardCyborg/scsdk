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

#include "standard_cyborg/io/pbio/PerspectiveCameraPBIO.hpp"

#include "standard_cyborg/sc3d/PerspectiveCamera.hpp"

#include "standard_cyborg/io/pbio/TransformPBIO.hpp"

namespace standard_cyborg {
namespace io {
namespace pbio {

Result<sc3d::PerspectiveCamera> FromPB(const ::standard_cyborg::proto::sc3d::PinholeCamera &msg) {
    sc3d::PerspectiveCamera camera;
    
    if (msg.frame().empty()) {
        return {.error = "Camera has no frame"};
    }
    camera.setFrame(msg.frame());
    
    {
        auto fx = msg.focal_length().x();
        auto fy = msg.focal_length().y();
        auto cx = msg.principal_point().x();
        auto cy = msg.principal_point().y();
        auto s = msg.sensor_skew();
        camera.setNominalIntrinsicMatrix(
                                         math::Mat3x3(
                                                      fx, s,  cx,
                                                      0,  fy, cy,
                                                      0,  0,  1
                                                      ));
    }
    
    camera.setIntrinsicMatrixReferenceSize(
                                           math::Vec2(
                                                      msg.image_width(),
                                                      msg.image_height()
                                                      ));
    
    {
        auto maybe_transform = FromPB(msg.sensor_transform());
        if (!maybe_transform.IsOk()) {
            return {.error = maybe_transform.error};
        }
        camera.setExtrinsicMatrix(
                                  math::Mat3x4::fromTransform(*maybe_transform.value));
    }
    
    // Note: pixel pitch is unused
    
    {
        std::vector<float> calibration;
        calibration.resize(msg.radial_distortion_coefficients_size());
        for (int i = 0; i < msg.radial_distortion_coefficients_size(); ++i) {
            calibration[i] = msg.radial_distortion_coefficients(i);
        }
        camera.setLensDistortionLookupTable(calibration);
    }
    
    return {.value = camera};
}


Result<::standard_cyborg::proto::sc3d::PinholeCamera> ToPB(const sc3d::PerspectiveCamera &camera) {
    ::standard_cyborg::proto::sc3d::PinholeCamera msg;
    
    if (camera.getFrame().empty()) {
        return {.error = "Must give your camera a frame"};
    }
    msg.set_frame(camera.getFrame());
    
    {
        const auto &K = camera.getNominalIntrinsicMatrix();
        msg.mutable_focal_length()->set_x(K.m00);
        msg.mutable_focal_length()->set_y(K.m11);
        
        msg.mutable_principal_point()->set_x(K.m02);
        msg.mutable_principal_point()->set_y(K.m12);
        
        msg.set_sensor_skew(K.m01);
    }
    
    {
        const auto &size = camera.getIntrinsicMatrixReferenceSize();
        msg.set_image_width(size.x);
        msg.set_image_height(size.y);
    }
    
    {
        const auto &RT = camera.getExtrinsicMatrix();
        auto maybe_transform = ToPB(math::Transform::fromMat3x4(RT));
        if (!maybe_transform.IsOk()) {
            return {.error = maybe_transform.error};
        }
        *msg.mutable_sensor_transform() = *maybe_transform.value;
    }
    
    // Note: pixel pitch is unused
    
    {
        const auto &coeffs = camera.getLensDistortionCalibration();
        msg.mutable_radial_distortion_coefficients()->Reserve(coeffs.size());
        for (const auto &v : coeffs) {
            msg.add_radial_distortion_coefficients(v);
        }
    }
    
    return {.value = msg};
}

} // namespace pbio
} // namespace io
} // namespace standard_cyborg
