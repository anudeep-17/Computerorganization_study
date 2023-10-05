#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "ABMMethods.h"
#include "IndexKeywordCommandPair.h"
#include "Stack.h"
#include "Variablearray.h"
#include "addresstovaluedict.h"
#include "VariableManager.h"


void trimleadtrailspaces(char *line) {
    if(line == NULL)
    {
	     return;
    }

    char *start = line;

    while(isspace(*start))
    {
	     start++;
    }

    if(*line == '\0')
    {
	     return;
    }

    char *endchar = line + strlen(line) - 1;

    while(endchar > line && isspace(*endchar))
    {
	     endchar--;
    }

    *(endchar + 1) = '\0';

    if(start != line)
    {
	      memmove(line, start, strlen(start) + 1);
    }
}

void abmkeywordhelper(struct Pair* pair, struct CharStack* stack, struct VariableContainer* container, struct Map* labellocations)
{

    bool Beginevoked = false;
    bool Callevoked = false;
    bool Returnedfromcall = false;

    struct CharStack lastcontrol;
    initialize(&lastcontrol);

    int returnables = 0;

    for (int i = 0; i < pair->currentpairsize; i++)
    {
      char *keyword = pair->Key_CommandPair[i].keyword;
      char *command = pair->Key_CommandPair[i].command;

      if(strcmp(keyword, "show") == 0)
      {
        printf("%s\n", command);
      }
      else if (strcmp(keyword, "print") == 0)
      {

        char *peek = PeekStack(stack);
        (peek != NULL) ? printf("%s\n", peek)
                       : printf("no element in stack to peek\n");
      }
      else if (strcmp(keyword, "pop") == 0)
      {
        PopStack(stack);
      }
      else if (strcmp(keyword, "push") == 0)
      {
        PushIntoStack(stack, command);
        // PrintStack(stack);
      }
      else if (*keyword == '-')
      {
        int right = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int left = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int result = left - right;
        char resulttochar[result];
        sprintf(resulttochar, "%d", result);
        PushIntoStack(stack, resulttochar);
      }
      else if (*keyword == '+')
      {
        int right = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int left = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int result = left + right;
        char resulttochar[result];
        sprintf(resulttochar, "%d", result);
        PushIntoStack(stack, resulttochar);
      }
      else if (*keyword == '*')
      {
        int right = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int left = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int result = left * right;
        char resulttochar[result];
        sprintf(resulttochar, "%d", result);
        PushIntoStack(stack, resulttochar);
      }
      else if (*keyword == '/')
      {

        int right = !isEmpty(stack) ? atoi(PopStack(stack)) : 1;
        int left = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int result = left / right;
        char resulttochar[result];
        sprintf(resulttochar, "%d", result);
        PushIntoStack(stack, resulttochar);
      }
      else if (strcmp(keyword, "div") == 0)
      {
        int right = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int left = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int result = left % right;
        char resulttochar[result];
        sprintf(resulttochar, "%d", result);
        PushIntoStack(stack, resulttochar);
      }
      else if (*keyword == '&')
      {
        int right = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int left = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        bool result = ((left == 1) ? true : false) & ((right == 1) ? true : false);
        PushIntoStack(stack, result ? "1" : "0");
	    }
      else if (*keyword == '|')
      {
        int right = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int left = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        bool result = ((left == 1) ? true : false) | ((right == 1) ? true : false);
        PushIntoStack(stack, result ? "1" : "0");
      }
      else if (*keyword == '!')
      {
        int argument = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        bool result = ((argument == 1) ? false : true);
        PushIntoStack(stack, result ? "1" : "0");
      }
      else if (strcmp(keyword, "<>") == 0)
      {
        int right = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int left = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        bool result = (left != right);
        PushIntoStack(stack, result ? "1" : "0");
      }
      else if (strcmp(keyword, "<=") == 0)
      {
        int right = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int left = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        bool result = left <= right;
        PushIntoStack(stack, result ? "1" : "0");
      }
      else if (strcmp(keyword, ">=") == 0)
     {
        int right = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int left = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        bool result = left >= right;
        PushIntoStack(stack, result ? "1" : "0");
      }
      else if (strcmp(keyword, "<") == 0)
      {
        int right = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int left = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        bool result = left < right;
        PushIntoStack(stack, result ? "1" : "0");
      }
      else if (strcmp(keyword, ">") == 0)
      {
        int right = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        int left = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        bool result = left > right;
        PushIntoStack(stack, result ? "1" : "0");
      }
      else if (strcmp(keyword, "rvalue") == 0)
      {

        // if((Beginevoked && Callevoked) && FindInAboveScope(container, command) == INT_MIN && FindInContainer(container, command) != INT_MIN)
        // {
        // 	     int value = FindInContainer(container, command);
        // 	     char charvalue[20];
        //        sprintf(charvalue, "%d", value);
        // 	     PushIntoStack(stack, charvalue);
        // }
        // else if(!Beginevoked && !Callevoked && FindInContainer(container, command) != INT_MIN)
        // {
        //
      	//      int value = FindInContainer(container, command);
      	//      char charvalue[20];
        //      sprintf(charvalue, "%d", value);
      	//      PushIntoStack(stack, charvalue);
        // }
        // else if((Beginevoked || Callevoked) && FindInAboveScope(container, command) != INT_MIN)
        // {
        //   printf("I am here\n\n\n\n");
        //   int value = FindInAboveScope(container, command);
        //   char charvalue[20];
        //   sprintf(charvalue, "%d", value);
        //   PushIntoStack(stack, charvalue);
        // }
        // else
        // {
        //   	insertIntoContainer(container, command, 0);
        //   	PushIntoStack(stack, "0");
        // }

        if(Beginevoked && !Callevoked)
        {
          if(FindInAboveScope(container, command) != INT_MIN)
          {
             int value = FindInAboveScope(container, command);
      	     char charvalue[20];
             sprintf(charvalue, "%d", value);
      	     PushIntoStack(stack, charvalue);
          }
          else if(FindInContainer(container, command) != INT_MIN)
          {
            int value = FindInContainer(container, command);
            char charvalue[20];
            sprintf(charvalue, "%d", value);
            PushIntoStack(stack, charvalue);
          }
          else
          {
            insertIntoContainer(container, command, 0);
          	PushIntoStack(stack, "0");
          }
        }
        else
        {
          if(FindInContainer(container, command) !=INT_MIN)
          {
            int value = FindInContainer(container, command);
            char charvalue[20];
            sprintf(charvalue, "%d", value);
            PushIntoStack(stack, charvalue);
          }
          else
          {
            insertIntoContainer(container, command, 0);
          	PushIntoStack(stack, "0");
          }
        }

      }
      else if (strcmp(keyword, "lvalue") == 0)
      {
        if (FindInContainer(container, command) != INT_MIN)
        {
            PushIntoStack(stack, getaddressfromContainer(container, command));
        }
        else
        {
          insertIntoContainer(container, command, 0);
          PushIntoStack(stack, getaddressfromContainer(container, command));
        }

        if(Returnedfromcall == true && Beginevoked == true)
        {
          returnables++;
        }

      }

      else if (strcmp(keyword, ":=") == 0)
      {
        int value = !isEmpty(stack) ? atoi(PopStack(stack)) : 0;
        char *address = !isEmpty(stack) ? PeekStack(stack) : NULL;

        if (FindInContainerbyaddress(container, address) != -1)
        {
          updateContainerbyaddress(container, address, value);
        	PopStack(stack);
        }
        else
        {
        	printf("given l value never "
        	       "encountered...\n");
        }

      }
      else if (strcmp(keyword, "copy") == 0)
      {
        PushIntoStack(stack, PeekStack(stack));
      }
      else if (strcmp(keyword, "label") == 0)
      {
        char key[1024]="";
        if(find(labellocations,strcat(strcat(strcat(key, keyword), " "), command)) != -1)
        {
          return;
        }
        else
        {
          printf("label asked is not in given map, failed \n");
        }

      }
      else if (strcmp(keyword, "call") == 0)
      {
        char key[1024]= "";
        strcat(strcat(strcat(key, "label"), " "), command);
        int index = find(labellocations, key);

        char numtochar[100];
        sprintf(numtochar, "%d", i);
        PushIntoStack(&lastcontrol, numtochar);

        Callevoked = true;
        i = index+1;

      }
      else if(strcmp(keyword, "return") == 0)
      {
        Returnedfromcall = true;
        Callevoked = false;

        int lastcontrolindex = atoi(PopStack(&lastcontrol));
        i = lastcontrolindex;
      }
      else if(strcmp(keyword, "begin") == 0)
      {
        Beginevoked = true;
        NewScope(container);
      }
      else if(strcmp(keyword, "end") == 0)
      {
        if(returnables > 0)
        {
          MakeReturnablesAccesible(container, returnables);
        }
        DeleteScope(container);
        Beginevoked = false;
      }
      else if(strcmp(keyword, "goto") == 0)
      {
        char key[1024]= "";
        strcat(strcat(strcat(key, "label"), " "), command);
        int index = find(labellocations, key);

        char numtochar[100];
        sprintf(numtochar, "%d", i);
        PushIntoStack(&lastcontrol, numtochar);

        i=index+1;
      }
      else if(strcmp(keyword, "halt") == 0)
      {
        exit(0);
      }
   }
}

