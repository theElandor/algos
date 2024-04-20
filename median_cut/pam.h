#include <string>
#include <vector>
#pragma once
template <typename T>
struct pam{
    size_t r_;
    size_t c_;
    size_t d_;
    size_t m_;
    std::string t_;

    std::vector<T> data_;
    std::ifstream &is_; // image read
    std::ofstream &os_; // file to write on
    pam(std::ifstream &is, std::ofstream &os, size_t r = 0, size_t c = 0):
        r_(r), c_(c), data_(r*c), is_(is), os_(os) {}
    size_t rows() const {return r_;}
    size_t cols() const {return c_;}
    size_t size() const {return r_*c_;}
    T& operator ()(size_t r, size_t c){
        return data_[r*c_ + c];
    }
    const T& operator()(size_t r, size_t c) const{
        return data_[r*c_ + c];
    }
    // overload for multichannel images
    const T& operator()(size_t r, size_t c, size_t channel) const{
        return data_[r*d_*c_ + (c*d_) + channel];
    }    
    T& operator()(size_t r, size_t c, size_t channel){
        return data_[r*d_*c_ + (c*d_) + channel];
    }

    bool read(){
        std::string magic, t, token;
        unsigned short w, h, d, m; // numerical values
        is_>>magic;
        if(magic.compare("P7") != 0){
            std::cout<<"Invalid magic sequence!"<<std::endl;
            return false;
        }
        while(true){
            is_ >> token;
            if(token.compare("WIDTH") == 0){
                is_>>c_;
            }
            else if (token.compare("HEIGHT") == 0)
            {
                is_>>r_;
            }
            else if (token.compare("DEPTH") == 0)
            {
                is_>>d_;
            }
            else if (token.compare("MAXVAL") == 0)
            {
                is_>>m_;
            }
            else if (token.compare("TUPLTYPE") == 0)
            {
                is_>>t_; // not saved
                if(t_.compare("GRAYSCALE") != 0 && t_.compare("RGB") != 0){
                    std::cout<<"Operation not supported"<<std::endl;
                    return false;
                }
                if (t.compare("RGB") == 0 && d_ != 3){
                    std::cout<<"RGB mode selected without 3 channels."<<std::endl;
                    return false;
                }
                if (t.compare("GRAYSCALE") == 0 && d_ != 1){
                    std::cout<<"Grayscale mode selected with more thatn 1 channel."<<std::endl;
                    return false;
                }
            }
            else if (token.compare("ENDHDR") == 0)
            {
                is_.get();
                break;
            }
        }
        T current;
        while(is_.read(reinterpret_cast<char*> (&current), sizeof(T))){
            data_.push_back(current);
        }
        return true;
    }
    void write(){        
        os_<<"P7"<<std::endl;
        os_<<"WIDTH "<<c_<<std::endl;
        os_<<"HEIGHT "<<r_<<std::endl;
        os_<<"DEPTH "<<d_<<std::endl;
        os_<<"MAXVAL "<<m_<<std::endl;
        os_<<"TUPLTYPE "<<t_<<std::endl;
        os_<<"ENDHDR"<<std::endl;
        for(int i = 0; i < r_; i++){ // row
            for(int j = 0; j < c_; j++){ // column
                if(t_.compare("RGB") == 0){
                    for(int k=0; k < d_; k++){ // channel
                        os_.write(reinterpret_cast<const char*>(&(*this)(i,j,k)), sizeof(T));
                    }
                }                
                else if (t_.compare("GRAYSCALE") == 0)
                {
                    os_.write(reinterpret_cast<const char*>(&(*this)(i,j)), sizeof(T));
                }
                
            }
        }        
    }
    void flip(){
        /*
            Function that flips the images and writes the result to the output stream.
            Only works for grayscale images
        */
        size_t r1, r2;
        r1 = 0;
        r2 = (c_*(r_-1));
        while(r1 < r2){
            for(int i = 0; i < c_; i++){
                std::swap(data_[r1+i], data_[r2+i]);
                //std::swap((*this)(r1,i), (*this)(r2,i)); --> must be fixed
            }
            r1 = r1+c_;
            r2 = r2-c_;
        }
        write();
    }
    void mirror(){
        /*
            Function that mirrors the saved image in place.
            Only works for rgb images for now.
        */
        size_t R,L;
        for(size_t row = 0; row < r_; ++row){
            L = 0;
            R = c_-1;
            while(L < R){
                for(size_t k = 0; k < 3; k++){
                    std::swap((*this)(row,L,k), (*this)(row,R,k));
                }
                --R;
                ++L;                
            }
        }
        write();
    }
};