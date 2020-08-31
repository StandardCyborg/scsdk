#include <gtest/gtest.h>

#include <exception>

#include <protobag/Utils/PBUtils.hpp>


#include "standard_cyborg/sc3d/ColorImage.hpp"
#include "standard_cyborg/sc3d/DepthImage.hpp"
#include "standard_cyborg/io/pbio/ImagePBIO.hpp"
#include "standard_cyborg/io/pbio/TestUtils.hpp"
#include "standard_cyborg/test_helpers/TestHelpers.hpp"

using namespace standard_cyborg;
using namespace standard_cyborg::io::pbio;


// Utils ======================================================================

template <typename ResultPBT>
auto GetPBOrThrow(const ResultPBT &result_msg) {
    if (!result_msg.IsOk()) {
        throw std::runtime_error(result_msg.error);
    }
    return *result_msg.value;
}

// Hack of TestUtils.hpp stuff to support approx match
void CheckColorImageClose(
                          const ParsedImage &value,
                          const ParsedImage &decoded_value,
                          float tolerance) {
    if (decoded_value.color_image.has_value()) {
        
        EXPECT_TRUE(value.color_image.has_value());
        const auto &ci = value.color_image.value();
        const auto &decoded_ci = *decoded_value.color_image;
        EXPECT_EQ(ci.getFrame(), decoded_ci.getFrame());
        EXPECT_EQ(ci.getWidth(), decoded_ci.getWidth());
        EXPECT_EQ(ci.getHeight(), decoded_ci.getHeight());
        
        const auto actual = decoded_ci.getData();
        const auto expected = ci.getData();
        EXPECT_EQ(actual.size(), expected.size());
        for (int i = 0; i < expected.size(); i++) {
            EXPECT_TRUE(math::Vec4::almostEqual(expected[i], actual[i], tolerance)) <<
            "Expected: \n" << SCToString(value) <<
            "\n\nDecoded:\n" << SCToString(decoded_value);
        }
        
    } else {
        EXPECT_TRUE(false) << "No decoded color image";
    }
}

// Hack of TestUtils.hpp stuff to support approx match
void CheckCloseToFromPBBinaryRoundTrip(
                                       const ParsedImage &value,
                                       const ::standard_cyborg::proto::sc3d::Image &msg,
                                       float tolerance) {
    
    auto maybe_buf = protobag::PBFactory::ToBinaryString(msg);
    EXPECT_TRUE(maybe_buf.IsOk()) <<
    std::string("Failed to serialze to pb binary: ") + maybe_buf.error;
    
    auto maybe_decoded =
    protobag::PBFactory::LoadFromContainer<::standard_cyborg::proto::sc3d::Image>(*maybe_buf.value);
    EXPECT_TRUE(maybe_decoded.IsOk()) <<
    std::string("Failed to deserialze from pb binary: ") + maybe_decoded.error;
    
    auto maybe_decoded_value = io::pbio::FromPB(*maybe_decoded.value);
    EXPECT_TRUE(maybe_decoded_value.IsOk()) <<
    std::string("Failed to convert from PB: ") + maybe_decoded_value.error;
    auto decoded_value = *maybe_decoded_value.value;
    
    CheckColorImageClose(value, decoded_value, tolerance);
    
}

// Hack of TestUtils.hpp stuff to support approx match
void CheckCloseToFromPBText(
                            const ParsedImage &value,
                            const std::string &expected_pbtxt, const ::standard_cyborg::proto::sc3d::Image &msg,
                            float tolerance) {
    
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
    protobag::PBFactory::LoadFromString<::standard_cyborg::proto::sc3d::Image>(expected_pbtxt);
    EXPECT_TRUE(maybe_decoded.IsOk()) <<
    std::string("Failed to deserialze from pb binary: ") + maybe_decoded.error;
    
    auto maybe_decoded_value = io::pbio::FromPB(*maybe_decoded.value);
    EXPECT_TRUE(maybe_decoded_value.IsOk()) <<
    std::string("Failed to convert from PB: ") + maybe_decoded_value.error;
    auto decoded_value = *maybe_decoded_value.value;
    
    CheckColorImageClose(value, decoded_value, tolerance);
}




// ColorImage =================================================================

