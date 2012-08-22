## IRC-Bot
This project aims to be an easily extendable C++ IRC-bot.

### Features:
* Extensions are dynamic libraries, which are loaded automatically.
* Extensions can be loaded, unloaded and updated while the bot is running.

### To compile the bot:
* Compile the core of the bot with ```make```.
* Compile the bot extension ```./manage.sh build bot```.
 * Otherwise the bot will just connect and ping out.

### TODO:
* Fix bugs.
* Interacting with . SQLite3 database.
* Simple ini-file reader for settings that need to be changed often.
* Downloadable extensions.