void abminstructionrunner(FILE* abminstructionfile, struct CharStack* stack, struct VariableContainer* container)
{

    if (abminstructionfile == NULL)
    {
	     perror("Error opening file");
	     return;
    }

    char Eachline[1024];
    char *firstspace;
    char *keyword;
    char *command;

    struct Pair pair;
    struct Map labellocations;

    int linenumber = 0;

    initializePair(&pair);
    initializeMap(&labellocations);

    while (fgets(Eachline, sizeof(Eachline), abminstructionfile) != NULL)
    {
    	// printf("%s\n", Eachline);
      trimleadtrailspaces(Eachline);
    	firstspace = strchr(Eachline, ' ');

    	if (firstspace != NULL)
      {
    	    *firstspace = '\0';
    	    keyword = Eachline;
    	    command = firstspace + 1;
    	}
      else
      {
    	    keyword = Eachline;
    	    command = NULL;
    	}

    	trimleadtrailspaces(keyword);
    	trimleadtrailspaces(command);
    	addPair(&pair, linenumber, keyword, command);

      if(strcmp(keyword, "label") == 0)
      {
        char key[1024] = "";
        strcat(strcat(strcat(key, keyword), " "), command);
        insert(&labellocations,key,linenumber);
      }

    	linenumber++;
    }
    // printPair(&pair);
    // printMap(&labellocations);
    abmkeywordhelper(&pair, stack, container, &labellocations);
}
