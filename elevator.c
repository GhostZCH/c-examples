/************************************************************************/
/*                                                                      */
/*  该文件是一个模拟的电梯调度系统，通过命令行操作                      */
/*                                                                      */
/*  命令	作用	        格式	                示例                */
/*  reboot	系统初始化	    reboot	                reboot              */
/*  up  	请求上楼	    up 时间 当前层 目标层	up 1 8 10           */
/*  down	请求下楼	    down时间 当前层 目标层	down 3 1 -2         */
/*  state	查看当前状态	state 电梯号	        State 1             */
/*  history	查看历史记录	history             	history             */
/*  exit	系统退出	    exit	                exit                */
/*                                                                      */
/* 备注：                                                               */
/*    1. 正确的电梯编号 [1, 5]，正确时间[0,99], 正确楼层{-2,-1,0,1..10} */
/*    2. state 命令使用参数 0 查询所有记录  例：state 0                 */
/*    3. vc6.0 编译通过                                                 */
/*                                                                      */
/* 作者: Ghost      2014年1月                                           */ 
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MATH.H>
#include <TIME.H>

/************************************************************************/
/*   基础数据定义                                                       */
/************************************************************************/

// 定义
#define MAX_MSG_LEN        100 
#define MAX_CMD_TMP_LEN    100

#define MIN_ELV_ID         1
#define MAX_ELV_ID         5
#define ELV_COUNT          MAX_ELV_ID - MIN_ELV_ID + 1

#define MIN_TIME           0
#define MAX_TIME           99

// 生成消息集合相关的宏定义
#define END_ID_OF_MSG_SET      -1
#define BEGIN_MSG_SET          MSG_INFO g_szMsg[] = {
#define ADD_MSG(ID,MSG_STR)    {ID,MSG_STR},
#define END_MSG_SET            {END_ID_OF_MSG_SET,""}};

// 布尔值
typedef enum emBoolVal
{
    TRUE,
    FALSE
}BOOL;

// 消息ID 系统通过该ID打印消息
typedef enum emMsgID
{
    MSG_SYS_INNER_ERROR  = 0 ,  /* 内部错误     */
    MSG_SYS_INIT_SUCCESS = 1 ,  /* 初始化成功   */
    MSG_SYS_BAD_CMD      = 2 ,  /* 命令错误     */
    MSG_ELV_TIM_ERR      = 11,  /* 时间非法     */
    MSG_ELV_FLR_ERR      = 12,  /* 楼层非法     */
    MSG_ELV_NUM_ERR      = 13,  /* 编号非法     */
    MSG_ELV_FLR_SUB      = 14,  /* 楼层逻辑错误 */
    MSG_ELV_TIM_SUB      = 14,  /* 时间错误递减 */
    MSG_ELV_NO_HIS       = 15,  /* 无历史记录   */
}MSG_ID_EM;

// 消息结构体
typedef struct tagMsgInfo
{
    MSG_ID_EM emID;
    char szMessage[MAX_MSG_LEN];
}MSG_INFO;

// 生成消息集，供系统按ID打印消息
BEGIN_MSG_SET
    ADD_MSG(MSG_SYS_INNER_ERROR , "内部错误"    )
    ADD_MSG(MSG_SYS_INIT_SUCCESS, "初始化成功"  )
    ADD_MSG(MSG_SYS_BAD_CMD     , "命令错误"    )
    ADD_MSG(MSG_ELV_TIM_ERR     , "时间非法"    )
    ADD_MSG(MSG_ELV_FLR_ERR     , "楼层非法"    )
    ADD_MSG(MSG_ELV_NUM_ERR     , "编号非法"    )
    ADD_MSG(MSG_ELV_FLR_SUB     , "楼层逻辑错误")
    ADD_MSG(MSG_ELV_TIM_SUB     , "时间错误递减")
    ADD_MSG(MSG_ELV_NO_HIS      , "无历史记录"  )
END_MSG_SET

// 电梯状态
typedef enum emElvState
{
    ELV_UP,     // 向上
    ELV_DOWN,   // 向下
    ELV_STOP    // 停止
}ELV_STATE_EM;

// 命令类型
typedef enum emCmd
{
    CMD_UP,     // 向上
    CMD_DOWN,   // 向下
}CMD_EM;

