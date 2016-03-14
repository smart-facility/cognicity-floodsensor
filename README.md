# cognicity-floodsensor
IoT flood height sensor code

# Installation
First clone this repository and install dependencies:
```shell
git clone https://github.com/smart-facility/cognicity-floodsensor
cd cognicity-floodsensor
npm install
```

Set up a device and associated certificate in AWS IoT (CLI or console) and then copy the private key (as *private.pem.key*) and certificate (as *certificate.pem.crt*) under *awsCerts/*.

Set the configuration options as described in [config.js](config.js)

# Running it
Run this (note it needs to be run as root to access the GPIO memory):
```shell
sudo node device.js
```
Ctrl-C to quit.
