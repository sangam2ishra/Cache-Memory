#include <bits/stdc++.h>
using namespace std;

#define wordSize 4
#define Mem_size 100

struct CacheLine
{
    int State;
    long long Tag;
    vector<long long> Data;

    CacheLine() : State(0), Tag(-1), Data(16, -1) {}
};



uint64_t generate_40_bit_random()
{
    random_device rd;

    mt19937_64 gen(rd());

    uniform_int_distribution<uint64_t> dist(0, (1ULL << 40) - 1);

    return dist(gen);
}




class Memory
{
public:
    vector<vector<long long>> Mem;

    Memory(long long num_Blocks, long long block_sz)
    {
        // int num_elements = block_sz / wordSize; 
        // Mem = vector<vector<int>>(num_Blocks, vector<int>(num_elements));
        // populateMemory();
    }

    // void displayMemory() const
    // {
    //     for (size_t i = 0; i < Mem.size(); ++i)
    //     {
    //         cout << "Block " << i << ": ";
    //         for (int data : Mem[i])
    //         {
    //             cout << data << " ";
    //         }
    //         cout << endl;
    //     }
    // }

    // void populateMemory()
    // {
    //     for (auto &block : Mem)
    //     {
    //         for (int &data : block)
    //         {
    //             data = rand(); // Generates a random integer
    //         }
    //     }
    // }

    CacheLine Process_mem_req(long long MemReq)
    {
        CacheLine MemResp;
        int tag, BA, A = MemReq, B = 16, index, S = 64, BO;

        BA = A / B;
        index = BA % S;
        tag = BA / S;
        BO = A % B;

        // if (BA >= Mem.size())
        // {
        //     BA = rand() % Mem.size(); // Handle out-of-bounds access
        // }

        // MemResp.Data = Mem[BA];
        cout<<"Getting Memory Response as memory block: "<<BA<<endl;
        MemResp.Tag = tag;

        return MemResp;
    }

    CacheLine Process_mem_req(long long MemReq, int type, long long data)
    {
        CacheLine MemResp;
        long long tag, BA, A = MemReq, B = 16, index, S = 64, BO;

        BA = A / B;
        index = BA % S;
        tag = BA / S;
        BO = A % B;

        // if (BA >= Mem.size())
        // {
        //     BA = rand() % Mem.size();
        // }

        cout<<"Writing to Memory in block Number: "<<BA<<" at Offset "<<BO<<endl;
        // Mem[BA][index] = data;
        // MemResp.Data = Mem[BA];
        MemResp.Tag = tag;
        return MemResp;
    }
};

class SetAssociativeCache
{
public:
    vector<vector<CacheLine>> cache; // 2D vector for sets and ways

    SetAssociativeCache(long long ways, long long sets)
    {
        cache = vector<vector<CacheLine>>(sets, vector<CacheLine>(ways));
    }

    vector<long long> get_data(const CacheLine &line)
    {
        return line.Data;
    }

    long long evict(vector<CacheLine> &S)
    {
        long long sz = S.size();
        long long evict_index = rand() % sz;
        S[evict_index].State = 0; // Invalidate the chosen line
        return evict_index;
    }
};

class CPU
{
public:
    long long tag, BA, A, B = 16, index, S = 64, BO, type;
    long long to_be_written;
    vector<long long> Data;
    void request()
    {
        A = generate_40_bit_random();
        BA = A / B;
        index = BA % S;
        tag = BA / S;
        BO = A % B;
        type = rand() % 2;
        if (type == 1)
        {
            // Data = vector<long long>(16, -1);
            // for (long long i = 0; i < 16; i++)
            // {
            to_be_written = rand();
            // }
        }
    }
};

void solve()
{
    Memory memory(Mem_size, 64);
    SetAssociativeCache cache(8, 64);
    long long q = 100000;

    long long H = 0;
    while (q--)
    {
        cout<<endl;
        cout<<"-----------------------------------------------------------------------------------------------------"<<endl<<endl;
        CPU cpu;
        cpu.request();
        cout << "CPU is requesting address " << cpu.A << endl;
        cout << "Request type is: " << (cpu.type == 0 ? "Read" : "Write") << endl;
        if(cpu.type==1)
        {
            cout<<"Data to be written in memory is: "<<cpu.to_be_written<<endl;
        }
        cout << "To be searched in Set " << cpu.index << " in the cache"<<endl;
        vector<CacheLine> &Block = cache.cache[cpu.index];
        vector<long long> CPUResp;

        bool hit = false;
        for (int i = 0; i < Block.size(); i++)
        {
            if (Block[i].Tag == cpu.tag)
            {
                cout << "Tag Matched with way " << i << endl;

                if (Block[i].State == 1)
                {
                    hit = true;
                    cout << "State is valid" << endl;
                    H++;
                    cout<<"HIT"<<endl;

                    if (cpu.type == 0)
                    {
                        CPUResp = cache.get_data(Block[i]);
                        cout << "Cache hit - Data retrieved" << endl;
                        // for (auto &d : CPUResp)
                        // {
                        //     cout << d << " ";
                        // }
                        // cout << endl;
                        break;
                    }
                    else
                    {
                        cout<<"Writing "<<cpu.to_be_written<<" in the way "<<i<<" at BO: "<<cpu.BO<<endl;
                        Block[i].Data[cpu.BO] = cpu.to_be_written;
                        cout<<"Changing the state to mispending"<<endl;
                        Block[i].State = 2;

                        // write in memory and change state to valid again
                        break;
                    }
                }
                else if (Block[i].State == 2)
                {
                    cout << "State is pending, waiting..." << endl;
                    this_thread::sleep_for(chrono::seconds(2)); // Simulate wait
                    break;
                }
            }
        }

        if (!hit)
        {
            if (cpu.type == 0)
            {
                cout<<"Cache miss"<<endl;
                cout<<"Replacing it"<<endl;
                long long evict_index = cache.evict(Block);
                cout<<"Replacing Block at index: "<<evict_index<<" in the cache\n";
                CacheLine &evictedLine = Block[evict_index];
                cout<<"Changing the state to mispending\n";
                evictedLine.State = 2;
                cout<<"Changing the tag of block "<<evict_index<<" to "<<cpu.tag<<endl;
                evictedLine.Tag = cpu.tag;

                cout<<"Processing memory request for the address "<<cpu.A<<" and getting memory response\n";
                CacheLine MemResp = memory.Process_mem_req(cpu.A);
                cout<<"Filling data at Block "<<evict_index<<" in the cache"<<endl;
                // evictedLine.Data = MemResp.Data;
                cout<<"Changing the state of Block "<<evict_index<<" to mispending"<<endl;
                evictedLine.State = 1;

                cout<<"Sending CPU Response\n";
                // CPUResp = MemResp.Data;
                // cout << "Cache miss - Loaded from memory" << endl;
                // for (auto &e : CPUResp)
                // {
                //     cout << e << " ";
                // }
                // cout << endl;
            }
        }
        if (cpu.type == 1)
        {
            auto MemResp = memory.Process_mem_req(cpu.A, 1, cpu.to_be_written);
            for (int i = 0; i < Block.size(); i++)
            {
                if (Block[i].Tag == MemResp.Tag && Block[i].State == 2)
                {
                    cout<<"Changing the state of block at way: "<<i<<" to valid"<<endl;
                    Block[i].State = 1;
                }
            }
        }
    }
    cout<<"Total hit is: "<<H<<endl;
    // cout << H << endl;
}

signed main()
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
