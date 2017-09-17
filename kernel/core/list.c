
/*
    EventOS V1.0 - Copyright (C) 2014 Edielson Prevato Frigieri.

       This file is part of the EventOS distribution.

    EventTOS is free software; you can redistribute it and/or modify it under
    the terms of the MIT License (MIT).
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

 */

#include <stdlib.h>
#include "EventOS.h"
#include "list.h"

/*********************************************************
    Operations implementation
*********************************************************/

void	vList_initialize(xList* pxList)
{
	if(pxList == NULL) return;

	/* The list structure contains a list item which is used to mark the
	end of the list, called sentinel.  To initialize the list the list end
	is inserted as the only list entry. */
	pxList->xIndex = ( xListNode* ) &( pxList->xListSentinel);

	/* The list end value is the highest possible value in the list to
	ensure it remains at the end of the list. */
	pxList->xListSentinel.xNodeValue = portMAX_DELAY;

	/* The list sentinel next and previous pointers point to itself so we know
	when the list is empty. */
	pxList->xListSentinel.xNext = ( xListNode * ) &( pxList->xListSentinel );
	pxList->xListSentinel.xPrevious = ( xListNode * ) &( pxList->xListSentinel );

	pxList->xNumberOfNodes = 0;
}

void vList_initialiseNode( xListNode* xNode )
{
	/* Make sure the list node is not recorded as being on a list. */
	xNode->pvContainer = NULL;
}

void vList_insertHead( xList* pxList, xListNode* pxNewListNode )
{
	volatile xListNode* xIndex;

	/* Insert a new list node into xList, but rather than sort the list,
	makes the new list node the header node, but the last to be removed. */

	//Points to the sentinel
	xIndex = pxList->xIndex;

	pxNewListNode->xNext = xIndex->xNext;
	xIndex->xNext->xPrevious = ( volatile xListNode*) pxNewListNode;
	xIndex->xNext = ( volatile xListNode* ) pxNewListNode;
	pxNewListNode->xPrevious = pxList->xIndex;

	/* Remember which list the node is in. */
	pxNewListNode->pvContainer = ( void * ) pxList;

	( pxList->uxNumberOfItems )++;
}

/*-----------------------------------------------------------*/
void vList_insert( xList* pxList, xListNode *pxNewListNode )
{
	volatile xListNode* pxIterator;
	portTickType ulValueOfInsertion;

	/* Insert the new list node into the list, sorted in xNodeValue order. */
	ulValueOfInsertion = pxNewListNode->xNodeValue;

	/* if the node item value is equal to portMAX_DELAY, the node goes to the end of the list, just after the sentinel. */
	if( ulValueOfInsertion == portMAX_DELAY )
	{
		//point to the list tail, i.e., the last node
		pxIterator = pxList->xIndex;
	}
	else
	{
		//TODO change the iteration to run from tail to head
		for( pxIterator = ( xListNode* ) &( pxList->xListSentinel); pxIterator->xPrevious->xNodeValue > ulValueOfInsertion; pxIterator = pxIterator->xPrevious )
		{
			/* There is nothing to do here, we are just iterating to the
			wanted insertion position. */
		}
	}
	pxNewListNode->xPrevious = pxIterator->xPrevious;
	pxIterator->xPrevious->xNext = ( volatile xListNode* ) pxNewListNode;
	pxIterator->xPrevious = ( volatile xListNode* ) pxNewListNode;
	pxNewListNode->xNext = pxIterator;

	/* Remember which list the item is in.  This allows fast removal of the
	item later. */
	pxNewListNode->pvContainer = ( void * ) pxList;

	( pxList->xNumberOfNodes )++;
}
/*-----------------------------------------------------------*/

void vList_remove( xListNode* pxNodeToRemove )
{
	xList* pxList;

	pxNodeToRemove->xPrevious->xNext = pxNodeToRemove->xNext;
	pxNodeToRemove->xNext->xPrevious = pxNodeToRemove->xPrevious;

	pxNodeToRemove->pvContainer = NULL;

	/* The list item knows which list it is in.  Obtain the list from the list
	item. */
	pxList = ( xList*) pxNodeToRemove->pvContainer;
	( pxList->xNumberOfNodes )--;
}
/*-----------------------------------------------------------*/

