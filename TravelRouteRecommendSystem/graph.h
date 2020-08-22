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
	//�ڵ���
	template<class T>
	using VertexData = T;
	//�ڵ���
	using VertexIndex = int;
	//Ȩ��
	template<class T>
	using Weight = T;
	//�ڵ��б�����
	template<class T>
	using VertexList = vector<T>;
	//����ĳ�������ڽӵ��±�
	using NodeList = vector<VertexIndex>;

	enum OperateGraphStatue
	{
		CREATE_GRAPH_FAILED,
		CREATE_GRAPH_SUCCEED,
		CREATE_GRAPH_FAILED_NO_VERTEX,
		CREATE_GRAPH_FAILED_NO_EDGE,//��ͨ��ͼ����û�бߣ�����������·�ߣ������б�
		CREATE_GRAPH_FAILED_NO_WEIGHT,
	};
}
/*
	�ڽӾ����ʾ��Graph
	*vertex_list:�ڵ��б�
	*matrix_edge_table:�ڽӾ��� tip:�����0->���1�б� ��matrix_edge_table[0][1]=1
	*vertex_list_map:�ڵ��б��ֵ� tip:��Ϊ�ڵ��źͱ��ʵ����û��ʲô�����Ӧ��ϵ Ҫͨ��һ���ֵ�������һ���ڵ�ı�źͱ�� ���������ñ�Ų��� �Ϳ��Ժܷ���õ����
	*edge_nums:������ tip:����ͼ����������Ҫ��2
	*vertex_nums:�ڵ�����
 Tip:
		1. ͨ����Ż�ȡ��� ��vertex_list_map[���]
		2. ͨ����Ż�ȡ��� ��vertex_list[���]
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
		�ж��Ƿ���ڸñ� �������� ��������
	* start_index:��ʼ���
	* end_index:�������
	*/
	inline bool isEdge(VertexIndex start_index, VertexIndex end_index)
	{
		if (this->matrix_edge_table[start_index][end_index] >= 1)
			return true;
		return false;
	}

	/*
		* �ж��Ƿ���ڸõ�
	*node:����
	*/
	inline bool isNode(VertexData<T> node)
	{
		if (vertex_list_map.find(node) != vertex_list_map.end())
			return true;
		return false;
	}

	/*
		��ӽڵ� ��������ͼ����ͼͨ�õĺ���
		*node:Ҫ��ӵĵ�ı��
	*/
	void newNode(VertexData<T> node);

	/*
		ɾ���ڵ��Լ������� ��������ͼ������ͼͨ�õĺ���
		*node:Ҫɾ���Ľڵ���
		Tip:ֱ��ɾ����������Ҫɾ���Ľڵ��Ӧ���к��о�����
	*/
	void delNode(VertexData<T> node);

	/*
		����������һ���� ���麯��
	* start_node:��ʼ��ı��
	* end_node:������ı�� 
	* weight:Ȩ��
	*/
	void addEdge(VertexData<T> start_node, VertexData<T> end_node, Weight weight = NULL);

	/*
		ɾ����
	* start_node:��ʼ��ı��
	* end_node:������ı�� 
	*/
	void delEdge(VertexData<T> start_node, VertexData<T> end_node);
public:
	~Graph() {}
	/*
		���캯��
	* vertex_datas:���нڵ���
	* edges:�� ��pair���
	* vertex_nums:�ڵ��� Ĭ��Ϊ0 ��ȻΪ0���޷�����ͼ
	* edge_nums:�ߵ���Ŀ
	* weights:Ȩ�� ������ ��Ϊ���벻����Ϊ1��
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