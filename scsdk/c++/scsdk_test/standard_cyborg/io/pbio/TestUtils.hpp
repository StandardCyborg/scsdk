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
#include <sstream>

#include <protobag/Utils/PBUtils.hpp>


#include "standard_cyborg/util/DebugHelpers.hpp"

// Emit literal macro param as a string
#define SC_TEST_STRINGIFY(X) #X


// dunno why ::testing::PrintToString doesn't find the right operator<<
template <typename T>
std::string SCToString(const T &v) {
  std::stringstream ss;
  ss << v; // Uses DebugHelpers
  return ss.str();
}

namespace standard_cyborg {

/**
 * Helper for `CheckToFromPBBinaryRoundTrip()` below that allows the user
 * to call their own ToPB() before test checks.
 */
template <typename T, typename MT>
void CheckToFromPBBinaryRoundTrip(const T &value, const MT &msg) {
  auto maybe_buf = protobag::PBFactory::ToBinaryString(msg);
  EXPECT_TRUE(maybe_buf.IsOk()) << 
      std::string("Failed to serialze to pb binary: ") + maybe_buf.error;
  
  auto maybe_decoded = 
    protobag::PBFactory::LoadFromContainer<MT>(*maybe_buf.value);
  EXPECT_TRUE(maybe_decoded.IsOk()) <<
      std::string("Failed to deserialze from pb binary: ") + maybe_decoded.error;
  
  auto maybe_decoded_value = io::pbio::FromPB(*maybe_decoded.value);
  EXPECT_TRUE(maybe_decoded_value.IsOk()) <<
      std::string("Failed to convert from PB: ") + maybe_decoded_value.error;
  auto decoded_value = *maybe_decoded_value.value;
  
  EXPECT_EQ(value, decoded_value) <<
    "CheckToFromPBBinaryRoundTrip Failure \n" <<
    "Expected: \n" << SCToString(value) << 
    "\n\nDecoded:\n" << SCToString(decoded_value);
}


/**
 * Given a sc3d / math object `value` (or anything with a ToPB and FromPB
 * pair defined), test encoding and decoding from binary format.  Expects
 * the decoded object to be equal to the given `value`.
 */
template <typename T>
void CheckToFromPBBinaryRoundTrip(const T &value) {
  auto maybe_msg = io::pbio::ToPB(value);
  EXPECT_TRUE(maybe_msg.IsOk()) <<
      std::string("Failed to convert to PB: ") + maybe_msg.error;
  const auto &msg = *maybe_msg.value;
  CheckToFromPBBinaryRoundTrip(value, msg);
}


/**
 * Helper for `CheckToFromPBText()` below that allows user to apply
 * their own `ToPB()` function first.
 */
template <typename T, typename MT>
void CheckToFromPBText(const T &value, const std::string &expected_pbtxt, const MT &msg) {
  
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
    protobag::PBFactory::LoadFromString<MT>(expected_pbtxt);
  EXPECT_TRUE(maybe_decoded.IsOk()) <<
      std::string("Failed to deserialze from pb binary: ") + maybe_decoded.error;
  
  auto maybe_decoded_value = io::pbio::FromPB(*maybe_decoded.value);
  EXPECT_TRUE(maybe_decoded_value.IsOk()) <<
      std::string("Failed to convert from PB: ") + maybe_decoded_value.error;
  auto decoded_value = *maybe_decoded_value.value;
  
  EXPECT_EQ(value, decoded_value) <<
    "pbtext -> value failure: \n" <<
    "Expected: \n" << SCToString(value) << 
    "\n\nDecoded:\n" << SCToString(decoded_value);
}


/**
 * Given a sc3d / math object `value` (or anything with a ToPB and FromPB
 * pair defined) and a `expected_pbtxt` protobuf text format string, ensure
 * each is equivalent to the other.  To generate a `expected_pbtxt` fixture,
 * try using protobag::PBToString().
 */
template <typename T>
void CheckToFromPBText(const T &value, const std::string &expected_pbtxt) {
  
  /// Check value -> pbtxt
  auto maybe_msg = io::pbio::ToPB(value);
  EXPECT_TRUE(maybe_msg.IsOk()) <<
      std::string("Failed to convert to PB: ") + maybe_msg.error;
  const auto &msg = *maybe_msg.value;
  CheckToFromPBText(value, expected_pbtxt, msg);
}


} // namespace standard_cyborg