// 处理状态
typedef enum emProcessState
{
    PRO_HAS,    // 已处理
    PRO_NOT,    // 未处理
}PROCESS_STATE_EM;

// 电梯信息
typedef struct stElevatorInfo  
{
    int iElvID;             //  编号
    int iFloor;             //	当前楼层
    int iTargetFloor;       //	目标楼层    
    ELV_STATE_EM emState;   //	状态
}ELV_INFO;

// 请求历史记录
typedef struct stCmdHistoryInfo  
{
    int iReqTime;              //  请求时间
    int iResTime;              //  响应时间
    int iReqFloor;             //  请求楼层  
    int iTarFloor;             //  目标楼层    
    CMD_EM emCMD ;             //  状态
    PROCESS_STATE_EM emProSta; //  处理状态
}CMD_INFO;


/************************************************************************/
/*  界面显示  API                                                       */
/************************************************************************/

char *g_szCmdTypeString[3]   = {"上升", "下降", "非法数据"};            // 命令类型的字符串

char *g_szCmdStateString[3]  = {"已处理", "未处理", "非法数据"};        // 命令状态的字符串

char *g_szElvStateString[4]  = {"上升", "下降", "等待", "非法数据"};    // 电梯状态的字符串


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
        }
    }
}

//********************************************
// Method      :  apiPrintBookInfo
// Description :  打印图书详情
// Param       :  输入 ELV_INFO* pstElvInfo 电梯数组指针
//             :  输入 int iCount  数组长度
//*******************************************
void apiPrintElvInfo(ELV_INFO* pstElvInfo, int iCount)
{
    int i = 0, iStateIndex = 2;
    
    if ( NULL == pstElvInfo || 0 >= iCount)
    {
        apiPrintMsg(MSG_SYS_INNER_ERROR);
        return;
    }
    
    printf("\n+=======================================+\n");
    printf("| 编号 |   状态   | 当前楼层 | 目标楼层 |\n");
    for ( i = 0; i < iCount; i++)
    {
        switch(pstElvInfo[i].emState)
        {
            case ELV_UP   : iStateIndex = 0; break;
            case ELV_DOWN : iStateIndex = 1; break;
            case ELV_STOP : iStateIndex = 2; break;
            default: iStateIndex = 3;
        }
        printf("|   %1d  |   %s   |    %2d    |    %2d    |\n", 
                pstElvInfo[i].iElvID, g_szElvStateString[iStateIndex],
                pstElvInfo[i].iFloor, pstElvInfo[i].iTargetFloor );
    }
    printf("+=======================================+\n\n");
}

//********************************************
// Method      :  apiPrintCmdHistoryInfo
// Description :  打印请求历史信息
// Param       :  输入 ELV_HIS_INFO *pstCmdHistroyList 命令历史记录数组指针
//             :  输入 int iCount  数组长度
//*******************************************
void apiPrintCmdHistoryInfo(CMD_INFO *pstCmdHistroyList, int iCount)
{
    int i = 0, iCmdID = 3, iStateID = 3;

    if ( 0 > iCount)
    {
        apiPrintMsg(MSG_SYS_INNER_ERROR);
        return;
    }

    if ( NULL == pstCmdHistroyList || 0 == iCount)
    {
        apiPrintMsg(MSG_ELV_NO_HIS);
        return;
    }
    
    printf("\n+===============================================================+\n");
    printf("| 请求时间 | 响应时间 | 请求类型 | 请求楼层 | 目的楼层 |  状态  |\n");
    for ( i = 0; i < iCount; i++)
    {
        switch( pstCmdHistroyList[i].emCMD )
        {
            case CMD_UP   : iCmdID = 0; break;
            case CMD_DOWN : iCmdID = 1; break;
            default: iCmdID = 2;
        }

        switch( pstCmdHistroyList[i].emProSta )
        {
            case PRO_HAS   : iStateID = 0; break;
            case PRO_NOT   : iStateID = 1; break;
            default: iStateID = 2;
        }

        printf("|    %02d    |    %02d    |   %s   |    %02d    |    %02d    | %s |\n",
                pstCmdHistroyList[i].iReqTime ,
                pstCmdHistroyList[i].iResTime ,
                g_szCmdTypeString[iCmdID]     ,
                pstCmdHistroyList[i].iReqFloor, 
                pstCmdHistroyList[i].iTarFloor,
                g_szCmdStateString[iStateID] );
    }
   printf("+===============================================================+\n\n");
}

