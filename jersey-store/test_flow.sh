#!/bin/bash

# 1. Login as Tarun (fan)
echo "Logging in as Tarun..."
FAN_TOKEN=$(curl -s -X POST http://localhost:8080/auth/login \
    -H "Content-Type: application/json" \
    -d '{"user":"tarun","pass":"tarun123"}' | jq -r '.data.token')

echo "Fan token: $FAN_TOKEN"

# 2. Add jersey to cart
echo "Adding jersey to cart..."
curl -s -X POST http://localhost:8080/cart/101/add \
    -H "Authorization: Bearer $FAN_TOKEN" \
    -H "Content-Type: application/json" \
    -d '{"jId":7,"quantity":1}' | jq

# 3. View cart
echo "Viewing cart..."
curl -s http://localhost:8080/cart/101 \
    -H "Authorization: Bearer $FAN_TOKEN" | jq

# 4. Checkout
echo "Checking out..."
curl -s -X POST http://localhost:8080/cart/101/checkout \
    -H "Authorization: Bearer $FAN_TOKEN" | jq

# 5. Login as Admin
echo "Logging in as Admin..."
ADMIN_TOKEN=$(curl -s -X POST http://localhost:8080/auth/login \
    -H "Content-Type: application/json" \
    -d '{"user":"admin","pass":"admin123"}' | jq -r '.data.token')

echo "Admin token: $ADMIN_TOKEN"

# 6. Analytics dashboard
echo "Fetching analytics..."
curl -s http://localhost:8080/admin/analytics \
    -H "Authorization: Bearer $ADMIN_TOKEN" | jq

# 7. Low stock alerts
echo "Fetching low stock alerts..."
curl -s http://localhost:8080/admin/lowstock \
    -H "Authorization: Bearer $ADMIN_TOKEN" | jq
