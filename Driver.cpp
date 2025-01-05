// CACHE Assignment details: Cache Size= 32 KB, Block Size=64B, Assoc=8 way, Random Replacement, Write through, Address Size=40 bits

// Block Size=64B =>Block Offset=6bits
// Cache size=32kB =>No. of Blocks=32k/64=512
// Ways=8
// NO. of sets= 512/8=64
// Indexing or Set bits=log 64=6
// Tag bits=40-6-6=28
// No. of elements in a block=16

#include <bits/stdc++.h>
using namespace std;

#define wordSize 4
#define Mem_size 100

struct CacheLine
{
    int State; // Represents the state of the cache line (e.g., valid/invalid)
    int Tag;   // Tag for identifying the memory block
    vector<int> Data(16, -1);  // Data stored in the cache line

    CacheLine() : State(0), Tag(-1) {} // Initialize with default values
};

class Memory
{
public:
    vector<vector<int>> Mem;

    Memory(int num_Blocks, int block_sz)
    {
        // Initialize memory with `num_Blocks` blocks, each containing `block_sz` integers
        int num_elements = block_sz / wordSize;
        Mem = vector<vector<int>>(num_Blocks, vector<int>(block_sz));

        // Fill memory with random numbers
        populateMemory();
    }

    void displayMemory() const
    {
        for (size_t i = 0; i < Mem.size(); ++i)
        {
            cout << "Block " << i << ": ";
            for (int data : Mem[i])
            {
                cout << data << " ";
            }
            cout << endl;
        }
    }

public:
    void populateMemory()
    {
        // Seed for random number generation
        srand(static_cast<unsigned>(time(0)));

        // Populate each block with random numbers
        for (auto &block : Mem)
        {
            for (int &data : block)
            {
                data = rand(); // Generates a random integer
            }
        }
    }
};
class SetAssociativeCache
{
public:
    vector<vector<CacheLine>> cache; // 2D vector for sets and ways

    SetAssociativeCache(int ways, int sets)
    {
        // Initialize cache with `sets` sets, each containing `ways` CacheLine objects
        cache = vector<vector<CacheLine>>(sets, vector<CacheLine>(ways));

        // randomly fill data matrix
    }
    vector<int> get_data(vector<CacheLine> v)
    {
        vector<int> res;
        for(auto &e: v)
        {
            res.push_back(e.Data);

        }
        return res;
    }
};

// int random_num_gen() {
//     return rand() % 1600; // Generates a number from 0 to 1599
// }

class CPU
{
public:
    int tag;
    int BA;
    int A, B = 16;
    int index;
    int S = 64;
    int BO;
    void request()
    {
        A = rand() % 1600;
        BA = A / B;
        index = BA % S;
        tag = BA / S;
        BO = A % B;
    }
};

void solve()
{
    Memory memory(Mem_size, 16);
    SetAssociativeCache cache(8, 64);

    CPU cpu;
    cpu.request();
    // cout<<a<<endl;

    // Got the particular set
    vector<CacheLine> &Block = cache.cache[cpu.index];
    vector<int> CPUResp;
    for (int i = 0; i < Block.size(); i++)
    {
        if((Block[i].Tag==cpu.tag) && (Block[i].State=1))
        {
            CPUResp=cache.get_data(Block[i]);
        }
    }
}

signed main()
{
#ifndef ONLINE_JUDGE
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    freopen("error.txt", "w", stderr);
#endif

    solve();

    return 0;
}
