# Compiling `inventory-system.c`

This guide explains how to compile the single-file `inventory-system.c` Inventory Management System using:

- **SQLite amalgamation** (with `sqlite3.c` and `sqlite3.h`)

---

## 🛠 Requirements

- GCC or another standard C compiler
- SQLite3 development files
---

## Compile on Linux/macOS

### Option 1: Using SQLite Amalgamation

If you have `sqlite3.c` and `sqlite3.h` in the same directory:

```bash
gcc inventory-system.c sqlite3.c -o inventory_system
````

---

## Compile on Windows (MSYS2/CMD)

### Option 1: With SQLite Amalgamation

Open the **MSYS2 terminal** or **CMD**, then run:

```bash
gcc inventory-system.c sqlite3.c -o inventory_system.exe
```

Then run the program:

```bash
./inventory_system.exe
````

---

## Output

* **Executable name:** `inventory_system` (or `inventory_system.exe` on Windows)
* **Database file:** `data.db` (created automatically)
* **Login file:** `configs.dat` (created at runtime)

This executable is portable and ready to run in any compatible terminal.
