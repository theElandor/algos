class DSET{
public:    
    std::vector<int> rep;
    std::vector<int> rank;
    int nodes;
    DSET(int n){
        nodes = n;
        for(int i = 0; i < nodes; i++){
            rep.push_back(i);
            rank.push_back(1);
        }
    }
    void print(){
        std::cout<<"REP:\n";
        for(int i = 0; i < nodes; i++){
            std::cout<<i+1<<" --> "<<get_rep(i)+1<<std::endl;
        }
    }
    void update_rep(int source, int dest){
        int u_rep = get_rep(source);
        int v_rep  = get_rep(dest);
        int u_rep_rank = rank[u_rep];
        int v_rep_rank = rank[v_rep];
        if(u_rep_rank > v_rep_rank){
            rep[v_rep] = u_rep;
            rank[u_rep] += v_rep_rank;
        }
        else{
            rep[u_rep] = v_rep;
            rank[v_rep] += u_rep_rank;
        }
    }
    int get_rep(int node){        
        if(node == rep[node]){return node;}
        else{
            return get_rep(rep[node]);
        }
    }
    bool fully_connected(){
        int first = get_rep(0);
        for(int i = 1; i < nodes; i++){
            if(get_rep(i) != first){
                return false;
            }
        }
        return true;
    }
};
class Solution {
public:
    int maxNumEdgesToRemove(int n, vector<vector<int>>& edges) { 
        DSET alice(n);
        int t = edges.size();
        // update with lv 3 edges
        int needed =0;
        for(int i = 0; i < t; i++){
            int type = edges[i][0];
            int source = edges[i][1]-1;
            int dest = edges[i][2]-1;
            if(type == 3){
                if(alice.get_rep(source) != alice.get_rep(dest)){
                    alice.update_rep(source, dest);
                    needed ++;
                }
            }
        }
        DSET bob = alice;
        for(int i = 0; i < t; i++){
            int type = edges[i][0];
            int source = edges[i][1]-1;
            int dest = edges[i][2]-1;
            if(type == 1){
                if(alice.get_rep(source) != alice.get_rep(dest)){
                    alice.update_rep(source, dest);
                    needed ++;
                }
            }
            else if(type == 2){
                if(bob.get_rep(source) != bob.get_rep(dest)){
                    bob.update_rep(source, dest);
                    needed ++;
                }
            }
        }
        if(!bob.fully_connected() || !alice.fully_connected()){
            return -1;
        }
        return t-needed;      
    }
};
