#include <bits/stdc++.h>
using namespace std;

#define wordSize 4
#define Mem_size 100

struct CacheLine
{
    int State;        // Represents the state of the cache line (e.g., valid/invalid)
    int Tag;          // Tag for identifying the memory block
    vector<int> Data; // Data stored in the cache line

    CacheLine() : State(0), Tag(-1), Data(16, -1) {} // Initialize with default values
};

class Memory
{
public:
    vector<vector<int>> Mem;

    Memory(int num_Blocks, int block_sz)
    {
        int num_elements = block_sz / wordSize; // Number of words per block
        Mem = vector<vector<int>>(num_Blocks, vector<int>(num_elements));

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

    void populateMemory()
    {
        for (auto &block : Mem)
        {
            for (int &data : block)
            {
                data = rand(); // Generates a random integer
            }
        }
    }
    CacheLine Process_mem_req(int MemReq)
    {

        CacheLine MemResp;
        int tag;
        int BA;
        int A = MemReq, B = 16;
        int index;
        int S = 64;
        int BO;

        BA = A / B;
        index = BA % S;
        tag = BA / S;
        BO = A % B;
        if(BA>99)
        {
            BA=rand()%99;
        }
        MemResp.Data = Mem[BA];
        MemResp.Tag = tag;

        return MemResp;
    }
};

class SetAssociativeCache
{
public:
    vector<vector<CacheLine>> cache; // 2D vector for sets and ways

    SetAssociativeCache(int ways, int sets)
    {
        cache = vector<vector<CacheLine>>(sets, vector<CacheLine>(ways));
    }

    vector<int> get_data(CacheLine &line)
    {
        return line.Data;
    }

    CacheLine evict(vector<CacheLine> &S)
    {
        int sz = S.size();
        int random_address = rand() % sz;
        return S[random_address];
    }
};

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

    int q = 10;

    while (q--)
    {

        CPU cpu;
        cpu.request();
        cout<<"CPU is requesting address ";
        cout << cpu.A << endl;
        vector<CacheLine> &Block = cache.cache[cpu.index];
        vector<int> CPUResp;

        bool check = false;
        for (int i = 0; i < Block.size(); i++)
        {
            if (Block[i].Tag == cpu.tag)
            {
                cout<<"Tag Matched with way "<<i<<endl;
                check = true;
                if (Block[i].State == 1)
                {
                    cout<<"State is valid"<<endl;
                    
                    CPUResp = cache.get_data(Block[i]);
                    cout<<"Got cpu response"<<endl;
                    break;
                }
                else if (Block[i].State == 2)
                {
                    // wait
                    cout<<"State is Mispending";
                    cout << "Waiting..." << endl;
                    sleep(2);
                    break;
                }
            }
        }
        int MemReq;
        if (!check)
        {
            CacheLine B = cache.evict(Block);
            B.State = 2;
            B.Tag = cpu.tag;
            MemReq = cpu.A;

            // Process MemReq
            CacheLine MemResp = memory.Process_mem_req(MemReq);
            // CacheLine cpuResp;
            for (int i = 0; i < Block.size(); i++)
            {
                if (Block[i].Tag == MemResp.Tag && Block[i].State == 2)
                {
                    Block[i].State = 1;
                    Block[i].Data = MemResp.Data;
                    // for(auto &e: MemResp.Data)
                    // {
                    //     CPUResp.push_back(e.Data);
                    // }
                    CPUResp=MemResp.Data;
                    cout<<"All waitinig notified\n";
                    // cpuResp.Data = MemResp.Data;
                }
            }
            // cout<<cpuResp.Tag<<endl;
        }
    }
}

int main()
{
    srand(static_cast<unsigned>(time(0))); // Seed the random number generator

#ifndef ONLINE_JUDGE
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    freopen("error.txt", "w", stderr);
#endif

    solve();

    return 0;
}
