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


#include "standard_cyborg/io/pbio/ImagePBIO.hpp"

#include <fmt/format.h>

#include <protobag/Utils/PBUtils.hpp>

#include "standard_cyborg/io/imgfile/ColorImageFileIO.hpp"
#include "standard_cyborg/util/DebugHelpers.hpp"

using Tensor = ::standard_cyborg::proto::math::Tensor;
using NumericType = ::standard_cyborg::proto::math::NumericType;

namespace standard_cyborg {
namespace io {
namespace pbio {

namespace detail {

// FromPB Helpers =============================================================

Result<ParsedImage> FromPBColor(const standard_cyborg::proto::sc3d::Image &msg) {
    using Image = ::standard_cyborg::proto::sc3d::Image;
    
    if (!msg.has_pixels()) { return {.error = "Invalid color image" }; }
    
    const Tensor &tensor_image = msg.pixels();
    const auto &props = tensor_image.properties();
    
    const bool is_HWC = 
    props.shape_size() == 3 &&
    props.shape(0).name() == "height" &&
    props.shape(1).name() == "width" &&
    props.shape(2).name() == "channels" &&
    
    props.shape(2).size() == 4 &&
    props.shape(2).field_names_size() >= 4 &&
    props.shape(2).field_names(0) == "r" &&
    props.shape(2).field_names(1) == "g" &&
    props.shape(2).field_names(2) == "b" &&
    props.shape(2).field_names(3) == "a";
    if (!is_HWC) {
        return {.error = fmt::format(
                                     "Image is not HWC RGBA or not indicated so: {}",
                                     protobag::PBToString(props))
        };
    }
    
    const int height = props.shape(0).size();
    const int width = props.shape(1).size();
    const int channels = props.shape(2).size();
    if (channels != 4) {
        return {.error = 
            fmt::format("Unsupported num channels {}", channels)
        };
    }
    
    sc3d::ColorImage ci(width, height);
    ci.setFrame(msg.frame());
    
    if (props.numeric_type() == NumericType::NUMERIC_TYPE_FLOAT) {
        
        const auto &floatvs = tensor_image.float_values();
        if (floatvs.size() != height * width * channels) {
            return {
                .error = fmt::format(
                                     "NUMERIC_TYPE_FLOAT color image has unexpected number of pixel values {}",
                                     floatvs.size())
            };
        }
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                const size_t p = (row * width * channels) + (col * channels);
                ci.setPixelAtColRow(
                                    col, row, math::Vec4(
                                                         floatvs[p + 0],
                                                         floatvs[p + 1],
                                                         floatvs[p + 2],
                                                         floatvs[p + 3]
                                                         ));
            }
        }
        
        if (msg.color_space() == Image::COLOR_SPACE_SRGB) {
            
            // ColorImage expects to be in linear gamma
            for (int row = 0; row < ci.getHeight(); ++row) {
                for (int col = 0; col < ci.getWidth(); ++col) {
                    const auto rgba = ci.getPixelAtColRow(col, row);
                    ci.setPixelAtColRow(
                                        col, row,
                                        math::Vec4(
                                                   sc3d::ApproximateSRGBGammaToLinear(rgba.xyz()),
                                                   rgba.w
                                                   ));
                }
            }
            
        } else if (msg.color_space() != Image::COLOR_SPACE_LINEAR) {
            return {.error = fmt::format(
                                         "Don't know how to convert image from color space {} to linear ",
                                         msg.color_space())
            };
        }
        
        // TODO(ricky) fix and test or remove
        // } else if (props.numeric_type() == NumericType::NUMERIC_TYPE_FLOAT_IEEE754_LITTLE_ENDIAN_BYTES) {
        //   const std::string &img_bytes = tensor_image.float_ieee754_little_endian_bytes();
        //   if (img_bytes.size() != height * width * channels * sizeof(float)) {
        //     return {
        //       .error = (
        //         "NUMERIC_TYPE_FLOAT_IEEE754_LITTLE_ENDIAN_BYTES image has unexpected "
        //         "number of bytes! Who would have guessed this might break one day?!")
        //     };
        //   }
        
        //   // TODO endianness check !!!!!!
        //   const float *buf = (const float *)img_bytes.data();
        //   std::vector<math::Vec4> vec4s(buf, img_bytes.size() / sizeof(float) + buf);
        //   ci.reset(width, height, vec4s);
        
        //   // for (int row = 0; row < height; ++row) {
        //   //   for (int col = 0; col < width; ++col) {
        //   //     const size_t p = (row * width * channels) + (col * channels);
        //   //     const float *pb = img_bytes.data();
        
