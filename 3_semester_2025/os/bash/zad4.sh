#!/bin/bash
CAT_URL=$(curl -s https://api.thecatapi.com/v1/images/search | jq -r '.[0].url')
curl -s "$CAT_URL" -o cat.jpg
catimg cat.jpg
echo
echo "Losowy żart Chucka Norrisa:"
JOKE=$(curl -s https://api.chucknorris.io/jokes/random | jq -r '.value')
echo "$JOKE"