/************************************************************************/
/*  数据存储和基础功能  被cmd函数调用 不允许调用 api                    */
/************************************************************************/

// TODO : 开发人员添加自定义变量核函数

/************************************************************************/
/*  系统调用的事件响应函数  通过api输出结果                             */
/************************************************************************/

//********************************************
// Method      :  cmdSysInit
// Description :  系统初始化事件
//*******************************************
void cmdSysInit() 
{
    // TODO: 实现该方法
    apiPrintMsg(MSG_SYS_INIT_SUCCESS);
}

//********************************************
// Method      :  cmdElvUp
// Description :  电梯上升事件
// Param       :  输入 int iTime 请求时间
//             :  输入 int iReqFloor 请求楼层（出发楼层）
//             :  输入 int iTarFloor 目标楼层（到达楼层）
//********************************************
void cmdElvUp(int iTime, int iReqFloor, int iTarFloor)
{
    // TODO: 实现该方法
    apiPrintMsg(MSG_SYS_INNER_ERROR);
}

//********************************************
// Method      :  cmdElvDown
// Description :  电梯下降事件
// Param       :  输入 int iTime 请求时间
//             :  输入 int iReqFloor 请求楼层（出发楼层）
//             :  输入 int iTarFloor 目标楼层（到达楼层）
//********************************************
void cmdElvDown(int iTime, int iReqFloor, int iTarFloor)
{
    // TODO: 实现该方法
    apiPrintMsg(MSG_SYS_INNER_ERROR);
}

//********************************************
// Method      :  cmdQueryState
// Description :  查询电梯状态事件
// Param       :  输入 int iElvID 电梯编号
//********************************************
void cmdQueryState(int iElvID)
{
    // TODO: 实现该方法
    apiPrintMsg(MSG_SYS_INNER_ERROR);
//     ELV_INFO info;
//     info.emState = ELV_STOP;
//     info.iElvID = iElvID;
//     info.iFloor = 4;
//     info.iTargetFloor = 4;
//     apiPrintElvInfo(&info,1);
}

//********************************************
// Method      :  cmdQueryHistory
// Description :  查询历史
//********************************************
void cmdQueryHistory()
{
    // TODO: 实现该方法
    apiPrintMsg(MSG_SYS_INNER_ERROR);
//     ELV_HIS_INFO info ;
//     info.emCMD = CMD_DOWN;
//     info.emProSta = PRO_NOT;
//     info.iReqFloor = 3;
//     info.iReqTime = 1;
//     info.iTarFloor = -2;
//     info.iResTime = 99;
//     apiPrintCmdHistoryInfo(&info, 1);
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
    char scTmp[MAX_CMD_TMP_LEN + 1] = {0};
    int iTime = 0, iReqFloor = 0, iTarFloor = 0, iElvID = 0 ;

    printf("请输入命令\n<<");
    scanf("%s", scTmp);
    scTmp[MAX_CMD_TMP_LEN] = '\0';

    if( 0 == strcmp(scTmp, "exit") )
    {
        return FALSE;
    }

    if( 0 == strcmp(scTmp,"reboot"))
    {
        cmdSysInit();
        return TRUE;
    }

    if( 0 == strcmp(scTmp, "up") )
    {
        scanf("%d",&iTime);
        scanf("%d",&iReqFloor);
        scanf("%d",&iTarFloor);
        cmdElvUp(iTime,iReqFloor,iTarFloor);
        return TRUE;
    }

    if( 0 == strcmp(scTmp, "down") )
    {
        scanf("%d",&iTime);
        scanf("%d",&iReqFloor);
        scanf("%d",&iTarFloor);
        cmdElvDown(iTime,iReqFloor,iTarFloor);
        return TRUE;
    }

    if( 0 == strcmp(scTmp, "state") )
    {
        scanf("%d",&iElvID);
        cmdQueryState(iElvID);
        return TRUE;
    }

    if( 0 == strcmp(scTmp, "history") )
    {
        cmdQueryHistory();
        return TRUE;
    }

    apiPrintMsg(MSG_SYS_BAD_CMD);
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
