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

### [](#header-3)Header 3

```js
// Javascript code with syntax highlighting.
var fun = function lang(l) {
  dateformat.i18n = require('./lang/' + l)
  return true;
}
```

```ruby
# Ruby code with syntax highlighting
GitHubPages::Dependencies.gems.each do |gem, version|
  s.add_dependency(gem, "= #{version}")
end
```

#### [](#header-4)Header 4

*   This is an unordered list following a header.
*   This is an unordered list following a header.
*   This is an unordered list following a header.

##### [](#header-5)Header 5

1.  This is an ordered list following a header.
2.  This is an ordered list following a header.
3.  This is an ordered list following a header.

###### [](#header-6)Header 6

| head1        | head two          | three |
|:-------------|:------------------|:------|
| ok           | good swedish fish | nice  |
| out of stock | good and plenty   | nice  |
| ok           | good `oreos`      | hmm   |
| ok           | good `zoute` drop | yumm  |

### There's a horizontal rule below this.

* * *

### Here is an unordered list:

*   Item foo
*   Item bar
*   Item baz
*   Item zip

### And an ordered list:

1.  Item one
1.  Item two
1.  Item three
1.  Item four

### And a nested list:

- level 1 item
  - level 2 item
  - level 2 item
    - level 3 item
    - level 3 item
- level 1 item
  - level 2 item
  - level 2 item
  - level 2 item
- level 1 item
  - level 2 item
  - level 2 item
- level 1 item

### Small image

![](https://assets-cdn.github.com/images/icons/emoji/octocat.png)

### Large image

![](https://guides.github.com/activities/hello-world/branching.png)


### Definition lists can be used with HTML syntax.

<dl>
<dt>Name</dt>
<dd>Godzilla</dd>
<dt>Born</dt>
<dd>1952</dd>
<dt>Birthplace</dt>
<dd>Japan</dd>
<dt>Color</dt>
<dd>Green</dd>
</dl>

```
Long, single-line code blocks should not wrap. They should horizontally scroll if they are too long. This line should be long enough to demonstrate this.
```

```
The final element.
```
