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

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "protobag/Utils/Result.hpp"

namespace protobag {
namespace archive {

class MemoryArchive;

// Try to return a valid value for `Spec.format` below given a file `path`
// based on the path's filename extension (or if `path` is an existing
// directory).  May return "" -- no format detected.
std::string InferFormat(const std::string &path);

// An interface abstracting away the archive 
class Archive {
public:
  typedef std::shared_ptr<Archive> Ptr;
  virtual ~Archive() { Close(); }
  
  // Opening an archive for reading / writing
  struct Spec {
    // clang-format off
    std::string mode;
      // Choices: "read", "write" ("append" not yet tested / supported)
    std::string path;
      // A local path for the archive
      // Special values:
      //   "<tempfile>" - Generate (and write to) a temporary file
    std::string format;
      // Choices:
      //   "memory" - Simply use an in-memory hashmap to store all archive
      //     data.  Does not require a 3rd party back-end.  Most useful for
      //     testing.
      //   "directory" - Simply use an on-disk directory as an "archive". Does
      //     not require a 3rd party back-end.
      //   "zip", "tar" - Use a LibArchiveArchive back-end to write a
      //     zip/tar/etc archive
    std::shared_ptr<MemoryArchive> memory_archive;
      // Optional: when using "memory" format, use this `memory_archive`
      // instead of creating a new one.
    // clang-format on
    static Spec WriteToTempdir() {
      return {
        .mode = "write",
        .path = "<tempfile>",
        .format = "directory",
      };
    }
  };
  static Result<Ptr> Open(const Spec &s=Spec::WriteToTempdir());
  virtual void Close() { }

  // Reading ------------------------------------------------------------------
  virtual std::vector<std::string> GetNamelist() { return {}; }


  // A Result<string> with special status codes for "entry not found" (which
  // sometimes is an acceptable error) as well as "end of archive."  The
  // string value is the payload data read.
  struct ReadStatus : public Result<std::string> {
    static ReadStatus EntryNotFound() { return Err("EntryNotFound"); }
    bool IsEntryNotFound() const { return error == "EntryNotFound"; }

    static ReadStatus Err(const std::string &s) {
      ReadStatus st; st.error = s; return st;
    }

    static ReadStatus OK(std::string &&s) {
      ReadStatus st; st.value = std::move(s); return st;
    }

    bool operator==(const ReadStatus &other) const {
      return error == other.error && value == other.value;
    }
  };

  virtual ReadStatus ReadAsStr(const std::string &entryname) {
    return ReadStatus::Err("Reading unsupported in base");
  }

  // TODO: bulk reads of several entries, probably be faster


  // Writing ------------------------------------------------------------------
  virtual OkOrErr Write(
    const std::string &entryname, const std::string &data) {
      return OkOrErr::Err("Writing unsupported in base");
  }

  // Properties
  virtual const Spec &GetSpec() const { return _spec; }
  virtual std::string ToString() const { return "Base"; }

protected:
  Archive() { }
  Archive(const Archive&) = delete;
  Archive& operator=(const Archive&) = delete;

  Spec _spec;
};

} /* namespace archive */
} /* namespace protobag */
