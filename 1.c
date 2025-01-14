#include <stdio.h>
#include <stdlib.h>

void printer(int currx, int curry, int *arr, int R, int C) {
    int rx = R;
    int cx = C;
    if (currx + rx > R) {
        rx = R - currx;
    }
    if (curry + cx > C) {
        cx = C - curry;
    }

    // Generate and print labels
    for (int i = 0; i < cx; i++) {
        int index = i;
        char label[4] = ""; // Holds labels like "A", "AA", "AB", ..., "ZZZ"

        // Generate the label
        int chr_pos = 0;
        do {
            label[chr_pos++] = 'A' + (index % 26);
            index /= 26;
            
        } while (index > 0);
        label[chr_pos] = '\0';

        // Reverse the label because it's built in reverse order
        for (int j = 0; j < chr_pos / 2; j++) {
            char temp = label[j];
            label[j] = label[chr_pos - j - 1];
            label[chr_pos - j - 1] = temp;
        }

        // Print the label
        printf("%s ", label);
    }

    printf("\n");

    // Print the subarray
    for (int i = 0; i < rx; i++) {
        for (int j = 0; j < cx; j++) {
            printf("%d ", arr[(currx + i) * C + (curry + j)]);
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

