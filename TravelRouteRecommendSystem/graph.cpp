#include "graph.h"

template<class T, class WeightType>
void Graph<T, WeightType>::newNode(VertexData<T> node)
{
	this->vertex_list.push_back(node);
	this->vertex_list_map[node] = this->vertex_nums;
	this->vertex_nums++;
	this->matrix_edge_table.push_back(vector<Weight>(this->vertex_nums, 0));//添加新矩阵行
	for (int i = 0; i < this->vertex_nums - 1; i++)//在原先每一行后面新增一列
	{
		this->matrix_edge_table[i].push_back(0);
	}
}

template<class T, class WeightType>
void Graph<T, WeightType>::delNode(VertexData<T> node)
{
	NodeList adjNodes = vector<VertexIndex>;
	VertexIndex node_index = this->vertex_list_map[node];
	for (int i = 0; i < this->vertex_nums; i++)
	{
		if (isEdge(Graph, node_index, i))
			adjNodes.push_back(i);
	}
	VertexIndex deleted_node_index = this->vertex_list_map[node];

	if (adjNodes.empty())
	{
		return;
	}
	this->vertex_list.erase(this->vertex_list.begin() + deleted_node_index);
	this->vertex_list_map.erase(node);
	this->vertex_nums--;
	//删除边后修改矩阵 那边不就没了 所以不需要额外 delEdge
	this->matrix_edge_table.erase(this->matrix_edge_table.begin() + deleted_node_index);
	for (int i = 0; i < this->vertex_nums; i++)
	{
		this->matrix_edge_table[i].erase(this->matrix_edge_table[i].begin() + deleted_node_index);
	}
}

template<class T, class WeightType>
void Graph<T, WeightType>::addEdge(VertexData<T> start_node, VertexData<T> end_node, Weight<WeightType> weight)
{
	if (!isNode(start_node) || !isNode(end_node))
	{
		cout << "Sorry.At least one node does not exist\n";
		return;
	}
	VertexIndex start_index = this->vertex_list_map[start_node];
	VertexIndex end_index = this->vertex_list_map[end_node];

	if (this->isEdge(start_index, end_index))
	{
		cout << "Sorry.This edge had existed\n";
		return;
	}
		
	this->matrix_edge_table[start_index][end_index] = weight;
}

template<class T, class WeightType>
void Graph<T, WeightType>::delEdge(VertexData<T> start_node, VertexData<T> end_node)
{
	if (!isNode(start_node) || !isNode(end_node))
	{
		cout << "Sorry.At least one node does not exist\n";
		return;
	}

	VertexIndex start_index = this->vertex_list_map[start_node];
	VertexIndex end_index = this->vertex_list_map[end_node];

	if (!this->isEdge(start_index, end_index))
	{
		cout << "Sorry.This edge does not exist\n";
		return;
	}

	this->matrix_edge_table[start_index][end_index] = 0;
	this->edge_nums--;
}

template<class T, class WeightType>
OperateGraphStatue Graph<T, WeightType>::createGraph(vector<VertexData<T>> vertex_datas, vector<pair<VertexData<T>, VertexData<T>>> edges, vector<Weight<WeightType>> weights)
{
	if (this->vertex_nums == 0)
	{
		cout << "Sorry.The graph has no vertex so that you can not create this graph";
		return CREATE_GRAPH_FAILED_NO_VERTEX;
	}

	if (this->edge_nums == 0)
	{
		cout << "Sorry.The graph has vertex but has no edge is wrong.";
		return CREATE_GRAPH_FAILED_NO_EDGE;
	}

	for (int i = 0; i < this->vertex_nums; i++)
	{
		this->vertex_list[i] = vertex_datas[i];
		this->vertex_list_map[vertex_datas[i]] = i;
	}

	for (int i = 0; i < edge_nums; i++)
	{
		if (!weights.empty())
			this->addEdge(edges[i].first, edges[i].second, weights[i]);
		else
		{
			cout << "Sorry.We must get the weight of every edge.";
			return CREATE_GRAPH_FAILED_NO_WEIGHT;
		}
	}

	return CREATE_GRAPH_SUCCEED;
}

