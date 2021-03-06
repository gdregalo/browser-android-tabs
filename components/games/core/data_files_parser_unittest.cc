// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/games/core/data_files_parser.h"

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "components/games/core/games_utils.h"
#include "components/games/core/proto/games_catalog.pb.h"
#include "components/games/core/proto/highlighted_games.pb.h"
#include "components/games/core/test/test_utils.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace games {

class DataFilesParserTest : public testing::Test {
 protected:
  void SetUp() override { ASSERT_TRUE(temp_dir_.CreateUniqueTempDir()); }

  void WriteStringToFile(const base::FilePath& file_path,
                         const std::string& string_data) {
    ASSERT_NE(
        -1, base::WriteFile(file_path, string_data.data(), string_data.size()));
  }

  DataFilesParser parser_;
  base::ScopedTempDir temp_dir_;
};

TEST_F(DataFilesParserTest, TryParseCatalog_FileDoesNotExist) {
  GamesCatalog catalog;
  EXPECT_FALSE(parser_.TryParseCatalog(temp_dir_.GetPath(), &catalog));
}

TEST_F(DataFilesParserTest, TryParseCatalog_NullOutPtr) {
  EXPECT_FALSE(parser_.TryParseCatalog(temp_dir_.GetPath(), nullptr));
}

TEST_F(DataFilesParserTest, TryParseCatalog_BadProto) {
  WriteStringToFile(GetGamesCatalogPath(temp_dir_.GetPath()),
                    "well something is odd");

  GamesCatalog test_catalog;
  EXPECT_FALSE(parser_.TryParseCatalog(temp_dir_.GetPath(), &test_catalog));
}

TEST_F(DataFilesParserTest, TryParseCatalog_Success) {
  GamesCatalog expected_catalog = test::CreateGamesCatalogWithOneGame();
  WriteStringToFile(GetGamesCatalogPath(temp_dir_.GetPath()),
                    expected_catalog.SerializeAsString());

  GamesCatalog test_catalog;
  EXPECT_TRUE(parser_.TryParseCatalog(temp_dir_.GetPath(), &test_catalog));
  EXPECT_TRUE(test::AreProtosEqual(expected_catalog, test_catalog));
}

TEST_F(DataFilesParserTest, TryParseHighlightedGames_FileDoesNotExist) {
  HighlightedGamesResponse highlighted_games;
  EXPECT_FALSE(parser_.TryParseHighlightedGames(temp_dir_.GetPath(),
                                                &highlighted_games));
}

TEST_F(DataFilesParserTest, TryParseHighlightedGames_NullOutPtr) {
  EXPECT_FALSE(parser_.TryParseHighlightedGames(temp_dir_.GetPath(), nullptr));
}

TEST_F(DataFilesParserTest, TryParseHighlightedGames_BadData) {
  GamesCatalog bad_proto = test::CreateGamesCatalogWithOneGame();
  WriteStringToFile(GetHighlightedGamesPath(temp_dir_.GetPath()),
                    "well something is odd");

  HighlightedGamesResponse test_response;
  EXPECT_FALSE(
      parser_.TryParseHighlightedGames(temp_dir_.GetPath(), &test_response));
}

TEST_F(DataFilesParserTest, TryParseHighlightedGames_Success) {
  HighlightedGamesResponse expected_response =
      test::CreateHighlightedGamesResponse();
  WriteStringToFile(GetHighlightedGamesPath(temp_dir_.GetPath()),
                    expected_response.SerializeAsString());

  HighlightedGamesResponse test_response;
  EXPECT_TRUE(
      parser_.TryParseHighlightedGames(temp_dir_.GetPath(), &test_response));
  EXPECT_TRUE(test::AreProtosEqual(expected_response, test_response));
}

}  // namespace games
