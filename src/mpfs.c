//MPFS - MysticPrism File System
//MAX File Size 512KB
//
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 32 
#define MAX_BUFF 512000

typedef struct folder //Directory descriptor
{	char name[MAX];
	long int up, in, next, prev;
	int file_in;
	int size;
	long int addr; 
}folder; 

typedef struct file_s  //File descriptor
{
	char name[MAX];
	long next, prev;
	long up, addr;
	int data_addr;
	int size;
}file_t;

typedef char buff[MAX_BUFF];

void init_fs() //Initializing file system
{
	folder *root;
	FILE *fp;
	
	root = (folder*)malloc(sizeof(folder));
	strcpy(root->name, "root");	
	root->up=0;
	root->in = 0;
	root->next = 0;
	root->prev= 0;
	root->file_in = 0;
	root->addr=0;

	fp = fopen("mpfs.fs","w");
	if(fp==NULL)
	{
		printf("\nFILESYSTEM CREATION ERROR!!!");
		exit(1);
	}

	fwrite(root, sizeof(folder),1,fp);
	fclose(fp);
}

void my_mkdir(folder *CURR_DIR, char name[MAX], FILE *fp) //Making new directory in current directory
{
	folder *temp, *temp_d;

	temp = (folder *)malloc(sizeof(folder));
	strcpy(temp->name,name);
	temp->up=CURR_DIR->addr;
	temp->in = 0;
	temp->next = 0;
	temp->prev= 0;
	temp->file_in = 0;
	

	fseek(fp,0,SEEK_END);
	temp->addr=ftell(fp);
	fwrite(temp,sizeof(folder),1,fp);		


	if(CURR_DIR->in == 0)
	{
		CURR_DIR->in = temp->addr;
		fseek(fp,CURR_DIR->addr,SEEK_SET);
		fwrite(CURR_DIR,sizeof(folder),1,fp);
	}
	else
	{
		temp_d = (folder *)malloc(sizeof(folder));

		fseek(fp,CURR_DIR->in,SEEK_SET);
		fread(temp_d, sizeof(folder), 1,fp);

		while(temp_d->next!=0)
		{
			fseek(fp,temp_d->next,SEEK_SET);
			fread(temp_d, sizeof(folder), 1,fp);
		}

		temp_d->next = temp->addr;
		temp->prev = temp_d->addr;
		fseek(fp,temp->addr,SEEK_SET);	
		fwrite(temp,sizeof(folder),1,fp);
		fseek(fp,temp_d->addr,SEEK_SET);
		fwrite(temp_d,sizeof(folder),1,fp);
	}
	
}

folder *my_cd(char name[MAX], folder *CURR_DIR, FILE *fp) //Changing directory to name specified by user
{
	folder *temp;

	temp = (folder *)malloc(sizeof(folder));
	fseek(fp,CURR_DIR->in,SEEK_SET);
	fread(temp, sizeof(folder),1,fp);

	if(strcmp(name,"..")==0)
	{
		fseek(fp,CURR_DIR->up,SEEK_SET);
		fread(CURR_DIR,sizeof(folder),1,fp);
		return CURR_DIR;
	}	
	while(strcmp(temp->name,name)!=0 && temp->next!=0)
	{
		fseek(fp,temp->next,SEEK_SET);
		fread(temp, sizeof(folder),1,fp);			
	}			
	
	if(strcmp(temp->name,name)==0)
		CURR_DIR = temp;
	else
		printf("NO SUCH DIRECTORY!!");		

	return CURR_DIR;
}


void my_ls(folder *CURR_DIR, FILE *fp) //Listing directories & files
{
	folder *temp;
	file_t *temp_f;
	temp = (folder*)malloc(sizeof(folder));
	
	if(CURR_DIR->in!=0)
	{
		fseek(fp,CURR_DIR->in,SEEK_SET);
		fread(temp,sizeof(folder),1,fp);
		
		while(temp->next!=0)
		{
			printf("%s\\ \t", temp->name);
			fseek(fp,temp->next,SEEK_SET);
			fread(temp,sizeof(folder),1,fp);
		}
		
		printf("%s\\ \t", temp->name);
	}

	if(CURR_DIR->file_in!=0 )
	{
		temp_f = (file_t*)malloc(sizeof(file_t));
	
		fseek(fp,CURR_DIR->file_in,SEEK_SET);
		fread(temp_f,sizeof(file_t),1,fp);
		
		while(temp_f->next!=0)
		{
			printf("%s\t", temp_f->name);
			fseek(fp,temp_f->next,SEEK_SET);
			fread(temp_f,sizeof(file_t),1,fp);
		}
		
		printf("%s\t", temp_f->name);
	}
	
}
 
