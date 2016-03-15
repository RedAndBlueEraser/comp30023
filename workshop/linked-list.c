/*
 * linked-list.c
 * Version 20160307
 * Written by Harry Wong
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
// typedef declarations. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// data type stored in linked list.
typedef int data_t;

// node in linked list.
typedef struct linkedlistnode_t
{
    data_t data;
    struct linkedlistnode_t *next;
} linkedlistnode_t;

// linked list.
typedef struct linkedlist_t
{
    linkedlistnode_t *head;
    linkedlistnode_t *foot;
    size_t size;
} linkedlist_t;

////////////////////////////////////////////////////////////////////////////////
// function prototypes. ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * Allocate memory for, initialize and return a new linked list.
 * @return empty linked list
 */
linkedlist_t *newlinkedlist();

/**
 * Remove and free the memory taken up by the linked list.
 * @param list linked list to removed
 */
void freelinkedlist(linkedlist_t *list);

/**
 * Get data at the specified position of the linked list.
 * @param list  linked list to be retrieved from
 * @param index position of the linked list at which the data is retrieved
 * @return empty linked list
 */
data_t getatlinkedlist(linkedlist_t *list, size_t index);

/**
 * Append data to the end of the linked list.
 * @param list linked list to be appended to
 * @param data data to be appended as a node
 * @return data at the specified position of the linked list
 */
void appendlinkedlist(linkedlist_t *list, data_t data);

/**
 * Insert data to the specified position of the linked list.
 * @param list  linked list to be inserted into
 * @param index position of the linked list at which the data is inserted
 * @param data  data to be inserted as a node
 */
void addlinkedlist(linkedlist_t *list, size_t index, data_t data);

/**
 * Return and remove the data at the end of the linked list.
 * @param list linked list to be popped from
 * @return data at the end of the linked list
 */
data_t poplinkedlist(linkedlist_t *list);

/**
 * Return and remove the data at the specified position of the linked list.
 * @param list  linked list to be removed from
 * @param index position of the linked list at which the data is removed
 * @return data at the specified position of the linked list
 */
data_t removeatlinkedlist(linkedlist_t *list, size_t index);

/**
 * Remove the specified data from the linked list.
 * @param list linked list to be removed from
 * @param data data to be removed
 * @return 1 if the linked list contained the specified data else 0
 */
int removefromlinkedlist(linkedlist_t *list, data_t data);

/**
 * Return the index of the specified element in the linked list.
 * @param list linked list to be searched from
 * @param data data to be searched
 * @return index of the first occurrence of the specified data or -1
 */
size_t indexoflinkedlist(linkedlist_t *list, data_t data);

/**
 * Return the array representation of the linked list.
 * @param list linked list to be represented as array
 * @return array of data
 */
data_t *toarraylinkedlist(linkedlist_t *list);

////////////////////////////////////////////////////////////////////////////////
// function definitions. ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int isconsistentlinkedlist(linkedlist_t *list)
{
    int isheadnull, isfootnull, isempty;
    isheadnull = list->head == NULL;
    isfootnull = list->foot == NULL;
    isempty = list->size == 0;
    return (isheadnull == isfootnull) && (isfootnull == isempty);
}

linkedlist_t *newlinkedlist()
{
    linkedlist_t *list;  // pointer to new linked list

    // allocate memory for new linked list
    list = (linkedlist_t*)malloc(sizeof(linkedlist_t));
    assert(list != NULL);
    // initialize values for new linked list
    list->head = NULL;
    list->foot = NULL;
    list->size = 0;

    return list;
}

void freelinkedlist(linkedlist_t *list)
{
    linkedlistnode_t *node,      // pointer to node to be removed and freed
                     *nextnode;  // pointer to next node to be removed and freed

    // make sure list is not null
    if (list != NULL)
    {
        node = list->head;
        // iterate over linked list
        while (node != NULL)
        {
            nextnode = node->next;
            free(node);
            node = nextnode;
        }

        free(list);
    }

    return;
}

