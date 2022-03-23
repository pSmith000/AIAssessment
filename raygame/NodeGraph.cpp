#include "NodeGraph.h"
#include <raylib.h>
#include <xlocinfo>

DynamicArray<NodeGraph::Node*> reconstructPath(NodeGraph::Node* start, NodeGraph::Node* end)
{
	DynamicArray<NodeGraph::Node*> path;
	NodeGraph::Node* currentNode = end;

	while (currentNode != start->previous)
	{
		currentNode->color = 0xFFFF00FF;
		path.insert(currentNode, 0);
		currentNode = currentNode->previous;
	}

	return path;
}

float diagonalDistance(NodeGraph::Node* node, NodeGraph::Node* goal, float cardinalCost, float diagonalCost)
{
	float displacementX = abs(node->position.x - goal->position.x);
	float displacementY = abs(node->position.y - goal->position.y);

	return cardinalCost * (displacementX + displacementY) + (diagonalCost - 2 * cardinalCost) * fmin(displacementX, displacementY);
}

void sortFScore(DynamicArray<NodeGraph::Node*>& nodes)
{
	NodeGraph::Node* key = nullptr;
	int j = 0;

	for (int i = 1; i < nodes.getLength(); i++) {
		key = nodes[i];
		j = i - 1;
		while (j >= 0 && nodes[j]->fScore > key->fScore) {
			nodes[j + 1] = nodes[j];
			j--;
		}

		nodes[j + 1] = key;
	}
}

DynamicArray<NodeGraph::Node*> NodeGraph::findPath(Node* start, Node* goal)
{
	//Reset the graph score to fix the drawing to the screen
	resetGraphScore(start);
	//Insert algorithm here
	DynamicArray<NodeGraph::Node*> openSet = DynamicArray<NodeGraph::Node*>();
	DynamicArray<NodeGraph::Node*> closedSet = DynamicArray<NodeGraph::Node*>();

	//Sets the current node to be the starting node and adds it to the open list
	Node* currentNode = start;
	start->color = 0x00FF00FF;
	openSet.addItem(start);

	//While the open list has items in it
	while (openSet.getLength() > 0)
	{
		//Sort the open list, set the current node to be the first item and remove it
		sortFScore(openSet);
		currentNode = openSet[0];
		openSet.remove(currentNode);

		if (!closedSet.contains(currentNode))
		{
			//Loop through all of the connected edges to the current node
			for (int i = 0; i < currentNode->edges.getLength(); i++)
			{
				//Set the target node to be the edges target
				NodeGraph::Node* targetNode = currentNode->edges[i].target;
				targetNode->color = 0xFF0000FF;

				//If the path is not possible continue
				if (!targetNode->walkable)
					continue;

				//If the gscore is greater than 0 and more than the cost of a different path
				if (targetNode->gScore == 0 || targetNode->gScore > currentNode->gScore + currentNode->edges[i].cost)
				{
					//reset the score to be the better path
					targetNode->gScore = currentNode->gScore + currentNode->edges[i].cost;
					targetNode->hScore = getManhattanDistance(targetNode, goal);
					targetNode->fScore = targetNode->gScore + targetNode->hScore;
					targetNode->previous = currentNode;
				}
				//if we have not looked at the target node in the open list
				if (!openSet.contains(targetNode))
					//add it to the open list
					openSet.addItem(targetNode);
			}
			//After we are done add the item to the closed list
			closedSet.addItem(currentNode);
		}
		//if the current node is the goal then return
		if (currentNode == goal)
			return reconstructPath(start, goal);
	}

	return reconstructPath(start, goal);
}

float NodeGraph::getManhattanDistance(Node* start, Node* end) 
{
	//returns the manhattan distance formula used to get the hscore of the node
	return abs(start->position.x - end->position.x)
		+ abs(start->position.y - end->position.y);
}


void NodeGraph::sortGScore(DynamicArray<NodeGraph::Node*>& nodes)
{
	NodeGraph::Node* key = nullptr;
	int j = 0;

	for (int i = 1; i < nodes.getLength(); i++) {
		key = nodes[i];
		j = i - 1;
		while (j >= 0 && nodes[j]->gScore > key->gScore) {
			nodes[j + 1] = nodes[j];
			j--;
		}

		nodes[j + 1] = key;
	}
}

void NodeGraph::drawGraph(Node* start)
{
	DynamicArray<Node*> drawnList = DynamicArray<Node*>();
	drawConnectedNodes(start, drawnList);
}

void NodeGraph::drawNode(Node* node, float size)
{
	static char buffer[10];
	sprintf_s(buffer, "%.0f", node->gScore);

	//Draw the circle for the outline
	DrawCircle((int)node->position.x, (int)node->position.y, size + 1, GetColor(node->color));
	//Draw the inner circle
	DrawCircle((int)node->position.x, (int)node->position.y, size, GetColor(node->color));
	//Draw the text
	DrawText(buffer, (int)node->position.x, (int)node->position.y, .8f, BLACK);
}

void NodeGraph::drawConnectedNodes(Node* node, DynamicArray<Node*>& drawnList)
{
	drawnList.addItem(node);
	if (node->walkable)
		drawNode(node, 8);

	for (int i = 0; i < node->edges.getLength(); i++)
	{
		Edge e = node->edges[i];
		////Draw the Edge
		//DrawLine((int)node->position.x, (int)node->position.y, (int)e.target->position.x, (int)e.target->position.y, WHITE);
		////Draw the cost
		//MathLibrary::Vector2 costPos = { (node->position.x + e.target->position.x) / 2, (node->position.y + e.target->position.y) / 2 };
		//static char buffer[10];
		//sprintf_s(buffer, "%.0f", e.cost);
		//DrawText(buffer, (int)costPos.x, (int)costPos.y, 16, RAYWHITE);
		//Draw the target node
		if (!drawnList.contains(e.target)) {
			drawConnectedNodes(e.target, drawnList);
		}
	}
}

void NodeGraph::resetGraphScore(Node * start)
{
	DynamicArray<Node*> resetList = DynamicArray<Node*>();
	resetConnectedNodes(start, resetList);
}

void NodeGraph::resetConnectedNodes(Node* node, DynamicArray<Node*>& resetList)
{
	resetList.addItem(node);

	for (int i = 0; i < node->edges.getLength(); i++)
	{
		node->edges[i].target->gScore = 0;
		node->edges[i].target->hScore = 0;
		node->edges[i].target->fScore = 0;
		node->edges[i].target->color = 0xFFFFFFFF;

		//Draw the target node
		if (!resetList.contains(node->edges[i].target)) {
			resetConnectedNodes(node->edges[i].target, resetList);
		}
	}
}