#include <stdio.h>
#include <stdlib.h>    // malloc, free �Լ��� ����� ��� ����
#include <string.h>    // memcpy �Լ��� ����� ��� ����

#define DEBUG_MODE 0

enum Direction { LEFT = 100, RIGHT, UP, DOWN }; // 100, 101, 102, 103
enum ChoiceHead { OPEN = 90, CLOSED, ROUTE}; // 90, 91
enum TrueOrFalse {FALSE = 0, TRUE}; // 0, 1

//��忡 ���� ����ü ����
typedef struct anode *NodePtr;
typedef struct anode {
	int tile[4][4]; // ����:  Ÿ���� ���� ������ 0 �� �־� ���´�.
	double fHat, gHat, hHat;
	NodePtr preNode;
} Node;

//Open, Closed ��  ���Ḯ��Ʈ�� ��忡 ���� ����ü ����
typedef struct alinkedlistnode *LinkedNodePtr;
typedef struct alinkedlistnode {
	NodePtr nodeptr;
	LinkedNodePtr next;
} LinkedNode;

typedef struct location *LocationZeroPtr;
typedef struct location {
	int i;
	int j;
} LocationZero;

//Open, Closed :  ��忡 ���� �����͵��� ������ ���Ḯ��Ʈ
LinkedNodePtr Open = NULL;
LinkedNodePtr Closed = NULL;
LinkedNodePtr Route = NULL;

//Start, Goal ��带 ���� �Է�
int start[4][4] = { { 0, 2, 4, 3 },{ 10, 5, 8, 11 },{ 12, 14, 9, 7 },{ 1, 13, 6, 15 } };
int goal[4][4] = { { 2, 4, 8, 3 },{ 10, 5, 0, 11 },{ 12, 14, 9, 7 },{ 1, 13, 6, 15 } };

NodePtr createNode();
LinkedNodePtr createLinkedNode();
void setStartNode(NodePtr nodePtr);
void printNodeOfTile_OpenClose(enum ChoiceHead, int Mode);
void insertLinkedNodePtr(enum ChoiceHead choiceHead, LinkedNodePtr linkedNodePtr);
LinkedNodePtr removeLinkedNodePtr(enum ChoiceHead choiceHead, LinkedNodePtr linkedNodePtr);
enum TrueOrFalse isGoalNode(NodePtr nodePtr);
void printGoalToStart(NodePtr nodePtr);
void printStartToGoal(NodePtr nodePtr);
LocationZero findZeroLocation(NodePtr nodePtr);
void expandCurrentNode(LinkedNodePtr currentLinkedNode, LocationZero locationZero, enum Direction direction);
NodePtr getMovedNodeFromCurrentNodePtr(NodePtr currentNode, LocationZero locationZero, enum Direction direction);
void copyTile(NodePtr successorNodePtr, NodePtr currentNodePtr);
enum TrueOrFalse isExist(enum ChoiceHead choiceHead, NodePtr node);
int return_hHat(NodePtr node);
enum Direction numWhereToMove(LocationZero preLocationZero, LocationZero currentLocationZero);

void NodeInput(int position[4][4]);