        //   //     const float *pv = static_cast<const float *>(img_bytes.data() + (p * sizeof(float)));
        //   //     ci.setPixelAtColRow(
        //   //       col, row, {
        //   //         .x = *(pv + 0),
        //   //         .y = *(pv + 1),
        //   //         .z = *(pv + 2),
        //   //         .w = *(pv + 3),
        //   //       });
        //   //   }
        //   // }
        
    } else if (!tensor_image.jpeg_bytes().empty()) {
        
        bool success = imgfile::ReadColorImageFromBuffer(ci, (const uint8_t *)tensor_image.jpeg_bytes().data(), tensor_image.jpeg_bytes().size());
        if (!success) { return {.error = "STBI failed to read JPEG"}; }
        if (ci.getHeight() != height) { return {.error = "STBI read unexpected height"}; }
        if (ci.getWidth() != width) { return {.error = "STBI read unexpected width"}; }
        
    } else if (!tensor_image.png_bytes().empty()) {
        
        bool success = imgfile::ReadColorImageFromBuffer(ci, (const uint8_t *)tensor_image.png_bytes().data(), tensor_image.png_bytes().size());
        if (!success) { return {.error = "STBI failed to read PNG"}; }
        if (ci.getHeight() != height) { return {.error = "STBI read unexpected height"}; }
        if (ci.getWidth() != width) { return {.error = "STBI read unexpected width"}; }
        
    } else {
        return {.error = "Unsupported color image encoding"};
    }
    
    ParsedImage pi;
    pi.color_image = ci;
    return {.value = pi };
}


Result<ParsedImage> FromPBDepth(const ::standard_cyborg::proto::sc3d::Image &msg) {
    if (!msg.has_pixels()) { return {.error = "Invalid depth image" }; }
    
    const Tensor &tensor_image = msg.pixels();
    const auto &props = tensor_image.properties();
    
    const bool is_HWC_depth = 
    props.shape_size() == 3 &&
    props.shape(0).name() == "height" &&
    props.shape(1).name() == "width" &&
    props.shape(2).name() == "channels" &&
    
    props.shape(2).size() == 1 &&
    props.shape(2).field_names_size() >= 1 &&
    props.shape(2).field_names(0) == "depth";
    if (!is_HWC_depth) {
        return {.error = fmt::format(
                                     "Image is not HWC depth or not indicated so: {}",
                                     protobag::PBToString(props))
        };
    }
    
    const int height = props.shape(0).size();
    const int width = props.shape(1).size();
    const int channels = props.shape(2).size();
    if (channels != 1) {
        return {.error = 
            fmt::format("Unsupported num channels {}", channels)
        };
    }
    
    sc3d::DepthImage di(width, height);
    di.setFrame(msg.frame());
    
    if (props.numeric_type() == NumericType::NUMERIC_TYPE_FLOAT) {
        const auto &img_floats = tensor_image.float_values();
        if (img_floats.size() != height * width * channels) {
            return {
                .error = 
                "NUMERIC_TYPE_FLOAT image has unexpected number of depth bytes"
            };
        }
        
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                const size_t p = (row * width) + col;
                di.setPixelAtColRow(col, row, img_floats[p]);
            }
        }
        
    } else if (props.numeric_type() == NumericType::NUMERIC_TYPE_FLOAT_IEEE754_LITTLE_ENDIAN_BYTES) {
        const std::string &img_bytes = tensor_image.float_ieee754_little_endian_bytes();
        if (img_bytes.size() != height * width * channels * sizeof(float)) {
            return {
                .error = (
                          "NUMERIC_TYPE_FLOAT_IEEE754_LITTLE_ENDIAN_BYTES image has unexpected "
                          "number of bytes! Who would have guessed this might break one day?!")
            };
        }
        
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                const size_t p = (row * width * channels) + (col * channels);
                const float *pv = (const float *)(img_bytes.data()) + p;
                const float v = *pv;
                di.setPixelAtColRow(col, row, v);
            }
        }
        
    } else {
        return {.error = "Unsupported color image encoding"};
    }
    
    ParsedImage pi;
    pi.depth_image = di;
    return {.value = pi };
    
}

void SetRGBA(Tensor &tensor_image, size_t width, size_t height) {
    
    auto &props = *tensor_image.mutable_properties();
    auto &s_height = *props.add_shape();
    s_height.set_name("height");
    s_height.set_size(height);
    
    auto &s_width = *props.add_shape();
    s_width.set_name("width");
    s_width.set_size(width);
    
    auto &s_chan = *props.add_shape();
    s_chan.set_name("channels");
    s_chan.set_size(4);
    s_chan.add_field_names("r");
    s_chan.add_field_names("g");
    s_chan.add_field_names("b");
    s_chan.add_field_names("a");
    
}


} // namespace detail


