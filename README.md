<!-- GETTING STARTED -->
<a name="readme-top"></a>
# Sipping Tools
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

1. Go to the path
   ```sh
   cd /CloningPath/SipUdpDebianTool
   ```
2. Run the container for testing
   ```sh
   docker build -t debian/sipping_test .
   ```

<!-- USAGE EXAMPLES -->
## Usage
### Functional Requirement:
* A tool “sipping” 
* sipping supports three kinds of parameters: ip, port, max-retry 
* sipping can send UDP SIP Options message to specified ip/port 
* if max-retry is specified, sipping will try to resend message until reach limitation or reply is received. For example, if max-retry = 10, then sipping will resend message at most 10 times. The delay time between each retry should follow the [exponential backoff algorithm](https://www.baeldung.com/resilience4j-backoff-jitter) :
  * multiplier = 2
  * base = 500ms 
  * max wait_interval = 8s
* anytime when sipping receives a reply from testing target:
  * If status code is 200, show success message, exit program with exit code 0 
  * if status code is not 200, show error message, exit program with exit code 1
* if reach max-retry limitation, show timeout message, exit program with exit code 1
### Non-Functional Requirement:
* Can compile and run on Debian 10 
* Use [POSIX library](https://zh.wikipedia.org/zh-tw/C_POSIX_library) 
* Sniff network traffic during your testing and save as .pcap (you can use tshark, tcpdump or sngrep) 
<p align="right">(<a href="#readme-top">back to top</a>)</p>