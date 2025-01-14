#include <stdio.h>
#include <stdlib.h>

void printer(int currx, int curry,int *arr, int R, int C)
{
   int rx = R; 
   int cx = C;
   if (currx + rx > R)
   {
      rx = R - currx;
   }
   if (curry + cx > C)
   {
      cx = C - curry;
   }
   char *s = (char *)malloc(17576 * sizeof(char));
   int chr = 0;
   

   for (int i = 0; i<cx; i++){
      if (chr == 26){

         chr = 0;
      }
   
      
      char c = 'A' + chr;
      chr++;

      printf("%c ", c);
      
   }
   printf("\n");

   for (int i = 0; i < rx; i++)
   {
      for (int j = 0; j < cx; j++)
      {
         printf("%d ", arr[(currx + i) + C * (curry + j)]);
      }
      printf("\n");
   }
}

int main(int argc, char *argv[])
{
   if (argc != 3)
   {
      printf("Usage: %s <rows> <columns>\n", argv[0]);
      return 1;
   }

   int R = atoi(argv[1]);
   int C = atoi(argv[2]);

   int *arr = (int *)calloc(R * C, sizeof(int));
   if (arr == NULL)
   {
      printf("Memory allocation failed\n");
      return 1;
   }

   int currx = 0;
   int curry = 0;

   printer(currx, curry, arr, R,C);
   char *a = (char *)malloc(200 * sizeof(char));

   int flag = 0;
   while (1)
   {

      scanf(" %s", a);

      if (a[0] == 'w')
      {
         if (curry < 10)
         {
            flag = 1;
         }
         else
         {
            curry -= 10;
            printer(currx, curry, arr,R, C);
         }
      }
      else if (a[0] == 'd')
      {
         if (currx > C - 10)
         {
            flag = 1;
         }
         else
         {

            currx += 10;
            printer(currx, curry, arr, R, C);
         }
      }
      else if (a[0] == 'a')
      {
         if (currx < 10)
         {
            flag = 1;
         }
         else
         {
            currx -= 10;
            printer(currx, curry, arr,R, C);
         }
      }
   
   else if (a[0] == 's')
   {
      if (curry > R - 10)
      {
         flag = 1;
      }
      else
      {
         curry += 10;
         printer(currx, curry, arr, R,C);
      }
   }
   else if (a[0] == 'q')
   {
      break;
   }
   else
   {
      printf("unrecognized command");
   }
   if (flag)
   {
      printf("Invalid\n");
      flag = 0;
   }
   else
   {
      printf("Ok\n");
   }

   free(a);
   char *a = (char *)malloc(200 * sizeof(char));
   free(arr);


   }


return 0;
}

