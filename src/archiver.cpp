#include <iostream>

#include "huffman_code.h"
#include "lib/cla_parser.h"

int main(int argc, char **argv) {
    try {
        CLAParser parser;
        parser.AddFlag('c', "compress",
                       "using: -c archive file1 file2...\n"
                       "    Compress files file1, file2, ... to archive");
        parser.AddFlag('d', "decompress",
                       "using: -d archive\n"
                       "    Decompress archive");
        parser.AddFlag('h', "help",
                       "using: -h\n"
                       "    Help information");
        parser.AddMultipleArguments<std::string>("PATH", "Files", 0);

        if (!parser.Parse(argc, argv)) {
            std::cerr << parser.GetHelp() << std::endl;
            std::cerr << "Incorrect arguments" << std::endl;
            return 111;
        }

        bool help_mode = *parser.GetArgumentValue<bool>("help");
        if (help_mode) {
            std::cout << parser.GetHelp() << std::endl;
            return 0;
        }

        bool compress_mode = *parser.GetArgumentValue<bool>("compress");
        bool decompress_mode = *parser.GetArgumentValue<bool>("decompress");

        if (compress_mode == decompress_mode) {
            std::cerr << parser.GetHelp() << std::endl;
            std::cerr << "Choose compress or decompress mode" << std::endl;
            return 111;
        }
        if (compress_mode) {
            if (parser.GetMultiplyArgumentsNumber<std::string>() <= 1) {
                std::cerr << parser.GetHelp() << std::endl;
                std::cerr << "Nothing to compress" << std::endl;
                return 111;
            }
            Writer writer(*parser.GetMultiplyArgumentValue<std::string>(0));
            std::vector<std::string> file_names(parser.GetMultiplyArgumentsNumber<std::string>() - 1);
            for (size_t i = 0; i < file_names.size(); ++i) {
                file_names[i] = *parser.GetMultiplyArgumentValue<std::string>(i + 1);
            }
            HuffmanEncoder encoder;
            encoder.EncodeFiles(file_names, writer);
        } else {
            if (parser.GetMultiplyArgumentsNumber<std::string>() < 1) {
                std::cerr << parser.GetHelp() << std::endl;
                std::cerr << "Nothing to decompress" << std::endl;
                return 111;
            }
            if (parser.GetMultiplyArgumentsNumber<std::string>() > 1) {
                std::cerr << parser.GetHelp() << std::endl;
                std::cerr << "Too many files to decompress, please, use only 1" << std::endl;
                return 111;
            }
            Reader reader(*parser.GetMultiplyArgumentValue<std::string>(0));
            HuffmanDecoder decoder;
            if (!decoder.Decode(reader)) {
                std::cerr << "Decode failed" << std::endl;
                return 111;
            }
        }
    } catch (const Reader::FileReadError &e) {
        std::cerr << "Incorrect file data" << std::endl;
        return 111;
    } catch (const std::exception &e) {
        std::cerr << "Unknown error" << std::endl;
        return 111;
    }
    return 0;
}
