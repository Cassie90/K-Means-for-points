#include "StdAfx.h"
#include "PointKMeanClusterizer.h"
#include "line.h"
#include <cassert>
#include <cmath>

// �������� ��� ��������� ���� ����� �� ���������
bool operator<(const CPoint& left, const CPoint& right) {
	if (left.x < right.x) {
		return true;
	}
	else if (left.x > right.x) {
		return false;
	}
	else {
		if (left.y < right.y)
			return true;
		else
			return false;
	}
}

// ��������� ��������� ���������� ����� ����� ������� ��� �������� ����� ����� �������
double operator-(const CPoint& left, const CPoint& right) {
	return sqrt(pow(left.x - right.x, 2.0) + pow(left.y - right.y, 2.0));
}

CPointKMeanClusterizer::CPointKMeanClusterizer(void) : _run_mode(false)
{
}

CPointKMeanClusterizer::~CPointKMeanClusterizer(void)
{
}

CPointKMeanClusterizer::CPointKMeanClusterizer(size_t num_points, size_t num_clusters, const CRect& border, size_t max_steps) 
{
	Generate(num_points, num_clusters, border, max_steps);
}

void CPointKMeanClusterizer::Generate(size_t num_points, size_t num_clusters, const CRect& border, size_t max_steps) {
	assert(!_run_mode);

	// ���������� �����
	K_POINTS points = _GenerateUniquePoints(num_points + num_clusters, border);
	// ����������� ������ ���������
	K_POINTS kmeans = _ExtractClusterCenters(num_clusters, points);
	K_POINTS old_kmeans;
	size_t i = 0;
	do {
		// ��� ��� ���� ����� �� �����������, ������ ����������� �� ���������� ��������
		if (i++ > max_steps) break;
		// ������������ ����� �� �������
		_clusters = _MakeCluster(kmeans, points);
		old_kmeans = kmeans;
		// �������� �����
		kmeans = _ClusterizeStep(_clusters);
	}
	while (old_kmeans != kmeans);
}

CLUSTERS CPointKMeanClusterizer::GetClusters() const {
	return _clusters;
}

K_POINTS CPointKMeanClusterizer::_GenerateUniquePoints(size_t num_points, const CRect& border)
{
	K_POINTS points;
	while(points.size() < num_points) {
		// ���������� �����, ���� �� ������� num_points ����������
		points.insert(_GetRandomPoint(border));
	}
	return points;
}

K_POINTS CPointKMeanClusterizer::_ExtractClusterCenters(size_t num_clusters, K_POINTS& points)
{
	assert(num_clusters > 0 && num_clusters <= points.size());
	K_POINTS kmeans;
	for(size_t i=0; i<num_clusters; ++i) {
		// ����������� ��������� ����� �� ������ ����� � ����� �������
		K_POINTS::iterator rnd = points.begin();
		std::advance(rnd, (static_cast<int>(abs(normal()) * points.size()) % points.size()));
		kmeans.insert(*rnd);
		points.erase(rnd);
	}
	return kmeans;
}

CPoint CPointKMeanClusterizer::_GetRandomPoint(const CRect& border)
{
	int x = (static_cast<int>(abs(normal()) * border.Width()) % border.Width()) + border.left;
	int y = (static_cast<int>(abs(normal()) * border.Height()) % border.Height()) + border.top;
	return CPoint(x, y);
}

CLUSTERS CPointKMeanClusterizer::_MakeCluster(const K_POINTS& kmeans, const K_POINTS& points) 
{
	assert(kmeans.size() > 0 && kmeans.size() <= points.size());

	CLUSTERS clusters;
	K_POINTS::const_iterator it = kmeans.begin(), end = kmeans.end();
	while (it != end) {
		clusters.push_back(CLUSTER(*(it++), K_POINTS()));
	}

	// ���������� ��� �����
	it = points.begin(), end = points.end();
	while (it != end) {
		// ���������� ��� k-������� � ������� ���������
		CLUSTERS::iterator cit = clusters.begin(), cend = clusters.end();
		CLUSTERS::iterator nearest_cluster = cit++;
		double min_lenght = *it - nearest_cluster->first;
		while (cit != cend) {
			// ��������� ����� ����� k-������� � ������� ������
			double tmp_length = *it - cit->first;
			if (tmp_length < min_lenght) { // ���� ����� ����� � �������� k-��������, ���������� ���
				nearest_cluster = cit;
				min_lenght = tmp_length;
			}
			++cit;
		}
		// ��������� ����� � ������� ��������������� ���������� k-��������
		nearest_cluster->second.insert(*(it++));
	}
	return clusters;
}

K_POINTS CPointKMeanClusterizer::_ClusterizeStep(const CLUSTERS& clusters) 
{
	assert(!clusters.empty());

	K_POINTS new_kmeans;
	// ��������� �� ��� ������������� ���������
	CLUSTERS::const_iterator it = clusters.begin(), end = clusters.end();
	while (it != end) {
		const CPoint& kmean = it->first;
		const K_POINTS& points = it->second;
		// ���� ������� ������� ����, ��������� ���� ��� ���� � ��������� � ���������� ��������
		if (points.empty()) {
			new_kmeans.insert(kmean);
			++it;
			continue;
		}
		// ��������� �� ���� ������ �������� � ��������� ������� �������������� ������������ �������
		K_POINTS::const_iterator pit = points.begin(), pend = points.end();
		int left = pit->x, top = pit->y, right = pit->x, bottom = pit->y;
		++pit;
		while (pit != pend) {
			if (pit->x < left)
				left = pit->x;
			else if (pit->x > right)
				right = pit->x;

			if (pit->y < top)
				top = pit->y;
			else if (pit->y > bottom)
				bottom = pit->y;
			++pit;
		}
		// ��������� ����� ���������� �������������� � ������ ����� �������
		new_kmeans.insert(CPoint(left + (right - left)/2, top + (bottom - top)/2));
		++it;
	}
	return new_kmeans;
}

void CPointKMeanClusterizer::Run(size_t num_points, size_t num_clusters, const CRect& border) {
	_run_mode = true;
	_points = _GenerateUniquePoints(num_points + num_clusters, border);
	K_POINTS kmeans = _ExtractClusterCenters(num_clusters, _points);
	_clusters = _MakeCluster(kmeans, _points);
}

void CPointKMeanClusterizer::Step() {
	K_POINTS kmeans = _ClusterizeStep(_clusters);
	_clusters = _MakeCluster(kmeans, _points);
}

void CPointKMeanClusterizer::Stop() {
	_run_mode = false;
	K_POINTS().swap(_points);
}

bool CPointKMeanClusterizer::IsRun() const {
	return _run_mode;
}