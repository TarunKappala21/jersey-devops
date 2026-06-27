# ── Stage 1: Build ────────────────────────────
FROM gcc:13 AS builder

WORKDIR /app

# Copy source
COPY src/ ./src/
COPY Makefile .

# Install make
RUN apt-get update && apt-get install -y make && rm -rf /var/lib/apt/lists/*

# Build only the server binary
RUN make server

# ── Stage 2: Runtime ──────────────────────────
FROM debian:bookworm-slim

WORKDIR /app

# Copy binary from builder
COPY --from=builder /app/jersey-server .

# Create data + invoices dirs
RUN mkdir -p data invoices

EXPOSE 8080

CMD ["./jersey-server", "8080"]
