Sure, here is a longer and more detailed description of the program in English:

## Inter-Process Communication Using Shared Memory and Semaphores (C)

**Introduction and Purpose**

The `process_a.c` and `process_b.c` programs demonstrate inter-process communication (IPC) between two processes using shared memory. Their primary objective is to exchange messages by segmenting them into chunks and reassembling them later.

**Key Features**

* **Shared Memory Implementation:** A dedicated shared memory segment is created to facilitate efficient message transfer between processes. This shared data region enables seamless communication without the overhead associated with other IPC methods.

* **Segmentation and Reassembly:** To optimize transmission, larger messages are strategically segmented into smaller, manageable units. These segments are stored in the shared memory buffer. The receiving process meticulously reassembles segmented messages before delivery, maintaining message integrity.

* **Semaphore-Based Synchronization:** Semaphores are employed as indispensable synchronization mechanisms. They enforce mutual exclusion on the shared memory region, preventing race conditions and ensuring that processes access the shared data in a well-defined,  coordinated manner.

* **Termination Handling:** The code gracefully handles termination signals passed between processes. A dedicated termination flag within the shared memory ensures a clean exit from the communication loop upon receiving the signal.

* **Comprehensive Reporting:** The project includes a reporting component. It tracks metrics such as messages sent and received, the average number of segments per message, and average wait times. This reporting provides valuable insights into communication performance.

**Technical Considerations**

* **Error Handling:** While not explicitly present in the source files, it's crucial to incorporate robust error handling mechanisms for handling shared memory creation, semaphore operations, and file operations.

* **Code Structure:** The code is purposefully structured to promote modularity and readability, using descriptive variable and function names.

**Usage Instructions**

1. **Compilation:** Navigate to the project directory and execute the following command to compile the programs:

```bash
make
```

2. **Cleaning:** To remove the generated object and executable files, use the following command:

```bash
make clean
```

3. **Running the Programs:** Open two separate terminals:

   * **Terminal 1:** Execute the following command:

     ```bash
     ./process_a
     ```

   * **Terminal 2:** Execute the following command:

     ```bash
     ./process_b
     ```

4. **Optional Flags:**

   * `-debug`: Enables debug mode, providing additional diagnostic messages.

   * `-file`: Saves all program printouts to specified files.

**Program Description**

**Shared Memory Management**

* **Creating Shared Memory:** The `shm_open` function is employed to create or attach to shared memory.

* **Mapping Memory:** The `mmap` function maps the shared memory into the process's address space.

**Message Management**

* **Message Segmentation:** Each message is divided into segments, which are stored in shared memory.

* **Message Reassembly:** The segments are reassembled to reconstruct the original message.

**Synchronization with Semaphores**

* **Semaphores:** Semaphores are utilized to synchronize process access to shared memory.

**Thread Management**

* **Threads:** Both programs create two threads: one for sending and one for receiving messages. Threads enable concurrent sending and receiving operations, enhancing communication efficiency.

   * **Send Thread (`send_message`):** Reads user input, segments messages, and sends them via shared memory.

   * **Receive Thread (`receive_message`):** Monitors shared memory for segmented messages, reassembles them, and displays them to the user.

**Semaphore Usage**

* `sem1`: Ensures that one thread writes to shared memory before the other thread reads from it.

* `sem2`: Notifies the receive thread when data is available for retrieval.

**Message Segmentation and Reassembly**

* `segment_message` function: Splits the message into segments of the predefined `SEGMENT_SIZE` and stores them in shared memory.

* `reassemble_message` function: Reconstructs the message from the segments stored in shared memory.

**Termination Handling and Resource Cleanup**

* **Communication Termination:** Upon receiving a specific signal (e.g., "#BYE#\n"), processes set a termination flag (`termination_flag`). This alerts threads to stop processing and perform cleanup.

* **Resource Cleanup:** Involves destroying semaphores, unmapping shared memory using `munmap`, and closing the corresponding shared memory file with `shm_unlink`.

**Debug Mode Operation**

* The programs can be run in "debug mode" by providing the `-debug` flag on the command line. This displays additional diagnostic messages.

**File Output Option**

* The `-file` flag can be used to save all program printouts to specified