data_t getatlinkedlist(linkedlist_t *list, size_t index)
{
    int i;                   // iteration in the linked list
    linkedlistnode_t *node;  /* pointer to node at the i'th position of the
                              * linked list
                              */

    // make sure list is not empty and list size is greater than index
    assert(list != NULL && list->head != NULL && list->size > index);

    node = list->head;
    // iterate over linked list
    for (i = 0; i < index; i++)
    {
        // make sure node is not null
        assert(node != NULL);

        node = node->next;
    }

    return node->data;
}

void appendlinkedlist(linkedlist_t *list, data_t data)
{
    linkedlistnode_t *node;  /* pointer to node to be appended to the linked
                              * list
                              */

    // make sure list is not null and list state is consistent
    assert(list != NULL && isconsistentlinkedlist(list));

    // allocate memory for new node
    node = (linkedlistnode_t*)malloc(sizeof(linkedlistnode_t));
    assert(node != NULL);
    // initialize values for new node
    node->data = data;
    node->next = NULL;

    // adding node as list's first node
    if (list->size == 0)
    {
        list->head = node;
        list->foot = node;
        list->size = 1;
    }
    // appending node to already filled list
    else
    {
        list->foot->next = node;
        list->foot = node;
        list->size++;
    }

    return;
}

void addlinkedlist(linkedlist_t *list, size_t index, data_t data)
{
    int i;                    // iteration in the linked list
    linkedlistnode_t *node,   // pointer to node to be added to the linked list
                     *nodei;  /* pointer to node at the i'th position of the
                               * linked list
                               */

    /* make sure list is not null and list size is gte to index and list state
     * is consistent
     */
    assert(list != NULL && list->size >= index && isconsistentlinkedlist(list));

    // allocate memory for new node
    node = (linkedlistnode_t*)malloc(sizeof(linkedlistnode_t));
    assert(node != NULL);
    // initialize values for new node
    node->data = data;
    node->next = NULL;

    // adding node as list's first node
    if (list->size == 0)
    {
        list->head = node;
        list->foot = node;
        list->size = 1;
    }
    // adding node to already filled list
    else
    {
        // adding node as the list's head
        if (index == 0)
        {
            node->next = list->head;
            list->head = node;
            list->size++;
        }
        // adding node as the list's foot
        else if (index == list->size)
        {
            list->foot->next = node;
            list->foot = node;
            list->size++;
        }
        // adding node in the middle of the list
        else
        {
            nodei = list->head;
            // iterate over linked list
            for (i = 0; i < index - 1; i++)
            {
                // make sure node is not null
                assert(nodei != NULL);
                nodei = nodei->next;
            }

            node->next = nodei->next;
            nodei->next = node;
            list->size++;
        }
    }

    return;
}

data_t poplinkedlist(linkedlist_t *list)
{
    linkedlistnode_t *node;  /* pointer to node to become the foot of the linked
                              * list
                              */
    data_t data;             // data to be returned

    // make sure list is not empty
    assert(list != NULL && list->head != NULL && isconsistentlinkedlist(list));

    data = list->foot->data;

    node = list->head;
    // iterate over linked list
    while (node != NULL)
    {
        if (node->next == list->foot)
        {
            // make before foot node the list's foot node
            free(list->foot);
            list->foot = node;
            node->next = NULL;
            list->size--;
        }
        node = node->next;
    }

    return data;
}

data_t removeatlinkedlist(linkedlist_t *list, size_t index)
{
    int i;                    // iteration in the linked list
    linkedlistnode_t *node,   /* pointer to node to be removed from the linked
                               * list
                               */
                     *nodei;  /* pointer to node at the i'th position of the
                               * linked list
                               */
    data_t data;              // data to be returned

    /* make sure list is not null and list size is greater than index and list
     * state is consistent
     */
    assert(list != NULL && list->size > index && isconsistentlinkedlist(list));

    // removing list's only node
    if (list->size == 1)
    {
        data = list->head->data;
        free(list->head);
        list->head = NULL;
        list->foot = NULL;
        list->size = 0;
    }
    // removing node from list with more than one node
    else
    {
        // removing list's head
        if (index == 0)
        {
            node = list->head;
            data = node->data;
            list->head = node->next;
            free(node);
            list->size--;
        }
        // removing node from beyond the list's head
        else
        {
            nodei = list->head;
            // iterate over linked list
            for (i = 0; i < index - 1; i++)
            {
                // make sure node is not null
                assert(nodei != NULL);
                nodei = nodei->next;
            }

            node = nodei->next;
            // removing list's foot
            if (index == list->size - 1)
            {
                // make sure node being removed is foot
                assert(node == list->foot);
            }
            data = node->data;
            nodei->next = node->next;
            free(node);
            // removing list's foot
            if (index == list->size - 1)
            {
                list->foot = nodei;
            }
            list->size--;
        }
    }

    return data;
}

