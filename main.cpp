#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <set>

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

// Write a simple HTML visualization (uses vis-network CDN) and open it.
void writeVisualization(const vector<vector<Edge>>& graph, int V, int source, int target, const vector<int>& path) {
        string outDir = "output";
        ostringstream ss_mkdir;
        ss_mkdir << "mkdir \"" << outDir << "\" >nul 2>&1";
        string mkdirCmd = ss_mkdir.str();
        system(mkdirCmd.c_str());
        string filename = outDir + "\\visualization.html";
        ofstream viz(filename);
    if (!viz.is_open()) return;

    int cols = max(2, (int)floor(sqrt(V)));
    int rows = (V + cols - 1) / cols;
    int spacing = 160;
    vector<pair<int,int>> coords(V);
    int xOffset = (cols - 1) * spacing / 2;
    int yOffset = (rows - 1) * spacing / 2;
    for (int i = 0; i < V; ++i) {
        int row = i / cols;
        int col = i % cols;
        coords[i] = {col * spacing - xOffset, row * spacing - yOffset};
    }

    set<pair<int,int>> pathEdges;
    for (size_t i = 1; i < path.size(); ++i) {
        int u = min(path[i - 1], path[i]);
        int v = max(path[i - 1], path[i]);
        pathEdges.insert({u, v});
    }

        viz << R"HTML(<!doctype html>
<html lang="id">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Visualisasi Jaringan Dijkstra</title>
    <style>
        :root {
            color-scheme: dark;
            --bg-1: #07111f;
            --bg-2: #0d1b31;
            --panel: rgba(10, 18, 34, 0.76);
            --panel-border: rgba(255, 255, 255, 0.1);
            --text: #eef3ff;
            --muted: #aab7d6;
            --accent: #64d2ff;
            --accent-2: #ffb703;
            --accent-3: #f15bb5;
        }

        * { box-sizing: border-box; }

        body {
            margin: 0;
            min-height: 100vh;
            font-family: "Segoe UI", Tahoma, Arial, sans-serif;
            color: var(--text);
            background:
                radial-gradient(circle at top left, rgba(100, 210, 255, 0.24), transparent 32%),
                radial-gradient(circle at top right, rgba(241, 91, 181, 0.2), transparent 28%),
                linear-gradient(160deg, var(--bg-1), var(--bg-2));
        }

        .shell {
            min-height: 100vh;
            padding: 24px;
            display: flex;
            justify-content: center;
            align-items: stretch;
        }

        .panel {
            width: min(1260px, 100%);
            background: var(--panel);
            border: 1px solid var(--panel-border);
            border-radius: 28px;
            box-shadow: 0 28px 80px rgba(0, 0, 0, 0.45);
            backdrop-filter: blur(18px);
            overflow: hidden;
        }

        .hero {
            padding: 26px 28px 18px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.08);
            background: linear-gradient(135deg, rgba(100, 210, 255, 0.12), rgba(241, 91, 181, 0.09));
        }

        .badge {
            display: inline-flex;
            align-items: center;
            gap: 8px;
            padding: 8px 14px;
            border-radius: 999px;
            background: rgba(255, 255, 255, 0.08);
            color: var(--muted);
            font-size: 13px;
            letter-spacing: 0.08em;
            text-transform: uppercase;
            animation: floatBadge 3s ease-in-out infinite;
        }

        h1 {
            margin: 14px 0 8px;
            font-size: clamp(28px, 3vw, 44px);
            line-height: 1.05;
        }

        .subtitle {
            margin: 0;
            color: var(--muted);
            max-width: 760px;
            line-height: 1.6;
        }

        .legend {
            display: flex;
            flex-wrap: wrap;
            gap: 12px;
            margin-top: 18px;
        }

        .chip {
            display: inline-flex;
            align-items: center;
            gap: 10px;
            padding: 10px 14px;
            border-radius: 999px;
            background: rgba(255, 255, 255, 0.06);
            border: 1px solid rgba(255, 255, 255, 0.08);
            color: var(--text);
            font-size: 14px;
        }

        .dot {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            box-shadow: 0 0 0 4px rgba(255, 255, 255, 0.05);
        }

        .dot.blue { background: var(--accent); }
        .dot.gold { background: var(--accent-2); }
        .dot.pink { background: var(--accent-3); }

        .canvas-wrap {
            padding: 18px;
        }

        #mynetwork {
            width: 100%;
            height: 680px;
            border-radius: 24px;
            border: 1px solid rgba(255, 255, 255, 0.1);
            background:
                radial-gradient(circle at center, rgba(255, 255, 255, 0.04), transparent 55%),
                linear-gradient(180deg, rgba(8, 15, 29, 0.92), rgba(11, 20, 40, 0.98));
            box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.05);
            opacity: 0;
            transform: translateY(14px);
            animation: fadeInUp 900ms ease forwards;
        }

        .footer {
            display: flex;
            justify-content: space-between;
            gap: 16px;
            flex-wrap: wrap;
            padding: 0 28px 24px;
            color: var(--muted);
            font-size: 14px;
        }

        .footer strong {
            color: var(--text);
            font-weight: 600;
        }

        @keyframes fadeInUp {
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }

        @keyframes floatBadge {
            0%, 100% { transform: translateY(0); }
            50% { transform: translateY(-3px); }
        }
    </style>
    <script type="text/javascript" src="https://unpkg.com/vis-network/standalone/umd/vis-network.min.js"></script>
