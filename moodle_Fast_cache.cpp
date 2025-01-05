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
#define Mem_size 1e9

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
    // uniform_int_distribution<uint64_t> dist(0, (1ULL << 10) - 1);
    uniform_int_distribution<uint64_t> dist(0, (1ULL << 38) - 1);

    return dist(gen)*4;
}


// uint64_t generate_40_bit_random_normal() {
//     random_device rd;
//     mt19937_64 gen(rd());

//     // Normal distribution with a mean around (1ULL << 37) for mid-range and stddev to cover the desired range
//     normal_distribution<double> dist(static_cast<double>(1ULL << 37), static_cast<double>(1ULL << 36));

//     uint64_t address;
//     do {
//         address = static_cast<uint64_t>(dist(gen));
//     } while (address >= (1ULL << 38)); // Ensure it stays within the 38-bit range

//     // return (address*4)%(1<<12);
//     return (address*4);

// }


uint64_t generate_40_bit_random_normal() {
    random_device rd;
    mt19937_64 gen(rd());

    // Normal distribution with a mean around (1ULL << 37) for mid-range and stddev to cover the desired range
    normal_distribution<double> dist(static_cast<double>(1ULL << 37), static_cast<double>(1000ULL));

    uint64_t address;
    do {
        address = static_cast<uint64_t>(dist(gen));
    } while (address >= (1ULL << 38)); // Ensure it stays within the 38-bit range

    // return (address*4)%(1<<12);
    return (address*4);

}

class Memory
{
public:
    Memory(long long num_Blocks, long long block_sz)
    {
        // No array created in memory
    }

    CacheLine Process_mem_req(long long MemReq)
    {
        CacheLine MemResp;
        long long tag, BA, A = MemReq, B = 16, index, S = 64, BO;

        BA = A / B;
        index = BA % S;
        tag = BA / S;
        BO = A % B;

        //cout << "Getting Memory Response for memory block: " << BA << endl;
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

        //cout << "Writing to Memory in block Number: " << BA << " at Offset " << BO << endl;
        MemResp.Tag = tag;
        return MemResp;
    }
};

class SetAssociativeCache
{
public:
    vector<vector<CacheLine>> cache;

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

    void request()
    {
        // A = generate_40_bit_random();
        A=generate_40_bit_random_normal();
        // A%=100;
        BA = A / B;
        index = BA % S;
        tag = BA / S;
        BO = A % B;
        type = rand() % 2;
        if (type == 1)
        {
            to_be_written = rand();
        }
    }
};

void solve()
{
    Memory memory(Mem_size, 64);
    SetAssociativeCache cache(8, 64);
    // long long q = 1000;
    long long q=100000;
    long long t=q;
    long long H = 0;
    while (q--)
    {
        cout<<t-q<<" "<<H<<endl;
        //cout << endl;
        //cout << "-----------------------------------------------------------------------------------------------------" << endl << endl;
        CPU cpu;
        cpu.request();
        //cout << "CPU is requesting address " << cpu.A << endl;
        //cout << "Request type is: " << (cpu.type == 0 ? "Read" : "Write") << endl;
        if (cpu.type == 1)
        {
            //cout << "Data to be written in memory is: " << cpu.to_be_written << endl;
        }
        //cout << "To be searched in Set " << cpu.index << " in the cache" << endl;
        vector<CacheLine> &Block = cache.cache[cpu.index];
        vector<long long> CPUResp;

        bool hit = false;
        for (int i = 0; i < Block.size(); i++)
        {
            if (Block[i].Tag == cpu.tag)
            {
                //cout << "Tag Matched with way " << i << endl;

                if (Block[i].State == 1)
                {
                    hit = true;
                    //cout << "State is valid" << endl;
                    H++;
                    //cout << "HIT" << endl;

                    if (cpu.type == 0)
                    {
                        CPUResp = cache.get_data(Block[i]);
                        //cout << "Cache hit - Data retrieved" << endl;
                        break;
                    }
                    else
                    {
                        //cout << "Writing " << cpu.to_be_written << " in way " << i << " at BO: " << cpu.BO << endl;
                        Block[i].Data[cpu.BO] = cpu.to_be_written;
                        Block[i].State = 2;
                        break;
                    }
                }
                else if (Block[i].State == 2)
                {
                    //cout << "State is pending, waiting..." << endl;
                    this_thread::sleep_for(chrono::seconds(2)); // Simulate wait
                    break;
                }
            }
        }

        if (!hit)
        // {
        //     if (cpu.type == 0)
        //     {
        //         //cout << "Cache miss" << endl;
        //         //cout << "Replacing it" << endl;
        //         long long evict_index = cache.evict(Block);
        //         //cout << "Replacing Block at index: " << evict_index << " in the cache\n";
        //         CacheLine &evictedLine = Block[evict_index];
        //         evictedLine.State = 2;
        //         evictedLine.Tag = cpu.tag;

        //         //cout << "Processing memory request for address " << cpu.A << " and getting memory response\n";
        //         CacheLine MemResp = memory.Process_mem_req(cpu.A);
        //         //cout << "Changing the state of Block " << evict_index << " to valid" << endl;
        //         evictedLine.State = 1;
        //     }
        // }
             if (!hit)
        {
            if (cpu.type == 0)
            {
                // cout << "Cache miss" << endl;
                // cout << "Replacing it" << endl;
                bool check_space = false;
                for (int i = 0; i < Block.size(); i++)
                {
                    if (Block[i].Tag == cpu.tag)
                    {
                        Block[i].State = 2;
                        // cout << "Setting state of Block :" << i << " to mispending " << endl;
                        // cout << "Processing Memory request for address " << cpu.A << " and getting memory response" << endl;
                        CacheLine MemResp = memory.Process_mem_req(cpu.A);
                        // cout << "Changing the state of the Block " << i << " to valid" << endl;
                        Block[i].State = 1;
                        check_space = true;
                    }
                }
                if (!check_space)
                {

                    long long evict_index = cache.evict(Block);
                    // cout << "Replacing Block at index: " << evict_index << " in the cache\n";
                    CacheLine &evictedLine = Block[evict_index];
                    evictedLine.State = 2;
                    evictedLine.Tag = cpu.tag;

                    // cout << "Processing memory request for address " << cpu.A << " and getting memory response\n";
                    CacheLine MemResp = memory.Process_mem_req(cpu.A);
                    // cout << "Changing the state of Block " << evict_index << " to valid" << endl;
                    evictedLine.State = 1;
                }
            }
        }
        if (cpu.type == 1)
        {
            auto MemResp = memory.Process_mem_req(cpu.A, 1, cpu.to_be_written);
            for (int i = 0; i < Block.size(); i++)
            {
                if (Block[i].Tag == MemResp.Tag && Block[i].State == 2)
                {
                    //cout << "Changing the state of block at way: " << i << " to valid" << endl;
                    Block[i].State = 1;
                }
            }
        }
    }
    //cout << "Total hits: " << H << endl;
    double hit_rate=H*1LL*1.0*100/t;
    cout<<"Hit Rate= "<<hit_rate<<"%"<<endl;
    cout<<"Miss Rate="<<100-hit_rate<<"%"<<endl;
}

signed main()
{
    srand(static_cast<unsigned>(time(0))); // Seed the random number generator

// #ifndef ONLINE_JUDGE
//     freopen("input.txt", "r", stdin);
//     freopen("output.txt", "w", stdout);
//     freopen("error.txt", "w", stderr);
// #endif

    solve();

    return 0;
}
