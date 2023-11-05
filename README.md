# Skylight
Thank you all for the incredible support!
## A bright opening, a clean window.

### **Etymology**
According to [dictionary.com](https://www.dictionary.com/browse/skylight#), a "skylight" is:
> an opening in a roof or ceiling, fitted with glass, for admitting daylight.

Skylight is an operating system that is uniquely modeled, and aims to have clean, transparent, and clear code (like a window) and more efficient.

### **License**
As of June 18th, 2022, I have updated the license on this repository to the MIT license. However, any commits prior to commit `47912c3` still fall under the CC0 license applied at the time.

For more information on the terms of the MIT license, see [LICENSE](LICENSE).

### **Building**
Skylight uses Linux software to compile, therefore any build process must take place under native Linux, emulated Linux, or the Windows Subsystem for Linux.

Additionally, you require these packages:
```
util-linux 
mtools 
nasm 
clang 
build-essential 
wget
```

To build, simply enter the root of the source tree, and run `make image`. A FAT32-formatted hard drive image will be generated. For more information on build dependencies and required versions, [see here](docs/BUILD-DEPS.md).

### Testing
In order to test, you need to ensure that your testing emulator/hardware supports:
 - x86-64
 - UEFI v2.0
 - PCIe
 - 256MiB RAM (min.)

To test in an emulator, enter the source tree after building an image, and type `make run`.

To test on real hardware, meet the requirements above, and write the image to a drive.
Boot into it through your UEFI boot menu.

### Development Cycle
OS development is... time consuming. I have other things to dedicate my time to, so I can't always work on Skylight. I will try to push at least one commit every week, but I can't guarantee that I will be able to do so. In summer months you can expect a high volume of commits. December is also a rather popular month for me. I have been working on this operating system project since mid-2021, and I don't intend on abandoning it. Just some times, it's a literal pain to work on. Other times, I don't have the time to do it.

### Contributing
I largely do not take contributions. I do take suggestions and feedback, but most of the source code should be written by me. If the project plateaus and the work is no longer worth my dedication I may open it up to contributions to keep it alive, but for now, it's principally my effort.
