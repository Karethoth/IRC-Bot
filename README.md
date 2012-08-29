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
* Owners, administrators and authentication stuff. - Works pretty well.
* Downloadable extensions.
* Communication interface for sending and receiving data between extensions.
* Fix bugs.