void main()
{
	LinkedNodePtr newLinkedNodePtr;
	//0. start���� goal��带 �Է¹޴´�.
	
	printf("<4 X 4 A*Ž���ý���>\n");
	printf("���۳�带 �����̽� ������ �Է��Ͻÿ�.\n");
	NodeInput(start);
	printf("������带 �����̽� ������ �Է��Ͻÿ�.\n");
	NodeInput(goal);


	Open = createLinkedNode();					//1. ����ִ� Open ����, Open -> NULL


	newLinkedNodePtr = createLinkedNode();		//	 Start����� n0�� �����ؼ� Open�� �ִ´�. Open -> �� ->NULL
	newLinkedNodePtr->nodeptr = createNode();
	insertLinkedNodePtr(OPEN, newLinkedNodePtr);

	setStartNode(newLinkedNodePtr->nodeptr);	//   �Է¹��� start���� ���� n0��忡 �����Ѵ�.(tile, f^,g^,h^,preNode����)

	Closed = createLinkedNode();				//2. ����ִ� Closed ����, Closed -> NULL

	printNodeOfTile_OpenClose(OPEN, DEBUG_MODE);
	printNodeOfTile_OpenClose(CLOSED, DEBUG_MODE);

	while (1)
	{
		if (Open->next == NULL)					//3. Open�� ��� �ִٸ�, "Ž������"�� �����Ѵ�.
		{
			printf("Ž������ : ���̻� Ž���� Open�� �������� ����\n");
			break;
		}
		else						
		{
			LinkedNodePtr currentLinkedNode; 
			currentLinkedNode = removeLinkedNodePtr(OPEN, Open->next);//4. Open���� �Ǿհ�(f^�� ���� ���� ��)�� �ϳ� ������ currentNode��� �ϰ� �̰��� Closed�� �ִ´�.
			insertLinkedNodePtr(CLOSED, currentLinkedNode);

			if(isGoalNode(currentLinkedNode->nodeptr))		// 5. ���� currentNode�� Goal����� �Ʒ������� Start���� �����.
			{
				//printGoalToStart(currentLinkedNode->nodeptr);
				printStartToGoal(currentLinkedNode->nodeptr);
				break;
			}	
			else											// 6. expand ����
			{
				LocationZero locationZero = findZeroLocation(currentLinkedNode->nodeptr);

				if (locationZero.j - 1 != -1)
					expandCurrentNode(currentLinkedNode, locationZero, LEFT);
				if (locationZero.j + 1 != 4)
					expandCurrentNode(currentLinkedNode, locationZero, RIGHT);
				if (locationZero.i - 1 != -1)
					expandCurrentNode(currentLinkedNode, locationZero, UP);
				if (locationZero.i + 1 != 4)
					expandCurrentNode(currentLinkedNode, locationZero, DOWN);
			}
		} 
	}
}

NodePtr createNode()
{
	NodePtr nodePtr = (NodePtr)malloc(sizeof(Node));
	nodePtr->fHat = 0;
	nodePtr->gHat = 0;
	nodePtr->hHat = 0;
	nodePtr->preNode = NULL;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			nodePtr->tile[i][j] = 0;
		}
	}

	return nodePtr;
}

LinkedNodePtr createLinkedNode()
{
	LinkedNodePtr linkedNodePtr = (LinkedNodePtr)malloc(sizeof(LinkedNode));

	linkedNodePtr->next = NULL;
	linkedNodePtr->nodeptr = NULL;

	return linkedNodePtr;
}

void setStartNode(NodePtr nodePtr)
{
	int differenceWithGaol = 0;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			nodePtr->tile[i][j] = start[i][j];
			if (nodePtr->tile[i][j] != goal[i][j])
				differenceWithGaol++;
		}
	}

	nodePtr->gHat = 0;
	nodePtr->hHat = differenceWithGaol;
	nodePtr->fHat = nodePtr->gHat + nodePtr->hHat;

	nodePtr->preNode = NULL;
}

void printNodeOfTile_OpenClose(enum ChoiceHead choiceHead, int Mode)
{
	if (Mode == 1)
	{
		LinkedNodePtr currentLinkedNodePtr;

		if (choiceHead == OPEN)
		{
			printf("<Open>\n");
			currentLinkedNodePtr = Open;
		}
		else if (choiceHead == CLOSED)
		{
			printf("<Closed>\n");
			currentLinkedNodePtr = Closed;
		}
		while (currentLinkedNodePtr != NULL)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if ((currentLinkedNodePtr->next)->nodeptr->tile[i][j] != 0)
						printf("%3d", (currentLinkedNodePtr->next)->nodeptr->tile[i][j]);
					else printf("   ");
				}
				printf("\n");
			}
			printf("\n");
			currentLinkedNodePtr = currentLinkedNodePtr->next;
		}
	}
}

