#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class viterbi{
public:
    viterbi();
    void read_file(char *input_name);
    void decode();
    void write_output(char *);
    void print_info();
private:
    enum state {sunny, foggy, rainy};
    enum coat {no, yes};
    vector<int> state_seq;
    vector<int> dec_seq; // in reverse order
    vector<int> obs_seq;
    double start[3]={0.5, 0.25, 0.25};
    double transition[3][3]={0.8, 0.15, 0.05, 0.2, 0.5, 0.3, 0.2, 0.2, 0.6};
    double emission[3][2]={0.9, 0.1, 0.7, 0.3, 0.2, 0.8};
};

viterbi::viterbi(){
    ;
}

void viterbi::read_file(char *input_name){
    ifstream fin(input_name);
    if(!fin.is_open()){
        cout << "failed when opening input file..." << endl;
        exit(1);
    }
    string buf;
    fin >> buf;
    while(fin >> buf){
        switch(buf[0]){
            case 's':
                state_seq.push_back(sunny);
                break;
            case 'f':
                state_seq.push_back(foggy);
                break;
            case 'r':
                state_seq.push_back(rainy);
                break;
            default:
                cout << "input file has wrong representation..." << endl;
                break;
        }
        switch(buf[buf.length()-1]){
            case 'o':
                obs_seq.push_back(no);
                break;
            case 's':
                obs_seq.push_back(yes);
                break;
            default:
                cout << "input file has wrong representation..." << endl;
                break;
        }
    }
    fin.close();
}

void viterbi::decode(){
    double max_p_mat[obs_seq.size()][3] = {0};
    int prev_s_mat[obs_seq.size()][3] = {0};
    // dynamically and chronologically get the most likely seq
    for(int t=0; t<obs_seq.size(); ++t)
        for(int s=0; s<3; ++s)
            if(t == 0)
                max_p_mat[0][s] = start[s] * emission[s][obs_seq[0]];
            else{
                // find the most likely prev_state coming to this state s
                double max_p = -1;
                for(int prev_s=0; prev_s<3; ++prev_s){
                    double p = max_p_mat[t-1][prev_s] * transition[prev_s][s];
                    if(p > max_p){
                        max_p = p;
                        prev_s_mat[t][s] = prev_s;
                    }
                }
                // for this state s, times the emission prob
                max_p_mat[t][s] = max_p * emission[s][obs_seq[t]];
            }
    // find the most likely last state
    double max_p = -1;
    int most_likely_s = 0;
    for(int s=0; s<3; ++s)
        if(max_p_mat[obs_seq.size()-1][s] > max_p){
            max_p = max_p_mat[obs_seq.size()-1][s];
            most_likely_s = s;
        }
    dec_seq.push_back(most_likely_s);
    // backtrack through the prev_s_mat
    for(int t=obs_seq.size()-1; t>0; --t)
        dec_seq.push_back(prev_s_mat[t][dec_seq.back()]);
}

void viterbi::write_output(char *output_name){
    double accuracy = 0.0;
    for(unsigned i=0; i<dec_seq.size(); i++)
        if(dec_seq[i] == state_seq[dec_seq.size()-i-1])
            accuracy += 1;
    accuracy /= dec_seq.size();
    ofstream fout(output_name);
    fout << accuracy << endl;
    for(int i=dec_seq.size()-1; i>=0; --i){
        switch(dec_seq[i]){
            case sunny:
                fout << "sunny" << endl;
                break;
            case foggy:
                fout << "foggy" << endl;
                break;
            case rainy:
                fout << "rainy" << endl;
                break;
            default:
                break;
        }
    }
    fout.close();
}

void viterbi::print_info(){
    cout << "start prob:" << endl;;
    for(unsigned i=0; i<3; i++)
        cout << start[i] << ' ';
    cout << endl;

    cout << "transition:" << endl;
    for(unsigned i=0; i<3; i++){
        for(unsigned j=0; j<3; j++)
            cout << transition[i][j] << ' ';
        cout << endl;
    }

    cout << "emission:" << endl;
    for(unsigned i=0; i<3; i++){
        for(unsigned j=0; j<2; j++)
            cout << emission[i][j] << ' ';
        cout << endl;
    }

    cout << "state sequence:" << endl;
    for(vector<int>::iterator it=state_seq.begin(); it!=state_seq.end(); ++it)
        cout << *it << ' ';
    cout << endl;

    cout << "observation sequence:" << endl;
    for(vector<int>::iterator it=obs_seq.begin(); it!=obs_seq.end(); ++it)
        cout << *it << ' ';
    cout << endl;

    cout << "decoded sequence:" << endl;
    for(int i=dec_seq.size()-1; i>=0; --i) 
        cout << dec_seq[i] << ' ';
    cout << endl;
}

int main(int argc, char *argv[]){
    if(argc != 3)
        exit(1);
    viterbi vit;
    vit.read_file(argv[1]);
    vit.decode();
    vit.write_output(argv[2]);
    vit.print_info();

    return 0;
}
