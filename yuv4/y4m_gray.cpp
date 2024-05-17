// W352 C420jpeg
// FRAME
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "mat.h"
void read_dim(std::ifstream &is, int  &h, int &w){
    char type;
    is.get();
    type = is.get();
    if(type == 'W'){is>>w;}
    else{is>>h;}    
}
bool y4m_extract_gray(const std::string& filename, std::vector<mat<uint8_t>>& frames){
  // loads a vector of mats(frames). Returns false on failure;
    std::string buffer;   // buffers to store discarded stuff
    std::vector<uint8_t> crap;

    std::ifstream is{filename, std::ios::binary};
    if(!is){
        std::cout<<"Error in opening input file."<<std::endl;
        return false;
    }
    std::string magic,encoding, fr;
    is>>magic;
    if(magic != "YUV4MPEG2"){
        return false;
    }
    int w,h;    
    read_dim(is,h,w);
    read_dim(is,h,w);
    is>>encoding;
    std::getline(is,buffer); // discard all of the other parameters
    while(true){
        is>>fr;
        if(fr != "FRAME"){
            // no more frames to read
            break;
        }
        std::getline(is, buffer);
        // read until a newline is reached
        mat<uint8_t> current_frame;
        current_frame = mat<uint8_t>(h,w); // standard pointer
        if(!is.read(current_frame.rawdata(), current_frame.rawsize())){
            break;
        }
        crap = std::vector<uint8_t>(current_frame.rawsize()/2);
        if(!is.read(reinterpret_cast<char*>(&crap[0]), current_frame.rawsize()/2)){
            break;
        }
        frames.push_back(std::move(current_frame));
    }
    return true;  
}
// int main(){
//     {
//         std::vector<mat<uint8_t>> frames;
//         if(!y4m_extract_gray("foreman_cif.y4m", frames)){
//         std::cout<<"Error in reading input file."<<std::endl;
//             return EXIT_FAILURE;
//         }        
//     }
//     _CrtDumpMemoryLeaks();
//     return EXIT_SUCCESS;    
// }