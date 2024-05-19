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
#include "types.h"
void read_dim(std::ifstream &is, int  &h, int &w){
    char type;
    is.get();
    type = is.get();
    if(type == 'W'){is>>w;}
    else{is>>h;}
}
void upsample(std::vector<uint8_t> &source, std::vector<uint8_t> &dest){
	size_t n = source.size();
	for(size_t i=0; i<n-1; i+=2){
	  dest.push_back(source[i]);
	  dest.push_back(source[i]);	  
	  dest.push_back(source[i+1]);
	  dest.push_back(source[i+1]);	  
	  dest.push_back(source[i]);
	  dest.push_back(source[i]);	  
	  dest.push_back(source[i+1]);
	  dest.push_back(source[i+1]);      
  }
}
uint8_t saturate1(uint8_t base) {
	if(base < 16){
		return 16;
	}
	if(base > 240){
		return 240;
	}
	return 0;
}
uint8_t saturate2(float base){
	uint8_t conv;
	if(base > 255){
		conv = 255;
	}
    else if(base < 0){
		conv = 0;
	}
	else{
		conv = static_cast<uint8_t>(base);
	}
	return conv;
}
bool y4m_extract_color(const std::string &filename, std::vector<mat<vec3b>> &frames){
	// vec3b is just a vector of 3 uint8
    std::string buffer;   // buffers to store discarded stuff
	
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
	int f = 0;
    while(true){
		if(!(is>>fr)){
			break;
		}
		f+=1;
		std::cout<<"Frame "<<f<<std::endl;
        std::getline(is, buffer);
        // read until a newline is reached
        mat<vec3b> current_frame(h,w);
		// hxw pixels of 3 elements
		std::vector<uint8_t> y(h*w);
		std::vector<uint8_t> Cb((h*w)/4);
		std::vector<uint8_t> Cr((h*w)/4);
		if(!is.read(reinterpret_cast<char*>(&y[0]), h*w)){
			break;
		}
		if(!is.read(reinterpret_cast<char*>(&Cb[0]), (h*w)/4)){
			break;
		}
		if(!is.read(reinterpret_cast<char*>(&Cr[0]), (h*w)/4)){
			break;
		}
		std::vector<uint8_t> up_Cb;
		std::vector<uint8_t> up_Cr;
		upsample(Cb, up_Cb);
		upsample(Cr, up_Cr);
		// fill current frame
		for(int i = 0; i < h; i++){
			for(int j =0; j < w; j++){
			    uint8_t Y, cb, cr;
			    uint8_t R,G,B;
				Y  = saturate1(y[i*w+j]);
				cb = saturate1(up_Cb[i*w+j]);
				cr = saturate1(up_Cr[i*w+j]);
				// conversion
				R = saturate2(1.164*(Y-16) + 0*(cb-128) + 1.596*(cr-128));
				G = saturate2(1.164*(Y-16) + -0.392*(cb-128) + -0.813*(cr-128));
				B = saturate2(1.164*(Y-16) + 2.017*(cb-128) + 0*(cr-128));				
				//current_frame(i,j) = {y[i*w+j],up_Cb[i*w+j], up_Cr[i*w+j]};
				// saturation
				current_frame(i,j) = {R,G,B};
			}
		}
		frames.push_back(std::move(current_frame));
		if(is.peek() == EOF){
			std::cout<<"Reached end of file"<<std::endl;
			break;
		}
	}
	return true;
}
int main(){
	std::vector<mat<vec3b>> frames;
	//	std::vector<mat<uint8_t>> frames;	
	std::string filename = "foreman_cif.y4m";
	if(!y4m_extract_color(filename, frames)){
        std::cout<<"Error in reading input file."<<std::endl;
		return EXIT_FAILURE;
	}
	// if(!y4m_extract_gray(filename, frames)){
    //     std::cout<<"Error in reading input file."<<std::endl;
	// 	return EXIT_FAILURE;
	// }	
	std::cout<<filename<<" read successfully"<<std::endl;
    return EXIT_SUCCESS;
}
