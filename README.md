# 🏟️ Jersey DevOps

**Full-stack jersey store with a C++ API, React frontend, ML integration, and monitoring.**

A sports jersey marketplace built to demonstrate end-to-end engineering: a C++17 OOP backend exposed as a REST API, a React frontend, a Python ML microservice for recommendations and pricing, and a complete Docker + Kubernetes + Prometheus/Grafana deployment pipeline.

---

## ✨ Features

- **Jersey management** — full CRUD, shopping cart, checkout, invoice generation
- **ML features** — personalized recommendations, demand forecasting, AI-suggested discounts
- **Monitoring** — Prometheus scraping + Grafana dashboards
- **Docker Compose** — one command spins up all 5 services
- **Kubernetes manifests** — Deployments, Services, Ingress, HPA autoscaling
- **CI/CD** — GitHub Actions build/test/deploy pipeline

---

## 🏗️ Architecture

See [`docs/architecture.md`](docs/architecture.md) for the full diagram. High level:

```
React Frontend  →  C++ REST API  →  Python ML Microservice
                         ↓
                  Prometheus → Grafana
```

The frontend only ever talks to the C++ API. The C++ API calls the ML service internally for recommendations, demand forecasts, and discount suggestions — the frontend never calls the ML service directly.

---

## 🛠️ Tech Stack

| Layer | Tech |
|---|---|
| Backend       | C++17, cpp-httplib |
| Frontend      | React, Vite |
| ML Service    | Python, FastAPI, XGBoost, scikit-learn |
| Monitoring    | Prometheus, Grafana |
| Orchestration | Docker, Docker Compose, Kubernetes |
| CI/CD         | GitHub Actions |

---

## 📁 Project Structure

```
jersey-devops/
├── jersey-store/              # C++17 OOP backend + REST API
│   ├── src/
│   │   ├── inventory/         # Jersey, FootballJersey, CricketJersey, Inventory
│   │   ├── customer/          # Person, Fan
│   │   ├── order/             # Order
│   │   ├── cart/              # Cart
│   │   ├── analytics/         # Analytics (sales + recommender)
│   │   ├── auth/              # AuthManager
│   │   ├── api/               # ApiServer, JsonBuilder, Serializer, httplib.h
│   │   ├── main.cpp           # CLI entry point
│   │   └── server.cpp         # REST API entry point
│   ├── Dockerfile
│   └── Makefile
│
├── jersey-frontend-v2-ml/     # React + Vite frontend
│   ├── src/
│   │   ├── api/client.js      # Axios client (C++ API + ML API)
│   │   ├── components/        # Navbar, JerseyCard
│   │   ├── pages/             # Home, Shop, Cart, Admin, Login, Register
│   │   └── context/           # AuthContext
│   ├── Dockerfile
│   └── nginx.conf
│
├── jersey-ml/                 # Python FastAPI ML microservice
│   ├── api/main.py            # /recommend /demand /discount endpoints
│   ├── generate_data.py       # synthetic training data
│   ├── train.py               # trains all 3 models
│   └── Dockerfile
│
├── monitoring/
│   ├── prometheus.yml
│   └── grafana/provisioning/
│
├── k8s/                        # Kubernetes manifests
│   ├── cpp-api-deployment.yaml
│   ├── frontend-deployment.yaml
│   ├── hpa.yaml
│   ├── ingress.yaml
│   └── monitoring.yaml
│
├── .github/workflows/ci-cd.yml
├── docker-compose.yml
├── docs/
│   ├── architecture.md
│   └── uml.md
├── .gitignore
├── LICENSE
└── README.md
```

---

## 🚀 Setup Instructions

### Prerequisites
- Docker & Docker Compose
- Node.js 20+ (only if running the frontend outside Docker)
- g++ / make (only if building the C++ API outside Docker)
- Python 3.11+ (only if running the ML service outside Docker)

### Run everything with Docker

```bash
docker compose up --build
```

### Service URLs

