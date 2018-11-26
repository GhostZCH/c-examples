////////////////////////////////////////////////
//                                            //
//    Time : 2011-1                           //
//                                            //
////////////////////////////////////////////////



# include <stdio.h>

int count = 1;

void move(char a,char b,FILE *fp )
{
   fprintf(fp,"%d: %c-->%c\n",count,a,b);
   printf("%d: %c-->%c\n",count++,a,b); 
}

void HTower(char a,char b,char c,int n)
{
   FILE *fp ;
   fp = fopen("a.data","a");
   if( n>1)  { HTower(a,c,b,n-1);   move(a,c,fp); HTower(b,a,c,n-1); }
   else  if(n==1)  move(a,c,fp);
}

int main()
{ 
   int n =1;
   
   while(n)
   {
     scanf("%d",&n); 
     HTower('A','B','C',n);
   }
   getchar();
   return 0;
}
