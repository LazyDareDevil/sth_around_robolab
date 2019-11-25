#include <algorithm>
#include <chrono>
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>

using namespace std;

class GRAPH {
private:
	int number_of_vertices,
		number_of_edges;
	bool** orgraph;
	vector<vector<int>> strong_connect_components;
	chrono::microseconds alghotithm_time;

	int generate_orgraph(int number_of_vertices, bool**& orgraph) {
		int number_of_edges = 0;
		int flag = 1 + rand() % 3;
		int a;
		if (flag == 1) a = 1 + rand() % 9;
		if (flag == 2) a = 10 + rand() % 19;
		if (flag == 3) a = 100 + rand() % 90;

		for (int i = 0; i < number_of_vertices; ++i) {
			orgraph[i] = new bool[number_of_vertices];
			for (int j = 0; j < number_of_vertices; ++j) {
				int b = 1 + rand() % (number_of_vertices * (number_of_vertices - 1));
				orgraph[i][j] = b % a == 0;
			}
			orgraph[i][i] = false;
		}
		for (int i = 0; i < number_of_vertices; ++i) {
			for (int j = 0; j < number_of_vertices; ++j) {
				if (orgraph[i][j] == true) ++number_of_edges;
			}
		}
		return number_of_edges;
	}

	bool** get_revesed_orgraph(int number_of_vertices, bool** mtx) {
		bool** t_mtx = new bool*[number_of_vertices];
		for (int i = 0; i < number_of_vertices; ++i) {
			t_mtx[i] = new bool[number_of_vertices];
		}
		for (int i = 0; i < number_of_vertices - 1; ++i) {
			t_mtx[i][i] = mtx[i][i];
			for (int j = i + 1; j < number_of_vertices; ++j) {
				t_mtx[i][j] = mtx[j][i];
				t_mtx[j][i] = mtx[i][j];
			}
		}
		t_mtx[number_of_vertices - 1][number_of_vertices - 1] = mtx[number_of_vertices - 1][number_of_vertices - 1];
		return t_mtx;
	}

	void dfs(int number_of_vertices, bool** mtx, int start, vector<bool>& visited_vertices, vector<int>& connect_component) {
		visited_vertices[start] = true;
		for (int i = 0; i < number_of_vertices; ++i) {
			if (mtx[start][i] && !visited_vertices[i]) {
				dfs(number_of_vertices, mtx, i, visited_vertices, connect_component);
			}
		}
		connect_component.push_back(start);
	}

	auto get_time_of_strong_connect_components(int number_of_vertices, bool** mtx, vector<vector<int>>& all_components) {

		vector<bool> visited_vertices;
		vector<int> all_vertices;
		bool** rev_mtx = get_revesed_orgraph(number_of_vertices, mtx);

		for (int i = 0; i < number_of_vertices; ++i) {
			visited_vertices.push_back(false);
		}

		auto start = chrono::steady_clock::now();

		for (int i = 0; i < number_of_vertices; ++i) {
			if (!visited_vertices[i]) {
				dfs(number_of_vertices, rev_mtx, i, visited_vertices, all_vertices);
			}
		}

		visited_vertices.clear();
		for (int i = 0; i < number_of_vertices; ++i) {
			visited_vertices.push_back(false);
		}

		vector<int> start_vertices = all_vertices;
		reverse(start_vertices.begin(), start_vertices.end());

		for (int i = 0; i < start_vertices.size(); ++i) {
			if (!visited_vertices[start_vertices[i]] && find(all_vertices.begin(), all_vertices.end(), start_vertices[i]) != all_vertices.end()) {

				vector<int> component;
				dfs(number_of_vertices, mtx, start_vertices[i], visited_vertices, component);
				for (int j = 0; j < component.size(); ++j) {
					all_vertices.erase(find(all_vertices.begin(), all_vertices.end(), component[j]));
				}
				all_components.push_back(component);
			}
		}
		auto end = chrono::steady_clock::now();
		return chrono::duration_cast<chrono::microseconds>(end - start);
	}

public:
	GRAPH(int number) {
		if (number < 1) {
			throw 1;
		}
		else {
			number_of_vertices = number;
			orgraph = new bool*[number_of_vertices];
			number_of_edges = generate_orgraph(number_of_vertices, orgraph);
			alghotithm_time = get_time_of_strong_connect_components(number_of_vertices, orgraph, strong_connect_components);
		}
	}

	~GRAPH() {
		for (int i = 0; i < number_of_vertices; ++i) {delete[] orgraph[i];}
		delete[] orgraph;
		for (int i = 0; i < strong_connect_components.size(); ++i) {
			strong_connect_components[i].clear();
		}
		strong_connect_components.clear();
	}

	int get_number_of_vertices() {return number_of_vertices;}

	int get_number_of_edges() {return number_of_edges;}

	bool** get_orgraph() {return orgraph;}

	chrono::microseconds get_alghorithm_time_in_ms() {return alghotithm_time;}
};

int main() {

	ofstream output_file("output.txt");

	try {
		int number_of_vertices;
		cout << "Input number of vertices: ";
		cin >> number_of_vertices;
		GRAPH *orgraph = new GRAPH(number_of_vertices);
		output_file << (*orgraph).get_number_of_vertices() << "\n" <<
			(*orgraph).get_number_of_edges() << "\n" <<
			(*orgraph).get_alghorithm_time_in_ms().count() << "\n";
		delete orgraph;
	}
	catch (int i) {
		if (i == 1) {
			cout << "Wrong number of vertices\n";
		}
	}
	return 0;
}
