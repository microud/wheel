#ss-auto-config

This is a tool for generating random port and password for shadowsocks.

##Features

- Generate the random port and password for shadowsocks
- Generate the QRcode of current configuration
- Control the shadowsocks service restart from new config file

##Requires

- shadowsocks server like `ssserver` or others(need change the code)
- base64 for encrypt the text to prepare for generating QRcode
- qrencode which can generate the QRcode
