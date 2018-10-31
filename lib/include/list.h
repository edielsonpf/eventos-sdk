/*
    EventOS V1.2.0 - Copyright (C) 2014 Edielson Prevato Frigieri.

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

#ifndef LIST_H_
#define LIST_H_

/*
 * Definition of the only type of object that a list can contain.
 */
struct xLIST_NODE
{
	portTickType xNodeValue;					/*< This value is used to sort the list in descending order. */

	volatile struct xLIST_NODE* pxNext;		/*< Pointer to the next xListItem in the list. */
	volatile struct xLIST_NODE* pxPrevious;	/*< Pointer to the previous xListItem in the list. */
	void* pvOwner;								/*< Pointer to the object that contains the list item.  There is therefore a two way link between the object containing the list item and the list item itself. */
	void* pvContainer;							/*< Pointer to the list in which this list item is placed (if any). */
};
typedef struct xLIST_NODE xListNode;

struct xLIST_SENTINEL
{
	portTickType xNodeValue;					/*< This value is used to sort the list in descending order. */
	volatile struct xLIST_NODE* pxNext;		/*< Pointer to the next xListItem in the list. */
	volatile struct xLIST_NODE* pxPrevious;	/*< Pointer to the previous xListItem in the list. */
};
typedef struct xLIST_SENTINEL xSentinelListNode;

/*
 * Definition of the type of queue used by the scheduler.
 */
typedef struct xLIST
{
	volatile unsigned portBASE_TYPE xNumberOfNodes;
	volatile xListNode* pxIndex;						/*< Used to walk through the list.  Points to the last item returned by a call to pvListGetOwnerOfNextEntry (). */
	volatile xSentinelListNode xListSentinel;
}xList;

/*
 * Access macro to set the owner of a list node.  The owner of a list node
 * is the object that contains the list node.
 */
#define listSET_LIST_NODE_OWNER( pxListNode, pxOwner )		( pxListNode )->pvOwner = ( void*) pxOwner

/*
 * Access function to obtain the owner of the next event in a list.
 *
 * The pxOwner parameter of a list node is a pointer to the object that owns
 * the list node.  In the scheduler this is normally a event control block.
 * The pxOwner parameter effectively creates a two way link between the list
 * node and its owner.
 *
 * @param pxList: The list from which the next node owner is to be returned.
 *
 */
#define listGET_OWNER_OF_NEXT_NODE( pxXCB, pxList )											\
{																									\
	xList* const pxConstList = pxList;														\
	/* Increment the index to the next item and return the item, ensuring */						\
	/* we don't return the marker used at the end of the list.  */									\
	( pxConstList )->pxIndex = ( pxConstList )->pxIndex->pxNext;							\
	if( ( pxConstList )->pxIndex == ( xListNode* ) &( ( pxConstList )->xListSentinel ) )	\
	{																								\
		pxXCB = NULL;																				\
	}																								\
	else																							\
	{																								\
		pxXCB = ( pxConstList )->pxIndex->pvOwner;												\
	}																								\
}

/*
 * Access function to obtain the owner of the first entry in a list.  Lists
 * are normally sorted in ascending item value order.
 *
 * This function returns the pxOwner member of the first item in the list.
 * The pxOwner parameter of a list item is a pointer to the object that owns
 * the list item.  In the scheduler this is normally a task control block.
 * The pxOwner parameter effectively creates a two way link between the list
 * item and its owner.
 *
 * @param pxList The list from which the owner of the head item is to be
 * returned.
 *
 * \page listGET_OWNER_OF_HEAD_ENTRY listGET_OWNER_OF_HEAD_ENTRY
 * \ingroup LinkedList
 */
#define listGET_OWNER_OF_HEAD_ENTRY( pxList )  ( ( ( pxList )->xNumberOfNodes != ( unsigned portBASE_TYPE ) 0 ) ? ( ( &( ( pxList )->xListSentinel ) )->pxNext->pvOwner ) : ( NULL ) )


/*
 * Access macro to set the value of the list node.  In most cases the value is
 * used to sort the list in descending order.
 *
 * \page listSET_LIST_ITEM_VALUE listSET_LIST_ITEM_VALUE
 * \ingroup LinkedList
 */
#define listSET_LIST_NODE_VALUE( pxListNode, xValue )		( pxListNode )->xNodeValue = xValue

/*
 * Access macro the retrieve the value of the list item.  The value can
 * represent anything - for example a the priority of a task, or the time at
 * which a task should be unblocked.
 *
 * \page listGET_LIST_ITEM_VALUE listGET_LIST_ITEM_VALUE
 * \ingroup LinkedList
 */
#define listGET_LIST_ITEM_VALUE( pxListNode )				( ( pxListNode )->xNodeValue )

/*
 * Access macro to determine if a list contains any items.  The macro will
 * only have the value true if the list is empty.
 *
 * \page listLIST_IS_EMPTY listLIST_IS_EMPTY
 * \ingroup LinkedList
 */
#define listIS_EMPTY( pxList ) ( ( pxList )->xNumberOfNodes == ( unsigned portBASE_TYPE ) 0 )

/**
	This method is responsible responsible for initializing
	a linked list structure.

    @param void
    @return void
    @author edielson
    @date   14/09/2017
*/
void vList_initialize(xList* pxList);
void vList_initialiseNode( xListNode* pxNode );
void vList_insertHead( xList* pxList, xListNode* pxNewListNode );
void vList_insert( xList* pxList, xListNode *pxNewListNode );
void vList_remove( xListNode* pxNodeToRemove );

#endif /* LIST_H_ */
