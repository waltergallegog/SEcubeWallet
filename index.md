---
layout: default
---

[Authors Bio](another-page)

_Digital passwords are among the most sensitive data one person or organization can have. They protect private information ranging from personal files to bank accounts, and if stolen, the consequences can be catastrophic. In this work we develop a Qt application, SECubeWallet, able to secure any number of passwords, relaying on the encryption power provided by the SEcube platform, which uses a software password together with Hardware keys existing in a small and portable device, making the hacking of the secured data virtually impossible. The main additional feature offered is a realistic password strength estimation using the zxcvbn dropbox project._

# [](#header-1)SECUBE OPEN SECURITY PLATFORM

The core of our implementation is the SEcube Platform.
>The SEcube (Secure Environment cube) Open Security Platform is an open source security oriented hardware and software platform, designed and constructed with ease of integration and service-orientation in mind. The hardware part of the platform was originally designed by Blu5 Group1, whereas the software libraries stem from a strong cooperation among international research institutions.

>The software libraries and design environment allow developers who are not willing or able to produce the security APIs and protocols themselves to exploit the ready functions provided (currently as APIs and soon as services) within the SEcube platform and experience the platform as a high-security black box.

#### [](#header-4)Hardware device family
The SECube device family comprise three major members:
*   **The Chip**, named SEcube Chip, or simply SEcube.
*   **The Development Board**, named SEcube DevKit.
*   **The USB Stick**, named USEcube Stick.

In our project we used the Development Board provided by the professors, already configured to our needs. For thefinal user the board would be of course too inconvenient to carry, and instead the USEcube Stick would be preferred.

#### [](#header-4)L2 Security APIs

From the user/developer point of view, the APIs have been implemented targeting two nested environments depending on where physically the code runs:
*   **Device-Side**, including the libraries of basic functionalities that are executed on the embedded processor of the SEcube-based hardware device.
*   **Host-Side**, containing libraries of functions executed on the host PC and interface functions for calling services and processes residing on the embedded processor of the SEcube device.

From the architectural point of view, the Host-Side Libraries have been implemented targeting 4 hierarchical abstraction levels, and namely:
*   **Level 0**: Communication Protocol and Provisioning APIs.
*   **Level 1**: Basic Security APIs (Level1 Host-Side L1).
*   **Level 2**: Intermediate Security APIs (Level2 L2).
*   **Level 3**: Advanced Security APIs (Level3 L3).

L2 can be considered as the merge of two projects: SEfile, concerning data at rest, and SElink, concerning instead data at motion.

>SEfile targets any user that, by moving inside a secure environment, wants to perform basic operation on regular files. It must be pointed out that all encryption functionalities are demanded to the secure device in their entirety. In addition, SEfile does not expose to the host device details about what, or where it is reading/writing data: thus, the host OS, which might be untrusted, is totally unaware of what it is writing

# [](#header-1)QT

Our application’s graphical interface was developed using the Qt framework.
>Qt is a cross-platform application development framework for desktop, embedded and mobile. Supported Platforms include Linux, OS X, Windows, VxWorks, QNX, Android, iOS, BlackBerry, Sailfish OS and others. Qt is not a programming language on its own. It is a framework written in C++. A preprocessor, the MOC (Meta-Object Compiler), is used to extend the C++ language with features like signals and slots. Before the compilation step, the MOC parses the source files written in Qt-extended C++ and generates standard compliant C++ sources from them. Thus the framework itself and applications/libraries using it can be compiled by any standard compliant C++ compiler like Clang, GCC, ICC, MinGW and MSVC

#### [](#header-4)Qt creator

For development we used the IDE Qt Creator.
>Qt Creator provides a cross-platform, complete integrated development environment (IDE) for application developers to create applications for multiple desktop, embedded, and mobile device platforms, such as Android and iOS. It is available for Linux, macOS and Windows operating systems

Using Qt creator allowed us to implement easily and fast a robust and elegant solution to our problem. Thanks to the multitude of functions specially dedicated to the management of SQlite databases and display tables, we were able to focus or attention to solve other problems and to extend the functionalities of the project.


# [](#header-1)ZXCVBN: Realistic Password Strength Estimation

An important feature to final users is the possibility to realistically estimate how strong a password is, i.e., how hard could it be for hackers to crack it, as there is no point in using our system to protect weak passwords, that could be easily guessed with brute force attacks.

As it is out of our expertise to develop a reliable function to make this estimation, we preferred to use a trusted project coming from dropbox hackweek event in 2012. The estimator called zxcvbn was originally written in JavaScript aiming for an easy integration with multiple web browsers and OS.

