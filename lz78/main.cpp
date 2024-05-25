#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

class Node{
private:
    std::vector<std::pair<char, Node*>> connections_;
    size_t val_;
    bool leaf_;
public:
    Node(size_t val):val_(val), leaf_(true){};
    Node(){
        val_ = 0;
        leaf_ = true;
    }
    const std::pair<char, Node*> get_connection(size_t i) const{
        return connections_[i];
    }
    void add_conn(char c, size_t val){
        Node* n = new Node(val);
        connections_.emplace_back(c,n);
    }
    const size_t conn_size() const{
        return connections_.size();
    }
    bool is_leaf() const{
        return leaf_;
    }
    const size_t get_val() const{
        return val_;
    }
};
using cnn = std::pair<char, Node*>;

class lz78{
private:
    std::string filename_;
    std::string outname_;
    Node* root_;
public:
    lz78(std::string filename, std::string outname){
        filename_ = filename;
        outname_ = outname;
        root_ = new Node(0);
    };
    void deallocate(Node* n){
        for(int i =0; i < n->conn_size(); i++){
            deallocate(n->get_connection(i).second);
        }
        delete n;
    }
    ~lz78(){
        deallocate(root_);
    }    
    void encode(){
        std::ifstream is{filename_};
        if(!is){
            std::cout<<"Error in reading the file"<<std::endl;
            return;
        }
        std::ofstream os{outname_};
        if(!os){
            std::cout<<"Error in creating the output file"<<std::endl;
            return;
        }
        char c;
        size_t key = 1;
        Node* current_node = root_;
        while(true){
            if(!is.read(&c, 1)){
                if(current_node != root_){
                    os<<current_node->get_val();
                }
                break;
            }
            bool found = false;
            for(int i = 0; i < current_node->conn_size(); ++i){
                if(current_node->get_connection(i).first == c){
                    // make a step
                    current_node = current_node->get_connection(i).second;
                    found = true;
                    break;
                }
            }
            if(!found){
                os<<current_node->get_val()<<c;
                current_node->add_conn(c, key);
                key +=1;
                current_node = root_; // go back to root
            }
        }
    }
    void decode(){
        std::ifstream is{filename_};
        if(!is){
            std::cout<<"Error in reading the file"<<std::endl;
            return;
        }
        std::ofstream os{outname_};
        if(!os){
            std::cout<<"Error in creating the output file"<<std::endl;
            return;
        }
        char c;
        size_t key=0;
        size_t n;
        std::string result;
        std::map<size_t, std::string> dict;
        dict[0] = "";        
        while(is>>n){            
            if(!is.read(&c, 1)){
                result += dict[n];
                break;
            }
            result += dict[n] + c;
            dict[key+1] = dict[n] + c;
            key++;
        }        
        os<<result;
    }
};

int main(int argc, char **argv){
    {
        if(argc != 4){
            std::cout<<"Usage: lz78 c/d input output"<<std::endl;
            return EXIT_FAILURE;
        }
        char mode = *argv[1];
        std::string filename = argv[2];
        std::string outname = argv[3];
        lz78 compressor{filename, outname};
        if(mode == 'c'){
            compressor.encode();
        }
        if(mode == 'd'){
            compressor.decode();
        }        
    }
    _CrtDumpMemoryLeaks();
    return 0;
}