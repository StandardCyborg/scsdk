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

#include "standard_cyborg/io/pbio/ProtobagIO.hpp"

#include <fmt/format.h>

#include <protobag/Protobag.hpp>
#include <protobag/Utils/PBUtils.hpp>
#include <protobag/Utils/TimeSync.hpp>
#include <protobag/Utils/PBUtils.hpp>

#include "standard_cyborg/sc3d/ColorImage.hpp"
#include "standard_cyborg/sc3d/DepthImage.hpp"
#include "standard_cyborg/sc3d/PerspectiveCamera.hpp"
#include "standard_cyborg/util/DebugHelpers.hpp"
#include "standard_cyborg/io/pbio/CorePBIO.hpp"
#include "standard_cyborg/io/pbio/TransformPBIO.hpp"
#include "standard_cyborg/io/pbio/ImagePBIO.hpp"
#include "standard_cyborg/io/pbio/PerspectiveCameraPBIO.hpp"

namespace standard_cyborg {
namespace io {
namespace pbio {

namespace sg = ::standard_cyborg::scene_graph;

std::string GetNodeName(const protobag::Entry &entry) {
    auto maybeTT = entry.GetTopicTime();
    if (maybeTT.has_value()) {
        const protobag::TopicTime &tt = *maybeTT;
        return fmt::format(
                           "{}:{}.{}",
                           tt.topic(),
                           tt.timestamp().seconds(),
                           tt.timestamp().nanos());
    } else {
        return entry.entryname;
    }
}

Result<std::shared_ptr<scene_graph::Node>>
ImageToNode(const protobag::Entry &current) {
    auto maybeMsg = current.GetAs<::standard_cyborg::proto::sc3d::Image>();
    if (!maybeMsg.IsOk()) {
        return {.error = fmt::format(
                                     "Error reading a {}\n Entry {}\n Error: {}",
                                     protobag::GetTypeURL<::standard_cyborg::proto::sc3d::Image>(),
                                     current.ToString(),
                                     maybeMsg.error)
        };
    }
    const ::standard_cyborg::proto::sc3d::Image &msg = *maybeMsg.value;
    
    auto maybePI = pbio::FromPB(msg);
    if (!maybePI.IsOk()) {
        return {.error = fmt::format(
                                     "Error decoding image from a {}\n Entry {}\n Error: {}",
                                     protobag::GetTypeURL<::standard_cyborg::proto::sc3d::Image>(),
                                     current.ToString(),
                                     maybePI.error)
        };
    }
    const pbio::ParsedImage &pi = *maybePI.value;
    
    std::shared_ptr<scene_graph::Node> node;
    if (pi.color_image.has_value()) {
        node.reset(
                   new sg::ColorImageNode(
                                          GetNodeName(current),
                                          std::move(*pi.color_image)));
    } else if (pi.depth_image.has_value()) {
        node.reset(
                   new sg::DepthImageNode(
                                          GetNodeName(current),
                                          std::move(*pi.depth_image)));
        
    } else {
        return {.error = fmt::format(
                                     "Parsed no color nor depth image from entry {}",
                                     current.ToString())
        };
    }
    return {.value = node};
}

Result<std::shared_ptr<scene_graph::Node>>
PerspectiveCameraToNode(const protobag::Entry &current) {
    auto maybeMsg = 
    current.GetAs<::standard_cyborg::proto::sc3d::PinholeCamera>();
    if (!maybeMsg.IsOk()) {
        return {.error = fmt::format(
                                     "Error reading a {}\n Entry {}\n Error: {}",
                                     protobag::GetTypeURL<::standard_cyborg::proto::sc3d::PinholeCamera>(),
                                     current.ToString(),
                                     maybeMsg.error)
        };
    }
    const ::standard_cyborg::proto::sc3d::PinholeCamera &msg = *maybeMsg.value;
    
    auto maybeCamera = pbio::FromPB(msg);
    if (!maybeCamera.IsOk()) {
        return {.error = fmt::format(
                                     "Could not decode a PerspectiveCamera while reading a {}\n Entry {}\n Error: {}",
                                     protobag::GetTypeURL<::standard_cyborg::proto::sc3d::PinholeCamera>(),
                                     current.ToString(),
                                     maybeCamera.error)
        };
    }
    
    const sc3d::PerspectiveCamera &camera = *maybeCamera.value;
    std::shared_ptr<scene_graph::Node> node(
                                            new sg::PerspectiveCameraNode(
                                                                          GetNodeName(current),
                                                                          camera));
    return {.value = node};
}


// May return null, which means don't know how to convert this entry
Result<std::shared_ptr<scene_graph::Node>>
EntryToNode(const protobag::Entry &current) {
    
    if (current.IsA<::standard_cyborg::proto::sc3d::Image>()) {
        return ImageToNode(current);
    } else if (current.IsA<::standard_cyborg::proto::sc3d::PinholeCamera>()) {
        return PerspectiveCameraToNode(current);
    } else {
        return {.value = nullptr};
    }
}


Result<std::shared_ptr<scene_graph::Node>>
ReadSceneGraphFromProtobag(const std::string &path) {
    protobag::Protobag bag(path);
    
    auto readEverything = protobag::ReadSession::Spec::ReadAllFromPath(path);
    auto maybeReader = protobag::ReadSession::Create(readEverything);
    if (!maybeReader.IsOk()) {
        return {.error = 
            fmt::format("Could not read {}: {}", path, maybeReader.error)
        };
    }
    protobag::ReadSession &reader = **maybeReader.value;
    
    std::shared_ptr<scene_graph::Node> root(new sg::Node());
    while (true) {
        const auto &maybeNext = reader.GetNext();
        if (maybeNext.IsEndOfSequence()) {
            break;
        } else if (!maybeNext.IsOk()) {
            return {.error = 
                fmt::format("Error while reading {}: {}", path, maybeNext.error)
            };
        }
        const protobag::Entry &current = *maybeNext.value;
        
        auto maybeNode = EntryToNode(current);
        if (!maybeNode.IsOk()) {
            return {.error = fmt::format(
                                         "Error while decoding node from {}: {}",
                                         path,
                                         maybeNode.error)
            };
        }
        auto nodep = *maybeNode.value;
        if (nodep) {
            root->appendChild(nodep);
        }
    }
    
    return {.value = root};
}

struct IterSceneGraphsFromProtobag::Impl {
    protobag::TimeSync::Ptr iter;
    