</head>
<body>
    <div class="shell">
        <section class="panel">
            <header class="hero">
                <span class="badge">Visualisasi Rute Dijkstra</span>
                <h1>Visualisasi Jaringan Lokasi Interaktif</h1>
                <p class="subtitle">Antarmuka menampilkan lokasi dan ruas jalan dengan animasi, sehingga jalur terpendek dapat dilihat lebih jelas.</p>
                <div class="legend">
                    <span class="chip"><span class="dot blue"></span>Lokasi</span>
                    <span class="chip"><span class="dot gold"></span>Bobot ruas jalan</span>
                    <span class="chip"><span class="dot pink"></span>Rute terpendek aktif</span>
                </div>
            </header>
            <div class="canvas-wrap">
                <div id="mynetwork"></div>
            </div>
            <div class="footer">
                <span><strong>Tip:</strong> geser atau zoom untuk eksplorasi jaringan lokasi.</span>
                <span>File ini dibuat otomatis setelah program dijalankan.</span>
            </div>
        </section>
    </div>
    <script>
        const nodes = [
)HTML";

    for (int i = 0; i < V; ++i) {
        viz << "      { id: " << i << ", label: '" << char('A' + i) << "', x: " << coords[i].first << ", y: " << coords[i].second;
        if (i == source) {
            viz << ", color: { background: '#2ec4b6', border: '#0cf5a7' }";
        } else if (i == target) {
            viz << ", color: { background: '#ff4d6d', border: '#ffb3c1' }";
        } else {
            viz << ", color: { background: '#4cc9f0', border: '#ffffff' }";
        }
        viz << " }";
        if (i < V - 1) viz << ",\n";
        else viz << "\n";
    }

    viz << "    ];\n\n    const edges = [\n";

    // To avoid duplicate edges (graph stored undirected as two entries), only add where u < v
    for (int u = 0; u < V; ++u) {
        for (auto &e : graph[u]) {
            int v = e.to;
            if (u < v) {
                string edgeId = to_string(u) + "_" + to_string(v);
                bool isPath = pathEdges.count({u, v}) > 0;
                int edgeLength = 100 + e.weight * 30;
                viz << "      { id: '" << edgeId << "', from: " << u << ", to: " << v << ", label: '" << e.weight << "', length: " << edgeLength << ", color: { color: '" << (isPath ? "#ffb703" : "rgba(138, 180, 255, 0.9)") << "' }, width: " << (isPath ? 5 : 3) << " }";
                viz << ",\n";
            }
        }
    }

        viz << R"HTML(    ];

        const routePath = [)HTML";
    for (size_t i = 0; i < path.size(); ++i) {
        viz << path[i];
        if (i + 1 < path.size()) viz << ", ";
    }
    viz << R"HTML(];

        const container = document.getElementById('mynetwork');
        const data = {
            nodes: new vis.DataSet(nodes),
            edges: new vis.DataSet(edges)
        };

        const options = {
            autoResize: true,
            interaction: {
                hover: true,
                tooltipDelay: 120,
                navigationButtons: true,
                keyboard: true
            },
            layout: {
                improvedLayout: false
            },
            nodes: {
                shape: 'dot',
                size: 28,
                borderWidth: 3,
                borderWidthSelected: 5,
                font: {
                    size: 22,
                    face: 'Segoe UI',
                    color: '#f7f9ff'
                },
                shadow: {
                    enabled: true,
                    color: 'rgba(0, 0, 0, 0.35)',
                    size: 18,
                    x: 0,
                    y: 6
                }
            },
            edges: {
                smooth: {
                    enabled: true,
                    type: 'dynamic',
                    roundness: 0.35
                },
                color: {
                    highlight: '#ffd166',
                    hover: '#ffffff'
                },
                font: {
                    align: 'middle',
                    size: 15,
                    color: '#f8f9ff',
                    strokeWidth: 5,
                    strokeColor: 'rgba(5, 10, 20, 0.84)'
                }
            },
            physics: {
                enabled: true,
                stabilization: {
                    iterations: 200,
                    fit: true
                },
                barnesHut: {
                    gravitationalConstant: -8000,
                    centralGravity: 0.15,
                    springLength: 180,
                    springConstant: 0.05,
                    damping: 0.25
                }
            }
        };

        const network = new vis.Network(container, data, options);

        function formatEdgeId(u, v) {
            return u < v ? `${u}_${v}` : `${v}_${u}`;
        }

        function animateRoute(positions) {
            if (routePath.length === 0) return;
            let step = 0;
            const interval = 700;

            data.nodes.add({
                id: 'package',
                label: '',
                shape: 'dot',
                size: 18,
                color: { background: '#ffffff', border: '#ff4d6d' },
                physics: false,
                x: positions[routePath[0]].x,
                y: positions[routePath[0]].y
            });

            const highlightNext = () => {
                if (step >= routePath.length) return;

                const nodeId = routePath[step];
                data.nodes.update({
                    id: nodeId,
                    color: { background: '#ffb703', border: '#ffffff' }
                });

                data.nodes.update({
                    id: 'package',
                    x: positions[nodeId].x,
                    y: positions[nodeId].y
                });

                if (step > 0) {
                    const prevId = routePath[step - 1];
                    const edgeId = formatEdgeId(prevId, nodeId);
                    data.edges.update({
                        id: edgeId,
                        color: { color: '#ffb703' },
                        width: 6
                    });
                }
                step += 1;
                setTimeout(highlightNext, interval);
            };

            highlightNext();
        }

        network.once('stabilized', () => {
            const positions = network.getPositions(routePath);
            animateRoute(positions);
            network.fit({ animation: { duration: 900, easingFunction: 'easeInOutQuad' } });
        });
    </script>
