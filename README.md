# Key Value Server (KVS) -> VectorDB 

## Overview
This project is a working high-performance, networked key-value store that supports **hash maps, sorted sets (ZSets), TTL-based expiration, and efficient multi-threading** using a thread pool. The system is designed with **modern C++ (C++23)**, leveraging **RAII, std::expected, spans, and shared mutexes** to ensure optimal performance and safety. 

Currently, this project is still **in progress** - and will serve as the foundation for a vector store in the future. 
If it is not already clear, this is pedagogical and not suitable for any workload.  

### Key Features
- **Asynchronous Networking:** Uses non-blocking I/O (`poll`) for efficient connection handling.
- **Thread Pool:** Optimized for multi-threading with worker threads.
- **Hash Table & Sorted Set Support:** Efficient key-value storage with advanced querying features.
- **TTL Management:** Uses a **min-heap** for expiration handling.
- **RAII and Modern C++:** Proper resource management with `std::unique_ptr`, `std::shared_mutex`, and `std::expected`.
- **Efficient Serialization:** Uses binary format serialization for fast data transmission.

---

## Project Structure
```
/project
    ├── command_processor.hpp   # Command parsing & execution
    ├── common.hpp              # Common utilities and constants
    ├── connection.hpp          # Client connection handling
    ├── entry_manager.hpp       # Key-value store logic
    ├── logging.hpp             # Logger utility
    ├── request_parser.hpp      # Request parsing logic
    ├── response_serializer.hpp # Response formatting
    ├── server_state.hpp        # Global server state management
    ├── server.hpp              # Main server class
    ├── socket.hpp              # RAII-based socket wrapper
    ├── thread_pool.hpp         # Multi-threaded task execution
    ├── heap.hpp                # TTL handling with min-heap
    ├── zset.hpp                # Sorted set (ZSet) data structure
    ├── hashtable.hpp           # Hash table for key-value storage
    ├── list.hpp                # Doubly-linked list utility
    ├── common.hpp              # Common utilities and constants
    ├── avl.hpp                 # AVL Tree for fast sorting
``` 

---

## Installation & Build
### **Requirements**
- **C++23 compiler** (GCC 11+/Clang 14+/MSVC 19.3+)
- **CMake 3.31+**
- **Linux/macOS**

### **Build Instructions**
1. **Clone the repository:**
   ```sh
   git clone <repo_url>
   cd project
   ```
2. **Compile the project using CMake:**
   ```sh
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```

---

## Usage
### **Starting the Server**
Run the server with the default port:
```sh
./server
```

### **Example Client Interaction (Netcat)**
To set and retrieve a value:
```sh
echo "SET key1 value1" | nc localhost 1234
echo "GET key1" | nc localhost 1234
```

### **Supported Commands**
| Command | Description |
|---------|------------|
| `SET key value` | Stores a key-value pair |
| `GET key` | Retrieves the value of a key |
| `DEL key` | Deletes a key-value pair |
| `ZADD key score member` | Adds a member to a sorted set |
| `ZQUERY key min max limit` | Queries a sorted set |
| `PEXPIRE key milliseconds` | Sets a TTL on a key |
| `PTTL key` | Retrieves remaining TTL |

---

## **Performance Optimizations / Coming Soon**
- **Unit and Integration Testing**
- **Memory Pooling and Lock-Free Data Structures**
- **Zero/Copy Send/Recv**
- **Thread Affinity & NUMA Awareness**
- **Raft Replication**
- **Multi-Tiered Caching w/ w-TinyLFU**
- **Zero-copy string parsing with `std::span`**
- **Minimized system calls with batch processing**
- **Lock-free worker queue in thread pool**
- **Cache-friendly data structures**
- **Efficient memory management using RAII**



## License
This project is licensed under the MIT License. See `LICENSE` for details.

