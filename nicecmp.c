#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>


#define LINELEN (80)
char *mygets(char *buf, int len);
int mygeti();

int main(int argc, char *argv[])
{
	char *cmpstr[] = {"lexcmp", "lencmp"};
	int veclen = sizeof(cmpstr)/sizeof(char *);
	char str1[LINELEN + 1];
	char str2[LINELEN + 1];
	int index;
	int father_lex[2];
	int lex_father[2];
	int father_len[2];
	int len_father[2];
	char* arr[3];
	
	if (pipe(father_lex)==-1)
	{
		printf("pipe filed");
		return -2;
	}
	if (pipe(lex_father)==-1)
	{
		printf("pipe filed");
		return -2;
	}
	if (pipe(father_len)==-1)
	{
		printf("pipe filed");
		return -2;
	}
	if (pipe(len_father)==-1)
	{
		printf("pipe filed");
		return -2;
	}
	int pid_lex = fork();	
	if (pid_lex>0) 
	{
		int pid_len=fork();
		if (pid_len>0) //in father
		{
			if (close(father_lex[0])==-1)
			{
				printf("close read filed\n");
				return -2;
			}
			if (close(lex_father[1])==-1)
			{
				printf("close write filed\n");
				return -2;
			}
			if (close(father_len[0])==-1)
			{
				printf("close read filed\n");
				return -2;
			}
			// close the option of the son to write
			if (close(len_father[1])==-1)
			{
				printf("close write filed\n");
				return -2;
			}
			while (1)
			{
				printf("Please enter first string:\n");
				if (mygets(str1, LINELEN) == NULL)
					break;
				printf("Please enter second string:\n");
				if (mygets(str2, LINELEN) == NULL)
					break;
				do {
					printf("Please choose:\n");
					for (int i=0 ; i < veclen ; i++)
						printf("%d - %s\n", i, cmpstr[i]);
					index = mygeti();
				} while ((index < 0) || (index >= veclen));
				if (index==0)
				{
					printf("lexcmp\n");
					int len1=strlen(str1);
					if (write(father_lex[1], str1, len1) != len1)  
						printf("write str1 filed\n");
					if (write(father_lex[1],"\n",1)!=1)
						printf("write enter 1 filed\n");
					int len2=strlen(str2);
					if (write(father_lex[1], str2, len2) != len2)  
						printf("write str2 filed\n");
					if (write(father_lex[1],"\n",1)!=1)
						printf("write enter 2 filed\n");
					char ans;
					do{
						read(lex_father[0],&ans,sizeof(char));
					}while (isspace(ans));
					printf("str1= %s , str2= %s, ans= %c\n",str1,str2,ans);
				}
				if (index==1)
				{
					printf("lencmp\n");
					int len1=strlen(str1);
					if (write(father_len[1], str1, len1) != len1)  
						printf("write str1 filed\n");
					if (write(father_len[1],"\n",1)!=1)
						printf("write enter 1 filed\n");
					int len2=strlen(str2);
					if (write(father_len[1], str2, len2) != len2)  
						printf("write str2 filed\n");
					if (write(father_len[1],"\n",1)!=1)
						printf("write enter 2 filed\n");
					char ans;
					do{
						read(len_father[0],&ans,sizeof(char));
					}while (isspace(ans));
					printf("str1= %s , str2= %s, ans= %c\n",str1,str2,ans);	
					
				}
			fflush(stdout);
			}
			
		}
		
		else if(pid_len==0) //len child
		{
			if (dup2(father_len[0], STDIN_FILENO) == -1) {
				printf("change child in failed");
				exit(-2);
			}
			if (dup2(len_father[1], STDOUT_FILENO) == -1) {
				printf("change child out failed");
				exit(-2);
			}		
			if(close(father_len[1])==-1)
			{
				printf("close in child in pipe failed");
				exit(-2);
			}
			if(close(len_father[0])==-1)
			{
				printf("close in child out pipe failed");
				exit(-2);
			}
			arr[0]="./loopcmp";
			arr[1]=cmpstr[1];
			arr[2]=NULL;	
			execvp(arr[0],arr);
			return -2;
		}
		else
		{
			printf("fork faild");
			exit(-2);
		}
	}
	else if (pid_lex==0) //lex chiled
	{
		if (dup2(father_lex[0], STDIN_FILENO) == -1) {
			printf("change child in failed");
			exit(-2);
		}
		if (dup2(lex_father[1], STDOUT_FILENO) == -1) {
			printf("change child out failed");
			exit(-2);
		}		
		if(close(father_lex[1])==-1)
		{
			printf("close in child in pipe failed");
			exit(-2);
		}
		if(close(lex_father[0])==-1)
		{
			printf("close in child out pipe failed");
			exit(-2);
		}
		arr[0]="./loopcmp";
		arr[1]=cmpstr[0];
		arr[2]=NULL;	
		execvp(arr[0],arr);
		return -2;
	}
	else
	{
		printf("fork faild");
		exit(-2);
	}			
	fflush(stdout);
	exit(-1);
}

char *mygets(char *buf, int len)
{
	char *retval;

	retval = fgets(buf, len, stdin);
	buf[len] = '\0';
	if (buf[strlen(buf) - 1] == 10) /* trim \r */
		buf[strlen(buf) - 1] = '\0';
	else if (retval) 
		while (getchar() != '\n'); /* get to eol */

	return retval;
}

int mygeti()
{
	int ch;
	int retval=0;

	while(isspace(ch=getchar()));
	while(isdigit(ch))
	{
		retval = retval * 10 + ch - '0';
		ch = getchar();
	}
	while (ch != '\n')
		ch = getchar();
	return retval;
}