folder *create_file(folder *CURR_DIR, FILE *fp, char name[MAX]) //Creating new file in current directory
{
	file_t *new, *temp_f;
	
	long f_addr;	
	buff file_new;
	

	new = (file_t *)malloc(sizeof(file_t));
	
	fflush(NULL);	
	printf("TYPE FILE CONTENTS:\n");
	fgets(file_new,MAX_BUFF,stdin);
	fgets(file_new,MAX_BUFF,stdin);
	fseek(fp,0,SEEK_END);
	f_addr = ftell(fp);
	fwrite(file_new,sizeof(file_new),1,fp);
	
	strcpy(new->name,name);
	new->up = CURR_DIR->addr;
	new->next=0;
	new->prev=0;
	new->addr=ftell(fp);
	new->data_addr = f_addr;
	new->size = sizeof(file_new);
	fwrite(new,sizeof(file_t),1,fp);

	if(CURR_DIR->file_in==0)
	{
		CURR_DIR->file_in = new->addr;
		fseek(fp,CURR_DIR->addr,SEEK_SET);
		fwrite(CURR_DIR,sizeof(folder),1,fp);		
	}
	else
	{
		temp_f = (file_t *)malloc(sizeof(file_t));

		fseek(fp,CURR_DIR->file_in,SEEK_SET);
		fread(temp_f, sizeof(file_t), 1,fp);

		while(temp_f->next!=0)
		{
			fseek(fp,temp_f->next,SEEK_SET);
			fread(temp_f, sizeof(file_t), 1,fp);
		}

		temp_f->next = new->addr;
		new->prev = temp_f->addr;
		fseek(fp,new->addr,SEEK_SET);	
		fwrite(new,sizeof(file_t),1,fp);
		fseek(fp,temp_f->addr,SEEK_SET);
		fwrite(temp_f,sizeof(file_t),1,fp);
	}

	return CURR_DIR;
}

void open(char ch[MAX], folder *root,FILE *fp)//Opening file specified by user in current directory  
{	
	file_t *temp;
	buff buff_t;
	int i=0;
	temp = (file_t *)malloc(sizeof(file_t));
	fseek(fp,root->file_in,SEEK_SET);
	fread(temp,sizeof(file_t),1,fp);

	while(strcmp(temp->name,ch)!=0)
	{
		if(temp->next==0)
		{
			printf("\nNo such file!!");
			return;
		}
		fseek(fp,temp->next,SEEK_SET);
		fread(temp,sizeof(file_t),1,fp);
	}

	fseek(fp,temp->data_addr,SEEK_SET);
	fread(buff_t,sizeof(temp->size),1,fp);
	puts(buff_t);
}


void rm(folder *CURR_DIR, char name[MAX], FILE *fp)//Remove file from current directory
{
	file_t *temp, *temp1;

	temp = (file_t *)malloc(sizeof(file_t));
	fseek(fp,CURR_DIR->file_in,SEEK_SET);
	fread(temp,sizeof(file_t),1,fp);

	while(strcmp(temp->name,name)!=0)
	{
		if(temp->next==0)
		{
			printf("\nNo such file!!");
			return;
		}
		fseek(fp,temp->next,SEEK_SET);
		fread(temp,sizeof(file_t),1,fp);
	}
	
	if(CURR_DIR->file_in !=temp->addr)
	{
		fseek(fp,temp->prev,SEEK_SET);
		temp1 = (file_t *)malloc(sizeof(file_t));
		fread(temp1,sizeof(file_t),1,fp);
		temp1->next = temp->next;
		fseek(fp,temp1->addr,SEEK_SET);
		fwrite(temp1,sizeof(file_t),1,fp);
	}
	else
	{
		CURR_DIR->file_in =temp->next;		
		fseek(fp,CURR_DIR->addr,SEEK_SET);
		fwrite(CURR_DIR,sizeof(folder),1,fp);
	}
}

