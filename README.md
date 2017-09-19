# Hideo


### What is it?
**Hideo is a lightweight, multithreaded HTTP/1.1 web server with static image content adaptation using [WURFL InFuze module](https://www.scientiamobile.com/page/wurfl-infuze) developed by ScentiaMobile .**

### What is this project intended for?

This project is didactical, meaning that used alone is useless. But it can be improved and it is a good starting point if you want to program your own webserver or if you want to study network programming using C language and Berkeley sockets API.

### How does it work?
When a clients requests for the server's homepage it will be redirected to index.html, where there are thumbs of all images ready for download.
When the user clicks on an image, based on the HTTP header requests, the image will be resized and converted. The optimal device resolution is given by a WURFL module, based on the client's User Agent. The converted image will be then saved on an internal cache to satisfy successive requests, lowering the system overhead and improving response time.

### Features

* Pre-threaded server architecture with static worker threads (number of threads configurable).
* LRU Cache.
* Asynchronous and fast multilivel logging system.
* Robust: no memory leaks, no crashes, no race conditions.

### Prerequisites

* WURFL InFuze module
