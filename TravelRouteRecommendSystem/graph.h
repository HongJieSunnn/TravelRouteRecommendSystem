#pragma once
#ifndef GRAPH_H
#define GRAPH_H

#include<iostream>
#include<vector>
#include<unordered_map>
#include<queue>
#include<algorithm>
#include<queue>
#include"vehicle.h"
using std::vector;
using std::unordered_map;
using std::pair;
using std::unique_ptr;
using std::queue;
using std::priority_queue;
using std::max;
using std::min;
using std::sort;
using std::cout;
using std::make_pair;

namespace
{
	//节点标号
	template<class T>
	using VertexData = T;
	//节点编号
	using VertexIndex = int;
	//权重
	template<class T>
	using Weight = T;
	//节点列表类型
	template<class T>
	using VertexList = vector<T>;
	//保存某点所有邻接点下标
	using NodeList = vector<VertexIndex>;

	enum OperateGraphStatue
	{
		CREATE_GRAPH_FAILED,
		CREATE_GRAPH_SUCCEED,
		CREATE_GRAPH_FAILED_NO_VERTEX,
		CREATE_GRAPH_FAILED_NO_EDGE,//普通的图可以没有边，但是我们是路线，必须有边
		CREATE_GRAPH_FAILED_NO_WEIGHT,
	};
}
/*
	邻接矩阵表示的Graph
	*vertex_list:节点列表
	*matrix_edge_table:邻接矩阵 tip:若编号0->编号1有边 则matrix_edge_table[0][1]=1
	*vertex_list_map:节点列表字典 tip:因为节点标号和编号实际上没有什么特殊对应关系 要通过一个字典来保存一个节点的标号和编号 这样我们用标号操作 就可以很方便得到编号
	*edge_nums:边数量 tip:无向图边数量不需要乘2
	*vertex_nums:节点数量
 Tip:
		1. 通过标号获取编号 用vertex_list_map[标号]
		2. 通过编号获取标号 用vertex_list[编号]
*/
template<class T>
class Graph
{
private:
	VertexList<T> vertex_list;
	vector<vector<Weight>> matrix_edge_table;
	unordered_map<VertexData<T>, VertexIndex> vertex_list_map;
	int edge_nums;
	int vertex_nums;

	/*
		判断是否存在该边 内联函数 行数很少
	* start_index:开始编号
	* end_index:结束编号
	*/
	inline bool isEdge(VertexIndex start_index, VertexIndex end_index)
	{
		if (this->matrix_edge_table[start_index][end_index] >= 1)
			return true;
		return false;
	}

	/*
		* 判断是否存在该点
	*node:点标号
	*/
	inline bool isNode(VertexData<T> node)
	{
		if (vertex_list_map.find(node) != vertex_list_map.end())
			return true;
		return false;
	}

	/*
		添加节点 这是无向图有向图通用的函数
		*node:要添加的点的标号
	*/
	void newNode(VertexData<T> node);

	/*
		删除节点以及关联边 这是无向图和有向图通用的函数
		*node:要删除的节点标号
		Tip:直接删除掉矩阵里要删除的节点对应的行和列就行了
	*/
	void delNode(VertexData<T> node);

	/*
		建立两点间的一条边 纯虚函数
	* start_node:开始点的编号
	* end_node:结束点的编号 
	* weight:权重
	*/
	void addEdge(VertexData<T> start_node, VertexData<T> end_node, Weight weight = NULL);

	/*
		删除边
	* start_node:开始点的编号
	* end_node:结束点的编号 
	*/
	void delEdge(VertexData<T> start_node, VertexData<T> end_node);
public:
	~Graph() {}
	/*
		构造函数
	* vertex_datas:所有节点标号
	* edges:边 用pair存放
	* vertex_nums:节点数 默认为0 当然为0就无法构成图
	* edge_nums:边的数目
	* weights:权重 必须有 因为距离不可能为1啊
	*/
	Graph(int vertex_nums = 0, int edge_nums = 0)
	{
		this->edge_nums = edge_nums;
		this->vertex_nums = vertex_nums;
		this->vertex_list = VertexList<T>(vertex_nums);
		this->matrix_edge_table = vector<vector<Weight>>(vertex_nums, vector<Weight>(vertex_nums, 0));
	}

	OperateGraphStatue createGraph(vector<VertexData<T>> vertex_datas, vector<pair<VertexData<T>, VertexData<T>>> edges, Weight weights = nullptr);
};
#endif // GRAPH_H