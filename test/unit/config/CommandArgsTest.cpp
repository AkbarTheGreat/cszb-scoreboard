/*
test/CommandArgsTest.cpp: Tests for config/CommandArgs

Copyright 2021-2024 Tracy Beck

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

#include <wx/cmdargs.h>  // for wxCmdLineArgsArray
#include <wx/cmdline.h>  // for wxCmdLineParser

#include <array>   // for array
#include <memory>  // for allocator, unique_ptr, make_unique

#include "config/CommandArgs.h"   // for CommandArgs, ARG_LIST
#include "gtest/gtest.h"          // for AssertionResult, Test, Message, Tes...
#include "util/FilesystemPath.h"  // for FilesystemPath
#include "util/Singleton.h"       // for SingletonClass

// Macro to manage repetitive unpacking of a std::array for our helper methods
#define ARG_ARR(arr) arr.size(), const_cast<char **>(arr.data())

namespace cszb_scoreboard::test {

auto wxArgArray(int argc, char **argv) -> std::unique_ptr<wxCmdLineArgsArray> {
  auto wx_args = std::make_unique<wxCmdLineArgsArray>();
  wx_args->Init(argc, argv);
  return wx_args;
}

/** Parsing the command-line is mostly handled by wx, so we don't test a lot of
 * the edge cases that the wx suite should already be testing.  Also, testing
 * the error cases is tricky due to the way wx asserts in them, so we only test
 * valid parses. */
auto parseCommandLine(int argc, char **argv) -> std::unique_ptr<CommandArgs> {
  wxCmdLineParser parser(argc, argv);
  parser.SetDesc(ARG_LIST.data());
  parser.Parse();

  auto cmd_args = std::make_unique<CommandArgs>(SingletonClass{});
  auto wx_args = wxArgArray(argc, argv);
  cmd_args->process_args(parser, argc, *wx_args);
  return cmd_args;
}

TEST(CommandArgsTest, AutoUpdateFlagWorksCorrectly) {
  const std::array<const char *, 2> single_letter = {
      {"scoreboard_testing.exe", "-n"}};

  EXPECT_FALSE(parseCommandLine(ARG_ARR(single_letter))->autoUpdate());

  const std::array<const char *, 2> full_word = {
      {"scoreboard_testing.exe", "--noupdate"}};

  EXPECT_FALSE(parseCommandLine(ARG_ARR(full_word))->autoUpdate());
}

TEST(CommandArgsTest, ResetConfigFlagWorksCorrectly) {
  const std::array<const char *, 2> single_letter = {
      {"scoreboard_testing.exe", "-r"}};

  EXPECT_TRUE(parseCommandLine(ARG_ARR(single_letter))->resetConfig());

  const std::array<const char *, 2> full_word = {
      {"scoreboard_testing.exe", "--resetconfig"}};

  EXPECT_TRUE(parseCommandLine(ARG_ARR(full_word))->resetConfig());
}

TEST(CommandArgsTest, DefaultValuesAreCorrect) {
  const std::array<const char *, 1> no_args = {{"scoreboard_testing.exe"}};

  std::unique_ptr<CommandArgs> cmdArgs = parseCommandLine(ARG_ARR(no_args));

  EXPECT_TRUE(cmdArgs->autoUpdate());
  EXPECT_FALSE(cmdArgs->resetConfig());
}

TEST(CommandArgsTest, FlagsDoNotInteract) {
  // Defaults are still applied when the other flag is thrown.
  const std::array<const char *, 2> no_update = {
      {"scoreboard_testing.exe", "-n"}};

  EXPECT_FALSE(parseCommandLine(ARG_ARR(no_update))->resetConfig());

  const std::array<const char *, 2> reset_config = {
      {"scoreboard_testing.exe", "-r"}};

  EXPECT_TRUE(parseCommandLine(ARG_ARR(reset_config))->autoUpdate());
}

TEST(CommandArgsTest, CommandIsFirstArgument) {
  const std::array<const char *, 1> no_args = {{"scoreboard_testing.exe"}};

  EXPECT_EQ(FilesystemPath("scoreboard_testing.exe"),
            parseCommandLine(ARG_ARR(no_args))->commandPath());

  const std::array<const char *, 3> many_args = {{"many_args.exe", "-n", "-r"}};

  EXPECT_EQ(FilesystemPath("many_args.exe"),
            parseCommandLine(ARG_ARR(many_args))->commandPath());
}

}  // namespace cszb_scoreboard::test
