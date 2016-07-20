/************************************************************************/
/*                                                                      */
/* 2048 游戏，与原游戏的规则略有出入，整体相同，控制台显示              */
/* 原游戏参见：http://gabrielecirulli.github.io/2048/                   */
/*                                                                      */
/* 作者: Ghost      2014年3月                                           */ 
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MATH.H>
#include <conio.h>
#include <TIME.H>

/************************************************************************/
/*  定  义                                                              */
/************************************************************************/

// 基础数据定义
#define SIZE 4
#define UINT32 unsigned int 

// 键盘按键定义
#define KEY_CTRL  224
#define KEY_UP     72
#define KEY_DOWN   80
#define KEY_LEFT   75
#define KEY_RIGHT  77
#define KEY_EXIT   27

// 生成消息集合相关的宏定义
#define MAX_MSG_LEN 100
#define END_ID_OF_MSG_SET      -1
#define BEGIN_MSG_SET          MSG_INFO g_szMsg[] = {
#define ADD_MSG(ID,MSG_STR)    {ID,MSG_STR},
#define END_MSG_SET            {END_ID_OF_MSG_SET,""}};

// 布尔值
typedef enum emBoolVal
{
    TRUE = 1,
    FALSE = 0
}BOOL;

// 消息ID 系统通过该ID打印消息
typedef enum emMsgID
{
    MSG_SYS_INNER_ERROR  = 0 ,  /* 内部错误     */
    MSG_SYS_INIT_SUCCESS = 1 ,  /* 游戏开始     */
    MSG_SYS_BAD_CMD      = 2 ,  /* 命令错误     */
    MSG_GAME_WIN         = 11,  /* 游戏胜利     */
    MSG_GAME_LOS         = 12,  /* 游戏失败     */
    MSG_GAME_END         = 13   /* 游戏结束     */
}MSG_ID_EM;

// 消息结构体
typedef struct tagMsgInfo
{
    MSG_ID_EM emID;
    char szMessage[MAX_MSG_LEN];
}MSG_INFO;

// 生成消息集，供系统按ID打印消息
BEGIN_MSG_SET
    ADD_MSG(MSG_SYS_INNER_ERROR , "内部错误" )
    ADD_MSG(MSG_SYS_INIT_SUCCESS, "游戏开始" )
    ADD_MSG(MSG_GAME_WIN        , "游戏胜利" )
    ADD_MSG(MSG_GAME_LOS        , "游戏失败" )
    ADD_MSG(MSG_GAME_END        , "游戏结束，按R键重新开始" )
END_MSG_SET


/************************************************************************/
/*  界面显示  API                                                       */
/************************************************************************/


//********************************************
// Method      :  apiPrintMsg
// Description :  按消息ID（MSG_ID_EM）打印系统消息
// Param       :  输入 MSG_ID_EM emMsgID 消息ID
//*******************************************
void apiPrintMsg(MSG_ID_EM emMsgID)
{
    int i;
    char *pcTimeStr = NULL;
    for ( i = 0; END_ID_OF_MSG_SET != g_szMsg[i].emID; i++ )
    {
        if ( g_szMsg[i].emID == emMsgID )
        {
            printf("\n>> %s\n\n", g_szMsg[i].szMessage);  
            break;
        }
    }
}

//********************************************
// Method      :  apiPrintMat
// Description :  打印结果矩阵
// Param       :  输入 UINT32 * pszMat 需要打印的矩阵
//*******************************************
void apiPrintMat(UINT32 * pszMat)
{
    int i, j, uData;
    
    if ( NULL == pszMat )
    {
        apiPrintMsg(MSG_SYS_INNER_ERROR);
        return;
    }
    
    printf("\n===============================\n");
    for( i = 0; SIZE > i ; i++ )
    {
        printf("||");
        for( j = 0; SIZE > j; j++ )
        {
            uData = pszMat[SIZE * i + j];
            if ( 0 == uData )
            {
                printf("      |");
            }
            else
            {
                printf(" %4d |", uData);
            }
        }
        printf("|\n");
    }
    printf("===============================\n\n");
}

/************************************************************************/
/*  数据存储和基础功能  被cmd函数调用 不允许调用 api                    */
/************************************************************************/

//  全局变量

BOOL   g_bIsEnd =  FALSE;          // 是否执行结束（胜利或失败）

UINT32 g_szMat[SIZE*SIZE];         // 存储数字的矩阵

BOOL   g_szAddMat[SIZE*SIZE];      // 相加矩阵（为保证每个数字在移动时仅进行一次相加设置的标记矩阵）

