# **Parallel Game of Life**

![Game of Life](https://upload.wikimedia.org/wikipedia/commons/e/e5/Gospers_glider_gun.gif)

A multi-threaded implementation of **Conway's Game of Life**, utilizing a **thread pool** for efficient parallel processing.
This project follows synchronization principles using `pthread` and mutexes to optimize performance.

---

## 🚀 **Features**
- ✅ **Parallel execution** using a thread pool
- ✅ **Customizable grid size & thread count**
- ✅ **Interactive & non-interactive modes**
- ✅ **Optimized synchronization using mutex & condition variables**

---

## 🛠 **Installation & Compilation**

### **Prerequisites**
Ensure you have the following installed:
- **C++11 or later**
- **Make**
- **g++**
- **pthreads library**

### **Build Instructions**
Run the following commands in the project directory:
```bash
make
```
This will generate the executable `GameOfLife`.

---

## 📌 **Usage**

Run the program using:
```bash
./GameOfLife <matrixfile.txt> <num_generations> <num_threads> <interactive(Y/N)> <print_output(Y/N)>
```

### **Arguments**
| Argument | Description |
|----------|------------|
| `<matrixfile.txt>` | Input file containing the initial grid state |
| `<num_generations>` | Number of generations to simulate |
| `<num_threads>` | Number of threads to use |
| `<interactive (Y/N)>` | Enables interactive animation if `Y` |
| `<print_output (Y/N)>` | Prints output to console if `Y` |

### **Example Usage**
```bash
./GameOfLife input.txt 50 4 Y N
```

---

## 📄 **Project Structure**
```bash
📂 GameOfLife
├── 📂 src                    # Source files (C++ implementation)
│   ├── main.cpp              # Main driver file
│   ├── Game.cpp              # Core logic for game execution
│   ├── Semaphore.cpp         # Semaphore implementation
│   ├── utils.cpp             # Utility functions
├── 📂 include                # Header files
│   ├── Game.hpp
│   ├── Semaphore.hpp
│   ├── utils.hpp
│   ├── Thread.hpp
│   ├── PCQueue.hpp
│   ├── Headers.hpp           # Global headers (if needed)
├── 📂 input                  # Example input files
│   ├── sample_matrix.txt
├── 📂 assets                 # (Optional) Store images or GIFs for README
│   ├── game_of_life.gif
├── LICENSE                   # License file
├── Makefile                  # Makefile for compiling the project
├── README.md                 # Main project documentation
```

---

## 🎮 **Game Rules**
1. **A live cell** with **2 or 3** live neighbors **survives** to the next generation.
2. **A dead cell** with **exactly 3** live neighbors **becomes alive**.
3. In all other cases, the cell **dies or remains dead**.

---

## 📈 **Performance & Parallelism**
This project uses **thread pooling** for efficient parallel execution:
- The grid is split among threads **horizontally**.
- Each thread processes its assigned rows independently.
- **Synchronization** is handled via `pthread_mutex_t` and `pthread_cond_t`.
- Ensures minimal idle time with balanced workload distribution.

---

## 🏗 **Contributing**
Want to improve this project? Feel free to fork & submit a PR!

1. Clone the repository:
```bash
git clone https://github.com/yourusername/GameOfLife.git
```
2. Create a new branch:
```bash
git checkout -b feature-branch
```
3. Make changes and commit:
```bash
git commit -m "Add a cool feature"
```
4. Push and open a **Pull Request**!

---

## 📜 **License**
This project is licensed under the **MIT License**.

---

### 💡 **References & Resources**
- [Conway's Game of Life (Wikipedia)](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)
- [Thread Pool Pattern](https://en.wikipedia.org/wiki/Thread_pool)

