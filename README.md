# ContainerIDO
The container code for the final project in the scope of my IoT class.

> TEAM 3

#### Team Members

- Jenna Lee
- Raph

---

# SETUP

###### Prerequisites

> Make sure you have downloaded and configured the mosquitto library. More information can be found [here]().

1. Clone project

```git
git clone https://github.com/Raaphe/ContainerIDO.git
```

2. Correct Constants

> In the `container.c` file, change the `PI_IP` constant to your pi's IP as well as the `PORT` constant to whatever TCP port you're running your TCP socket off of.

3. Compile and Run

```bash
cd /ContainerIDO
make
```

---

#### Note

The required certificates and keys can be found under `/auth`.
