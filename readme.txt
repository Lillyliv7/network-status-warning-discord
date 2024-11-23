This is a lightweight utility to notify you over discord if one of your servers goes offline.

The program regularly pings every server in your servers list and if one times out (500ms timeout, editable in ping.c on line 37)
then it sends a ping over your discord websocket notifying you with the ip and a custom nickname. It sends another ping when the
server comes back online.

The program was built for Linux but also likely compiles for most unix based systems.

It was written in C without many protections so exposing your config file to the internet isn't
safe although with normal use it shouldn't be possible to compromise.

Supports ICMP pinging a host (syntax <host>@192.168.1.4) and connecting specific ports (syntax <host>@192.168.1.4:80).

Example usage:
<path-to-exe> settings.cfg

Settings file format:

WEBHOOK_URL=<URL HERE>
PING_ID=582273277544759296
SERVER=raspberrypi5@192.168.4.77
SERVER=searx@192.168.4.77:80

settings file must have no whitespace(lazy parser). WEBHOOK_URL and PING_ID must not be redefined but SERVER gets redefined for 
each server that you want to add.