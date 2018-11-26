#include <stdio.h>
#define T int 
#define SIZE 10

void PrintList(T list[],int n)
{
    int i;
    for(i=0;i<n;i++) 
      printf("[%3d]  ",list[i]);
    printf("\n");
}

void QuickSort(T list[],int left,int right)
{ 
     int lP = left,rP = right;

     T key = list[left];
     while(lP<rP)
     { 
	     while((list[rP]>=key)&&(lP<rP)) rP--; 
         list[lP]=list[rP];
         
		 while((list[lP]<=key)&&(lP<rP)) lP++;
         list[rP]=list[lP];
	 }
	 list[lP] = key;
	 	 
	 if( left<(lP-1)) QuickSort(list,left,lP-1);
 	 if((lP+1)<right) QuickSort(list,lP+1,right); 
} 



int main()
{
    T list1[SIZE] = {1,2,3,4,5,6,7,8,9,10 };
    T list2[SIZE] = {10,9,8,7,6,5,4,3,2,1 };
    T list3[SIZE] = {10,1,8,3,6,5,4,7,2,9 };
    T list4[SIZE] = {1,5,8,2,6,4,2,3,50,155 };
    T list5[SIZE] = {7,100,65,7,77,8,4,0,4,11 };
    
    PrintList(list1,SIZE);
    QuickSort(list1,0,SIZE-1);
    PrintList(list1,SIZE);
    printf("\n");
    
    PrintList(list2,SIZE);
    QuickSort(list2,0,SIZE-1);
    PrintList(list2,SIZE);
    printf("\n");
    
    PrintList(list3,SIZE);
    QuickSort(list3,0,SIZE-1);
    PrintList(list3,SIZE);
    printf("\n");
    
    PrintList(list4,SIZE);
    QuickSort(list4,0,SIZE-1);
    PrintList(list4,SIZE);
    printf("\n");
    
    PrintList(list5,SIZE);
    QuickSort(list5,0,SIZE-1);
    PrintList(list5,SIZE);
    printf("\n");
    
    getchar();
    return 0;
}
