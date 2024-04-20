#include <iostream>
#include <fstream>
#include <array>
#include <algorithm>
#include "pam.h"
// step 1: read image
uint8_t mean(const std::vector<size_t> &v){
    int acc = 0;
    for(int i = 0; i < v.size(); i++){
        acc+=v[i];
    }
    return acc/v.size();
}
std::vector<size_t> get_channel(const std::vector<std::array<size_t, 5>> &si, size_t s, size_t e, short channel){
    std::vector<size_t> temp;
    for(size_t i = s; i < e; i++){
        temp.push_back(si[i][channel]);
    }
    return temp;
}
void quantize(std::vector<std::array<size_t, 5>> &si, pam<uint8_t> &img, size_t s, size_t e){
    std::cout<<"s: "<<s<<" e: "<<e<<std::endl;
    std::vector<size_t> r = get_channel(si, s, e, 0);
    std::vector<size_t> g = get_channel(si, s, e, 1);
    std::vector<size_t> b = get_channel(si, s, e, 2);
    uint8_t r_mean = mean(r);
    uint8_t g_mean = mean(g);
    uint8_t b_mean = mean(b);
    size_t i, j;
    for(size_t i = s; i < e; i++){
        img(si[i][3], si[i][4], 0) = r_mean;
        img(si[i][3], si[i][4], 1) = g_mean;
        img(si[i][3], si[i][4], 2) = b_mean;
    }
    return;
}
void sort_and_split(std::vector<std::array<size_t, 5>> &si, pam<uint8_t> &img, int depth, size_t s, size_t e){
    // step 1: find greatest range
    if(s > e){
        return;
    }
    if(depth == 0){
        quantize(si, img, s, e);
        return;
    }
    typedef std::array<size_t, 5> s_pixel;    
    std::vector<size_t> r = get_channel(si, s, e, 0);
    std::vector<size_t> g = get_channel(si, s, e, 1);
    std::vector<size_t> b = get_channel(si, s, e, 2);
    uint8_t r_range = *std::max_element(r.begin(), r.begin()) - *std::min_element(r.begin(), r.begin());
    uint8_t g_range = *std::max_element(g.begin(), g.begin()) - *std::min_element(g.begin(), g.begin());
    uint8_t b_range = *std::max_element(b.begin(), b.begin()) - *std::min_element(b.begin(), b.begin());

    uint8_t max_range = std::max(std::max(r_range, g_range), b_range);
    if (max_range == r_range){
        sort(si.begin()+s, si.begin()+e, [](s_pixel &a, s_pixel &b){return a[0] > b[0];});
    }
    else if (max_range == g_range){
        sort(si.begin()+s, si.begin()+e, [](s_pixel &a, s_pixel &b){return a[1] > b[1];});        
    }
    else{
        sort(si.begin()+s, si.begin()+e, [](s_pixel &a, s_pixel &b){return a[2] > b[2];});
    }
    size_t median = ((e+s)+1)/2;
    std::cout<<"Splitting: "<<s<<"-"<<median<<"-"<<e<<std::endl;
    sort_and_split(si,img,depth-1, s, median);
    sort_and_split(si,img,depth-1, median, e);
    return;
}
void median_cut(pam<uint8_t> &img, int palette){
    // step 1: create sortable data structure that also keeps track of original locations
    
    std::vector<std::array<size_t, 5>> sortable_img;
    for(size_t i = 0; i < img.r_; ++i){
        for(size_t j = 0; j < img.c_; ++j){
            std::array<size_t, 5> temp;
            for(int k = 0; k < 3; ++k){
                temp[k] = img(i,j,k);
            }
            temp[3] = i;
            temp[4] = j;
            sortable_img.push_back(std::move(temp));
        }
    }
    sort_and_split(sortable_img, img, palette,0,sortable_img.size());
}

int main(int argc, char **argv){
    if (argc != 3){
        std::cout<<"Incorrect usage: use es2.exe input"<<std::endl;
        return EXIT_FAILURE;
    }
    std::ifstream is {argv[1], std::ios::binary};
    std::ofstream os {argv[2], std::ios::binary};
    if(!is){
        std::cout<<"Error in reading input file!"<<std::endl;
        return EXIT_FAILURE;
    }
    if(!os){
        std::cout<<"Error in creating output file!"<<std::endl;
        return EXIT_FAILURE;
    }
    pam<uint8_t> p(is,os);
    p.read();
    median_cut(p, 3);
    p.write();
    return 0;
}