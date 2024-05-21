#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <cmath>
#include <fstream>
#include "utils.h"
// data in test.raw is little endian
#define WINSIZE 1024;
#define SAMPLE_TYPE uint16_t
#define COEFF_TYPE uint32_t
#define L 1024

template <typename T>
double w(size_t n){
	// weight function
	return std::sin((M_PI/2*n)*(n+0.5));
}
double w(double n, double N){
	return sin((M_PI/(2*N))*(n+0.5));
}
template <typename T>
std::vector<COEFF_TYPE> MDCT(std::vector<T> &window){
	// input: vector of uint8_t in this case
	// output: vector of frequencies as int32_t
	// sizeof window = 2048
	double window_size = window.size();
	std::vector<COEFF_TYPE> coeff;	
	double N = window_size/2;
	for(int k = 0; k < N; k++){
		double s = 0;
		for(int n = 0; n < window_size; n++){
			s+=window[n]*w(n,N)*cos((M_PI/N)*(n+0.5+(N/2))*(k+0.5));
		}
		coeff.push_back(round(s)/10000);
	}
	return coeff;
}
void pad(std::vector<SAMPLE_TYPE> &samples){
	size_t rem = samples.size() % 1024;
	size_t toadd = 1024 - rem;
	for(int i = 0; i < toadd; i++){
		samples.push_back(0);
	}
}
std::vector<std::vector<COEFF_TYPE>> convert(std::vector<SAMPLE_TYPE> &samples){
	std::vector<std::vector<COEFF_TYPE>> final_coeff;   
	// for each window run MDCT and save result in temp;
	size_t shifts = (samples.size()/1024)-2;
	if(samples.size() % 1024 != 0){
		pad(samples);
		shifts+=1;
	}
	auto begin = samples.begin();
	auto end = begin+(2*L);	
	std::vector<SAMPLE_TYPE> temp{begin,end};
	final_coeff.push_back(std::move(MDCT(temp)));
	size_t windows = 1;
	for(int i = 0; i < shifts; i++){
		begin += L;
		end += L;
		if(end == samples.end()){
			break;
		}
		temp = std::vector<SAMPLE_TYPE>{begin,end};
		final_coeff.push_back(std::move(MDCT(temp)));
		std::cout<<"window "<<windows+1<<std::endl;
		windows++;
	}
	return final_coeff;
}

int main(int argc, char **argv){
	using namespace std;
	if(argc != 2){
		std::cout<<"Usage: ./main <file.raw>"<<std::endl;
		return EXIT_FAILURE;
	}
	std::string filename = argv[0];
	auto track = read_track<SAMPLE_TYPE>(filename, L);
	// test window
	// std::vector<SAMPLE_TYPE> test(track.begin()+1024,track.begin()+4096);	
	// std::vector<COEFF_TYPE> freqs = MDCT<SAMPLE_TYPE>(test);
	auto coffs = convert(track);
	return 0;
}
