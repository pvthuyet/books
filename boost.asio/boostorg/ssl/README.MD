## [How to generate key for SSL Server-Client](https://stackoverflow.com/questions/6452756/exception-running-boost-asio-ssl-example)
* Generate a private key
```
openssl genrsa -des3 -out server.key 1024
```
* Generate Certificate signing request
```
openssl req -new -key server.key -out server.csr
```
* Sign certificate with private key
```
openssl x509 -req -days 3650 -in server.csr -signkey server.key -out server.crt
```
* Remove password requirement (needed for example)
```
copy server.key server.key.secure
openssl rsa -in server.key.secure -out server.key
```
* Generate dhparam file
```
openssl dhparam -out dh2048.pem 2048
```
* server.cpp
```
context_.use_certificate_chain_file("server.crt"); 
context_.use_private_key_file("server.key", boost::asio::ssl::context::pem);
context_.use_tmp_dh_file("dh2048.pem");
```
* client.cpp
```
ctx.load_verify_file("server.crt");
```