    protobag::OkOrErr Start(const std::string &path) {
        if (iter) { return protobag::kOK; }
        
        // For now, just synchronize *all* topics (by timestamp)
        auto maybeTopics = protobag::ReadSession::GetAllTopics(path);
        if (!maybeTopics.IsOk()) {
            return {.error = 
                fmt::format("Could not get topics from {}: {}", path, maybeTopics.error)
            };
        }
        std::vector<std::string> topics = *maybeTopics.value;
        
        auto readEverything = protobag::ReadSession::Spec::ReadAllFromPath(path);
        auto maybeReader = protobag::ReadSession::Create(readEverything);
        if (!maybeReader.IsOk()) {
            return {.error = 
                fmt::format("Could not read {}: {}", path, maybeReader.error)
            };
        }
        auto readerp = *maybeReader.value;
        
        auto maybeSync = protobag::MaxSlopTimeSync::Create(
                                                           readerp,
                                                           {
            .topics = topics,
            .max_slop = protobag::SecondsToDuration(1.),
            .max_queue_size = 3,
        });
        if (!maybeSync.IsOk()) {
            return {.error = 
                fmt::format("Could not start sync {}: {}", path, maybeSync.error)
            };
        }
        
        iter = *maybeSync.value;
        return protobag::kOK;
    }
};

IterSceneGraphsFromProtobag::IterSceneGraphsFromProtobag(
                                                         const std::string &_path) {
    
    impl.reset(new Impl());
    path = _path;
}

IterSceneGraphsFromProtobag::MaybeSG IterSceneGraphsFromProtobag::GetNext() {
    if (!impl) { return MaybeSG::Err("Programming error: no impl"); }
    if (!impl->iter) { 
        auto status = impl->Start(path);
        if (!status.IsOk()) {
            return MaybeSG::Err(status.error);
        }
    }
    protobag::TimeSync &iter = *impl->iter;
    
    auto maybeBundle = iter.GetNext();
    if (maybeBundle.IsEndOfSequence()) {
        return MaybeSG::EndOfSequence();
    } else if (!maybeBundle.IsOk()) {
        return MaybeSG::Err(
                            fmt::format(
                                        "Error while reading {}: {}",
                                        path,
                                        maybeBundle.error));
    } else {
        const protobag::EntryBundle &bundle = *maybeBundle.value;
        std::shared_ptr<scene_graph::Node> root(new sg::Node());
        for (const auto &entry : bundle) {
            auto maybeChild = EntryToNode(entry);
            if (!maybeChild.IsOk()) { 
                return MaybeSG::Err(
                                    fmt::format(
                                                "Error while reading a child from {}: {}",
                                                path,
                                                maybeChild.error));
            }
            auto nodep = *maybeChild.value;
            if (nodep) {
                root->appendChild(nodep);
            }
        }
        return MaybeSG::Ok(std::move(root));
    }
}


} // namespace pbio
} // namespace io
} // namespace standard_cyborg