// Main Impl ==================================================================

Result<ParsedImage> FromPB(const ::standard_cyborg::proto::sc3d::Image &msg) {
    using Image = ::standard_cyborg::proto::sc3d::Image;
    
    if (msg.color_space() == Image::COLOR_SPACE_UNKNOWN) {
        return {.error = "Image has unknown color space"};
    }
    
    const bool is_color = (
                           msg.color_space() == Image::COLOR_SPACE_LINEAR ||
                           msg.color_space() == Image::COLOR_SPACE_SRGB);
    if (is_color) {
        return detail::FromPBColor(msg);
    }
    
    
    const bool is_depth = (
                           msg.color_space() == Image::COLOR_SPACE_DEPTH
                           );
    if (is_depth) {
        return detail::FromPBDepth(msg);
    }
    
    return {.error = 
        std::string("Don't know how to handle given color space: ")
        + std::to_string(msg.color_space())
    };
    
}


Result<::standard_cyborg::proto::sc3d::Image> ToPB(
                                                   const sc3d::ColorImage &img,
                                                   PBColorImageFormat format) {
    
    using Image = ::standard_cyborg::proto::sc3d::Image;
    Image msg;
    msg.set_frame(img.getFrame());
    
    const auto &data = img.getData();
    const auto width = img.getWidth();
    const auto height = img.getHeight();
    if (data.size() != width * height) {
        return {.error = "Color image has unexpected number of pixels"};
    }
    
    Tensor &tensor_image = *msg.mutable_pixels();
    detail::SetRGBA(tensor_image, width, height);
    
    if (format == PBColorImageFormat::UNCOMPRESSED) {
        
        tensor_image.mutable_properties()->set_numeric_type(
                                                            NumericType::NUMERIC_TYPE_FLOAT);
        auto &tvs = *tensor_image.mutable_float_values();
        
        tvs.Resize(data.size() * 4, 0);
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                const size_t p = (row * width * 4) + (col * 4);
                const auto &pixel = img.getPixelAtColRow(col, row);
                tvs[p + 0] = pixel.x;;
                tvs[p + 1] = pixel.y;
                tvs[p + 2] = pixel.z;
                tvs[p + 3] = pixel.w;
            }
        }
        
        // ColorImage is natively Linear color
        msg.set_color_space(Image::COLOR_SPACE_LINEAR);
        
        // TODO(ricky) fix and test or remove
        // } else if (format == PBColorImageFormat::UNCOMPRESSED_NUMERIC_TYPE_FLOAT_IEEE754_LITTLE_ENDIAN_BYTES) {
        //   tensor_image.mutable_properties()->set_numeric_type(
        //         NumericType::NUMERIC_TYPE_FLOAT_IEEE754_LITTLE_ENDIAN_BYTES);
        
        //   {
        //     std::vector<float> flattenedVec4s; // NB this needs little endian check !!! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //     flattenedVec4s.reserve(data.size() * 4);
        
        //     for (const math::Vec4& v: data) {
        //       flattenedVec4s.push_back(v.x);
        //       flattenedVec4s.push_back(v.y);
        //       flattenedVec4s.push_back(v.z);
        //       flattenedVec4s.push_back(v.w);
        //     }
        //     tensor_image.set_float_ieee754_little_endian_bytes(
        //       flattenedVec4s.data(),
        //       flattenedVec4s.size() * sizeof(decltype(flattenedVec4s.back())));
        //   }
        
    } else if (format == PBColorImageFormat::JPEG) {
        
        std::string &out = *tensor_image.mutable_jpeg_bytes();
        bool success = imgfile::WriteColorImageToBuffer(out, img, imgfile::ImageFormat::JPEG, 100);
        if (!success) {
            return {.error = "STBI write failed"};
        }
        tensor_image.mutable_properties()->set_numeric_type(
                                                            NumericType::NUMERIC_TYPE_JPEG_BYTES);
        
        // JPeg is sRGB color
        msg.set_color_space(Image::COLOR_SPACE_SRGB);
        
    } else if (format == PBColorImageFormat::PNG) {
        
        std::string &out = *tensor_image.mutable_png_bytes();
        bool success = imgfile::WriteColorImageToBuffer(out, img, imgfile::ImageFormat::PNG);
        if (!success) {
            return {.error = "STBI write failed"};
        }
        tensor_image.mutable_properties()->set_numeric_type(
                                                            NumericType::NUMERIC_TYPE_PNG_BYTES);
        
        // PNG is sRGB color
        msg.set_color_space(Image::COLOR_SPACE_SRGB);
        
    } else {
        return { .error = fmt::format(
                                      "Unsupported color image format {}", format)
        };
    }
    
    return {.value = msg};
}