void rmdir(folder *CURR_DIR, char name[MAX], FILE *fp)//Removing directory
{
	folder *temp, *temp1;

	temp = (folder *)malloc(sizeof(folder));
	fseek(fp,CURR_DIR->in,SEEK_SET);
	fread(temp,sizeof(folder),1,fp);

	while(strcmp(temp->name,name)!=0)
	{
		if(temp->next==0)
		{
			printf("\nNo such file!!");
			return;
		}
		fseek(fp,temp->next,SEEK_SET);
		fread(temp,sizeof(folder),1,fp);
	}
	
	if(CURR_DIR->in !=temp->addr)
	{
		fseek(fp,temp->prev,SEEK_SET);
		temp1 = (folder *)malloc(sizeof(folder));
		fread(temp1,sizeof(folder),1,fp);
		temp1->next = temp->next;
		fseek(fp,temp1->addr,SEEK_SET);
		fwrite(temp1,sizeof(folder),1,fp);
	}
	else
	{
		CURR_DIR->in =temp->next;		
		fseek(fp,CURR_DIR->addr,SEEK_SET);
		fwrite(CURR_DIR,sizeof(folder),1,fp);
	}
}

int main()
{	
	FILE *fp;
	char ch[MAX]={},d_name[10];
	folder *root, *CURR_DIR,*temp;

	root = (folder*)malloc(sizeof(folder));
	fp=fopen("mpfs.fs","r");
	if(fp == NULL)
	{	
		printf("def");		
		init_fs();
	}

	fp = fopen("mpfs.fs","r+");	
	fseek(fp,0,SEEK_SET);
	fread(root,sizeof(folder),1,fp);
	CURR_DIR = root;
	printf("\n\t\tMysticPrism File System(MPFS)\n\n");
	printf("Commands:\n\tmkdir [DIR NAME]\n\t\t--create directory in current direcotry");
	printf("\n\tcd [DIR NAME]\n\t\t--change direcotry\n\tmkf [FILE NAME]\n\t\t--create file in current directory");
	printf("\n\tls\n\t\t--list current directory\n\trm [FILE NAME]\n\t\t--delete file\n\trmdir [DIR NAME]\n\t\t--delete directory");
	printf("\n\tcat [FILE NAME]\n\t\t--print file contents\n\texit\n\t\t--exit MPFS");
	printf("\n\tAll operations for current directory. NO FULL PATHS ALLOWED!");
	temp=root;
	while(strcmp(ch,"exit")!=0)
	{	
		printf("\n%s\\",CURR_DIR->name);
		temp=root;
		printf("->");
		
		scanf("%s",ch);
		
		if(!strcmp(ch,"ls"))
		{	
			my_ls(CURR_DIR,fp);
			
		}
		else if(!strcmp(ch,"cd"))
		{	scanf("%s",ch);	
			
			CURR_DIR=my_cd(ch,CURR_DIR,fp);	
		}
		else if(!strcmp(ch,"mkdir"))
		{	
			scanf("%s",ch);			
	
			my_mkdir(CURR_DIR,ch,fp);
		}
		else if(!strcmp(ch,"mkf"))
		{	
			fflush(NULL);			
			scanf("%s",ch);
						
			CURR_DIR=create_file(CURR_DIR,fp,ch);
		}
		else if(!strcmp(ch,"cat"))
		{
			scanf("%s",ch);
			open(ch,root,fp);
		}
		else if(!strcmp(ch,"rm"))
		{
			scanf("%s",ch);			
			rm(CURR_DIR,ch,fp);
		}
		else if(!strcmp(ch,"rmdir"))
		{
			scanf("%s",ch);
			rmdir(CURR_DIR,ch,fp);
		}
		else if(strcmp(ch,"exit"))
		{
			printf("No such command!");
		}
	}

	fclose(fp);
	return 0;
}
