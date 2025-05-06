# VESA 3.00 Programming Development Kit (v3.0)

**Developed by Execom of Realtech**
📧 `execom@www.scene.org`
🌐 [http://www.realtech.scene.org](http://www.realtech.scene.org)

---

## 📘 Introduction

This SDK provides support for **VESA 2.0/3.0** video programming under DOS, enabling high-performance graphics with **linear frame buffer (LFB)** access and **bank-free video memory usage**.

> For detailed specifications on VESA standards, visit [http://www.vesa.org](http://www.vesa.org)

### ✨ Features

* Direct video access under DOS with no bank switching.
* Designed for **32-bit compilers** (not 16-bit).
* Works with **DOS extenders** like:

  * **DOS/4GW 1.95+**
  * **Watcom C/C++ 10.6/11**
  * **DJGPP v2**
* Includes a **safe LFB allocation/deallocation** system with memory protection.
* C++ wrapper available.
* Separate **DPMI service layer**, DJGPP-compatible, allows simulating real-mode interrupts (e.g., CD-ROM support via MSCDEX).
* Partial compatibility with **VESA 1.2** (bank switching support).
* Handles common quirks in video card drivers automatically (e.g., incorrect mode reporting).

### ✅ Tested Video Cards

* Matrox Millennium
* Chips 65550
* S3 Trio64/64V+, Virge, 968
* ET4000/W32p
* ATI Rage Pro, Rage 128
* NVIDIA GeForce
* Many others...

**Note:** Compatible with Windows 9x (not Windows NT).
⚠️ Some drivers may fail to restore correct video modes when returning to Windows — use in pure DOS when possible.

### ⚙ Driver Quirks Handled

* **15-bit mode = 16-bit mode** (Matrox)
* **800×600 mode = 960-byte scanline** (Matrox)

---

## 🛠 Compiling

**Important:** The archive contains subdirectories.

### Supported Compilers

* **Watcom C/C++ 10+**

  * Rename `makefile.wat` → `makefile`
* **DJGPP v2+**

  * Rename `makefile.gcc` → `makefile`

### Directory Structure

```
/VBELIB   → VBE Core library source and RM DPMI manager
/DEMOS    → Demo programs
/LIB      → Compiled VBE Core library
/INCLUDE  → Header files
```

---

## 🚀 Demos

1. **`demo0.exe`**
   Detects VBE support and lists available video modes.
   ⚠️ Some cards may list duplicate modes — ensure uniqueness in your mode list.

2. **`demo1.exe`**
   LFB write test in **640×480×16-bit** mode using a **Julia fractal**.
   Uses `VBE_LockSurfaceAlt` to access the front buffer.

3. **`demo2.exe`**
   Page flipping and back buffer test using basic line drawing.

---

## 📄 License & Credits

This SDK is **free to use**, but **do not modify the package contents**.
If you use this in your own projects, **credits are appreciated**.

While clones of this SDK have surfaced, this was the **first SDK dedicated to VESA 2.0**.

For questions, contact: `realtech@www.scene.org`

---

**End of File**

---

Would you like me to modernize this even further, such as adding GitHub-specific enhancements (e.g., usage instructions, badges, build steps, CI suggestions)?
