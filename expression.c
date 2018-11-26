/************************************************************************/
/* 表达式计算                                                               */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define _DEBUG_EXPRESION_ 

/************************************************************************/
/* 双链表                                                               */
/************************************************************************/
typedef enum emBoolVal
{
    TRUE,
    FALSE
}BOOL;

typedef enum emNodeType
{
    NODE_TYPE_ERR, // 错误类型
    NODE_TYPE_NUM, // 数字
    NODE_TYPE_PNT, // 小数点
    NODE_TYPE_ADD, // 加法
    NODE_TYPE_MUT, // 乘法
    NODE_TYPE_DEV, // 除法
    NODE_TYPE_SUB, // 减法
    NODE_TYPE_LBR, // 左括号
    NODE_TYPE_RBR  // 右括号
}NODE_TYPE_EM;

// 双链表节点
typedef struct tagDeListNode
{
    NODE_TYPE_EM emType; 
    double       dNum  ;
    struct tagDeListNode *pLast;
    struct tagDeListNode *pNext;
}LIST_NODE;

BOOL IsOperation(NODE_TYPE_EM emtype)
{
    if (NODE_TYPE_MUT == emtype || NODE_TYPE_SUB == emtype ||
        NODE_TYPE_ADD == emtype || NODE_TYPE_DEV == emtype )
    {  
        return TRUE;
    }

    return FALSE;
}

LIST_NODE* CreatNode(NODE_TYPE_EM emType, double dNum, LIST_NODE* pstLast, LIST_NODE* pstNext)
{
    LIST_NODE* pstNode = (LIST_NODE *)malloc(sizeof(LIST_NODE));
    if ( NULL == pstNode )
    {
        return NULL;
    }

    pstNode->dNum   = dNum;
    pstNode->emType = emType;
    pstNode->pLast  = pstLast;
    pstNode->pNext  = pstNext;

    return pstNode;
}

LIST_NODE *InsertNode(NODE_TYPE_EM emType, double dNum, LIST_NODE* pstLast, LIST_NODE* pstNext)
{
    LIST_NODE* pstNode = CreatNode(emType, dNum, pstLast, pstNext);
    if ( NULL == pstNode )
    {
        return NULL;
    }
    if ( NULL != pstLast )
    {
        pstLast->pNext = pstNode;
    }
    if ( NULL != pstNext )
    {
        pstNext->pLast = pstNode;
    }
    return pstNode;
}

void FreeList(LIST_NODE *pstHead, LIST_NODE *pstEnd)
{
    LIST_NODE *pstNode = NULL;
    while( pstEnd != pstHead )
    {
        pstNode = pstHead;
        pstHead = pstHead->pNext;
        free( pstNode );
    }
    if ( NULL != pstEnd )
    {
        free( pstEnd );
    }
}

LIST_NODE * RemoveRange(LIST_NODE *pstStart, LIST_NODE *pstEnd)
{
    LIST_NODE *pstRet = NULL;

    if ( NULL == pstStart || NULL == pstEnd )
    {
        return NULL;
    }
    pstRet = pstEnd->pNext;

    if ( NULL != pstStart->pLast )
    {
        pstStart->pLast->pNext = pstEnd->pNext;
    }
    if ( NULL != pstEnd->pNext)
    {
         pstEnd->pNext->pLast = pstStart->pLast;
    }

    FreeList( pstStart, pstEnd );
    return pstRet;
}


/************************************************************************/
/* 表达式计算                                                           */
/************************************************************************/

typedef enum emComputResult
{
    CPT_RET_SCS,                        // 处理成功
    CPT_RET_SYS_ERR,                    // 系统错误，如申请内存失败
    CPT_RET_PARAM_NULL,                 // 输入参数为空
    CPT_RET_EPRESS_ERR,                 // 表达式错误
    CPT_RET_DEVIDE_ZERO,                // 除零
    CPT_RET_EPRESS_ERR_INLEGAL_CHAR     // 非法字符
}COMPUT_RET_EM;


NODE_TYPE_EM GetCharType(char c)
{
    if ( '0' <= c  && '9' >= c )
    {
        return NODE_TYPE_NUM;
    }

    switch( c )
    {
        case '+': return NODE_TYPE_ADD;
        case '-': return NODE_TYPE_SUB;
        case '*': return NODE_TYPE_MUT;
        case '/': return NODE_TYPE_DEV;
        case '.': return NODE_TYPE_PNT;
        case '(': return NODE_TYPE_LBR;
        case ')': return NODE_TYPE_RBR;
    }

    return NODE_TYPE_ERR;
}

