## IRC-Bot
This project aims to be an easily extendable C++ IRC-bot.

### Features:
* Extensions are dynamic libraries, which are loaded automatically.
* Extensions can be loaded, unloaded and updated while the bot is running.
* Settings fetched and updated to a sqlite3 database.

### To compile the bot:
* Compile the core of the bot with ```make```.
* Compile the bot extension ```./manage.sh build bot```.
 * Otherwise the bot will just connect and ping out.

### TODO:
* Interacting with SQLite3 database - Still needs some work.
* Owners, administrators and authentication stuff.
* Downloadable extensions.
* Fix bugs.

