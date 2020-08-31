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
#include "standard_cyborg/sc3d/ColorImage.hpp"
#include "standard_cyborg/sc3d/DepthImage.hpp"
#include "standard_cyborg/sc3d/PerspectiveCamera.hpp"
#include "standard_cyborg/sc3d/Geometry.hpp"
#include "standard_cyborg/sc3d/Plane.hpp"
#include "standard_cyborg/sc3d/Landmark.hpp"

#include "standard_cyborg/io/pbio/SceneGraphNodePBIO.hpp"
#include "standard_cyborg/io/pbio/SceneGraphPBIO.hpp"

#include "standard_cyborg/io/pbio/TestUtils.hpp"

using namespace standard_cyborg;

using namespace standard_cyborg;
namespace sg = standard_cyborg::scene_graph;

static sc3d::ColorImage CreateTestImg() {
    sc3d::ColorImage ci(1, 1);
    ci.setFrame("test");
    ci.setPixelAtColRow(0, 0, math::Vec4(0.2f, 0.1f, 0.7f, 0.5f));
    
    return ci;
}

TEST(SceneGraphPBIOTest, EndToEnd) {
    
    std::shared_ptr<sg::Node> rootNode(new sg::Node());
    rootNode->setName("rootNode");
    
    std::shared_ptr<sg::Node> groupNode0(new sg::Node());
    groupNode0->setName("groupNode0");
    
    std::shared_ptr<sg::Node> groupNode1(new sg::Node());
    groupNode1->setName("groupNode1");
    
    rootNode->appendChild(groupNode0);
    rootNode->appendChild(groupNode1);
    
    
    {
        std::vector<math::Vec3> positions;
        positions.push_back(math::Vec3(1.1, 2.2, 3.2));
        sc3d::Polyline polyline(positions);
        polyline.setFrame("polylineFrame");
        
        std::shared_ptr<scene_graph::PolylineNode> node(std::make_shared<scene_graph::PolylineNode>());
        
        node->setName("polyineNode");
        node->setPolyline(polyline);
        node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
        node->getMaterial().materialModel = scene_graph::MaterialModel::Phong;
        
        groupNode0->appendChild(node);
    }
    
    {
        sc3d::ColorImage ci = sc3d::ColorImage(1, 1, std::vector<math::Vec4>{
            {0.0, 0.25, 0.5, 0.75},
        });
        ci.setFrame("colorImageFrame");
        
        std::shared_ptr<scene_graph::ColorImageNode> node(std::make_shared<scene_graph::ColorImageNode>());
        node->setName("colorImageNode");
        
        node->setColorImage(ci);
        node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
        node->getMaterial().materialModel = scene_graph::MaterialModel::Phong;
        
        groupNode0->appendChild(node);
    }
    
    {
        sc3d::DepthImage ci = sc3d::DepthImage(1, 1, std::vector<float>{ 0.5, });
        ci.setFrame("depthImageFrame");
        
        std::shared_ptr<scene_graph::DepthImageNode> node(std::make_shared<scene_graph::DepthImageNode>());
        node->setName("depthImageNode");
        
        node->setDepthImage(ci);
        node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
        
        groupNode0->appendChild(node);
    }
    
    {
        sc3d::PerspectiveCamera camera;
        camera.setFrame("perspectiveCameraFrame");
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
        
        std::shared_ptr<scene_graph::PerspectiveCameraNode> node(std::make_shared<scene_graph::PerspectiveCameraNode>());
        
        node->setPerspectiveCamera(camera);
        node->setName("perspectveCameraNode");
        node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
        
        groupNode1->appendChild(node);
    }
    
    {
        std::vector<math::Vec3> geoPositions;
        std::vector<math::Vec3> geoNormals;
        std::vector<math::Vec3> geoColors;
        
        geoPositions.push_back(math::Vec3(1.1f,2.2f,3.2f));
        
        geoNormals.push_back(math::Vec3(0.1f,0.6f,0.2f));
        
        geoColors.push_back(math::Vec3(0.95f,0.5f,0.9f));
        
        sc3d::Geometry geometry(geoPositions, geoNormals, geoColors);
        geometry.setFrame("pointCloudFrame");
        
        std::shared_ptr<scene_graph::GeometryNode> node(std::make_shared<scene_graph::GeometryNode>());
        
        node->setGeometry(geometry);
        node->setName("geometryNode");
        node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
        
        groupNode1->appendChild(node);
    }
    
    {
        std::vector<math::Vec3> geoPositions;
        std::vector<math::Vec3> geoNormals;
        std::vector<math::Vec3> geoColors;
        std::vector<math::Vec2> geoTexCoords;
        std::vector<sc3d::Face3> geoFaces;
        
        geoPositions.push_back(math::Vec3(1.1f,2.2f,3.2f));
        geoPositions.push_back(math::Vec3(10.1f,20.2f,30.2f));
        geoPositions.push_back(math::Vec3(100.1f,200.2f,300.2f));
        
        geoNormals.push_back(math::Vec3(0.1f,0.2f,0.2f));
        geoNormals.push_back(math::Vec3(0.2f,0.9f,0.4f));
        geoNormals.push_back(math::Vec3(0.3f,0.2f,0.2f));
        
        geoColors.push_back(math::Vec3(0.9f,0.5f,0.3f));
        geoColors.push_back(math::Vec3(0.7f,0.7f,0.5f));
        geoColors.push_back(math::Vec3(0.6f,0.3f,0.7f));
        
        geoTexCoords.push_back(math::Vec2(0.22f,0.14f));
        geoTexCoords.push_back(math::Vec2(0.24f,0.34f));
        geoTexCoords.push_back(math::Vec2(0.12f,0.38f));
        
        geoFaces.push_back(sc3d::Face3(2, 12, 14));
        geoFaces.push_back(sc3d::Face3(7, 3, 5));
        
        sc3d::Geometry geometry;
        geometry.setPositions(geoPositions);
        geometry.setNormals(geoNormals);
        geometry.setColors(geoColors);
        
        geometry.setTexCoords(geoTexCoords);
        geometry.setFaces(geoFaces);
        geometry.setTexture(CreateTestImg());
        
        geometry.setFrame("triangleMeshFrame");
        
        std::shared_ptr<scene_graph::GeometryNode> node(std::make_shared<scene_graph::GeometryNode>());
        
        node->setGeometry(geometry);
        
        node->setName("geometryFrame");
        
        node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
        
        groupNode1->appendChild(node);
    }
    
    {
        
        sc3d::Plane plane {math::Vec3(1.8, 1.3, 1.1), math::Vec3(0.1, 1.1, 2.1) };
        plane.setFrame("planeFrame");
        std::shared_ptr<scene_graph::PlaneNode> node(std::make_shared<scene_graph::PlaneNode>());
        node->setPlane(plane);
        node->setName("test");
        node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
        
        groupNode1->appendChild(node);
    }
    
    {
        std::shared_ptr<scene_graph::Node> node(std::make_shared<scene_graph::Node>());
        
        node->setName("node0");
        node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
        node->getMaterial().materialModel = scene_graph::MaterialModel::Phong;
        
        groupNode1->appendChild(node);
    }
    
    {
        std::shared_ptr<scene_graph::CoordinateFrameNode> node(std::make_shared<scene_graph::CoordinateFrameNode>());
        
        node->setName("node0");
        node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
        node->getMaterial().materialModel = scene_graph::MaterialModel::Phong;
        
        
        groupNode1->appendChild(node);
    }
    
    {
        std::shared_ptr<scene_graph::PointNode> node(std::make_shared<scene_graph::PointNode>());
        
        node->setPosition(math::Vec3(1.2, 1.3, 1.4));
        
        node->setName("node0");
        node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
        node->getMaterial().materialModel = scene_graph::MaterialModel::Phong;
        
        
        groupNode1->appendChild(node);
    }
    
    {
        sc3d::Landmark landmark {"landmark2", math::Vec3(0.1, 1.1, 2.1) };
        landmark.setFrame("landmarkFrame");
        std::shared_ptr<scene_graph::LandmarkNode> node(std::make_shared<scene_graph::LandmarkNode>());
        node->setLandmark(landmark);
        node->setName("test");
        node->getMaterial().objectColor = math::Vec3(0.9, 0.2, 0.1);
        
        groupNode1->appendChild(node);
    }
    
    
    standard_cyborg::math::TransformRegistry tr1;
    
    standard_cyborg::io::pbio::SceneGraph sceneGraph;
    
    bool result = sceneGraph.transformRegistry.registerTransforms({
        math::Transform{.srcFrame="world-frame", .destFrame="polylineFrame", .translation={1,2,3}},
        math::Transform{.srcFrame="world-frame", .destFrame="colorImageFrame", .translation={-1,-2,3}},
        math::Transform{.srcFrame="world-frame", .destFrame="depthImageFrame", .translation={-1,-2,-3}},
        math::Transform{.srcFrame="world-frame", .destFrame="perspectiveCameraFrame", .translation={6,7,8}},
        math::Transform{.srcFrame="world-frame", .destFrame="pointCloudFrame", .translation={-6,-7,-8}},
        math::Transform{.srcFrame="world-frame", .destFrame="triangleMeshFrame", .translation={-6,+7,-8}},
        math::Transform{.srcFrame="world-frame", .destFrame="planeFrame", .translation={0.1,0.1,0.1}},
        math::Transform{.srcFrame="world-frame", .destFrame="landmarkFrame", .translation={0.24,0.75,0.99}},
    }).IsOk();
    
    EXPECT_TRUE(result) << "failed to register transforms";
    
    sceneGraph.sceneGraph = rootNode;
    
    Result<standard_cyborg::proto::scene_graph::SceneGraph> maybeMsg = standard_cyborg::io::pbio::ToPB(sceneGraph);
    
    EXPECT_TRUE(maybeMsg.IsOk()) << "failed to convert scenegraph to pb";
    
    standard_cyborg::proto::scene_graph::SceneGraph msg = *maybeMsg.value;
    
    
    // Write the new address book back to disk.
    {
        std::fstream output("./scenegraph", std::ios::out | std::ios::trunc | std::ios::binary);
        if (!msg.SerializeToOstream(&output)) {
            std::cerr << "Failed to write address book." << std::endl;
            
            EXPECT_TRUE(false);
            
        }
    }
    
    
    standard_cyborg::proto::scene_graph::SceneGraph loadedSceneGraphMsg;
    {
        // Read the existing address book.
        std::fstream input("./scenegraph", std::ios::in | std::ios::binary);
        if (!loadedSceneGraphMsg.ParseFromIstream(&input)) {
            std::cerr << "Failed to parse address book." << std::endl;
            
            EXPECT_TRUE(false);
            
        }
    }
    
    auto loadedSceneGraph = standard_cyborg::io::pbio::FromPB(loadedSceneGraphMsg);
    
    EXPECT_TRUE(loadedSceneGraph.IsOk()) << "failed to convert pb to scenegraph";
    
    /*
     class SceneGraph {
     public:
     std::shared_ptr<standard_cyborg::scene_graph::Node> sceneGraph;
     math::TransformRegistry transformRegistry;
     };
     */
    
    std::shared_ptr<scene_graph::Node> loadedRootNode = loadedSceneGraph.value->sceneGraph;
    std::vector<math::Transform> loadedTransforms = loadedSceneGraph.value->transformRegistry.toList();
    
    bool b = loadedRootNode->equals(*sceneGraph.sceneGraph);
    
    EXPECT_TRUE(b);    
    EXPECT_EQ(tr1.toList(), loadedTransforms);
    
    
}
