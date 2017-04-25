#include <STDLIB.H>
#include <STDIO.H>

#define SUCC     ( 1)
#define FAIL     ( 0)

#define MAX_SIZE (12)
#define FACTOR   (11)

#define DEFAULT_KEY (0xFFFFFFFF)


typedef unsigned int UINT;

typedef struct stListNode
{
    UINT uKey;
    struct stListNode *pstNext;
}NODE,*P_NODE;

typedef struct stHashMap
{
    P_NODE apListHeads[MAX_SIZE];
}HASH_MAP, *P_HASH_MAP;


P_HASH_MAP InitHasMap()
{
    UINT i = 0, j =0;
    P_HASH_MAP pstHashMap = NULL;

    pstHashMap = (P_HASH_MAP)malloc(sizeof(HASH_MAP));

    if ( NULL == pstHashMap)
    {
        return NULL;
    }

    for ( i =0; i < MAX_SIZE; i++)
    {
        pstHashMap->apListHeads[i] = (P_NODE)malloc(sizeof(NODE));
        if ( NULL == pstHashMap->apListHeads[i] )
        {
            for ( j = 0; j < 0; j++)
            {
                free(pstHashMap->apListHeads[j]);
            }
            free(pstHashMap);
            return NULL;
        }

        pstHashMap->apListHeads[i]->uKey = DEFAULT_KEY;
        pstHashMap->apListHeads[i]->pstNext = NULL;
    }

    return pstHashMap;
}


UINT GetHashIndex(UINT uKey)
{
    return ( uKey * FACTOR) % MAX_SIZE;
}

P_NODE FindNodeOrTail(UINT uKey, P_HASH_MAP pstHashMap)
{
    UINT  uIndex = 0;
    P_NODE pstIterNode = NULL;

    if ( NULL == pstHashMap)
    {
        return NULL;
    }

    pstIterNode = pstHashMap->apListHeads[GetHashIndex(uKey)];
    while( (NULL != pstIterNode->pstNext) && (pstIterNode->uKey != uKey) )
    {
        pstIterNode = pstIterNode->pstNext;
    }

    return pstIterNode;
}


UINT InsertHashMap(P_HASH_MAP pstHashMap, UINT ukey)
{
    UINT uIndex = 0;
    P_NODE pstTail = NULL;
    P_NODE pstNode = NULL;

    if ( ukey == DEFAULT_KEY)
    {
        return FAIL;
    }

    pstTail = FindNodeOrTail(ukey, pstHashMap);
    if ( NULL == pstTail || pstTail->uKey == ukey)
    {
        return FAIL;
    }

    pstNode = (P_NODE)malloc(sizeof(NODE));
    if ( NULL == pstNode)
    {
        return FAIL;
    }

    pstNode->uKey = ukey;
    pstNode->pstNext = NULL;
    pstTail->pstNext = pstNode;

    return SUCC;
}


P_NODE FindNode(UINT uKey, P_HASH_MAP pstHashMap)
{
    P_NODE pstNode = FindNodeOrTail(uKey , pstHashMap);
    if ( NULL == pstNode ||pstNode->uKey != uKey)
    {
        return NULL;
    }
    return pstNode;
}

void FreeHashMap(P_HASH_MAP pstHashMap)
{
    UINT i = 0;
    P_NODE pstNode = NULL;
    P_NODE pstTemp = NULL;

    for ( i = 0; i < MAX_SIZE; i++)
    {
        pstNode = pstHashMap->apListHeads[i];
        pstHashMap->apListHeads[i] = NULL;
        while( NULL != pstNode)
        {
            pstTemp = pstNode->pstNext;
            free(pstNode);
            pstNode = pstTemp;
        }
    }

    free(pstHashMap);
}

int main()
{
    UINT i = 0;

    P_HASH_MAP pstMap = InitHasMap();

    for (i = 0; i < 100; i+=2 )
    {
        InsertHashMap(pstMap, i);
    }

    if ( SUCC == InsertHashMap(pstMap, 0))
    {
        printf("ERR\n");
    }

    if ( SUCC != InsertHashMap(pstMap, 1))
    {
        printf("ERR\n");
    }

    if ( NULL == FindNode(1, pstMap) || NULL == FindNode(0, pstMap) || NULL ==  FindNode(3,pstMap))
    {
        printf("ERR\n");
    }

    FreeHashMap(pstMap);

    return 0;
}
