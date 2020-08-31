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

#include "protobag/archive/DirectoryArchive.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>

#include <fmt/format.h>

#include "protobag/ArchiveUtil.hpp"
#include "protobag/Utils/Tempfile.hpp"

namespace protobag {
namespace archive {
  
namespace fs = std::filesystem;

std::string CanonicalEntryname(const std::string &entryname) {
  // Trim leading path sep from `entryname`, or else std::filesytem
  // will throw out the `_spec.path` base directory part.
  std::string entry_path_rel = entryname;
  if (
    !entry_path_rel.empty() && entry_path_rel[0] ==
    fs::path::preferred_separator) {
    
    entry_path_rel = entry_path_rel.substr(1, entry_path_rel.size() - 1);
  }
  return entry_path_rel;
}

Result<Archive::Ptr> DirectoryArchive::Open(Archive::Spec s) {
  if (s.mode == "read" && !fs::is_directory(s.path)) {

    return {.error = fmt::format("Can't find directory to read {}", s.path)};
  
  } else if (s.mode == "write" && s.path == "<tempfile>") {

    auto maybe_path = CreateTempdir(/*suffix=*/"_DirectoryArchive");
    if (maybe_path.IsOk()) {
      s.path = *maybe_path.value;
    } else {
      return {.error = maybe_path.error};
    }
    
  }

  DirectoryArchive *dar = new DirectoryArchive();
  dar->_spec = s;
  return {.value = Archive::Ptr(dar)};
}


std::vector<std::string> DirectoryArchive::GetNamelist() {
  std::vector<std::string> paths;
  for(auto& entry: fs::recursive_directory_iterator(_spec.path)) {
    if (fs::is_regular_file(entry)) {
      auto relpath = fs::relative(entry.path(), _spec.path);
      paths.push_back(fs::path::preferred_separator + relpath.u8string());
    }
  }
  return paths;
}

Archive::ReadStatus DirectoryArchive::ReadAsStr(const std::string &entryname) {

  std::string entry_path_rel = CanonicalEntryname(entryname);
  fs::path entry_path = fs::path(_spec.path) / entry_path_rel;
  if (!fs::is_regular_file(entry_path)) {
    return Archive::ReadStatus::EntryNotFound();
  }

  return Archive::ReadStatus::OK(ReadFile(entry_path.u8string()));
}

OkOrErr DirectoryArchive::Write(
    const std::string &entryname, const std::string &data) {

  std::string entry_path_rel = CanonicalEntryname(entryname);

  fs::path entry_path = fs::path(_spec.path) / entry_path_rel;
  fs::create_directories(entry_path.parent_path());

  // Write!
  {
    std::ofstream out(entry_path, std::ios::binary);
    out << data;
  }

  // Did that work?
  if (fs::is_regular_file(entry_path)) {
    return kOK;
  } else {
    return OkOrErr::Err( 
      fmt::format(
        "Failed to write entryname: {} entry_path: {} {}",
        entryname, entry_path.u8string(), ToString()));
  }
}

} /* namespace archive */
} /* namespace protobag */