void insertLinkedNodePtr(enum ChoiceHead choiceHead, LinkedNodePtr linkedNodePtr)
{
	LinkedNodePtr Head;
	
	if (choiceHead == OPEN)			// f^�� ������ �������� ���� �����Ѵ�.
	{
		Head = Open;
		LinkedNodePtr currentNodePtr = Head->next;
		LinkedNodePtr preNodePtr = Head;

		if (currentNodePtr == NULL)
		{
			Head->next = linkedNodePtr;
		}
		else
		{
			while (currentNodePtr != NULL)
			{
				if ((linkedNodePtr->nodeptr->fHat) <= (currentNodePtr->nodeptr->fHat))
				{
					linkedNodePtr->next = currentNodePtr;
					preNodePtr->next = linkedNodePtr;
					break;
				}
				else
				{
					currentNodePtr = currentNodePtr->next;
					preNodePtr = preNodePtr->next;
				}
			}
		}
	}

	else if (choiceHead == CLOSED)	// ������ Closed �ڿ� �����Ѵ�.
	{
		Head = Closed;
		linkedNodePtr->next = Head->next;
		Head->next = linkedNodePtr;
	}
}

LinkedNodePtr removeLinkedNodePtr(enum ChoiceHead choiceHead, LinkedNodePtr linkedNodePtr)
{
	LinkedNodePtr Head;
	LinkedNodePtr removedLinkedNodePtr = linkedNodePtr;

	if (choiceHead == OPEN)
		Head = Open;
	else if (choiceHead == CLOSED)
		Head = Closed;

	LinkedNodePtr currentNodePtr = Head->next;
	LinkedNodePtr preNodePtr = Head;

	while (currentNodePtr != NULL)
	{
		int count = 0;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (currentNodePtr->nodeptr->tile[i][j] == linkedNodePtr->nodeptr->tile[i][j])
					count++;
			}
		}

		if (count == 16)
			break;

		currentNodePtr = currentNodePtr->next;
		preNodePtr = preNodePtr->next;
	}

	preNodePtr->next = linkedNodePtr->next;

	return removedLinkedNodePtr;
}

enum TrueOrFalse isGoalNode(NodePtr nodePtr)
{
	int differenceWithGaol = 0;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0 ; j < 4 ; j++)
		{
			if (nodePtr->tile[i][j] == goal[i][j])
				differenceWithGaol++;
		}
	}

	if (differenceWithGaol == 16)
		return TRUE;
	else
		return FALSE;
}

void printGoalToStart(NodePtr nodePtr)
{
	NodePtr currentNodePtr = nodePtr;

	printf("<��ǥ��� -> ���۳��>\n");

	while (currentNodePtr != NULL)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (currentNodePtr->tile[i][j] != 0)
					printf("%3d", currentNodePtr->tile[i][j]);
				else
					printf("   ");
			}
			printf("\n");
		}
		currentNodePtr = currentNodePtr->preNode;
		printf("\n");
	}
}

LocationZero findZeroLocation(NodePtr nodePtr)
{
	LocationZero locationZero;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (nodePtr->tile[i][j] == 0) {
				locationZero.i = i;
				locationZero.j = j;
				break;
			}
		}
	}

	return locationZero;
}

void expandCurrentNode(LinkedNodePtr currentLinkedNode, LocationZero locationZero, enum Direction direction)
{
	NodePtr  movedNodePtr;
	LinkedNodePtr newLinkedNodePtr  = createLinkedNode();

	movedNodePtr = getMovedNodeFromCurrentNodePtr(currentLinkedNode->nodeptr, locationZero, direction);

	double newgHatOfmovedNode = currentLinkedNode->nodeptr->gHat + 1;


	double oldfHatOfmovedNode = movedNodePtr->fHat;
	double newfHatOfmovedNode = newgHatOfmovedNode + movedNodePtr->hHat;


	if ((!isExist(CLOSED, movedNodePtr)) && (!isExist(OPEN, movedNodePtr))) /////////////////����� �;��ϴµ� �̻��ϴ�??
	{
		movedNodePtr->preNode = currentLinkedNode->nodeptr;
		newLinkedNodePtr->nodeptr = movedNodePtr;
		insertLinkedNodePtr(OPEN, newLinkedNodePtr);
	}

	else
	{
		if (newfHatOfmovedNode < oldfHatOfmovedNode)
		{
			movedNodePtr->fHat = newfHatOfmovedNode;
			movedNodePtr->gHat = newgHatOfmovedNode;
			movedNodePtr->preNode = currentLinkedNode->nodeptr;

			if (isExist(CLOSED, movedNodePtr))
			{
				newLinkedNodePtr->nodeptr = movedNodePtr;
				insertLinkedNodePtr(OPEN, newLinkedNodePtr);
				removeLinkedNodePtr(CLOSED, newLinkedNodePtr);
			}

		}
	}
}

