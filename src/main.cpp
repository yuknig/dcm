#include <iostream>
#include <fstream>
#include <cstdint>
#include <Dicom/Parser.h>
#include <Util/Stream.h>

#include <thread>
#include <chrono>

//#define _ITERATOR_DEBUG_LEVEL 0

int main()
{
    const std::string path = "F:/develop/dicom/files/YBR_FULL_422.dcm";

    std::shared_ptr<std::vector<uint8_t>> file_data;
    {
        /*std::basic_ifstream<uint8_t> ifs(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
        auto file_size = ifs.tellg();
        ifs.seekg(0, SEEK_SET);
        file_data = std::make_shared<std::vector<uint8_t>>(static_cast<size_t>(file_size));
        ifs.read(file_data->data(), file_data->size());*/
        auto f = fopen(path.c_str(), "rb");
        fseek(f, 0, SEEK_END);
        auto file_size = static_cast<size_t>(ftell(f));
        fseek(f, 0, SEEK_SET);
        file_data = std::make_shared<std::vector<uint8_t>>(file_size);
        fread(file_data->data(), 1, file_data->size(), f);
        fclose(f);

        //auto eos = std::istreambuf_iterator<uint8_t>();
        //auto buf = std::make_shared<std::vector<uint8_t>>(std::istreambuf_iterator<uint8_t>(ifs), eos);
    }

    auto stream = StreamRead::Create<uint8_t>(file_data, 0, file_data->size());

    dcm::Parser parser(*stream);
    auto root = parser.root();
    //root->getTag()

    //stream.reset();
    file_data.reset();

    
    std::this_thread::sleep_for(std::chrono::seconds(3));
    //dcm::parseDicomFile("F:\\develop\\dicom\\files\\3mensioExport\\SurfaceObject.dcm");

    return 0;
}