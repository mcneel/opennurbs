#include <gtest/gtest.h>
#include <string>
#include <iostream>

#include "../opennurbs_public_examples.h"

namespace
{

    /*
    // basic list of files to start testing
    std::vector<ON_String> GetFileList()
    {
        std::vector<ON_String> files;
        //TODO
        return {
            new ON_String("../../../example_files/V8/v8_caps_and_joins.3dm"),
            new ON_String("../../../example_files/V8/v8_section_hatch.3dm"),
            new ON_String("../../../example_files/V8/v8_selective_clipping.3dm"),

        };
    }
    */

    /*

    // method to take a base folder and eventually return all of the file paths
    std::vector<ON_String> GetFileList(const ON_String fullpath)
    {
        //TODO
        return {};
    }
    */

    class ON_FileIO_ParameterizedTest : public testing::TestWithParam<ON_String>
    {
    protected:
        // start up ON once
        static void SetUpTestSuite()
        {
            // std::cout << "Starting OpenNURBS \n";
            ON::Begin();
        }

        // close ON once
        static void TearDownTestSuite()
        {
            // std::cout << "Closing OpenNURBS \n";
            ON::End();
        }
    };

    // the actual test code. Open the input
    TEST_P(ON_FileIO_ParameterizedTest, ReadWrite)
    {
        // TODO //
        // GetParam() gets the ON_String file path
        // std::cout << GetParam();
        bool res = false;
        ON_String full_path(GetParam());

        if (ON_FileSystem::IsFile(full_path))
        {
            if (ON_FileStream::Is3dmFile(full_path, false))
            {

                FILE* fp = nullptr;
                fp = ON_FileStream::Open3dmToRead(full_path);

                ON_BinaryFile source_archive(ON::archive_mode::read3dm, fp);
                const ON_wString wide_full_path(full_path);
                source_archive.SetArchiveFullPath(static_cast<const wchar_t*>(wide_full_path));

                ONX_ModelTest::Type test_type = ONX_ModelTest::Type::ReadWriteReadCompare;
                ONX_ModelTest test;

                const ON_String text_log_3dm_file_name = "";
                ON_TextLog text_log;
                const ON_wString wide_text_log_3dm_file_name(text_log_3dm_file_name);
                res = test.ReadTest(source_archive, test_type, true, wide_text_log_3dm_file_name, &text_log);

                // TODO: more from example_test

            }
        }

        EXPECT_TRUE(res);
    }

    // TODO: Replace this with a GetFileList() method that returns an array of files to test
    INSTANTIATE_TEST_SUITE_P(List, ON_FileIO_ParameterizedTest, testing::Values("../../../example_files/V8/v8_caps_and_joins.3dm", "../../../example_files/V8/v8_section_hatch.3dm", "../../../example_files/V8/v8_selective_clipping.3dm"),
    [](const testing::TestParamInfo<ON_FileIO_ParameterizedTest::ParamType>& info) {
      // Can use info.param here to generate the test suffix
      std::string data = static_cast<const char*>(info.param);
      std::string base_filename = data.substr(data.find_last_of("/\\") + 1);
      std::string::size_type const p(base_filename.find_last_of('.'));
      std::string name = base_filename.substr(0, p);
      return name;
      }
    );

    // Container of parameters
    // INSTANTIATE_TEST_SUITE_P(Container, ON_FileIO_ParameterizedTest, testing::ValuesIn(GetFileList()));

} // namespace