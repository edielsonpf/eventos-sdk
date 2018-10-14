/*
    EventOS V1.0.1
    Copyright (C) 2014 Edielson Prevato Frigieri. All Rights Reserved.

       This file is part of the EventOS distribution.

    EventOS is free software; you can redistribute it and/or modify it under
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
	pxList->pxIndex = ( xListNode* ) &( pxList->xListSentinel);

	/* The list end value is the highest possible value in the list to
	ensure it remains at the end of the list. */
	pxList->xListSentinel.xNodeValue = portMAX_DELAY;

	/* The list sentinel next and previous pointers point to itself so we know
	when the list is empty. */
	pxList->xListSentinel.pxNext = ( xListNode * ) &( pxList->xListSentinel );
	pxList->xListSentinel.pxPrevious = ( xListNode * ) &( pxList->xListSentinel );

	pxList->xNumberOfNodes = 0;
}

void vList_initialiseNode( xListNode* pxNode )
{
	/* Make sure the list node is not recorded as being on a list. */
	pxNode->pvContainer = NULL;
}

void vList_insertHead( xList* pxList, xListNode* pxNewListNode )
{
	volatile xListNode* pxIndex;

	/* Insert a new list node into xList, but rather than sort the list,
	makes the new list node the header node, but the last to be removed. */

	//Points to the sentinel
	pxIndex = ( xListNode* ) &( pxList->xListSentinel);

	pxNewListNode->pxNext = pxIndex->pxNext;
	pxIndex->pxNext->pxPrevious = ( volatile xListNode*) pxNewListNode;
	pxIndex->pxNext = ( volatile xListNode* ) pxNewListNode;
	pxNewListNode->pxPrevious = pxList->pxIndex;

	/* Remember which list the node is in. */
	pxNewListNode->pvContainer = ( void * ) pxList;

	( pxList->xNumberOfNodes )++;
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
		pxIterator = ( xListNode* ) &( pxList->xListSentinel);
	}
	else
	{
		for( pxIterator = ( xListNode* ) &( pxList->xListSentinel); pxIterator->pxPrevious->xNodeValue <= ulValueOfInsertion; pxIterator = pxIterator->pxPrevious )
		{
			/* There is nothing to do here, we are just iterating to the
			wanted insertion position. */
		}
	}
	pxNewListNode->pxPrevious = pxIterator->pxPrevious;
	pxIterator->pxPrevious->pxNext = ( volatile xListNode* ) pxNewListNode;
	pxIterator->pxPrevious = ( volatile xListNode* ) pxNewListNode;
	pxNewListNode->pxNext = pxIterator;

	/* Remember which list the item is in.  This allows fast removal of the
	item later. */
	pxNewListNode->pvContainer = ( void * ) pxList;

	( pxList->xNumberOfNodes )++;
}
/*-----------------------------------------------------------*/

void vList_remove( xListNode* pxNodeToRemove )
{
	/* The list item knows which list it is in.
	 * Obtain the list from the list item. */
	xList* pxList = ( xList*) pxNodeToRemove->pvContainer;

	pxNodeToRemove->pxPrevious->pxNext = pxNodeToRemove->pxNext;
	pxNodeToRemove->pxNext->pxPrevious = pxNodeToRemove->pxPrevious;

	pxNodeToRemove->pvContainer = NULL;
	( pxList->xNumberOfNodes )--;
}
/*-----------------------------------------------------------*/

