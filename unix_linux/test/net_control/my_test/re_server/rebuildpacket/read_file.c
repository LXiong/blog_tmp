///YS ����ʱ�ļ���Ӳ��������ڴ�

#include <stdio.h>
#include <stdlib.h>


/************************
����ʱ�ļ���Ӳ��������ڴ�
����1�����ļ���
����2�����ļ��ĳ���
����3�����ļ������ڴ�����ڵ�ָ��
*************************/
char *get_file_content (char *filename,long *len,char **buffer)
{
    FILE * pFile;
    long lSize;
    size_t result;
    ///YS ��Ҫһ��byte��©�ض��������ļ���ֻ�ܲ��ö����Ʒ�ʽ��
    pFile = fopen (filename, "rb" );
    if (pFile==NULL)
    {
//        dbug ("the filename is %s   's len is NULL??\n",filename);
        return 0;
        //exit (1);
    }

    ///YS  ��ȡ�ļ���С
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    ///YS  �����ڴ�洢�����ļ�

    *buffer = calloc (lSize,sizeof(char));
    if (*buffer == NULL)
    {
        fputs ("Memory error",stderr);
       // exit (2);
    }

    ///YS ���ļ�������buffer��
    result = fread (*buffer,1,lSize,pFile);
    if (result != lSize)
    {
        fputs ("Reading error",stderr);
     //   exit (3);
    }
    fclose (pFile);
    *len=lSize;///YS ���س���
return 0;
}