// 数据处理函数

//********************************************
// Method      :  GetAddMatByPosition
// Description :  获得相加矩阵某位置的值
//*******************************************
BOOL GetAddMatByPosition(int i, int j)
{
    if ( 0 > i || 0 > j || SIZE <= i || SIZE <= j)
    {
        apiPrintMsg(MSG_SYS_INNER_ERROR);
        return FALSE;
    }
    
    return g_szAddMat[SIZE * i + j];
}

//********************************************
// Method      :  SetAddMatByPosition
// Description :  设置相加矩阵某位置的值
//*******************************************
void SetAddMatByPosition(int i, int j, BOOL bData)
{
    if ( 0 > i || 0 > j || SIZE <= i || SIZE <= j)
    {
        apiPrintMsg(MSG_SYS_INNER_ERROR);
        return ;
    }
    
    g_szAddMat[SIZE * i + j] = bData;
}

//********************************************
// Method      :  ResetAddMat
// Description :  重置相加矩阵
//*******************************************
void ResetAddMat()
{
    memset( g_szAddMat, FALSE, SIZE * SIZE * sizeof(BOOL) );
}

//********************************************
// Method      :  GetByPosition
// Description :  获得结果矩阵某位置的值
//*******************************************
UINT32 GetByPosition(int i, int j)
{
    if ( 0 > i || 0 > j || SIZE <= i || SIZE <= j)
    {
        apiPrintMsg(MSG_SYS_INNER_ERROR);
        return 0;
    }
    
    return g_szMat[SIZE * i + j];
}

//********************************************
// Method      :  SetByPosition
// Description :  设置结果矩阵某位置的值
//*******************************************
void SetByPosition(int i, int j, UINT32 uData)
{
    if ( 0 > i || 0 > j || SIZE <= i || SIZE <= j)
    {
        apiPrintMsg(MSG_SYS_INNER_ERROR);
        return ;
    }
    
    g_szMat[SIZE * i + j] = uData;
}

//********************************************
// Method      :  ResetMat
// Description :  重置结果矩阵
//*******************************************
void ResetMat()
{
    memset( g_szMat, 0, SIZE * SIZE * sizeof(UINT32) );
    //g_szMat[0] = g_szMat[1] = 1024; // for test
}

//********************************************
// Method      :  GetBlankCount
// Description :  获得空位数量
//*******************************************
int GetBlankCount()
{
    int i, nCount = 0;

    for ( i= 0; i < SIZE * SIZE; i++)
    {
        nCount += 0==g_szMat[i] ? 1 : 0;
    }

    return nCount;
}

//********************************************
// Method      :  CanMove
// Description :  是否存在可移动块
//*******************************************
BOOL CanMove(int i, int j)
{
    int x,y,k;
    UINT32 uData;
    int szX[4] = {-1,0,0,1};
    int szY[4] = {0,1,-1,0};

    uData = GetByPosition(i,j);
    
    for ( k = 0; k < 4; k++ )
    {
        x = i + szX[k];
        y = j + szY[k];
        if ( 0 <= x && 0 <= y && SIZE > x && SIZE > y &&  uData == GetByPosition(x,y))
        {
            return TRUE;
        }
    }

    return FALSE;
}

