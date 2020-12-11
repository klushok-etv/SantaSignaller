# Santa signaller


## setup
- clone this repository
- cd into the cloned repository
- execute ``` git submodule update --init``` to get the libraries
- flash the santaSignaler.ino to you ESP module
- All attached leds should light up, this indicates the wificonfig portal has been launched
- connect to the wifi network `Santas-little-helper-<UID_of_ESP>`
- visit 192.168.4.1 and enter the credentials from the network you'd like to connect to
- click `save` and `restart`
- The santa signaller will now restart, connect to your chosen NTP server and start flickering more enthousiasticly as christmass approaches!
