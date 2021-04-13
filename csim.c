/*
Cache Lab
  谢子飏
  19307130037
*/

#include "cachelab.h"
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/* 
Cache组织模式
    2^b bytes组成一个 block，前有 t (m - (b + s)) tag bits，和一个 valid bit 组成一个 Line 为基本行
    E * Line 组成一个 Set
    S 个 Set 组成整个 Cache
*/

static int Hit = 0, Miss = 0, Evict = 0;
static int verbose = 0;

typedef struct 
{
    int valid; // 0/1 : invalid/valid
    int tag;
    int LRUstamp; // 越小代表越早使用，可以evict
    // int* Block; // B（2^b） 字节
} Line_t;

// s个Set组成Cache
typedef struct {
    int S;
    int E; // Line 数组大小
    int B;
    Line_t*** cache_;
} Cache_t; 

Cache_t Cache;

/* Usage */
void printHelp()
{
    printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
}

/* 分配 Cache */
void mallocCache()
{
    Cache.cache_ = (Line_t***)malloc(Cache.S* sizeof(Line_t **));
    for(int i = 0; i < Cache.S; ++i)
        Cache.cache_[i] = (Line_t **)malloc(Cache.E * sizeof(Line_t *));
    for(int i = 0; i < Cache.S; ++i)
        for(int j = 0; j < Cache.E; ++j)
            Cache.cache_[i][j] = (Line_t *)malloc(sizeof(Line_t));
    for(int i = 0; i < Cache.S; ++i)
        for(int j = 0; j < Cache.E; ++j) {
            Cache.cache_[i][j]->valid = 0;
            Cache.cache_[i][j]->tag = 0;
            Cache.cache_[i][j]->LRUstamp = 1e9;
        }
}

/* 释放 Cache */
void freeCache()
{
    for(int i = 0; i < Cache.S; ++i)
        for(int j = 0; j < Cache.E; ++j)
            free(Cache.cache_[i][j]);
    for(int i = 0; i < Cache.S; ++i)
        free(Cache.cache_[i]);
    free(Cache.cache_);
}

void initCache(const int s, const int E, const int b) 
{
    Cache.S = (1 << s), Cache.E = E, Cache.B = (1 << b);
}

/* 获得 Valid bit */
int getValid(const int s, const int E) {return Cache.cache_[s][E]->valid;}

/* 获得 Tag */
int getTag(const int s, const int E) {return Cache.cache_[s][E]->tag;}

/* 获得LRUstamp */
int getLRU(const int s, const int E) {return Cache.cache_[s][E]->LRUstamp;}

/* 查看是否命中 */
int HitOrMiss(const int s, const int tag)
{
    for(int i = 0; i < Cache.E; ++i)
        if(getValid(s, i) && getTag(s, i) == tag)
            return i; // 命中
    return -1; // Miss 
}

/* 是否Evcit */
void Eviction(const int s, const int E, const int tag)
{
    if(getValid(s, E) && getTag(s, E) != tag)
    {
        Evict++;
        if(verbose)
            printf("eviction ");
    }
}

/* 更新LRU，越小越早使用 */
void lruUpdate(const int s, const int E)
{
    Cache.cache_[s][E]->LRUstamp = 1e9;
    for(int i = 0; i < Cache.E; ++i)
    {
        if(i != E)
            Cache.cache_[s][i]->LRUstamp--;
    }
}

/* 更新Cache */
void WriteCache(const int s, const int E, const int tag)
{
    Eviction(s, E, tag);
    Cache.cache_[s][E]->valid = 1;
    Cache.cache_[s][E]->tag = tag;
    lruUpdate(s, E);
}

/* 获取可以被evict的Block */
int getEvictPos(const int s)
{
    int Min = 1e9, idx = 0, tmp = 0;
    for(int i = 0; i < Cache.E; ++i)
    {
        if(!getValid(s, i))
            return i; // Not Valid, Can be evicted
        tmp = getLRU(s, i);
        if(Min > tmp) {
            Min = tmp;
            idx = i;
        }
    }
    return idx;
}

/* 模拟Cache，获得 Hit 和 Miss 的次数 */
void getAns(const int s, const int tag)
{
    int hit_flag = HitOrMiss(s, tag);
    if(hit_flag != -1) { // Hit
        Hit++;
        if(verbose)
            printf("hit ");
        lruUpdate(s, hit_flag);
    }
    else  {
        Miss++;
        if(verbose)
            printf("miss ");
        WriteCache(s, getEvictPos(s), tag);
    }
}

typedef struct {
    char cmd;
    int addr;
    char commar;
    int data;
} line_decoder_t;

int main(int argc, char *const argv[])
{
    char opt;
    int s, E, b;
    FILE* Path = NULL;
    line_decoder_t line_decoder; 

    while((opt=getopt(argc, argv, "hvs:E:b:t:")) != -1){
        switch (opt)
        {
            case 'h':
                printHelp();
                exit(0);
            case 'v':
                verbose = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                Path = fopen(optarg, "r");
                break;
            default:
                printHelp();
                exit(-1);
                break;
        }
    }

    if(s <= 0 || E <= 0 || b <= 0 || !Path)
    {
        printHelp();
        exit(-1);
    }

    initCache(s, E, b);
    mallocCache();
    
    while(fscanf(Path, "%s %x %c %d", 
            &line_decoder.cmd, 
            &line_decoder.addr, 
            &line_decoder.commar, 
            &line_decoder.data) != EOF) 
        {
            if(verbose)
                printf("%c %x%c%d ", 
                    line_decoder.cmd, 
                    line_decoder.addr, 
                    line_decoder.commar, 
                    line_decoder.data);
            if(line_decoder.cmd == 'I')
                continue;
            int Tag = line_decoder.addr >> (s + b);
            int S = (line_decoder.addr & ((1 << (s + b)) - 1)) >> b; 

            if(line_decoder.cmd == 'M')
            {
                getAns(S, Tag); // Load
                getAns(S, Tag); // Store
            }
            else
                getAns(S, Tag); // Load or Store

            if(verbose)
                printf("\n");
        }

    printSummary(Hit, Miss, Evict);
    freeCache();
}

