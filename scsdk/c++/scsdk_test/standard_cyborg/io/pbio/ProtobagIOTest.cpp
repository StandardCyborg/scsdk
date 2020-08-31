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

#include <string>
#include <vector>

#include <protobag/Protobag.hpp>
#include <protobag/Utils/StdMsgUtils.hpp>

#include "standard_cyborg/io/pbio/ImagePBIO.hpp"
#include "standard_cyborg/io/pbio/IMUPBIO.hpp"
#include "standard_cyborg/io/pbio/PerspectiveCameraPBIO.hpp"
#include "standard_cyborg/io/pbio/ProtobagIO.hpp"
#include "standard_cyborg/sc3d/ColorImage.hpp"
#include "standard_cyborg/sc3d/DepthImage.hpp"
#include "standard_cyborg/sc3d/PerspectiveCamera.hpp"


using namespace standard_cyborg;
using namespace standard_cyborg::io::pbio;
namespace sg = standard_cyborg::scene_graph;

#define EXPECT_SORTED_SEQUENCES_EQUAL(expected, actual) do { \
auto e2 = expected; \
auto a2 = actual; \
std::sort(e2.begin(), e2.end()); \
std::sort(a2.begin(), a2.end()); \
EXPECT_EQ(a2, e2); \
} while(0)

template <typename EntryContainerT>
void WriteEntriesToPath(
                        const std::string &path,
                        const EntryContainerT &entries) {
    
    protobag::Protobag bag(path);
    auto maybeWriter = bag.StartWriteSession();
    if (!maybeWriter.IsOk()) {
        throw std::runtime_error(maybeWriter.error);
    }
    auto writer = *maybeWriter.value;
    
    for (const auto &entry : entries) {
        auto status = writer->WriteEntry(entry);
        if (!status.IsOk()) {
            throw std::runtime_error(status.error);
        }
    }
}

TEST(ProtobagIOTest, TestNoSceneGraphs) {
    static const std::string path = "/tmp/ProtobagIOTest.TestNoSceneGraphs.zip";
    
    // Create a bag with some messages that do *not* translate to scene
    // graph nodes
    {
        static const std::vector<protobag::Entry> kEntries = {
            protobag::Entry::Create("/moof", protobag::ToStringMsg("moof")),
            protobag::Entry::Create("/hi_1337", protobag::ToIntMsg(1337)),
        };
        
        WriteEntriesToPath(path, kEntries);
    }
    
    // Now test that we read zero scene graphs
    {
        auto maybeRoot = ReadSceneGraphFromProtobag(path);
        ASSERT_TRUE(maybeRoot.IsOk()) << maybeRoot.error;
        
        auto root = *maybeRoot.value;
        EXPECT_EQ(root->numChildren(), 0);
    }
    
    {
        auto iter = IterSceneGraphsFromProtobag(path);
        bool reading = true;
        size_t num_read = 0;
        while (reading) {
            auto maybeNext = iter.GetNext();
            if (maybeNext.IsEndOfSequence()) {
                reading = false;
                break;
            }
            
            ASSERT_TRUE(maybeNext.IsOk()) << maybeNext.error;
            num_read += 1;
        }
        
        EXPECT_EQ(num_read, 0);
    }
}

