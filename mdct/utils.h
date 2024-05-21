#include <vector>
#include <iostream>
#include <fstream>

template <typename T>
void d(std::vector<T> &v){
	for(int i = 0; i < v.size(); i++){
		std::cout<<v[i]<<" ";
	}
	std::cout<<std::endl;
	return;
}
template <typename T>
std::vector<T> read_track(const std::string& fname, size_t padding)
{
	// returns padded vector containing the track.
    using namespace std;
	ifstream is(fname, ios::binary);
	vector<T> em;
	if(!is){
		return em;
	}
	is.seekg(0, ios::end);
	auto dim = is.tellg();
	is.seekg(0, ios::beg);

	vector<T> samples((size_t(dim) / sizeof(T))+(2*padding), 0);	
	is.read(reinterpret_cast<char*>(&samples.data()[padding]), dim);
	return samples;
}
