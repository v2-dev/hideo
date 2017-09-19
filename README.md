# Hideo


### What is it?
**Hideo is a lightweight, multithreaded HTTP/1.1 web server with static image content adaptation using WURFL library.**

### What is this project intended for?

This project is didactical, meaning that used alone is useless. But it can be improved and it is a good starting point if you want to program your own webserver or if you want to study network programming using C language and Berkeley sockets API.

### How does it work?
When a clients requests for the server's homepage it will be redirected to index.html where are present thumbs of all the images ready for download.
When the user clicks on one image based on the client browser HTTP requests, the image will be resized and converted, using WURFL library. The converted image will be then saved on an internal cache satisfying successive request, lowering the system overhead.