TEST(ProtobagIOTest, Test2SceneGraphs) {
    static const std::string path = "/tmp/ProtobagIOTest.Test2SceneGraphs.zip";
    
    /// Dummy data to write to the bag
    sc3d::ColorImage ci = sc3d::ColorImage(1, 1, std::vector<math::Vec4>{
        {0.0, 0.25, 0.5, 0.75},
    });
    ci.setFrame("test");
    
    auto maybeCIMsg = io::pbio::ToPB(ci);
    ASSERT_TRUE(maybeCIMsg.IsOk()) << maybeCIMsg.error;
    auto CIMsg = *maybeCIMsg.value;
    
    
    sc3d::DepthImage di = sc3d::DepthImage(1, 1, std::vector<float>{ 0.5, });
    di.setFrame("test");
    
    auto maybeDIMsg = io::pbio::ToPB(di);
    ASSERT_TRUE(maybeDIMsg.IsOk()) << maybeDIMsg.error;
    auto DIMsg = *maybeDIMsg.value;
    
    
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
    
    auto maybePCMsg = io::pbio::ToPB(camera);
    ASSERT_TRUE(maybePCMsg.IsOk()) << maybePCMsg.error;
    auto cameraMsg = *maybePCMsg.value;
    
    
    
    /// Create bag fixture
    // Create a bag with some messages that do *not* translate to scene
    // graph nodes, as well as some that *do*
    {
        static const std::vector<protobag::Entry> kEntries = {
            // This is extra stuff in the bag; in practice, it could be
            // metatadata or other stuff that does not translate automatically
            // to SceneGraph nodes
            protobag::Entry::Create("/moof", protobag::ToStringMsg("moof")),
            protobag::Entry::Create("/hi_1337", protobag::ToIntMsg(1337)),
            
            // Here are two synced RGB-D bundles that *will* translate to scene
            // graph nodes
            // Bundle 1 at around time 0sec 0ns
            protobag::Entry::CreateStamped(
                                           "/front_camera/info",
                                           0, 0,
                                           cameraMsg),
            protobag::Entry::CreateStamped(
                                           "/front_camera/image",
                                           0, 0,
                                           CIMsg),
            protobag::Entry::CreateStamped(
                                           "/front_camera/depth",
                                           0, 1000, // Depth is slightly off; exercise sync logic
                                           DIMsg),
            
            // Bundle 2 at around time 0sec 1,000,000 ns (100ms)
            protobag::Entry::CreateStamped(
                                           "/front_camera/info",
                                           0, 0 + 1000000,
                                           cameraMsg),
            protobag::Entry::CreateStamped(
                                           "/front_camera/image",
                                           0, 0 + 1000000,
                                           CIMsg),
            protobag::Entry::CreateStamped(
                                           "/front_camera/depth",
                                           0, 1000 + 1000000, // Depth is slightly off; exercise sync logic
                                           DIMsg),
        };
        
        WriteEntriesToPath(path, kEntries);
    }
    
    
    /// Read Scene Graph(s)
    
    // Test that we can read one big scene graph
    {
        auto maybeRoot = ReadSceneGraphFromProtobag(path);
        ASSERT_TRUE(maybeRoot.IsOk()) << maybeRoot.error;
        
        auto root = *maybeRoot.value;
        
        static const std::vector<std::string> kExpectedChildNames = {
            "/front_camera/info:0.0",
            "/front_camera/image:0.0",
            "/front_camera/depth:0.1000",
            
            "/front_camera/info:0.1000000",
            "/front_camera/image:0.1000000",
            "/front_camera/depth:0.1001000",
        };
        
        std::vector<std::string> actualChildNames;
        for (size_t ch = 0; ch < root->numChildren(); ++ch) {
            auto node = root->getChildSharedPtr(ch);
            actualChildNames.push_back(node->getName());
            
            // Check contents
            if (node->isColorImageNode()) {
                EXPECT_EQ(node->asColorImageNode()->getColorImage(), ci);
            } else if (node->isDepthImageNode()) {
                EXPECT_EQ(node->asDepthImageNode()->getDepthImage(), di);
            } else if (node->isPerspectiveCameraNode()) {
                EXPECT_EQ(node->asPerspectiveCameraNode()->getPerspectiveCamera(), camera);
            }
        }
        
        EXPECT_SORTED_SEQUENCES_EQUAL(kExpectedChildNames, actualChildNames);
    }
    
    // Test that we can read one scene graph per bundle
    {
        auto iter = IterSceneGraphsFromProtobag(path);
        std::vector<std::shared_ptr<sg::Node>> roots;
        bool reading = true;
        while (reading) {
            auto maybeNext = iter.GetNext();
            if (maybeNext.IsEndOfSequence()) {
                reading = false;
                break;
            }
            
            ASSERT_TRUE(maybeNext.IsOk()) << maybeNext.error;
            roots.push_back(*maybeNext.value);
        }
        
        // Inspect bundle 1
        ASSERT_TRUE(roots.size() >= 1);
        {
            auto root = roots.at(0);
            static const std::vector<std::string> kExpectedChildNames = {
                "/front_camera/info:0.0",
                "/front_camera/image:0.0",
                "/front_camera/depth:0.1000",
            };
            
            std::vector<std::string> actualChildNames;
            for (size_t ch = 0; ch < root->numChildren(); ++ch) {
                auto node = root->getChildSharedPtr(ch);
                actualChildNames.push_back(node->getName());
                
                // Check contents
                if (node->isColorImageNode()) {
                    EXPECT_EQ(node->asColorImageNode()->getColorImage(), ci);
                } else if (node->isDepthImageNode()) {
                    EXPECT_EQ(node->asDepthImageNode()->getDepthImage(), di);
                } else if (node->isPerspectiveCameraNode()) {
                    EXPECT_EQ(node->asPerspectiveCameraNode()->getPerspectiveCamera(), camera);
                }
            }
            
            EXPECT_SORTED_SEQUENCES_EQUAL(kExpectedChildNames, actualChildNames);
        }
        
        
        // Inspect bundle 2
        ASSERT_TRUE(roots.size() >= 2);
        {
            auto root = roots.at(1);
            static const std::vector<std::string> kExpectedChildNames = {
                "/front_camera/info:0.1000000",
                "/front_camera/image:0.1000000",
                "/front_camera/depth:0.1001000",
            };
            
            std::vector<std::string> actualChildNames;
            for (size_t ch = 0; ch < root->numChildren(); ++ch) {
                auto node = root->getChildSharedPtr(ch);
                actualChildNames.push_back(node->getName());
                
                // Check contents
                if (node->isColorImageNode()) {
                    EXPECT_EQ(node->asColorImageNode()->getColorImage(), ci);
                } else if (node->isDepthImageNode()) {
                    EXPECT_EQ(node->asDepthImageNode()->getDepthImage(), di);
                } else if (node->isPerspectiveCameraNode()) {
                    EXPECT_EQ(node->asPerspectiveCameraNode()->getPerspectiveCamera(), camera);
                }
            }
            
            EXPECT_SORTED_SEQUENCES_EQUAL(kExpectedChildNames, actualChildNames);
        }
        
        // We should only have two bundles
        EXPECT_EQ(roots.size(), 2);
    }
}

#undef EXPECT_SORTED_SEQUENCES_EQUAL
