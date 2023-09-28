# QT6Database

A C++ Database GUI created using Qt creator(also Qt designer embedded in it) QT 6 Widgets, CMake and sqlite. To compile it either install qt creator and select CMakeLists.txt while opening a project OR use CMAKE and ninja.

The interface is intuitive. The database is about storing data about clients (name,year and additional notes) and activities(Activity name , date, additional notes) assosciated with them. Some examples are hard-coded and one button press presents them for ready reference and ease of understanding.

The data is stored in a hierarchical tree- like format thus distiguishing this application from generic DBMS applications as it supports assosciating a client with countless activities while each activity can have sub-activities and those can have their sub-actvities! The depth is limitless(ofc limited by your computer).

The application is fully functional and supports one-click save to a sqlite database at the location of your choice. you can also load the created sqlite databases back into the application also with a single shortcut key without a hitch and any loss of data.

Sample Image:
[Screenshot (146)](https://github.com/ForMyDreamsLetsGo/QT6Database/assets/97535853/b925b337-113a-4f12-94a0-3065883bf552)
![Screenshot (145)](https://github.com/ForMyDreamsLetsGo/QT6Database/assets/97535853/fac01e78-c00c-4672-82b9-9b24b5cfaf7c)

![Screenshot (144)](https://github.com/ForMyDreamsLetsGo/QT6Database/assets/97535853/f9530b51-6b4d-4715-9b3d-47132f06ad48)