NodePtr getMovedNodeFromCurrentNodePtr(NodePtr currentNode, LocationZero locationZero, enum Direction direction)
{
	NodePtr successorNodePtr = (NodePtr)malloc(sizeof(Node));

	copyTile(successorNodePtr, currentNode);

	if (direction == LEFT)
	{
		successorNodePtr->tile[locationZero.i][locationZero.j] = successorNodePtr->tile[locationZero.i][locationZero.j - 1];
		successorNodePtr->tile[locationZero.i][locationZero.j - 1] = 0;

		successorNodePtr->hHat = return_hHat(successorNodePtr);
		successorNodePtr->gHat = currentNode->gHat + 1;
		successorNodePtr->fHat = successorNodePtr->gHat + successorNodePtr->hHat;
	}
	else if (direction == RIGHT)
	{
		successorNodePtr->tile[locationZero.i][locationZero.j] = successorNodePtr->tile[locationZero.i][locationZero.j + 1];
		successorNodePtr->tile[locationZero.i][locationZero.j + 1] = 0;

		successorNodePtr->hHat = return_hHat(successorNodePtr);
		successorNodePtr->gHat = currentNode->gHat + 1;
		successorNodePtr->fHat = successorNodePtr->gHat + successorNodePtr->hHat;
	}
	else if (direction == UP)
	{
		successorNodePtr->tile[locationZero.i][locationZero.j] = successorNodePtr->tile[locationZero.i - 1][locationZero.j];
		successorNodePtr->tile[locationZero.i - 1][locationZero.j] = 0;

		successorNodePtr->hHat = return_hHat(successorNodePtr);
		successorNodePtr->gHat = currentNode->gHat + 1;
		successorNodePtr->fHat = successorNodePtr->gHat + successorNodePtr->hHat;
	}
	else if (direction == DOWN)
	{
		successorNodePtr->tile[locationZero.i][locationZero.j] = successorNodePtr->tile[locationZero.i + 1][locationZero.j];
		successorNodePtr->tile[locationZero.i + 1][locationZero.j] = 0;

		successorNodePtr->hHat = return_hHat(successorNodePtr);
		successorNodePtr->gHat = currentNode->gHat + 1;
		successorNodePtr->fHat = successorNodePtr->gHat + successorNodePtr->hHat;
	}

	return successorNodePtr;
}

void copyTile(NodePtr successorNodePtr, NodePtr currentNodePtr)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			successorNodePtr->tile[i][j] = currentNodePtr->tile[i][j];
		}
	}
}

enum TrueOrFalse isExist(enum ChoiceHead choiceHead, NodePtr node)
{
	int answer = FALSE; //defualt�� false(0)

	LinkedNodePtr currentLinkedNodePtr;

	if (choiceHead == OPEN)
		currentLinkedNodePtr = Open;
	else if(choiceHead == CLOSED)
		currentLinkedNodePtr = Closed;

	while (currentLinkedNodePtr->next != NULL)
	{
		int count = 0;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if ((currentLinkedNodePtr->next)->nodeptr->tile[i][j] == node->tile[i][j])
					count++;
			}
		}

		if (count == 16)
		{
			answer = TRUE;
			break;
		}

		currentLinkedNodePtr = currentLinkedNodePtr->next;
	}

	return answer;
}

