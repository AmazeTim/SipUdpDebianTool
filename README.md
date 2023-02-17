<!-- GETTING STARTED -->
## Getting Started

Followling utilize [Docker](https://www.docker.com/) to construct a [Debian 10](https://www.debian.org/releases/index.zh-tw.html) environment. Attached dockerfile could help you to setup and test the sipping tool based on UDP.

### Prerequisites

Setting up
* Clone the repo
  ```sh
  git clone https://github.com/AmazeTim/SipUdpDebianTool.git
  ```
* Download the image
  ```sh
  docker pull debian:10-slim
  ```

### Installation

1. Run the container
   ```sh
   docker run -name debian-container -p debian:10-slim
   ```
2. Install NPM packages
   ```sh
   npm install
   ```
3. Enter your API in `config.js`
   ```js
   const API_KEY = 'ENTER YOUR API';
   ```


<!-- USAGE EXAMPLES -->
## Usage

Use this space to show useful examples of how a project can be used. Additional screenshots, code examples and demos work well in this space. You may also link to more resources.

_For more examples, please refer to the [Documentation](https://example.com)_

<p align="right">(<a href="#readme-top">back to top</a>)</p>

