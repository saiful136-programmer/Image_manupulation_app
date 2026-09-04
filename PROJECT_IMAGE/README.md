# 🎨 Image Manipulation Software in C

![C](https://img.shields.io/badge/Language-C-blue.svg)
![OS](https://img.shields.io/badge/OS-Ubuntu%2FDebian-orange.svg)
![GUI](https://img.shields.io/badge/GUI-IUP-lightgrey.svg)

A graphical Image Manipulation Software built using the C programming language and the IUP toolkit. This application allows users to open 24-bit uncompressed BMP images, perform manual pixel manipulations (such as grayscale, blurring, cropping, and flipping), and save the modified results.

## 👨‍💻 Author

- **Name:** Hasnath Abdullah Akanda
- **Course/ID:** [Final Lab Project]
- **GitHub:** [@hasnath1](https://github.com/hasnath1)

---

## ⚙️ System Requirements

Ensure your environment meets the following dependencies before building:

- **OS:** Ubuntu / Debian
- **Compiler:** `gcc` (version >= 14.2.0)
- **Version Control:** `git` (version >= 2.47.3)

---

## 🚀 Setup & Installation

Run the following commands in your terminal to clone the repository, install the necessary system dependencies, and set up the isolated library environment.

```bash
# Clone the repo
git clone https://github.com/hasnath1/image_manipulation.git
cd image_manipulation

# Install system dependencies
sudo apt update
sudo apt install build-essential libgtk-3-dev libx11-dev pkg-config

# Create isolated library directories
mkdir ./iup
mkdir ./im

# Extract precompiled libraries into the isolated folders
tar -zxvf iup-3.32_Linux515_64_lib.tar.gz -C ./iup
tar -zxvf im-3.15_Linux515_64_lib.tar.gz -C ./im

```
