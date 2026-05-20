#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <fstream>
#include <algorithm>

using namespace std;

struct Edge {
    int to;
    int weight;
};

typedef pair<int, int> pii;

void printPath(vector<int>& parent, int node) {
    if (node == -1)
        return;

    printPath(parent, parent[node]);
    cout << char(node + 'A') << " ";
}

int main() {
    int V, E;

    cout << "=======================================\n";
    cout << "SISTEM OPTIMASI PENGIRIMAN PAKET\n";
    cout << "ALGORITMA DIJKSTRA\n";
    cout << "=======================================\n\n";

    cout << "Masukkan jumlah node: ";
    cin >> V;

    vector<vector<Edge>> graph(V);

    cout << "Masukkan jumlah edge: ";
    cin >> E;

    cout << "\nInput data edge\n";
    cout << "Format: asal tujuan bobot\n";
    cout << "Contoh: A B 8\n\n";

    for (int i = 0; i < E; i++) {
        char uChar, vChar;
        int w;

        cout << "Edge ke-" << i + 1 << ": ";
        cin >> uChar >> vChar >> w;

        int u = uChar - 'A';
        int v = vChar - 'A';

        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }

    char sourceChar;
    cout << "\nMasukkan node sumber: ";
    cin >> sourceChar;

    int source = sourceChar - 'A';

    vector<int> dist(V, INT_MAX);
    vector<int> parent(V, -1);

    priority_queue<pii, vector<pii>, greater<pii>> pq;

    dist[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        int currentDist = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (currentDist > dist[u])
            continue;

        for (auto edge : graph[u]) {
            int v = edge.to;
            int weight = edge.weight;

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;

                pq.push({dist[v], v});
            }
        }
    }

    ofstream outputFile("hasil.txt");

    cout << "\n=======================================\n";
    cout << "HASIL SHORTEST PATH\n";
    cout << "=======================================\n\n";

    outputFile << "HASIL SHORTEST PATH\n\n";

    for (int i = 0; i < V; i++) {
        cout << "Tujuan Node " << char(i + 'A') << endl;
        cout << "Jarak Minimum : " << dist[i] << endl;
        cout << "Rute          : ";

        printPath(parent, i);

        cout << endl;
        cout << "--------------------------------\n";

        outputFile << "Tujuan Node " << char(i + 'A') << endl;
        outputFile << "Jarak Minimum : " << dist[i] << endl;
        outputFile << "Rute          : ";

        vector<int> path;
        int temp = i;

        while (temp != -1) {
            path.push_back(temp);
            temp = parent[temp];
        }

        reverse(path.begin(), path.end());

        for (int node : path) {
            outputFile << char(node + 'A') << " ";
        }

        outputFile << endl;
        outputFile << "--------------------------------\n";
    }

    outputFile.close();

    cout << "\nData hasil juga disimpan ke file hasil.txt\n";

    return 0;
}