int return_hHat(NodePtr node)
{
	int differenceCount = 0;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (node->tile[i][j] != goal[i][j])
				differenceCount++;
		}
	}
	return differenceCount - 1; // empty tile�� ��ġ�� ���� ����.
}

void printStartToGoal(NodePtr nodePtr)
{	
	NodePtr currentNodePtr = nodePtr;

	Route = createLinkedNode(); // Route -> NULL

	LinkedNodePtr Head = Route;

	int size = 0;

	while (currentNodePtr != NULL)
	{
		NodePtr newNodePtr = createNode();

		copyTile(newNodePtr, currentNodePtr);
		newNodePtr->fHat;
		newNodePtr->gHat;
		newNodePtr->hHat;
		newNodePtr->preNode = currentNodePtr->preNode;

		LinkedNodePtr newLinkedNodePtr = createLinkedNode();

		newLinkedNodePtr->nodeptr = newNodePtr;
		Head->next = newLinkedNodePtr;


		currentNodePtr = currentNodePtr->preNode;
		Head = Head->next;
		size++;
	}
	
	LinkedNodePtr* tempStack = (LinkedNodePtr*)malloc(sizeof(LinkedNodePtr)*size);

	
	//printf("%d\n", size);
	Head = Route->next;
	int count = size;
	while (count > 0) {
		tempStack[count - 1] = Head;
		Head = Head->next;
		count--;
	}

	LocationZero currentLocationZero = {0,0};
	LocationZero preLocationZero = {0,0};
	
	while (count<size)
	{
		

		if (count == 0)
		{
			printf("��� Ž�� ����Դϴ�.\n");
			printf("[%d]This is the start state\n", count);
		}
		else
		{

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (tempStack[count]->nodeptr->tile[i][j] == 0)
					{
						currentLocationZero.i = i;
						currentLocationZero.j = j;
					}
				}
			}
		}
		switch (numWhereToMove(preLocationZero, currentLocationZero))
		{
		case LEFT:
			printf("[%d] => %d is moved to left\n", count, tempStack[count]->nodeptr->tile[preLocationZero.i][preLocationZero.j]);
			break;
		case RIGHT:
			printf("[%d] => %d is moved to right\n", count, tempStack[count]->nodeptr->tile[preLocationZero.i][preLocationZero.j]);
			break;
		case UP:
			printf("[%d] => %d is moved to up\n", count, tempStack[count]->nodeptr->tile[preLocationZero.i][preLocationZero.j]);
			break;
		case DOWN:
			printf("[%d] => %d is moved to down\n", count, tempStack[count]->nodeptr->tile[preLocationZero.i][preLocationZero.j]);
			break;
		default:
			break;
		}

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (tempStack[count]->nodeptr->tile[i][j] == 0)
				{
					currentLocationZero.i = i;
					currentLocationZero.j = j;
					printf("   ");
				}
				else if (tempStack[count]->nodeptr->tile[i][j] != 0)
					printf("%3d", tempStack[count]->nodeptr->tile[i][j]);
			}
			printf("\n");
		}
		printf("\n");

		count++;

		preLocationZero = currentLocationZero;
		
	}
	printf("This is goal! Success!!\n");
	printf("Number of moves required = %d\n", count - 1);
}

enum Direction numWhereToMove(LocationZero preLocationZero, LocationZero currentLocationZero)
{
	if (preLocationZero.j < currentLocationZero.j)
		return LEFT;
	else if (preLocationZero.j > currentLocationZero.j)
		return RIGHT;
	else if (preLocationZero.i < currentLocationZero.i)
		return UP;
	else if (preLocationZero.i > currentLocationZero.i)
		return DOWN;
}

void NodeInput(int position[4][4])
{
	char* check = NULL;
	char input[50];

	check = fgets(input, 50, stdin);
	strtok(input, "\n");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i == 0 && j == 0)
			{
				check = strtok(input, " ");
				position[i][j] = atoi(check);
			}
			else
			{
				check = strtok(NULL, " ");
				if (check != NULL)
					position[i][j] = atoi(check);
			}
		}
	}
}