Result<::standard_cyborg::proto::sc3d::Image> ToPB(
                                                   const sc3d::DepthImage &img,
                                                   PBDepthImageFormat format) {
    
    using Image = ::standard_cyborg::proto::sc3d::Image;
    Image msg;
    msg.set_frame(img.getFrame());
    msg.set_color_space(Image::COLOR_SPACE_DEPTH);
    
    const auto &data = img.getData();
    const auto width = img.getWidth();
    const auto height = img.getHeight();
    if (data.size() != width * height) {
        return {.error = "Depth image has unexpected number of pixels"};
    }
    
    Tensor &tensor_image = *msg.mutable_pixels();
    
    // Set 1-channel
    {
        auto &props = *tensor_image.mutable_properties();
        auto &s_height = *props.add_shape();
        s_height.set_name("height");
        s_height.set_size(height);
        
        auto &s_width = *props.add_shape();
        s_width.set_name("width");
        s_width.set_size(width);
        
        auto &s_chan = *props.add_shape();
        s_chan.set_name("channels");
        s_chan.set_size(1);
        s_chan.add_field_names("depth");
    }
    
    if (format == PBDepthImageFormat::UNCOMPRESSED) {
        
        tensor_image.mutable_properties()->set_numeric_type(
                                                            NumericType::NUMERIC_TYPE_FLOAT);
        auto &tvs = *tensor_image.mutable_float_values();
        
        tvs.Resize(data.size(), 0);
        for (size_t i = 0; i < data.size(); ++i) { tvs[i] = data[i]; }
        
        // TODO(ricky) fix and test or remove
        // } else if (format == PBDepthImageFormat::UNCOMPRESSED_NUMERIC_TYPE_FLOAT_IEEE754_LITTLE_ENDIAN_BYTES) {
        
        //   tensor_image.mutable_properties()->set_numeric_type(
        //         NumericType::NUMERIC_TYPE_FLOAT_IEEE754_LITTLE_ENDIAN_BYTES);
        //   tensor_image.set_float_ieee754_little_endian_bytes(
        //     data.data(), data.size() * sizeof(decltype(data.back()))); // NB: needs endianness check !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        
    } else {
        return {
            .error = fmt::format("Unsupported depth image format {}", format)
        };
    }
    
    return {.value = msg};
}


bool operator==(const ParsedImage &lhs, const ParsedImage &rhs) {
    bool equal = true;
    if (lhs.color_image.has_value()) {
        equal &= (
                  rhs.color_image.has_value() && (
                                                  lhs.color_image.value() == rhs.color_image.value()));
    }
    if (lhs.depth_image.has_value()) {
        equal &= (
                  rhs.depth_image.has_value() && (
                                                  lhs.depth_image.value() == rhs.depth_image.value()));
    }
    return equal;
}


// Other ======================================================================

extern Result<::standard_cyborg::proto::sc3d::Image> ToPBFromJpegBytes(
                                                                       const size_t image_width,
                                                                       const size_t image_height,
                                                                       const std::string &image_frame,
                                                                       std::string &&jpeg_bytes) {
    
    using Image = ::standard_cyborg::proto::sc3d::Image;
    
    Image msg;
    msg.set_frame(image_frame);
    
    msg.set_color_space(Image::COLOR_SPACE_SRGB);
    // NB: We assume the given jpeg is in SRGB color space
    
    Tensor &tensor_image = *msg.mutable_pixels();
    detail::SetRGBA(tensor_image, image_width, image_height);
    
    tensor_image.set_jpeg_bytes(std::move(jpeg_bytes));
    tensor_image.mutable_properties()->set_numeric_type(
                                                        NumericType::NUMERIC_TYPE_JPEG_BYTES);
    
    return {.value = msg};
}



} // namespace pbio
} // namespace io
} // namespace standard_cyborg


extern std::ostream& operator<<(std::ostream& os, const standard_cyborg::io::pbio::ParsedImage& pi) {
    os << "ParsedImage{\n";
    if (pi.color_image.has_value()) { os << pi.color_image.value(); }
    if (pi.depth_image.has_value()) { os << pi.depth_image.value(); }
    os << "}";
    return os;
}
