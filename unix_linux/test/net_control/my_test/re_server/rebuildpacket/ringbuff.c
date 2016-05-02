///YS ���ͻ������ݽṹ�Ĵ�������
#include <stdio.h>
#include <stdlib.h>
#include "ringbuff.h"
/* Opaque buffer element type.  This would be defined by the application. */
//typedef struct { ; } ElemType;

/* Circular buffer object */



/*
*ringbuff ��ʼ�������� cb ...��size  ��С��
*/
void cbInit(CircularBuffer *cb, int size)
{
    cb->size  = size;
    cb->start = 0;
    cb->count = 0;
    cb->elems = (ElemType *)calloc(cb->size, sizeof(ElemType));

}


/*
*�ж�ringbuff�Ƿ�Ϊ��
*/
int cbIsFull(CircularBuffer *cb)
{
    return cb->count == cb->size;
}


/*
*�ж�ringbuff�Ƿ�Ϊ��
*/
int cbIsEmpty(CircularBuffer *cb)
{
    return cb->count == 0;
}




/*
*�Ѳ���elemд�뵽ringbuff
*/
void cbWrite(CircularBuffer *cb, ElemType *elem)
{

    int end = (cb->start + cb->count) % cb->size;
    cb->elems[end] = *elem;
    if (cb->count == cb->size)
    {
        cb->start = (cb->start + 1) % cb->size;    /* full, overwrite */
//        dbug("3");
    }
    else
        ++ cb->count;
}

/*
*�Ѳ���elem��ringbuff�ж�����
*/
void cbRead(CircularBuffer *cb, ElemType *elem)
{
    *elem = cb->elems[cb->start];
    cb->start = (cb->start + 1) % cb->size;
    -- cb->count;
}

void cbFree(CircularBuffer *cb)
{
    free(cb->elems); /* OK if null */
}

