/**
 * --------------------------------------------------------------------------
 *  Copyright [2024] [Darie-Dragos Mitoiu]
 *
 * Licensed under the Amnesius License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http ://www.dmitoiu.ro/licenses/LICENSE-1.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * --------------------------------------------------------------------------
 */

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include <vector>


void serializeFiles(const std::vector<std::string>& inputFiles, const std::string& outputFile) {
    std::ofstream out(outputFile, std::ios_base::binary);
    if (!out.is_open()) {
        throw std::ios_base::failure("Failed to open output file: " + outputFile);
    }

    for (const auto& file : inputFiles) {
        std::ifstream in(file, std::ios_base::binary);
        if (!in.is_open()) {
            throw std::ios_base::failure("Failed to open input file: " + file);
        }

        // Write the file name name and length
        uint32_t fileNameLength = file.size();
        out.write(reinterpret_cast<const char*>(&fileNameLength), sizeof(fileNameLength));
        out.write(file.data(), fileNameLength);

        // Write the file content length and content
        in.seekg(0, std::ios_base::end);
        uint32_t fileContentLength = in.tellg();
        in.seekg(0, std::ios_base::beg);
        out.write(reinterpret_cast<const char*>(&fileContentLength), sizeof(fileContentLength));
        std::vector<char> buffer(fileContentLength);
        in.read(buffer.data(), fileContentLength);
        out.write(buffer.data(), fileContentLength);
    }

    out.close();
}


void deserializeFile(const std::string& inputFile) {
    std::ifstream in(inputFile, std::ios_base::binary);
    if (!in.is_open()) {
        throw std::ios_base::failure("Failed to open input file: " + inputFile);
    }

    while (in.peek() != EOF) {
        // Read the file name length and name
        uint32_t fileNameLength;
        in.read(reinterpret_cast<char*>(&fileNameLength), sizeof(fileNameLength));
        std::string fileName(fileNameLength, '\0');
        in.read(fileName.data(), fileNameLength);

        // Read the file content length and content
        uint32_t fileContentLength;
        in.read(reinterpret_cast<char*>(&fileContentLength), sizeof(fileContentLength));
        std::vector<char> buffer(fileContentLength);
        in.read(buffer.data(), fileContentLength);

        // Display file content
        std::cout << "File: " << fileName << "\n";
        std::cout << "Content:\n" << std::string(buffer.begin(), buffer.end()) << "\n\n";
    }

    in.close();
}

int main(int argc, char* argv[])
{
    std::cout << "Ryadus is running..." << std::endl;
    try {
        // Check if the correct number of arguments is provided
        if (argc < 1) {
            std::cerr << "Usage: " << argv[0] << " <file-1> <file-2> <file-n>\n";
            return 1;
        }

        // Collect input files
        std::vector<std::string> inputFiles(argv + 1, argv + argc);

        // Serialize files into data.dump
        const std::string outputFile = "data.dump";
        serializeFiles(inputFiles, outputFile);
        std::cout << "Files serialized into " << outputFile << " successfully.\n";

        // Deserialize and display content from data.dump
        std::cout << "Reading from " << outputFile << "...\n";
        deserializeFile(outputFile);

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}