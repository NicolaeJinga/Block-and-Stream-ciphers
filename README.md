# Block-and-Stream-ciphers (June 2017)
Bachelor's thesis project. Implemented AES, DES, 3DES and RC4 ciphers, as well as ECB and CBC operation modes. Developed in C++ with SFML. Rebuilt in August 2019 with SFML 2.5.1.

## Project Description

On the right side, there are the four ciphers implemented. In the middle, there is text field to point to the file which will be encrypted and just below it are the two main functions: Encrypt and Decrypt. Below the encrypt button, there is a button named "Pixels", which when is selected, the encrypt and decrypt functions will read the input as an array of pixels instead of bytes. To use this button only makes sense in the context of encrypting and decrypting images because they are represented as pixels. The reason for this button was to highlight the difference between encrypting with ECB operation mode and encrypting with CBC operation mode. ".ext" button, is a helper button, in the sense that it adds additional extensions to the encrypted or decrypted file (such as being encrypted with a certain cipher and certain operation modes)

![](licentaUI.png)