int removefromlinkedlist(linkedlist_t *list, data_t data)
{
////////////////////////////////////////////////////////////////////////////////
    linkedlistnode_t *node,             /* pointer to node to be removed from
								         * the linked list
                                         */
                     *prevnode = NULL;  /* pointer to node before node to be
										 * removed
										 */

	// make sure list is not null and list state is consistent
    assert(list != NULL && isconsistentlinkedlist(list));

	node = list->head;
    // iterate over linked list
    while (node != NULL)
	{
		// found node that stores data
		if (node->data == data)
		{
			// removing list's only node
			if (list->size == 1)
			{
				free(list->head);
				list->head = NULL;
				list->foot = NULL;
				list->size = 0;
			}
			// removing node from list with more than one node
			else
			{
				// removing list's head
				if (node == list->head)
				{
					list->head = node->next;
					free(node);
					list->size--;
				}
				// removing node from beyond the list's head
				else
				{
					prevnode->next = node->next;
					// removing list's foot
					if (node == list->foot)
					{
						list->foot = prevnode;
					}
					free(node);
					list->size--;
				}
			}

			return 1;
		}
		// this node doesn't store wanted data
		else
		{
			prevnode = node;
			node = node->next;
		}
	}
	return 0;
}

size_t indexoflinkedlist(linkedlist_t *list, data_t data)
{
    int i = 0;               // index of linked list
    linkedlistnode_t *node;  // pointer to node storing data

    // make sure list is not null
    assert(list != NULL);

    node = list->head;
    // iterate over linked list
    while (node != NULL)
    {
        if (node->data == data)
        {
            return i;
        }
        else
        {
            node = node->next;
            i++;
        }
    }

    return -1;
}

data_t *toarraylinkedlist(linkedlist_t *list)
{
    int i = 0;               // iteration in the linked list
    linkedlistnode_t *node;  /* pointer to node at the i'th position of the
                              * linked list
                              */
    data_t *array;          // array representation of linked list

    // allocate memory for array
    array = (data_t*)malloc(sizeof(data_t) * list->size);
    assert(array != NULL);

    // make sure list is not null
    assert(list != NULL);

    node = list->head;
    // iterate over linked list
    while (node != NULL)
    {
        array[i] = node->data;
        node = node->next;
        i++;
    }

    return array;
}




int main(int argc, char *argv[])
{
	int i;
	linkedlist_t *list = newlinkedlist();

	appendlinkedlist(list, 11);
	appendlinkedlist(list, 22);
	appendlinkedlist(list, 33);
	appendlinkedlist(list, 44);
	addlinkedlist(list, 4, 55);
	addlinkedlist(list, 5, 66);
	addlinkedlist(list, 6, 77);
	addlinkedlist(list, 7, 88);
	addlinkedlist(list, 0, 0);
	appendlinkedlist(list, 99);

	printf("popped number \"%d\" from list\n", poplinkedlist(list));
	printf("popped number \"%d\" from list\n", poplinkedlist(list));
	printf("popped number \"%d\" from list\n", poplinkedlist(list));
	printf("removed number \"%d\" from list\n", removeatlinkedlist(list, 0));
	printf("removed number \"%d\" from list\n", removeatlinkedlist(list, 4));
	
	for (i = 0; i < list->size; i++)
	{
		printf("%d ", getatlinkedlist(list, i));
	}
	printf("\n");
	return 0;
}
