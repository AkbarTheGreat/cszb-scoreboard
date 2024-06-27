
/*
test/util/TempFilesystem.h: A temporary filesystem object which self-deletes on
destruction.

Copyright 2024 Tracy Beck

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
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace cszb_scoreboard::test {

class TempFilesystem {
 public:
  TempFilesystem() {
    root = std::filesystem::temp_directory_path() / std::tmpnam(nullptr);
    if (std::filesystem::exists(root)) {
      throw std::runtime_error("Cannot create temporary directory, " +
                               root.string() + " it already exists");
    }
    std::filesystem::create_directory(root);
  }

  ~TempFilesystem() { removeDir(root); }

  static void removeDir(const std::filesystem::path& directory) {
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
      if (entry.is_directory()) {
        removeDir(entry.path());
      } else {
        std::filesystem::remove(entry.path());
      }
    }
    std::filesystem::remove(directory);
  }

  auto getRoot() const { return root; }

  void createFile(const std::string& filename, const std::string& contents) {
    std::ofstream filehandle(root / filename);
    if (filehandle.is_open()) {
      filehandle << contents;
      filehandle.close();
    } else {
      throw std::runtime_error("Failed to open file for writing");
    }
  }

  void createSubdir(const std::string& subdir) {
    std::filesystem::create_directory(root / subdir);
  }

 private:
  std::filesystem::path root;
};

}  // namespace cszb_scoreboard::test
