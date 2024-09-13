This is a lightweight utility to notify you over discord if one of your servers goes offline.

The program regularly pings every server in your servers list and if one times out (500ms timeout, editable in ping.c on line 37)
then it sends a ping over your discord websocket notifying you with the ip and a custom nickname. It sends another ping when the
server comes back online.

The program was built for Linux but also likely compiles for most unix based systems.

It was written in C without many protections so exposing your config file to the internet isn't
safe although with normal use it shouldn't be possible to compromise.

Example usage:
<path-to-exe> settings.txt

Example settings file:

<discord webhook url here>~<@582273277544759296>~%s is dead %s~%s is alive %s~2~Raspberry pi 5@192.168.4.77~IMac@192.168.4.73

the webhook url is censored here.

The settings file is an array of strings seperated by a '~' symbol.

The 1st item is the discord webhook url
The 2nd item is your discord id surrounded by <@ > to ping you\
The 3rd item is the string to send when an ip dies, it needs to have 2 "%s"'s in it, the first being the ip name+address and the 2nd being your discord id
The 4th item is the same as the 3rd but sent when an ip comes back one
The 5th item is how many ip addresses you are tracking
The 6th item and up is formatted as <ip name>@<ip address> for each server you want to monitor