>For over 30 years, password requirements and feedback have largely remained a product of LUDS: counts of lowerand uppercase letters, digits and symbols. LUDS remains
ubiquitous despite being a conclusively burdensome and ineffective security practice. zxcvbn is an alternative password strength estimator that is small, fast, and crucially no harder than LUDS to adopt. Using leaked passwords, we compare its estimations to the best of four modern guessing attacks and show it to be accurate and conservative at low magnitudes, suitable for mitigating online attacks. We find 1.5 MB of compressed storage is sufficient to accurately estimate the best-known guessing attacks up to 105 guesses, or 104 and 103 guesses, respectively, given 245 kB and 29 kB. zxcvbn can be adopted with 4 lines of code and downloaded in seconds. It runs in milliseconds and works as-is on web, iOS and Android.

![](https://raw.githubusercontent.com/duverleygrajales/SEcubeWallet/gh-pages/assets/images/password_strength.png)

To put it in other words, the authors of the project argue that a password like _correcthorsebatterystaple_ (a nonsense English phrase) is more strong than a password like _Tr0ub4dour&3_, even if the former does not have any upper cases or numbers, and the latter seems more complicated.

>People of course choose patterns dictionary words, spatial patterns like _qwerty_, _asdf_ or _zxcvbn_, repeats like _aaaaaaa_, sequences like _abcdef_ or _654321_, or some combination of the above. For passwords with uppercase letters, odds are its the first letter thats uppercase. Numbers and symbols are often predictable as well: _l33t_ speak (3 for e, 0 for o, @ or 4 for a), years, dates, zip codes, and so on. As a result, simplistic strength estimation gives bad advice. Without checking for common patterns, the practice of encouraging numbers and symbols means encouraging passwords that might only be slightly harder for a computer to crack, and yet frustratingly harder for a human to remember. _xkcd_ nailed it.

#### [](#header-4)zxcvbn and Qt integration
First of all, we modified the _zxcvbn makefile_, to remove the unnecessary files and to understand how to proceed with the integration. It is necessary to compile said makefile in order to generate the _dictionary_ used for passwords comparisons and entropy estimation. When we realized integration of the makefile with Qt was really cumbersome because Qt uses a qmake instead of a regular make, we decided to generate the source dictionary offline. Having the source dictionary ready, we included and modified the files _zxcvbn.cpp_ and _zxcvbn.h_ into our project. The modifications were necessary to use C++ libraries instead of C libraries.
Finally, with the files already integrated on the project, we just had to use function _ZxcvbnMatch(password)_ to estimate the entropy and give the user a metric about how strong their password is.

# [](#header-1)Implementation

The basic idea behind our project is: We use the SEfile APIs to encrypt a file containing the passwords the user wants to protect. Said file is encrypted by the SEcube device connected to the computer, and can only be decrypted later if the same device is connected. The encryption/decryption can be done in any computer where an appropriate version of Qt is installed.

A detailed description of the usage workflow, functionalities and components is given below:
1.  The user connects and mounts the SEcube device to the host computer.
2.  The user executes our application **SEcubeWallet**
3.  The application ask for a master password, which has been previously configured into the SEcube device (in our case is **test**). The entered password has to coincide with the stored one to allow the login.

After access is granted, the GUI shown in the figure allows the user to:
4.  Create a new Wallet.
5.  Cypher the wallet, close it and delete the regular (insecure) file.
6.  Open an existent cyphered Wallet.
7.  Add/Edit a new entry to an existing Wallet. The user fills the required fields, and for the password there is strength measure feature.
8.  Delete an existing entry.
9.  Launch a web browser and open the selected domain.
10.  Edit the environment variables.
11.  Safe application close.

![](https://raw.githubusercontent.com/duverleygrajales/SEcubeWallet/gh-pages/assets/images/gui.png)

#### [](#header-4)Device connection

The SEcube hardware devices SEcube DevKit and USEcube Stick can be easily connected to any pc host via USB ports. They appear in the system as a storage volume and need to be mounted in order to be used.

#### [](#header-4)SEcubeWallet execution

The application is executed as any standard QT application. The host pc requires then to have an appropriate version of Qt or Qt libraries installed and configured. To this moment, the version of Qt used for development is Qt 5.8.0.

#### [](#header-4)Secure login

For our project we used the Login dialogue from the example Secure Text Editor provided alongside with the L2 User Manual and shown in the figure. This dialogue opens a
secure connection with the SEcube device and as a result a _se3 session_ pointer _s_ is created. This pointer contains all the information that let the system acknowledge which board is connected and if the user has successfully logged in. After the board is connected and the user is correctly logged in, the _secure init()_ function is issued.

![](https://raw.githubusercontent.com/duverleygrajales/SEcubeWallet/gh-pages/assets/images/login.png)

#### [](#header-4)Create a new Wallet

Each wallet is stored as a table in a sqlite database, and is protected independently by a password. In this way the application supports multiple users working with the same SEcube device. This can be useful for companies that wish to protect the passwords of all of their employees, but keep their passwords separated from each other. The user enters the desired name and path using the dialogue shown in the figure, based on the example _Secure Text Editor_.

![](https://raw.githubusercontent.com/duverleygrajales/SEcubeWallet/gh-pages/assets/images/create.png)

The database is created with a _QSqlQuery_ and the function _query.prepare_ that allow us to execute sqlite commands. For the managing of the tables, we use a _QSqlTableModel_ which simplifies the process by wrapping the sqlite functions. To display it in a _QTableView_, we connect the model to the view table, resulting in the flow _Sqltable->Model->TableView_.

#### [](#header-4)Cypher and close

After the user has finished creating/editing the wallet, the Sqlite Database file containing the passwords needs to be cyphered using the SEcube device. This is possible using the _secure open_, _secure write_ and _secure close_ functions from the SEfile API. As a result, an encrypted version of the database file is created. Our implementation is based on the example _Secure Text Editor_.
After the file is encrypted we can proceed to close the database, delete the existing _plain_ sqlite file, which is non secure, and close the _tableView_ displaying the information. In this way, we are sure the passwords are safely stored in the encrypted file, and can only be read by the owner.

#### [](#header-4)Open existing wallet

To open cyphered wallets, the _Secure File Dialogue_ from the _Secure Text Editor_ example is used. This dialogue shows the user wallets in the current working directory that where previously encrypted using the connected SEcube device, as in the figure. The user then selects one of the wallets to be opened.

![](https://raw.githubusercontent.com/duverleygrajales/SEcubeWallet/gh-pages/assets/images/open.png)

With the selected cyphered file, and using the SEfile API functions _secure open_, _secure read_ and _secure close_, a Sqlite file containing the stored passwords is created. Then a database connection to the file is opened and the entries are displayed in a Table View.

#### [](#header-4)Add/Edit entry to wallet

To Add or Edit a new entry to the wallet, the dialogue shown in figure was implemented.

![](https://raw.githubusercontent.com/duverleygrajales/SEcubeWallet/gh-pages/assets/images/add.png)

In this dialogue the user enters the required fields _username_, _domain_ and _password_. The password has to be entered twice, to be sure is correct. Additionally, using **zxcvbn**, we estimate its strength and show it to the user with a progress bar. To edit, the user selects an entry, and clicks the _edit_ button. The same dialogue is called, but with a different constructor, that initialize the fields with the current data.
To insert data to the tables, we use again our model and the function _model->insertRecord(Position, record)_, and to submit the changes _model->submitAll()_.

#### [](#header-4)Delete an existing entry

To delete an entry, the user just has to select one of the entries in the table and click the _delete_ button. After this a confirmation prompt is shown warning the user an entry is about to be deleted forever. if the user clicks _ok_, the application proceeds to eliminate the data using the _removeRow_ function from our model.

#### [](#header-4)Launch web browser

If the user wishes to open in a web browser one of the domains in the wallet, it selects an entry and clicks the Launch button. Thanks to Qt function _QDesktopServices::OpenUrl(Qurl(string))_ we only had to modify the domain field to include _http://_ in the beginning of the string.

#### [](#header-4)Edit environment variables

This dialogue is intended for advanced users who would like to choose the algorithm and keys used for encryption in the SEcube device. Right now, with the device configuration provided by the professors, only one option is available.

#### [](#header-4)Safe Close

When the user decides to close the application, several actions need to be performed.
*    Encrypt file
*    Delete Sqlite file
*    Close the connection to the database
This is done by reusing the functions already developed for the cipher and close button.

# [](#header-1)Improvements and Future work

#### [](#header-4)Autofill login forms in website
When the user clicks the launch button, not only the selected domain is opened, also, the login forms are autofilled using the stored username and password. We did not implemented this functionality as it requires HTML/CSS knowledge an it is out of the scope of the course.

#### [](#header-4)OS integration
Some OS, like linux distributions running the KDE Plasma Desktop already offer a wallet solution integrated in the system. Merging our project with these existing wallets could be a great feature for some users.

#### [](#header-4)Browser integration
The vast majority of users store their passwords within their preferred web browser, alongside other sensitive information like browsing history and bookmarks. Using the capabilities of the SEcube security platform to protect all that data while maintaining ease of use and transparency to the final user would be a great advance. We did not investigate further in that direction, but we guess it is possible by developing a complement for the most popular web browsers (Firefox, Chrome, Opera) to run together with the main application.
