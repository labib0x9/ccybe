# docker build -t c-server .


# -------- BUILD --------
FROM debian:stable-slim AS builder

RUN apt-get update && \
    apt-get install -y build-essential cmake python3 git && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Initialize submodules inside container
# RUN git submodule update --init --recursive

# Build llhttp
WORKDIR /app/third_party/llhttp
RUN mkdir -p build && cd build && cmake .. && make

# Build server
WORKDIR /app
RUN make

# -------- RUNTIME --------
FROM debian:stable-slim

WORKDIR /app

# Copy only binary
COPY --from=builder /app/server .

# Create non-root user
RUN useradd -m appuser
USER appuser

EXPOSE 8080

CMD ["./server"]