</body>
</html>)HTML";

    viz.close();

    // Try to open the file automatically on Windows
    #ifdef _WIN32
    ostringstream ss_start;
    ss_start << "cmd /c start \"\" \"" << filename << "\"";
    string cmd = ss_start.str();
    system(cmd.c_str());
    #endif
}

int main() {
    int V, E;

    cout << "=======================================\n";
    cout << "SISTEM OPTIMASI PENGIRIMAN PAKET\n";
    cout << "ALGORITMA DIJKSTRA\n";
    cout << "=======================================\n\n";
    cout << "Program mencari jalur terpendek dari lokasi awal ke tujuan pengiriman paket.\n";
    cout << "Semua titik lokasi di peta akan diberi huruf: A, B, C, ...\n";
    cout << "\n";
    cout << "Masukkan jumlah titik lokasi di peta: ";
    cin >> V;
    cout << "(Misal 4 berarti lokasi: A, B, C, D)\n\n";

    vector<vector<Edge>> graph(V);

    cout << "Masukkan jumlah ruas jalan yang menghubungkan lokasi: ";
    cin >> E;

    cout << "\nMasukkan data ruas jalan antar lokasi\n";
    cout << "Format setiap baris: asal tujuan bobot\n";
    cout << "Contoh: A B 8  -> ruas jalan dari A ke B dengan bobot 8\n\n";
    cout << "Gunakan huruf A.." << char('A' + V - 1) << " untuk semua lokasi.\n";
    cout << "(Misal jika jumlah lokasi 5, gunakan A..E; jika jumlah lokasi 6, gunakan A..F.)\n";
    cout << "Setiap baris berisi 3 nilai: lokasi asal, lokasi tujuan, dan bobot jarak.\n";
    cout << "Contoh input ruas jalan: A B 8\n\n";

    for (int i = 0; i < E; i++) {
        char uChar, vChar;
        int w;

        cout << "\nRuas jalan ke-" << i + 1 << ":\n";
        cout << "Lokasi Asal: ";
        cin >> uChar;
        cout << "Lokasi Tujuan: ";
        cin >> vChar;
        cout << "Bobot Jarak: ";
        cin >> w;

        int u = uChar - 'A';
        int v = vChar - 'A';

        if (u < 0 || u >= V || v < 0 || v >= V) {
            cout << "Input lokasi salah. Gunakan huruf antara A dan " << char('A' + V - 1) << ".\n";
            --i; // ulang input ruas jalan ini
            continue;
        }

        if (w < 0) {
            cout << "Bobot harus non-negatif. Masukkan ulang ruas jalan ini.\n";
            --i;
            continue;
        }

        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }

    char sourceChar;
    cout << "\nMasukkan lokasi awal pengiriman (huruf): ";
    cin >> sourceChar;

    int source = sourceChar - 'A';

    if (source < 0 || source >= V) {
        cout << "Lokasi awal tidak valid. Gunakan huruf antara A dan " << char('A' + V - 1) << ".\n";
        return 1;
    }

    char targetChar;
    cout << "Masukkan lokasi tujuan pengiriman paket (huruf): ";
    cin >> targetChar;

    int target = targetChar - 'A';
    if (target < 0 || target >= V) {
        cout << "Lokasi tujuan tidak valid. Gunakan huruf antara A dan " << char('A' + V - 1) << ".\n";
        return 1;
    }

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

    vector<int> deliveryPath;
    if (dist[target] != INT_MAX) {
        int temp = target;
        while (temp != -1) {
            deliveryPath.push_back(temp);
            temp = parent[temp];
        }
        reverse(deliveryPath.begin(), deliveryPath.end());
    }

    cout << "\nRute optimal pengiriman paket dari " << char(source + 'A') << " ke " << char(target + 'A') << ":\n";
    if (dist[target] == INT_MAX) {
        cout << "Tidak ada rute pengiriman paket yang terjangkau.\n";
    } else {
        cout << "Jarak: " << dist[target] << "\n";
        cout << "Rute: ";
        for (size_t i = 0; i < deliveryPath.size(); ++i) {
            cout << char(deliveryPath[i] + 'A');
            if (i + 1 < deliveryPath.size()) cout << "->";
        }
        cout << "\n";
    }

    ostringstream ss_mkdir2;
    ss_mkdir2 << "mkdir \"output\" >nul 2>&1";
    system(ss_mkdir2.str().c_str());
    string outFile = string("output\\") + "hasil.txt";
    ofstream outputFile(outFile);

    cout << "\n=======================================\n";
    cout << "HASIL RUTE TERPENDEK\n";
    cout << "=======================================\n\n";

    outputFile << "HASIL RUTE TERPENDEK\n\n";

    // Print a neat table header
    cout << "Lokasi | Jarak Terpendek | Rute\n";
    cout << "--------------------------------" << string(20, '-') << "\n";

    for (int i = 0; i < V; i++) {
        cout << char(i + 'A') << "    | ";
        if (dist[i] == INT_MAX) {
            cout << "INF          | Tidak terjangkau\n";
        } else {
            cout << dist[i] << "            | ";

            // Build path
            vector<int> path;
            int temp = i;
            while (temp != -1) {
                path.push_back(temp);
                temp = parent[temp];
            }
            reverse(path.begin(), path.end());

            for (size_t k = 0; k < path.size(); ++k) {
                cout << char(path[k] + 'A');
                if (k + 1 < path.size()) cout << "->";
            }
            cout << "\n";
        }

        cout << "--------------------------------\n";

        // Also write to file
        outputFile << "Tujuan Lokasi " << char(i + 'A') << endl;
        if (dist[i] == INT_MAX) {
            outputFile << "Jarak Terpendek : INF" << endl;
            outputFile << "Rute            : Tidak terjangkau" << endl;
        } else {
            outputFile << "Jarak Terpendek : " << dist[i] << endl;
            outputFile << "Rute            : ";

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
        }

        outputFile << "--------------------------------\n";
    }

    outputFile.close();

    // Generate visualization and open
    writeVisualization(graph, V, source, target, deliveryPath);

    cout << "\nHasil perhitungan juga telah disimpan ke file hasil.txt\n";

    return 0;
}
