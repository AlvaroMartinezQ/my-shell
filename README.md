<!-- PROJECT SHIELDS -->
[![Contributors][contributors-shield]][contributors-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/AlvaroMartinezQ/my-shell">
    <img src="imgs/logo.jpg" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">MY SHELL</h3>

  <p align="center">
    Shell program to run a modified Linux shell.
    <br />
    <a href="https://github.com/AlvaroMartinezQ/my-shell/blob/master/Memoria.pdf"><strong>Explore the docs (spanish)»</strong></a>
    <br />
    <br />
    <a href="https://github.com/AlvaroMartinezQ/my-shell">View Demo</a>
    ·
    <a href="https://github.com/AlvaroMartinezQ/my-shell/issues">Report Bug</a>
    ·
    <a href="https://github.com/AlvaroMartinezQ/my-shell/issues">Request Feature</a>
  </p>
</p>

<!-- TABLE OF CONTENTS -->
## Table of Contents

* [About the Project](#about-the-project)
  * [Built With](#built-with)
* [Getting Started](#getting-started)
  * [Prerequisites](#prerequisites)
  * [Installation](#installation)
* [Usage](#usage)
* [Contributing](#contributing)
* [License](#license)
* [Contact](#contact)
* [Acknowledgements](#acknowledgements)

<!-- ABOUT THE PROJECT -->
## About The Project

C program to run a modified Linux shell. This project was developed for the subject of operative systems for the degree of Computer Science Engineering of the Rey Juan Carlos University, 2019 - 2020 course. For any further explanation, please read the docs, included as a PDF file in the repo.

### Built With

* [Linux](https://www.linux.org/)
* [C language](https://en.wikipedia.org/wiki/C_(programming_language))
* [University code](https://www.urjc.es/etsii)

<!-- GETTING STARTED -->
## Getting Started

To get a local copy up and running, follow these simple steps.

### Prerequisites

You should have installed:
* Linux
* C compiler (usually included in any Linux/Ubuntu distribution)
* Git, install it directly from core
```sh
sudo apt-get update
```
```sh
sudo apt-get install git
```

### Installation

1. Clone the repo
```sh
git clone https://github.com/AlvaroMartinezQ/my-shell.git
```

And that's all, simple installation.

<!-- USAGE EXAMPLES -->
## Usage

The new way:

1. Use the new Makefile to compile running the following command
```sh
make
```

2. Use the Makefile to run the shell
```sh
make run 
```

The old way:

1. Compile
```sh
gcc myshell.c libparser.a -static -o myshell 
```

2. Run
```sh
./myshell
```

<!-- CONTRIBUTING -->
## Contributing

Contributions are closed as this is a project which has been already rated. But any suggestions are welcomed via email, as well as if you find a bug, it would be highly appreciated if it's reported!

<!-- LICENSE -->
## License

This project is under the GNU GENERAL PUBLIC LICENSE, but please, keep in mind that we do not make ourselves responsible for any possible academic copy or copies of this project. The code has been uploaded for a learning purpose only. If any academic entity gets in contact with us and we are able to find similarities in the code, as this is a common learning project, it would be reported to the academic organization and actions will be under their discretion.

<!-- CONTACT -->
## Contact

* Alvaro Martinez Quiroga - alvaroo2302@gmail.com
* Patricia Tarazaga Cozas - patriciaxadm@gmail.com

<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements

* [University Rey Juan Carlos](https://www.urjc.es/)
* [Github README template](https://github.com/othneildrew/Best-README-Template)

[contributors-shield]: https://img.shields.io/github/contributors/AlvaroMartinezQ/my-shell.svg?style=flat-square
[contributors-url]: https://github.com/AlvaroMartinezQ/my-shell/graphs/contributors
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=flat-square&logo=linkedin&colorB=555
[linkedin-url]: https://es.linkedin.com/in/alvaro-martinez-quiroga-profile