| Service | URL |
|---|---|
| Frontend  | http://localhost:8081 |
| C++ API   | http://localhost:8080 |
| ML API    | http://localhost:8001 |
| Prometheus| http://localhost:9090 |
| Grafana   | http://localhost:3001 (admin / admin123) |

### Demo accounts

| Role | Username | Password |
|---|---|---|
| Admin    | `admin` | `admin123` |
| Customer | `tarun` | `tarun123` |

---

## 📡 API Endpoints

### C++ Core API (`:8080`)
```
POST   /auth/login
POST   /auth/register
GET    /jerseys
GET    /jerseys/football
GET    /jerseys/cricket
GET    /jerseys/search?q=
GET    /jerseys/filter?min=&max=
GET    /jerseys/:id
POST   /jerseys                 (admin)
PUT    /jerseys/:id/stock       (admin)
DELETE /jerseys/:id             (admin)
GET    /cart/:fanId
POST   /cart/:fanId/add
DELETE /cart/:fanId/:jerseyId
POST   /cart/:fanId/checkout
GET    /admin/analytics         (admin)
GET    /admin/lowstock          (admin)
GET    /health
```

### ML Microservice (`:8001`)
```
GET  /recommend/{fanId}?top_n=&already_bought=
POST /demand     { jersey_id, days_to_event }
POST /discount   { jersey_id, stock_level, days_since_launch, season_active }
GET  /health
```

---

## 📊 Monitoring

- `monitoring/prometheus.yml` scrapes `/health` on both the C++ API and ML API every 15s
- Grafana auto-provisions Prometheus as a datasource on startup (`monitoring/grafana/provisioning/`)
- Add custom dashboards via Grafana UI and export their JSON into `monitoring/grafana/dashboards/` to version-control them

---

## 🧱 OOP Design

The C++ core is built around a polymorphic class hierarchy:

| Class | Responsibility |
|---|---|
| `Jersey` (abstract) | Base type — pure virtual `getDiscount()`, `display()`, `getCategory()` |
| `FootballJersey` / `CricketJersey` | Concrete subclasses with sport-specific discount logic |
| `Person` (abstract) → `Fan` | Customer profile, preference tags, purchase history |
| `Inventory` | Owns all `Jersey` objects via `map<int, unique_ptr<Jersey>>`, exposes search/filter |
| `Cart` | Composition — holds `CartItem`s, computes GST + totals, generates invoices |
| `Order` | Ties a `Fan` + `Inventory` together for checkout |
| `Analytics` | Sales tracking + frequency-based recommender (pre-ML fallback) |
| `ApiServer` | Wires everything into REST endpoints via cpp-httplib |

Concepts demonstrated: inheritance, polymorphism, abstraction, encapsulation, composition, and use of STL containers (`map`, `vector`, `unique_ptr`).

See [`docs/uml.md`](docs/uml.md) for the full class diagram.

---

## 🧑‍💻 Development Notes

**Run the frontend locally (outside Docker):**
```bash
cd jersey-frontend-v2-ml
npm install
npm run dev
```

**Build the C++ API manually:**
```bash
cd jersey-store
make server
./jersey-server 8080
```

**Run the ML service manually:**
```bash
cd jersey-ml
pip install -r requirements.txt
python generate_data.py
python train.py
uvicorn api.main:app --port 8001
```

**Extending ML features:** add a new endpoint in `jersey-ml/api/main.py`, retrain via `train.py` if it needs a new model, then call it from the C++ `ApiServer` or directly from the frontend's `api/client.js`.

---

## 🗺️ Roadmap

- [ ] Export and commit Grafana dashboard JSON
- [ ] Expand ML features (churn prediction, size recommendation)
- [ ] Deploy to a managed Kubernetes cluster (GKE/EKS)
- [ ] Harden CI/CD — push images to a registry, add automated rollout

---

## 📄 License

MIT License — see [LICENSE](LICENSE).

**Author:** Kappala Hema Tarun
