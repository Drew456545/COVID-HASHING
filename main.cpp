#include <iostream>
#include <fstream>
#include "intset.h"
#include "locmap.h"
using namespace std;
int N, N_delta, K = 100;
string *sequence; //stores individual sequences
bool *is_delta;
int **hashes; //stores the hashes of each possible probe

/**
 * hashLocs is a hash map which matches probes with locations(sequence and position)
 * each probe has info which computes the error rate of said probe
 * the lower error rate corresponds with a more accurate probe
 */
Locmap hashlocs;

struct probe_info {
    int seq_index, pos;
    int fp_count, fn_count;
    double error_rate;
};
void read_input(string inFile)
{
    
    ifstream input(inFile);
    string label, seq;
    while (input >> label >> seq) N++;
    input.clear();
    input.seekg(0);
    sequence = new string[N];
    is_delta = new bool[N];
    for (int i=0; i<N; i++) {
        input >> label >> sequence[i];
        is_delta[i] = label == "delta_variant";
        if (is_delta[i]) N_delta++;
    }
}
/**
 *
 * @param i probe sequence
 * @param p probe index
 * @param j prospective sequence
 * @param q prospective index
 * @return true iff their is <= 1 mismatch in the second half of probe
 */
bool is_match(int i, int p, int j, int q)
{
     int mismatched = 0;
  if (p<0 || q<0 || p+K-1>=sequence[i].length() || q+K-1>=sequence[j].length()) 
    return false;
  for (int k=0; k<K; k++)
    if (sequence[i][p+k] != sequence[j][q+k]) {
      mismatched ++;
      if (mismatched > 1) return false;
    }
  return true;
}
/**
 * @param i probe index
 * @param p probe position
 * L is a list of locations that have perspective matches
 * Copies all first half matches to L
 * If second half matches, back up by K/2 and add to L
 * Then go over L adjust number of FP or FN
 *
 */
probe_info eval_probe(int i, int p)
{
    probe_info info;
    info.seq_index = i;
    info.pos = p;
    info.fp_count = 0;
    info.fn_count = N_delta;
    Intset matches;
    Location *L = copy_locs(hashlocs[hashes[i][p]]);
    
    for (Location *Lsecond = hashlocs[hashes[i][p+K-K/2]]; Lsecond != NULL;
       Lsecond = Lsecond->next)
        L = new Location(Lsecond->seq, Lsecond->index+K/2-K, L);
  
    for (Location *loc = L; loc != NULL; loc = loc->next)
        if (!matches.find(loc->seq) && is_match(i,p,loc->seq,loc->index)) {
            matches.insert(loc->seq);
        if (is_delta[loc->seq]) info.fn_count --;
        if (!is_delta[loc->seq]) info.fp_count ++;
    }
    
    free_locs(L);
  
    double FPR = (double)info.fp_count / (N-N_delta);
    double FNR = (double)info.fn_count / N_delta;
    info.error_rate = 2.0 * FPR + 1.0 * FNR;
    return info;
}
/**
 * compute and store the hash of every length-K/2 window in every sequence
 * Use rolling hash for quicker computation
 */
void hash_everything(void)
{
    long long x = 2120, xK2 = 1;
    int P = 1000000007;
    hashes = new int *[N];
    for (int i=0; i<N; i++) hashes[i] = new int[sequence[i].length()];
    for (int i=0; i<K/2; i++) xK2 = (xK2 * x) % P;

    for (int i=0; i<N; i++) {
        string &s = sequence[i];
        int h = 0;
        for (int j=0; j<K/2; j++) h = (h*x + s[j]) % P;
        hashes[i][0] = h;
        hashlocs[h] = new Location(i,0,hashlocs[h]);
        for (int p=1; p<sequence[i].length()-K/2+1; p++) {
            h = (h*x + P - (s[p-1]*xK2)%P + s[p-1+K/2]) % P;
            hashes[i][p] = h;
            hashlocs[h] = new Location(i,p,hashlocs[h]);
        }
    }
}
int main(int argc, char* argv[])
{
    read_input(argv[1]);
    hash_everything();

    // Loop over all possible probes, remember the best one...
    probe_info best;
    best.error_rate = 99999;
    for (int i=0; i<N; i++) {
        if (is_delta[i])
            for (int p=0; p<sequence[i].length()-K+1; p++) {
// Evaluate probe at position p in sequence[i]
                probe_info info = eval_probe(i,p);
                if (info.error_rate < best.error_rate) best = info;
            }
    }
    cout<< "\n";

    // Print out info about best
    cout << "Best probe: " << sequence[best.seq_index].substr(best.pos, K) << "\n";
    cout << "False positives: " << best.fp_count << "\n";
    cout << "False negatives: " << best.fn_count << "\n";
    cout << "Error_rate: " << best.error_rate << "\n";
    for (int i=0; i<N; i++) delete [] hashes[i];
    delete [] hashes;
    delete [] sequence;
    delete [] is_delta;
}

