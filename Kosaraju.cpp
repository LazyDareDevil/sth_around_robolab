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

	bool** generate_orgraph(int number_of_vertices, int number_of_edges) {

		bool** orgraph = new bool*[number_of_vertices];
		for (int i = 0; i < number_of_vertices; ++i) {
			orgraph[i] = new bool[number_of_vertices];
			for (int j = 0; j < number_of_vertices; ++j) {
				orgraph[i][j] = false;
			}
		}
		int row = -1, column = -1;
		for (int i = 0; i < number_of_edges; ++i) {
			row = rand() % number_of_vertices;
			column = rand() % number_of_vertices;
			if (orgraph[row][column] == false && row != column) orgraph[row][column] = true;
			else {
				while (orgraph[row][column] == true || row == column){
					row = rand() % number_of_vertices;
					column = rand() % number_of_vertices;
				}
				orgraph[row][column] = true;
			}
		}
		return orgraph;
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
	GRAPH(int number_v, int number_edg) {
		if (number_v < 2 || number_edg < number_v - 1 || number_edg > (number_v - 1)*number_v) {
			throw 1;
		}
		else {
			number_of_vertices = number_v;
			number_of_edges = number_edg;
			orgraph = generate_orgraph(number_of_vertices, number_of_edges);
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

	void print_graph() {
		for (int i = 0; i < number_of_vertices; ++i) {
			for (int j = 0; j < number_of_vertices; ++j) {
				cout << orgraph[i][j] << " ";
			}
			cout << "\n";
		}
	}

	int get_alghorithm_time_in_ms() {return alghotithm_time.count();}
};

int main(){
    ofstream output_file("output.txt");
    try {
		int number_of_vertices, number_of_edges;
		cout << "Input number of vertices: ";
		cin >> number_of_vertices;
		cout << "Input number of edges: ";
		cin >> number_of_edges;

		int time;
		GRAPH *orgraph = new GRAPH(number_of_vertices, number_of_edges); 
        time = (*orgraph).get_alghorithm_time_in_ms();
		delete orgraph;
        output_file << "vertices: " << number_of_vertices << "\nedges: " << number_of_edges << "\ntime: " << time << "\n";
	}
	catch (int i) {
		if (i == 1) {
			output_file << "Wrong number of vertices or edges\n";
		}
	}
    output_file.close();
    return 0;
}
