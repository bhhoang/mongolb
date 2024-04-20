# mongolb
Load balancer that only look for record of a brand name

## How to run
1. Clone this repo
```sh
git clone https://github.com/bhhoang/mongolb 
```
```sh
cd mongolb
```
Then install the module
```sh
git submodule --init --recursive --force
```
Build the module
```
./build.sh
```
2. Make load balancer file
```sh
make
```
3. Make the client file
```sh
g++ src/client/client.cpp -o client
```
4. Create build for backend
```sh
mkdir -p backend/build
```
```sh
cd backend/build
```
5. Build backend
```sh
cmake -S .. -B .
make
```
6. You should run 3 backend for 3 localhost ports (these port should match with those in backendList.json) and 3 mongodb replicas
```sh
./mongo_app 8080 27017
./mongo_app 8081 27018
./mongo_app 8082 27019
```
7. Go back to the mongolb folder 
```sh
cd ../..
```
8. Run the load balancer file with a port
```sh
./mongolb 3000
```
9. Run the client with the port that match loadbalancer
```sh
./client 3000
```