const std::string kTestColorEmptyPBTXT =
R"(frame: "test"
pixels {
  properties {
    numeric_type: NUMERIC_TYPE_FLOAT
    shape {
      name: "height"
    }
    shape {
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
}
color_space: COLOR_SPACE_LINEAR
)";


TEST(ImagePBIOTest, TestColorImageEmpty) {
    sc3d::ColorImage ci;
    ci.setFrame("test");
    CheckToFromPBBinaryRoundTrip(ParsedImage{.color_image=ci});
    CheckToFromPBText(ParsedImage{.color_image=ci}, kTestColorEmptyPBTXT);
}


sc3d::ColorImage CreateTestImg() {
    sc3d::ColorImage ci(3, 4);
    ci.setFrame("test");
    for (int r = 0; r < ci.getHeight(); ++r) {
        for (int c = 0; c < ci.getWidth(); ++c) {
            const float vx = c / float(ci.getWidth());
            const float vy = r / float(ci.getHeight());
            const float alpha = std::sqrt(vx*vx + vy*vy);
            ci.setPixelAtColRow(c, r, math::Vec4(vx, vy, 0, alpha));
        }
    }
    return ci;
}

const std::string kTestColorBTxt_UNCOMPRESSED =
R"(frame: "test"
pixels {
  properties {
    numeric_type: NUMERIC_TYPE_FLOAT
    shape {
      size: 4
      name: "height"
    }
    shape {
      size: 3
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
  float_values: 0
  float_values: 0
  float_values: 0
  float_values: 0.333333343
  float_values: 0
  float_values: 0
  float_values: 0.333333343
  float_values: 0.666666687
  float_values: 0
  float_values: 0
  float_values: 0.666666687
  float_values: 0
  float_values: 0.25
  float_values: 0
  float_values: 0.25
  float_values: 0.333333343
  float_values: 0.25
  float_values: 0
  float_values: 0.416666687
  float_values: 0.666666687
  float_values: 0.25
  float_values: 0
  float_values: 0.71200031
  float_values: 0
  float_values: 0.5
  float_values: 0
  float_values: 0.5
  float_values: 0.333333343
  float_values: 0.5
  float_values: 0
  float_values: 0.600925207
  float_values: 0.666666687
  float_values: 0.5
  float_values: 0
  float_values: 0.833333373
  float_values: 0
  float_values: 0.75
  float_values: 0
  float_values: 0.75
  float_values: 0.333333343
  float_values: 0.75
  float_values: 0
  float_values: 0.820738137
  float_values: 0.666666687
  float_values: 0.75
  float_values: 0
  float_values: 1.00346625
}
color_space: COLOR_SPACE_LINEAR
)";


TEST(ImagePBIOTest, TestColorImage_UNCOMPRESSED) {
    sc3d::ColorImage ci = CreateTestImg();
    auto msg = GetPBOrThrow(ToPB(ci, PBColorImageFormat::UNCOMPRESSED));
    CheckToFromPBBinaryRoundTrip(ParsedImage{.color_image=ci}, msg);
    CheckToFromPBText(ParsedImage{.color_image=ci}, kTestColorBTxt_UNCOMPRESSED, msg);
}


const std::string kTestColorBTxt_PNG =
R"(frame: "test"
pixels {
  properties {
    numeric_type: NUMERIC_TYPE_PNG_BYTES
    shape {
      size: 3
      name: "height"
    }
    shape {
      size: 2
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
  png_bytes: "\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000\002\000\000\000\003\010\006\000\000\000\271\352\336\201\000\000\000$IDATx^cdh\337\265\277JQ\200\227%\222\207\341s\253\267\314g\306\377\237\237mb```\000\000\200\034\t\200\023)\177\025\000\000\000\000IEND\256B`\202"
}
color_space: COLOR_SPACE_SRGB
)";


TEST(ImagePBIOTest, TestColorImage_PNG) {
    sc3d::ColorImage ci = sc3d::ColorImage(2, 3, std::vector<math::Vec4>{
        {0.0, 0.25, 0.5, 0.75},
        {0.2, 0.4, 0.6, 0.8},
        {0.1, 0.3, 0.5, 0.7},
        {1.0, 0.9, 0.8, 0.7},
        {1.0, 0.9, 0.8, 0.7},
        {1.0, 0.9, 0.8, 0.7},
    });
    ci.setFrame("test");
    
    auto msg = GetPBOrThrow(ToPB(ci, PBColorImageFormat::PNG));
    CheckCloseToFromPBBinaryRoundTrip(ParsedImage{.color_image=ci}, msg, 4e-3);
    CheckCloseToFromPBText(ParsedImage{.color_image=ci}, kTestColorBTxt_PNG, msg, 4e-3);
}


const std::string kTestColorBTxt_JPEG =
R"(frame: "test"
pixels {
  properties {
    numeric_type: NUMERIC_TYPE_JPEG_BYTES
    shape {
      size: 3
      name: "height"
    }
    shape {
      size: 2
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
  jpeg_bytes: "\377\330\377\340\000\020JFIF\000\001\001\000\000\001\000\001\000\000\377\333\000\204\000\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\001\377\300\000\021\010\000\003\000\002\003\001\021\000\002\021\001\003\021\001\377\304\001\242\000\000\001\005\001\001\001\001\001\001\000\000\000\000\000\000\000\000\001\002\003\004\005\006\007\010\t\n\013\020\000\002\001\003\003\002\004\003\005\005\004\004\000\000\001}\001\002\003\000\004\021\005\022!1A\006\023Qa\007\"q\0242\201\221\241\010#B\261\301\025R\321\360$3br\202\t\n\026\027\030\031\032%&\'()*456789:CDEFGHIJSTUVWXYZcdefghijstuvwxyz\203\204\205\206\207\210\211\212\222\223\224\225\226\227\230\231\232\242\243\244\245\246\247\250\251\252\262\263\264\265\266\267\270\271\272\302\303\304\305\306\307\310\311\312\322\323\324\325\326\327\330\331\332\341\342\343\344\345\346\347\350\351\352\361\362\363\364\365\366\367\370\371\372\001\000\003\001\001\001\001\001\001\001\001\001\000\000\000\000\000\000\001\002\003\004\005\006\007\010\t\n\013\021\000\002\001\002\004\004\003\004\007\005\004\004\000\001\002w\000\001\002\003\021\004\005!1\006\022AQ\007aq\023\"2\201\010\024B\221\241\261\301\t#3R\360\025br\321\n\026$4\341%\361\027\030\031\032&\'()*56789:CDEFGHIJSTUVWXYZcdefghijstuvwxyz\202\203\204\205\206\207\210\211\212\222\223\224\225\226\227\230\231\232\242\243\244\245\246\247\250\251\252\262\263\264\265\266\267\270\271\272\302\303\304\305\306\307\310\311\312\322\323\324\325\326\327\330\331\332\342\343\344\345\346\347\350\351\352\362\363\364\365\366\367\370\371\372\377\332\000\014\003\001\000\002\021\003\021\000?\000\376\317t/\331\263\366|\360\336\207\243xwH\370)\360\276\035\'@\322\264\355\027K\206\347\301\036\036\324\356b\323\264\253Hll\242\270\324\265K\013\315OP\236;h\"Io\265\033\313\253\353\271\003Ows=\304\222J\336&/\2168\313\033\212\304\343q<S\304\023\304b\361\025\261X\211\3076\306\321\214\353b*J\255YF\215\032\324\350\322\214\24795N\2258R\202|\264\341\030\245\025\333\200\340N\n\31308,\267\003\302|;C\005\227\3410\370\034\035\005\224`f\250\341p\224aC\017IN\255\t\324\232\247J\234 \245Rs\234\255y\312Rm\277\377\331"
}
color_space: COLOR_SPACE_SRGB
)";


TEST(ImagePBIOTest, TestColorImage_JPEG) {
    sc3d::ColorImage ci = sc3d::ColorImage(2, 3, std::vector<math::Vec4>{
        {0.0, 0.25, 0.5, 0.75},
        {0.2, 0.4, 0.6, 0.8},
        {0.1, 0.3, 0.5, 0.7},
        {1.0, 0.9, 0.8, 0.7},
        {1.0, 0.9, 0.8, 0.7},
        {1.0, 0.9, 0.8, 0.7},
    });
    ci.setFrame("test");
    
    auto msg = GetPBOrThrow(ToPB(ci, PBColorImageFormat::JPEG));
    CheckCloseToFromPBBinaryRoundTrip(ParsedImage{.color_image=ci}, msg, 0.3);
    CheckCloseToFromPBText(ParsedImage{.color_image=ci}, kTestColorBTxt_JPEG, msg, 0.3);
}





// DepthImage =================================================================

const std::string kTestDepthEmptyPBTXT =
R"(frame: "test"
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
)";

TEST(ImagePBIOTest, TestDepthImageEmpty) {
    sc3d::DepthImage di(1, 1,  std::vector<float>{0.5f} );
    di.setFrame("test");
    CheckToFromPBBinaryRoundTrip(ParsedImage{.depth_image=di});
    CheckToFromPBText(ParsedImage{.depth_image=di}, kTestDepthEmptyPBTXT);
}
