#pragma once

#include <set>
#include <deque>

typedef std::set<CPoint> K_POINTS; // Set ��� ���������. ����� ���� ��� � ������, �� ��� �������� ���������
typedef std::pair<CPoint, K_POINTS> CLUSTER; // pair ��� ��������� �� ���� �����
typedef std::deque<CLUSTER> CLUSTERS; // vector ��� ������ �������

class CPointKMeanClusterizer
{
public:
	CPointKMeanClusterizer(void);
	~CPointKMeanClusterizer(void);
	CPointKMeanClusterizer(size_t num_points, size_t num_clusters, const CRect& border, size_t max_steps); 

	bool IsRun() const;
	CLUSTERS GetClusters() const;

	// ��� ���������� ���������� ���������
	void Run(size_t num_points, size_t num_clusters, const CRect& border);
	void Step();
	void Stop();
	// ��� ���������� � ������������
	void Generate(size_t num_points, size_t num_clusters, const CRect& border, size_t max_steps);
private:
	// ���������� num_points ���������� ����� � �������� ������������� ������� border
	K_POINTS _GenerateUniquePoints(size_t num_points, const CRect& border);
	// ����������� �� points num_clusters ����� � ������ �� �������� ���������
	K_POINTS _ExtractClusterCenters(size_t num_clusters, K_POINTS& points);
	// ������� ��������� �����
	CPoint _GetRandomPoint(const CRect& border);
	// �������������� ����� ����� �� ������ �������
	CLUSTERS _MakeCluster(const K_POINTS& kmeans, const K_POINTS& points);
	// ���������� ������ ���������
	K_POINTS _ClusterizeStep(const CLUSTERS& clusters);
private:
	bool _run_mode;
	CLUSTERS _clusters; 
	K_POINTS _points;
};

bool operator<(const CPoint& left, const CPoint& right);
double operator-(const CPoint& left, const CPoint& right);