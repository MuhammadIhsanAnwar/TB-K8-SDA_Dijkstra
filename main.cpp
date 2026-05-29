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
#include <cctype>

using namespace std;

struct Edge {
    int to;
    int weight;
};

typedef pair<int, int> pii;

// Fungsi untuk menulis visualisasi HTML
void writeVisualization(const vector<vector<Edge>>& graph, int V, int source, int target,
                        const vector<int>& dist, const vector<int>& parent) {
    string outDir = "output";
    system(("mkdir \"" + outDir + "\" >nul 2>&1").c_str());
    string filename = outDir + "\\visualization.html";
    ofstream viz(filename);
    if (!viz.is_open()) return;

    int maxFiniteDist = 0;
    for (int d : dist) {
        if (d != INT_MAX) {
            maxFiniteDist = max(maxFiniteDist, d);
        }
    }

    int xScale = 54;
    int ySpread = 120;
    vector<pair<int, int>> coords(V);
    for (int i = 0; i < V; ++i) {
        int yBand = (i % 5) - 2;
        if (dist[i] == INT_MAX) {
            coords[i] = {maxFiniteDist * xScale + 320 + (i % 3) * 120, yBand * ySpread};
        } else {
            coords[i] = {dist[i] * xScale, yBand * ySpread};
        }
    }

    viz << R"HTML(<!doctype html>
<html lang="id">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Visualisasi Rute Dijkstra</title>
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700;800&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg-1: #07111f;
            --bg-2: #10213d;
            --bg-3: #0c172b;
            --panel: rgba(10, 18, 34, 0.74);
            --panel-border: rgba(255, 255, 255, 0.12);
            --text: #f4f7ff;
            --muted: #a7b3d4;
            --accent: #64d2ff;
            --accent-2: #ffb703;
            --accent-3: #ff4d6d;
            --accent-4: #2ec4b6;
        }

        * {
            box-sizing: border-box;
        }

        body {
            margin: 0;
            min-height: 100vh;
            font-family: "Inter", "Segoe UI", Tahoma, Arial, sans-serif;
            color: var(--text);
            background: 
                radial-gradient(circle at top left, rgba(100, 210, 255, 0.22), transparent 30%),
                radial-gradient(circle at top right, rgba(255, 183, 3, 0.14), transparent 24%),
                radial-gradient(circle at 75% 20%, rgba(255, 77, 109, 0.14), transparent 22%),
                linear-gradient(160deg, var(--bg-1), var(--bg-2) 52%, var(--bg-3));
        }

        body::before {
            content: '';
            position: fixed;
            inset: 0;
            pointer-events: none;
            background-image:
                linear-gradient(rgba(255, 255, 255, 0.03) 1px, transparent 1px),
                linear-gradient(90deg, rgba(255, 255, 255, 0.03) 1px, transparent 1px);
            background-size: 72px 72px;
            mask-image: radial-gradient(circle at center, rgba(0, 0, 0, 0.45), transparent 82%);
        }

        .shell {
            min-height: 100vh;
            padding: 22px;
            display: flex;
            justify-content: center;
            align-items: stretch;
            position: relative;
            z-index: 1;
        }

        .panel {
            width: min(1260px, 100%);
            background: var(--panel);
            border: 1px solid var(--panel-border);
            border-radius: 28px;
            box-shadow: 0 28px 80px rgba(0, 0, 0, 0.46), inset 0 1px 0 rgba(255, 255, 255, 0.05);
            backdrop-filter: blur(18px);
            overflow: hidden;
        }

        .hero {
            padding: 28px 28px 20px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.08);
            background: linear-gradient(135deg, rgba(100, 210, 255, 0.13), rgba(255, 183, 3, 0.08));
            position: relative;
            overflow: hidden;
        }

        .hero::after {
            content: '';
            position: absolute;
            inset: auto -120px -150px auto;
            width: 280px;
            height: 280px;
            border-radius: 50%;
            background: radial-gradient(circle, rgba(255, 183, 3, 0.22), transparent 68%);
            filter: blur(4px);
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
            position: relative;
            z-index: 1;
        }

        h1 {
            margin: 14px 0 8px;
            font-size: clamp(28px, 3vw, 44px);
            line-height: 1.05;
            letter-spacing: -0.03em;
            position: relative;
            z-index: 1;
        }

        .subtitle {
            margin: 0;
            color: var(--muted);
            max-width: 760px;
            line-height: 1.6;
            position: relative;
            z-index: 1;
        }

        .legend {
            display: flex;
            flex-wrap: wrap;
            gap: 12px;
            margin-top: 18px;
            position: relative;
            z-index: 1;
        }

        .chip {
            display: inline-flex;
            align-items: center;
            gap: 10px;
            padding: 10px 14px;
            border-radius: 999px;
            background: rgba(255, 255, 255, 0.07);
            border: 1px solid rgba(255, 255, 255, 0.09);
            color: var(--text);
            font-size: 14px;
            box-shadow: 0 10px 24px rgba(0, 0, 0, 0.12);
        }

        .dot {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            box-shadow: 0 0 0 4px rgba(255, 255, 255, 0.05);
        }

        .dot.teal { background: #2ec4b6; }
        .dot.blue { background: #4cc9f0; }
        .dot.yellow { background: #ffb703; }
        .dot.red { background: #ff4d6d; }

        .hero-grid {
            margin-top: 18px;
            display: grid;
            grid-template-columns: repeat(3, minmax(0, 1fr));
            gap: 12px;
        }

        .stat {
            padding: 14px 16px;
            border-radius: 18px;
            background: rgba(255, 255, 255, 0.06);
            border: 1px solid rgba(255, 255, 255, 0.08);
            backdrop-filter: blur(12px);
        }

        .stat span {
            display: block;
            color: var(--muted);
            font-size: 12px;
            text-transform: uppercase;
            letter-spacing: 0.08em;
        }

        .stat strong {
            display: block;
            margin-top: 8px;
            font-size: 18px;
            color: var(--text);
        }

        .canvas-wrap {
            padding: 18px;
        }

        #mynetwork {
            width: 100%;
            height: 680px;
            border-radius: 24px;
            border: 1px solid rgba(255, 255, 255, 0.11);
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

        .controls {
            padding: 16px 28px;
            display: flex;
            gap: 12px;
            align-items: center;
            flex-wrap: wrap;
            border-bottom: 1px solid rgba(255, 255, 255, 0.08);
            background: rgba(255, 255, 255, 0.03);
        }

        select, button {
            padding: 10px 14px;
            border-radius: 12px;
            border: 1px solid rgba(255, 255, 255, 0.16);
            background: rgba(255, 255, 255, 0.08);
            color: var(--text);
            font-size: 14px;
            cursor: pointer;
            transition: transform 180ms ease, background 180ms ease, border-color 180ms ease;
        }

        select:hover, button:hover {
            transform: translateY(-1px);
            border-color: rgba(100, 210, 255, 0.45);
        }

        button {
            background: linear-gradient(135deg, var(--accent), #8be9ff);
            color: #02101e;
            font-weight: 600;
            box-shadow: 0 12px 28px rgba(100, 210, 255, 0.2);
        }

        #routeInfo {
            padding: 14px 28px 0;
            color: var(--text);
            font-size: 14px;
            min-height: 38px;
        }

        @keyframes fadeInUp {
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }

        @keyframes floatBadge {
            0%, 100% {
                transform: translateY(0);
            }
            50% {
                transform: translateY(-3px);
            }
        }
    </style>
    <script src="https://unpkg.com/vis-network/standalone/umd/vis-network.min.js"></script>
</head>
<body>
    <div class="shell">
        <section class="panel">
            <header class="hero">
                <span class="badge">Visualisasi Graph Algoritma Dijkstra</span>
                <h1>Optimasi Pengiriman Paket Antar Wilayah</h1>
                <p class="subtitle">Pilih tujuan untuk melihat rute terpendek, jarak, dan animasi pengiriman paket secara real-time.</p>
                <div class="legend">
                    <span class="chip"><span class="dot teal"></span>Lokasi Awal</span>
                    <span class="chip"><span class="dot blue"></span>Node Biasa</span>
                    <span class="chip"><span class="dot red"></span>Lokasi Tujuan</span>
                    <span class="chip"><span class="dot yellow"></span>Rute Aktif</span>
                </div>
                <div class="hero-grid">
                    <div class="stat">
                        <span>Awal Pengiriman</span>
                        <strong>)HTML" << char('A' + source) << R"HTML(</strong>
                    </div>
                    <div class="stat">
                        <span>Tujuan Awal Visual</span>
                        <strong>)HTML" << char('A' + target) << R"HTML(</strong>
                    </div>
                    <div class="stat">
                        <span>Satuan Jarak</span>
                        <strong>Km</strong>
                    </div>
                </div>
            </header>
            <div class="controls">
                <label style="color:var(--muted)">Pilih Tujuan:</label>
                <select id="destSelect">)HTML";

    if (target != source) {
        viz << "<option value=\"" << target << "\" selected>" << char('A' + target) << "</option>";
    }

    for (int i = 0; i < V; ++i) {
        if (i == source || i == target) continue;
        viz << "<option value=\"" << i << "\">" << char('A' + i) << "</option>";
    }
    viz << R"HTML(</select>
                <button onclick="updateRoute()">🔍 Lihat Rute</button>
            </div>
            <div id="routeInfo"></div>
            <div class="canvas-wrap">
                <div id="mynetwork"></div>
            </div>
            <div class="footer">
                <span><strong>Tip:</strong> geser/zoom untuk eksplorasi.</span>
            </div>
        </section>
    </div>
    <script>
        // Inject Data Dijkstra
        const dijkstraDist = [)HTML";

    for (int i = 0; i < V; ++i) 
        viz << (dist[i] == INT_MAX ? -1 : dist[i]) << (i < V - 1 ? ", " : "");
    viz << R"HTML(];
        const dijkstraParent = [)HTML";

    for (int i = 0; i < V; ++i) 
        viz << parent[i] << (i < V - 1 ? ", " : "");
    viz << R"HTML(];
        const sourceNode = )HTML" << source << R"HTML(;

        // Generate Nodes
        const nodes = [)HTML";

    for (int i = 0; i < V; ++i) {
        string col = (i == source ? "#2ec4b6" : "#4cc9f0");
        viz << "            { id:" << i << ", label:'" << char('A' + i) << "', x:" << coords[i].first 
            << ", y:" << coords[i].second << ", color:{background:'" << col 
            << "', border:'#ffffff'}, title:'Jarak dari sumber: " 
            << (dist[i] == INT_MAX ? "∞ (tidak terjangkau)" : to_string(dist[i]) + " Km") + "' }";
        if (i < V - 1) viz << ",";
        viz << "\n";
    }
    viz << R"HTML(        ];

        // Generate Edges
        const edges = [)HTML";

    for (int u = 0; u < V; ++u) {
        for (auto &e : graph[u]) {
            int v = e.to;
            if (u < v) {
                viz << "            { id:'" << u << "_" << v << "', from:" << u 
                    << ", to:" << v << ", label:'" << e.weight << " Km', width:3 },";
            }
        }
    }
    viz << R"HTML(        ];

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
                font: {
                    size: 22,
                    face: 'Segoe UI',
                    color: '#f7f9ff'
                },
                shadow: {
                    enabled: true,
                    color: 'rgba(0, 0, 0, 0.35)',
                    size: 18
                }
            },
            edges: {
                smooth: {
                    enabled: true,
                    type: 'dynamic',
                    roundness: 0.35
                },
                color: {
                    color: 'rgba(138, 180, 255, 0.9)',
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
            physics: false
        };

        const network = new vis.Network(container, data, options);
        let routeTimer = null;
        let packageId = 'pkg';

        function getShortestPath(target) {
            const path = [];
            let curr = target;
            while (curr !== -1) {
                path.unshift(curr);
                curr = dijkstraParent[curr];
            }
            return path;
        }

        function clearTimer() {
            if (routeTimer) {
                cancelAnimationFrame(routeTimer);
                routeTimer = null;
            }
        }

        function updateRoute() {
            clearTimer();
            const target = parseInt(document.getElementById('destSelect').value);
            const info = document.getElementById('routeInfo');
            const targetNode = nodes.find(n => n.id === target);

            if (dijkstraDist[target] === -1) {
                info.innerHTML = `❌ <b>${targetNode.label}</b> tidak terjangkau dari sumber.`;
                return;
            }

            const path = getShortestPath(target);
            const dist = dijkstraDist[target];
            info.innerHTML = `✅ Rute: ${path.map(id => nodes.find(n => n.id === id).label).join(' → ')} | <b>Jarak: ${dist} Km</b>`;

            // Reset all nodes & edges
            nodes.forEach(n => {
                const col = (n.id === sourceNode ? '#2ec4b6' : (n.id === target ? '#ff4d6d' : '#4cc9f0'));
                data.nodes.update({ id: n.id, color: { background: col, border: '#ffffff' } });
            });

            edges.forEach(e => {
                data.edges.update({ id: e.id, width: 3, color: { color: 'rgba(138, 180, 255, 0.9)' } });
            });

            // Highlight path
            for (let i = 1; i < path.length; i++) {
                const u = Math.min(path[i - 1], path[i]);
                const v = Math.max(path[i - 1], path[i]);
                data.edges.update({ id: `${u}_${v}`, width: 6, color: { color: '#ffb703' } });
                data.nodes.update({ id: path[i], color: { background: '#ffb703', border: '#ffffff' } });
            }

            data.nodes.update({ id: path[0], color: { background: '#2ec4b6', border: '#ffffff' } });
            data.nodes.update({ id: target, color: { background: '#ff4d6d', border: '#ffffff' } });

            // Animate package
            if (!data.nodes.get(packageId)) {
                data.nodes.add({
                    id: packageId,
                    label: '',
                    shape: 'dot',
                    size: 18,
                    color: { background: '#ffffff', border: '#ff4d6d' },
                    physics: false
                });
            }

            if (path.length === 1) {
                const pos = network.getPosition(path[0]);
                data.nodes.update({ id: packageId, x: pos.x, y: pos.y });
                return;
            }

            const segmentDuration = 850;
            let segmentIndex = 0;
            let startedAt = null;

            const move = (timestamp) => {
                if (!startedAt) {
                    startedAt = timestamp;
                }

                const fromNode = path[segmentIndex];
                const toNode = path[segmentIndex + 1];
                const fromPos = network.getPosition(fromNode);
                const toPos = network.getPosition(toNode);
                const progress = Math.min(1, (timestamp - startedAt) / segmentDuration);

                data.nodes.update({
                    id: packageId,
                    x: fromPos.x + (toPos.x - fromPos.x) * progress,
                    y: fromPos.y + (toPos.y - fromPos.y) * progress
                });

                if (progress >= 1) {
                    segmentIndex++;
                    startedAt = timestamp;
                    if (segmentIndex >= path.length - 1) {
                        segmentIndex = 0;
                    }
                }

                routeTimer = requestAnimationFrame(move);
            };

            routeTimer = requestAnimationFrame(move);
        }

        requestAnimationFrame(() => {
            network.fit({ animation: { duration: 900, easingFunction: 'easeInOutQuad' } });
            updateRoute();
        });
    </script>
</body>
</html>)HTML";

    viz.close();
#ifdef _WIN32
    system(("cmd /c start \"\" \"" + filename + "\"").c_str());
#endif
}

int main() {
    int V, E;

    cout << "=======================================\n";
    cout << "SISTEM OPTIMASI PENGIRIMAN PAKET\n";
    cout << "ALGORITMA DIJKSTRA\n";
    cout << "=======================================\n\n";
    cout << "Program mencari jalur terpendek dari lokasi awal ke tujuan pengiriman paket.\n";
    cout << "Semua titik lokasi di peta akan diberi huruf: A, B, C, ...\n\n";

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

    auto readLocationIndex = [&](const string& prompt) {
        while (true) {
            cout << prompt;
            string token;
            cin >> token;

            if (token.size() == 1 && isalpha(static_cast<unsigned char>(token[0]))) {
                char letter = static_cast<char>(toupper(static_cast<unsigned char>(token[0])));
                int index = letter - 'A';
                if (index >= 0 && index < V) {
                    return index;
                }
            }

            cout << "Lokasi harus berupa huruf A sampai " << char('A' + V - 1) << ".\n";
        }
    };

    auto readWeight = [&]() {
        while (true) {
            cout << "Bobot Jarak: ";
            string token;
            cin >> token;

            bool numeric = !token.empty() && all_of(token.begin(), token.end(), [](unsigned char ch) {
                return isdigit(ch);
            });

            if (numeric) {
                int weight = stoi(token);
                return weight;
            }

            cout << "Bobot jarak harus angka non-negatif.\n";
        }
    };

    for (int i = 0; i < E; i++) {
        cout << "Masukkan ruas jalan ke-" << i + 1 << " (contoh: A B 8):\n";
        int u = readLocationIndex("Lokasi Asal: ");
        int v = readLocationIndex("Lokasi Tujuan: ");
        int w = readWeight();

        graph[u].push_back({v, w});
        graph[v].push_back({u, w});

        cout << "";
    }

    char sourceChar;
    cout << "\nMasukkan lokasi awal pengiriman (huruf): ";
    cin >> sourceChar;
    sourceChar = static_cast<char>(toupper(static_cast<unsigned char>(sourceChar)));

    int source = sourceChar - 'A';
    if (source < 0 || source >= V) {
        cout << "Lokasi awal tidak valid. Gunakan huruf antara A dan " << char('A' + V - 1) << ".\n";
        return 1;
    }

    char targetChar;
    cout << "Masukkan lokasi tujuan pengiriman paket (huruf): ";
    cin >> targetChar;
    targetChar = static_cast<char>(toupper(static_cast<unsigned char>(targetChar)));

    int target = targetChar - 'A';
    if (target < 0 || target >= V) {
        cout << "Lokasi tujuan tidak valid. Gunakan huruf antara A dan " << char('A' + V - 1) << ".\n";
        return 1;
    }

    // Algoritma Dijkstra
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

    // Rekonstruksi rute untuk tujuan utama
    vector<int> deliveryPath;
    if (dist[target] != INT_MAX) {
        int temp = target;
        while (temp != -1) {
            deliveryPath.push_back(temp);
            temp = parent[temp];
        }
        reverse(deliveryPath.begin(), deliveryPath.end());
    }

    // Output ke console
    cout << "\nRute optimal pengiriman paket dari " << char(source + 'A') 
         << " ke " << char(target + 'A') << ":\n";
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

    // Simpan ke file hasil.txt
    ostringstream ss_mkdir2;
    ss_mkdir2 << "mkdir \"output\" >nul 2>&1";
    system(ss_mkdir2.str().c_str());
    
    string outFile = string("output\\") + "hasil.txt";
    ofstream outputFile(outFile);

    cout << "\n=======================================\n";
    cout << "HASIL RUTE TERPENDEK\n";
    cout << "=======================================\n\n";

    outputFile << "HASIL RUTE TERPENDEK\n\n";
    cout << "Lokasi | Jarak Terpendek | Rute\n";
    cout << "--------------------------------" << string(20, '-') << "\n";

    for (int i = 0; i < V; i++) {
        cout << char(i + 'A') << "    | ";
        if (dist[i] == INT_MAX) {
            cout << "INF          | Tidak terjangkau\n";
        } else {
            cout << dist[i] << "            | ";
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

        // Tulis ke file
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

    // Generate visualisasi dan buka otomatis
    writeVisualization(graph, V, source, target, dist, parent);
    cout << "\nHasil perhitungan juga telah disimpan ke file hasil.txt\n";

    return 0;
}