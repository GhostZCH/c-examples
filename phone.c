

/*****************************************************************************

>>>>>>>>>>>>>>>>>>>>>>>>>>  BEGIN OF LINKLIST <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

******************************************************************************/


///////////////////////////////////////////////
//                                           //
//     CODER : ZCH                           // 
//     TIME  : 2011-3-10                     //
//     USE   : Linklist                      //
//                                           //
///////////////////////////////////////////////

# include < stdio.h >
# include < malloc.h >

/* Operation state */
typedef enum state 
{
	OK = 1 ,   
	ERROR = 0  
}STATE ;


/* node of list */
typedef struct node
{
    void *Data ;
	struct node *next ;
}NODE ;



/*  LinkList with headpointer */
typedef struct list
{
	/* lenth of list */
   int lenth ;

   /* head pointer */
   NODE * head ;

   /* tail pointer */
   NODE * tail ;
   
}LIST;



/* make a new list */
LIST * List_inital()
{
	NODE *head ;    
	LIST *list ;

	//inital head pointer
	head = ( NODE *) malloc ( sizeof(NODE) ); 
	head->Data = NULL ;
    head->Data = NULL ;

	//inital list 
    list = ( LIST *) malloc ( sizeof(LIST) );
	list ->head = head ;
	list ->lenth = 0 ;
	list ->tail = head ;

    return list ;
}


/* add a new data to list */
STATE List_Add(LIST *list , void * newdata)
{
   NODE *nd ;
   int l ;

   //  on error
   if( list==NULL || newdata==NULL  ) return ERROR ;

   l = list ->lenth ;
   l++ ;

   // build node
   nd = ( NODE * ) malloc ( sizeof(NODE) ) ;
   nd ->Data = newdata ;
   nd ->next = NULL ;

   // add to list
   list ->lenth = l;        // lenth+1
   list ->tail->next= nd ;  // add next to tail
   list ->tail = nd ;       // move tail to new tail

   return OK ;  
}




/*****************************************************************************

>>>>>>>>>>>>>>>>>>>>>>>>>>   END OF LINKLIST  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

******************************************************************************/
















/*****************************************************************************

>>>>>>>>>>>>>>>>>>>>>>>>>>>>    BEGIN OF MAIN  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

******************************************************************************/



///////////////////////////////////////////////
//                                           //
//     CODER : ZCH                           // 
//     TIME  : 2011-3-11                     //
//     USE   : Phone book                    //
//                                           //
///////////////////////////////////////////////

#define FILE_NAME "data.txt"
#define RECORD_SIZE sizeof(RECORD) 


/* operation */
typedef enum operation
{
	START = -1 ,
	EXIT = 0 ,
	ADDNEW = 1 ,
	DELETE = 2 ,
	EDIT = 3  ,
	VIEW = 4
}OPERATION ;


/* record */
typedef struct record
{
	char name[20] ;
	char number[12] ;
}RECORD;


/* main data */
LIST *list ;


/* print a record */
void RECORD_Printf(RECORD * record)
{
   printf( "name   : %s          ", record->name ) ;
   printf( "number : %s\n", record->number);
}


/* load data from file */
void LoadDatas()
{
	FILE *fp;
	RECORD * r ;

    list = List_inital();
	fp = fopen ( FILE_NAME, "r" ) ;
   
	while(!feof(fp))
	{
       r = ( RECORD* ) malloc ( RECORD_SIZE ); 
       if( fread( r , RECORD_SIZE , 1 , fp ) )
          List_Add(list,r);
	}

	fclose(fp);
}


/* show operations */
void PrintfOption ()
{
	printf("--------------------------------------------------------------------------------\n ");
	printf("\t\tClick number buttun to chose operation !\n\n ") ;
	printf("\t\t 0. EXIT   \n\n ") ;
	printf("\t\t 1. ADDNEW \n\n ") ;
	printf("\t\t 2. DELETE \n\n ") ;
	printf("\t\t 3. EDIT   \n\n ") ;
	printf("\t\t 4. VIEW   \n\n ") ;
}

/* end of an option */
void PrintfEnd()
{
  	printf("--------------------------------------------------------------------------------\n ");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n");
}

/* get user chose operaion */
OPERATION GetChoseOption ()
{
   OPERATION op ;
   int i ;
   char s[10];

   scanf( "%s", s ) ;
   i =s[0]-'0';

   while( i>4 || i<0 ) 
   {
	   printf("\tNo such operation ,please chose again !\n\n");
	    scanf( "%s", s ) ;
        i =s[0]-'0';
   }

   op = (OPERATION) i ;

   return op ;
}


/* Add new */
void OP_Addnew()
{
   RECORD *re ;
   FILE *fp ;
   
   fp = fopen( FILE_NAME ,"a");

   re = ( RECORD *) malloc ( RECORD_SIZE ) ;

   // get name
   printf("please in put user name !\n") ;
   scanf( "%s" , re->name );

   // get number
   printf("please in put user telephone number !\n");
   scanf( "%s" , re ->number ) ;

   // add to list
   List_Add( list , re );

   // add to file 
   fwrite( re , RECORD_SIZE , 1 , fp );

   fclose(fp);
}


/* Delete */
void OP_Delete()
{

}


/* edit */
void OP_Edit()
{

}


/* view all record */
void OP_View()
{
   int i = 1 ;
   NODE * head ;

   head = list->head ;

   while ( head->next != NULL )
   {
	  head = head->next ;
	  printf("%d . ", i++);
	  RECORD_Printf( head->Data );
   }
}


/* operation handler */
void HandleOption (OPERATION op)
{
   switch (op)
   {
      case  START   :                 break ;
      case	EXIT    :                 break ;
      case	ADDNEW  :  OP_Addnew() ;  break ;
      case	DELETE  :  OP_Delete() ;  break ;
      case	EDIT    :  OP_Edit()   ;  break ;
      case	VIEW    :  OP_View()   ;  break ;
   }
}




/* main() */
void main()
{
   OPERATION op = START;

   LoadDatas() ;

   while(op)
   {
       PrintfOption () ;
	   op = GetChoseOption () ;
       HandleOption(op) ;
	   PrintfEnd() ;
   }
}


/*****************************************************************************

>>>>>>>>>>>>>>>>>>>>>>>>>>>>   END OF MAIN     <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

******************************************************************************/