//********************************************
// Method      :  IsLose
// Description :  是否失败
//*******************************************
BOOL IsLose()
{
    int i,j;

    if ( 0 < GetBlankCount() )
    {
        return FALSE;
    }

    for ( i = 0; SIZE > i; i++)
    {
        for ( j = 0; SIZE > j; j++)
        {
            if ( TRUE == CanMove(i,j) )
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

//********************************************
// Method      :  IsWin
// Description :  是否胜利
//*******************************************
BOOL IsWin()
{
    int i;

    for ( i= 0; SIZE * SIZE > i; i++)
    {
        if ( 2048 == g_szMat[i] )
        {
            return TRUE;
        }
    }

    return FALSE;
}

//********************************************
// Method      :  AddRandNum
// Description :  向结果矩阵添加新数字
//*******************************************
void AddRandNum()
{
    int i,iPos;
    int iBlankCount = GetBlankCount();

    if ( 0 == iBlankCount )
    {
        apiPrintMsg(MSG_SYS_INNER_ERROR);
        return;
    }
    
    srand( (unsigned int)time(0) );
    iPos = rand() % iBlankCount + 1;
   
    for ( i= 0; i < SIZE * SIZE; i++)
    {
        if ( 0 == g_szMat[i] )
        {
            iPos --;
            if ( 0 == iPos)
            {
                srand( (unsigned int)time(0) );
                g_szMat[i] = ( rand() % 2 + 1 ) * 2;
                return ;
            }
        }
    }

    apiPrintMsg(MSG_SYS_INNER_ERROR);
}

//********************************************
// Method      :  MoveOneStep
// Description :  尝试从(i1,j1)移动到(i2,j2),返回移动结果，更新uData
//********************************************
BOOL MoveOneStep(int i1, int j1, int i2, int j2, UINT32* uData)
{
    UINT32 uNextData = GetByPosition( i2, j2 );

    if( 0 == uNextData )
    {
        SetAddMatByPosition( i2, j2, GetAddMatByPosition( i1, j1 ) );
        SetAddMatByPosition( i1, j1, FALSE );

        SetByPosition( i1, j1, 0 );
        SetByPosition( i2, j2, (*uData) );
        
        return TRUE;
    }
    
    if ( FALSE == GetAddMatByPosition( i1, j1 ) &&
         FALSE == GetAddMatByPosition( i2, j2 ) &&
         (*uData) == uNextData )
    {
        (*uData) *= 2;
        
        SetByPosition( i1, j1, 0 );
        SetByPosition( i2, j2, (*uData) );

        SetAddMatByPosition( i2, j2, TRUE );
       
        return TRUE;
    }
    
    return FALSE;
}

//********************************************
// Method      :  MoveOneUp
// Description :  向上移动一块
//********************************************
BOOL MoveOneUp(int i, int j)
{
    BOOL bHasMoved = FALSE;
    UINT32 uData = GetByPosition( i, j );
    
    if ( 0 == uData )
    {
        return FALSE;
    }

    while ( i > 0 )
    {
        if( FALSE == MoveOneStep( i, j, i-1, j, &uData) )
        {
            break; // 此次没有移动结束循环
        }
        else
        {
            bHasMoved = TRUE; // 此次移动更新状态
            i--;
        }
    }

    return bHasMoved;
}

//********************************************
// Method      :  MoveOneDown
// Description :  向下移动一块
//********************************************
BOOL MoveOneDown(int i, int j)
{
    BOOL bHasMoved = FALSE;
    UINT32 uData = GetByPosition( i, j );
    
    if ( 0 == uData )
    {
        return FALSE;
    }
    
    while ( i < (SIZE - 1) )
    {
        if( FALSE == MoveOneStep( i, j, i + 1, j, &uData) )
        {
            break; // 此次没有移动结束循环
        }
        else
        {
            bHasMoved = TRUE; // 此次移动更新状态
            i++;
        }
    }
    
    return bHasMoved;
}

//********************************************
// Method      :  MoveOneLeft
// Description :  向左移动一块
//********************************************
BOOL MoveOneLeft(int i, int j)
{
    BOOL bHasMoved = FALSE;
    UINT32 uData = GetByPosition( i, j );
    
    if ( 0 == uData )
    {
        return FALSE;
    }
    
    while ( j > 0 )
    {
        if( FALSE == MoveOneStep( i, j, i, j-1, &uData) )
        {
            break; // 此次没有移动结束循环
        }
        else
        {
            bHasMoved = TRUE; // 此次移动更新状态
            j--;
        }
    }
    
    return bHasMoved;
}

//********************************************
// Method      :  MoveOneRight
// Description :  向右移动一块
//********************************************
BOOL MoveOneRight(int i, int j)
{
    BOOL bHasMoved = FALSE;
    UINT32 uData = GetByPosition( i, j );
    
    if ( 0 == uData )
    {
        return FALSE;
    }

    while ( j < ( SIZE - 1 ) )
    {
        if( FALSE == MoveOneStep( i, j, i, j+1, &uData) )
        {
            break; // 此次没有移动结束循环
        }
        else
        {
            bHasMoved = TRUE; // 此次移动更新状态
            j++;
        }
    }
    
    return bHasMoved;
}

/************************************************************************/
/*  系统调用的事件响应函数  通过api输出结果                             */
/************************************************************************/

//********************************************
// Method      :  AfterMove
// Description :  移动结束后操作
//********************************************
void AfterMove()
{
    if ( TRUE == IsWin() )
    {
        g_bIsEnd = TRUE;
        apiPrintMat(g_szMat);
        apiPrintMsg(MSG_GAME_WIN);  
        return;
    }

    AddRandNum();
    apiPrintMat(g_szMat);
    if ( TRUE == IsLose() )
    {
        g_bIsEnd = TRUE;
        apiPrintMsg(MSG_GAME_LOS);
    }
}


//********************************************
// Method      :  BeforeMove
// Description :  移动开始前操作
//********************************************
BOOL BeforeMove()
{
    if ( TRUE == g_bIsEnd )
    {
        apiPrintMsg(MSG_GAME_END);
        return FALSE;
    }

    ResetAddMat();
    return TRUE;
}

//********************************************
// Method      :  cmdSysInit
// Description :  系统初始化事件
//*******************************************
void cmdSysInit() 
{
    g_bIsEnd = FALSE;
    ResetMat();
    
    AddRandNum();
    AddRandNum();

    apiPrintMsg(MSG_SYS_INIT_SUCCESS);

    apiPrintMat(g_szMat);
}

//********************************************
// Method      :  cmdUp
// Description :  ↑
//********************************************
void cmdUp()
{
    int i,j;
    BOOL bHasMoved = FALSE;

    if ( FALSE == BeforeMove() )
    {
        return ;
    }
    
    for ( i = 0; i < SIZE ; i ++)
    {
        for ( j = 0; j < SIZE ; j ++)
        {
            bHasMoved = (TRUE == MoveOneUp(i,j) ? TRUE : bHasMoved);
        }
    }

    if ( TRUE == bHasMoved)
    {
        AfterMove();
    }
}
   

//********************************************
// Method      :  cmdDown
// Description :  ↓
//********************************************
void cmdDown()
{
    int i,j;
    BOOL bHasMoved = FALSE;
    
    if ( FALSE == BeforeMove() )
    {
        return ;
    }
    
    for ( i = SIZE - 1; i >= 0 ; i --)
    {
        for ( j = 0; j < SIZE ; j ++)
        {
            bHasMoved = (TRUE == MoveOneDown(i,j) ? TRUE : bHasMoved);
        }
    }
    
    if ( TRUE == bHasMoved)
    {
        AfterMove();
    }
}

//********************************************
// Method      :  cmdLeft
// Description :  ←
//********************************************
void cmdLeft()
{
    int i,j;
    BOOL bHasMoved = FALSE;
    
    if ( FALSE == BeforeMove() )
    {
        return ;
    }
    
    for ( j = 0; j < SIZE ; j ++)
    {
        for ( i = 0; i < SIZE ; i ++)
        {
            bHasMoved = (TRUE == MoveOneLeft(i,j) ? TRUE : bHasMoved);
        }
    }
    
    if ( TRUE == bHasMoved)
    {
        AfterMove();
    }
}

//********************************************
// Method      :  cmdRight
// Description :  →
//********************************************
void cmdRight()
{
    int i,j;
    BOOL bHasMoved = FALSE;
    
    if ( FALSE == BeforeMove() )
    {
        return ;
    }
    
    for ( j = SIZE - 1; j >= 0 ; j--)
    {
        for ( i = 0; i < SIZE ; i++)
        {
            bHasMoved = (TRUE == MoveOneRight(i,j) ? TRUE : bHasMoved);
        }
    }
    
    if ( TRUE == bHasMoved)
    {
        AfterMove();
    }
}


/************************************************************************/
/*  系统函数 ，禁止其他函数调用                                         */
/************************************************************************/

//********************************************
// Method      :  GetUserCommand
// Description :  获得输入，激发系统事件 
// Return      :  BOOL 是否退出命令
//*******************************************
BOOL GetUserCommand()
{    
    int cCmd;
    
    cCmd = getch();
    if ( cCmd == KEY_EXIT )
    {
        return FALSE;
    }

    if ( cCmd == 'r' || cCmd == 'R')
    {
        cmdSysInit();
        return TRUE;
    }

    if ( cCmd == KEY_CTRL )
    {
        cCmd = getch();
        switch( cCmd )
        {
            case KEY_UP   : cmdUp();   break;
            case KEY_DOWN : cmdDown(); break;
            case KEY_LEFT : cmdLeft(); break;
            case KEY_RIGHT: cmdRight();break;
        }
    } 
    return TRUE;
}

//********************************************
// Method      :  main
// Description :  启动函数，开始主循环
//*******************************************
int main()
{
    BOOL bIsRun = TRUE;

    cmdSysInit();
    while ( TRUE == bIsRun )
    {
        bIsRun = GetUserCommand();
    }

    return 0;
}

/************************************************************************/
/*                       END OF FILE                                    */
/************************************************************************/
