import crocofix.utility;

#include <catch2/catch_all.hpp>

using namespace crocofix;

TEST_CASE("read_file", "[read_file]") {

/*
    SECTION("reading a non existent file returns a stream in an error state") {
        read_file("/no_such_file", 
            [&](std::istream& is)
            {
                REQUIRE(!is);
            });    
    }

    SECTION("read an uncommpressed file") {
        std::vector<std::string> lines;
        read_file("uncompressed.txt", 
            [&](std::istream& is) 
            {
                REQUIRE(is); 
                for (;;)
                {
                    std::string line;
                    if (!std::getline(is, line))
                    {
                        break;
                    }
                    lines.push_back(line);
                }
            });
        REQUIRE(lines == std::vector<std::string>({ "bart", "nelson", "milhouse" }));
    }

    SECTION("read a gzip commpressed file") {
        std::vector<std::string> lines;
        read_file("compressed.txt.gz", 
            [&](std::istream& is) 
            {
                REQUIRE(is); 
                for (;;)
                {
                    std::string line;
                    if (!std::getline(is, line))
                    {
                        break;
                    }
                    lines.push_back(line);
                }
            });
        REQUIRE(lines == std::vector<std::string>({ "bart", "nelson", "milhouse" }));
    }

    SECTION("read a bzip commpressed file") {
        std::vector<std::string> lines;
        read_file("compressed.txt.bz2", 
            [&](std::istream& is) 
            {
                REQUIRE(is); 
                for (;;)
                {
                    std::string line;
                    if (!std::getline(is, line))
                    {
                        break;
                    }
                    lines.push_back(line);
                }
            });
        REQUIRE(lines == std::vector<std::string>({ "bart", "nelson", "milhouse" }));
    }

    SECTION("read a file with a gzip filename that is not actually compressed") {
        read_file("uncompressed.txt.gz",
            [&](std::istream& is) 
                {
                    REQUIRE(is); 
                    std::string line;
                    REQUIRE(!std::getline(is, line));
                }
            );
    }

    SECTION("read a file with a bzip filename that is not actually compressed") {
        read_file("uncompressed.txt.bz2",
            [&](std::istream& is) 
                {
                    REQUIRE(is); 
                    std::string line;
                    REQUIRE(!std::getline(is, line));
                }
            );
    }
    */

}