COMPUT_RET_EM CheckInput(char *psExpress, double *dResult)
{
    unsigned int uLen = 0, i = 0;

    if ( NULL == psExpress || NULL == dResult )
    {
        return CPT_RET_PARAM_NULL;
    }

    uLen = strlen(psExpress);
    for ( i=0; i< uLen; i++)
    {
        if ( NODE_TYPE_ERR == GetCharType(psExpress[i]) )
        {
            return CPT_RET_EPRESS_ERR_INLEGAL_CHAR;
        }
    }
    
    return CPT_RET_SCS;
}

// 入参的正确有调用函数保证
// 表达是错误： 1 连续两个操作符 2 括号不匹配 3 数字中两个小数点 4 
COMPUT_RET_EM ParseList(char *psExpress, LIST_NODE **ppsthead)
{
    int iLeftBranketsCount = 0;
    double dNumber = 0;
    char *pcAfter = NULL;
    NODE_TYPE_EM emNodeType; 
    LIST_NODE *pstEnd  = NULL, *pstNode = NULL;
    COMPUT_RET_EM emRet = CPT_RET_SCS;

    *ppsthead = CreatNode( NODE_TYPE_ERR, 0, NULL, NULL); // 头结点
    if ( NULL == *ppsthead )
    {
        return CPT_RET_SYS_ERR;
    }

    pstEnd = *ppsthead;
    while( '\0' != *psExpress && CPT_RET_SCS == emRet )
    {
        dNumber = 0;
        emNodeType = GetCharType( *psExpress );

        if ( NODE_TYPE_PNT == emNodeType ) // 不允许出现在数字之外的小数点
        {
            emRet = CPT_RET_EPRESS_ERR;
        }
        else if ( NODE_TYPE_LBR == emNodeType ) // 左括号前面不能是数字
        {
            if ( NODE_TYPE_NUM == pstEnd->emType )
            {
                emRet = CPT_RET_EPRESS_ERR;
            }
            else 
            {
                ++iLeftBranketsCount;
            }
        }
        else if ( NODE_TYPE_RBR == emNodeType ) // 右括号前面不能是左括号或者操作符
        {
            if ( NODE_TYPE_LBR == pstEnd->emType || TRUE == IsOperation( pstEnd->emType )|| 0 > ( -- iLeftBranketsCount) )
            {
                emRet = CPT_RET_EPRESS_ERR;
            }
        }
        else if ( (NODE_TYPE_DEV == emNodeType || NODE_TYPE_MUT == emNodeType) && TRUE == IsOperation( pstEnd->emType )) 
        {
            emRet = CPT_RET_EPRESS_ERR;// 乘除法之前不能有其他操作符
        }
        else if ( NODE_TYPE_ADD == emNodeType || NODE_TYPE_SUB == emNodeType ) // 加减号不仅是操作符也可以表示正负
        {
            if (TRUE == IsOperation( pstEnd->emType )) // 前一个是操作符，表示正负
            {
                emNodeType = NODE_TYPE_NUM;
                dNumber = strtod( psExpress, &pcAfter );
                if ( psExpress == pcAfter ) // 非数字
                {
                    emRet = CPT_RET_EPRESS_ERR;
                }
                else
                {
                    psExpress = pcAfter - 1; // 后面还要 ++
                }
            }
        }
        else if ( NODE_TYPE_NUM == emNodeType ) // 数字前面不能是右括号
        {
            dNumber = strtod( psExpress, &pcAfter );
            psExpress = pcAfter -1 ; 
        }
        
        if ( CPT_RET_SCS == emRet ) // 加入链表
        {
            pstNode = InsertNode( emNodeType, dNumber, pstEnd, NULL); 
            if ( NULL != pstNode )
            {
                pstEnd = pstNode;
                psExpress++;
            }
            else
            {
                emRet = CPT_RET_SYS_ERR; // 内存申请失败
            }
        }
    }

    if (0 != iLeftBranketsCount || TRUE == IsOperation( pstEnd->emType ) )
    {
        emRet = CPT_RET_EPRESS_ERR;// 左右括号数量不相等, 之前的处理错误，最后一个是操作符
    }

    if ( CPT_RET_SCS != emRet ) 
    {
        FreeList(*ppsthead, NULL);
        *ppsthead = NULL ;
        return emRet;
    }

    return CPT_RET_SCS;
}

COMPUT_RET_EM GetExpressWithOutBrackets(LIST_NODE *pstStart, LIST_NODE *pstEnd, double *dRet)
{
    LIST_NODE *pstNode = pstStart, *pstEndNext = pstEnd->pNext, *pstTmp = NULL;

    // 先算乘除
    while( pstNode != pstEndNext )
    {
        if ( NODE_TYPE_MUT == pstNode->emType )
        {
            (pstNode->pLast->dNum) *= (pstNode->pNext->dNum);  
            pstNode = RemoveRange( pstNode, pstNode->pNext );
        }
        else if ( NODE_TYPE_DEV == pstNode->emType )
        {
            if ( 0.000001 >= fabs(pstNode->pNext->dNum) )
            {
                return CPT_RET_DEVIDE_ZERO;
            }
            (pstNode->pLast->dNum) /= (pstNode->pNext->dNum);
            pstNode = RemoveRange( pstNode, pstNode->pNext );
        }
        else
        {
            pstNode = pstNode->pNext;
        } 
    }

    // 再算加减
    pstNode = pstStart;
    while( pstNode != pstEndNext )
    {
        if ( NODE_TYPE_ADD == pstNode->emType )
        {
            (pstNode->pLast->dNum) += (pstNode->pNext->dNum);  
            pstNode = RemoveRange( pstNode, pstNode->pNext );
        }
        else if ( NODE_TYPE_SUB == pstNode->emType )
        {
            (pstNode->pLast->dNum) -= (pstNode->pNext->dNum);
            pstNode = RemoveRange( pstNode, pstNode->pNext );
        }
        else
        {
            pstNode = pstNode->pNext;
        }  
    }

    *dRet = pstStart->dNum;
    return CPT_RET_SCS;
}


COMPUT_RET_EM GetExpressWithBrackets(LIST_NODE *pstHead, double *dResult)
{
    COMPUT_RET_EM emRet;
    LIST_NODE *pstStart = NULL, *pstEnd = NULL, *pstNode = NULL;
    (*dResult) = 0;
    
    while( NULL != pstHead->pNext )
    {

#ifdef _DEBUG_EXPRESION_
        pstNode = pstHead;
        while( NULL != pstNode)
        {
            printf(" %d -> %f \n", pstNode->emType, pstNode->dNum);
            pstNode = pstNode->pNext;
        }
        printf("\n");
        pstNode = NULL;
#endif

        pstEnd = pstStart = pstHead->pNext;
        while( NULL != pstEnd->pNext && NODE_TYPE_RBR != pstEnd->pNext->emType )// 找到第一个右括号
        {
            pstEnd = pstEnd->pNext;
        }

        // 没有括号，取得结果退出
        if ( NULL == pstEnd->pNext )
        {
           return GetExpressWithOutBrackets(pstStart, pstEnd, dResult);
        }
        
        // 计算结果添加到链表，去掉原有的节点，褪掉一层括号
        pstStart = pstEnd;// 下一个是右括号
        while( NODE_TYPE_LBR != pstStart->pLast->emType )// 向前找到第一个左括号，一定存在, pstStart 前面一个节点是左括号
        {
            pstStart = pstStart->pLast;
        }
        emRet    = GetExpressWithOutBrackets(pstStart, pstEnd, dResult); // 此时pstStart的值没变pstEnd已经改变
        if ( CPT_RET_SCS != emRet ) // 计算失败
        {
            return emRet;
        }
        pstEnd   = pstStart->pNext; // 右括号
        pstStart = pstStart->pLast; // 左括号

        pstNode = InsertNode(NODE_TYPE_NUM, *dResult, pstStart->pLast, pstEnd->pNext);
        if ( NULL == pstNode)
        {
            return CPT_RET_SYS_ERR; // 内存申请失败
        }
        FreeList(pstStart, pstEnd);
    }

    return CPT_RET_SCS;
}

COMPUT_RET_EM GetExpressResult(char *psExpress, double *dResult)
{
    LIST_NODE *pstHead = NULL;
    COMPUT_RET_EM emRet;

    // 1 入参检测
    emRet = CheckInput(psExpress, dResult);
    if ( CPT_RET_SCS != emRet )
    {
        return emRet;
    }

    // 2 表达式检测并转换成链表
    emRet = ParseList( psExpress, &pstHead );
    if ( CPT_RET_SCS != emRet )
    {
        return emRet;
    }
    
    // 3 计算
    emRet = GetExpressWithBrackets( pstHead, dResult );
    FreeList(pstHead, NULL);
    if ( CPT_RET_SCS != emRet )
    {
        *dResult = 0;
        return emRet;
    }

    return CPT_RET_SCS;
}

int main()
{
    char szInput[300] ={0};
    double dResult = 0;
    COMPUT_RET_EM emRet;

    while( 1 )
    {
        scanf("%s", szInput);
        if ( 0 == strcmp(szInput,"exit"))
        {
            break;
        }
        emRet = GetExpressResult( szInput, &dResult );
        if ( CPT_RET_SCS  == emRet )
        {
            printf(">> %f \n\n", dResult );
        }
        else
        {
            printf(">> %d \n\n", emRet );
        }
    }

    return 0;
}
